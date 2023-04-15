#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <cassert>
#include <oneapi/tbb/parallel_for_each.h>
#include "util.h"
#include "world.hpp"

namespace {
 
sf::Color colorFromVelocity(const sf::Vector2f& vel, const float velExtent)
{
    float vel_percent = ((vel.x * vel.x + vel.y * vel.y)
                         / (velExtent * velExtent));
    int red_level = std::clamp((int)(vel_percent * 255), 0, 255);
    return sf::Color(red_level, 0, 255 - red_level);
}

// See for another approach (but this new one works really good!)
// https://gamedev.stackexchange.com/questions/15708/how-can-i-implement-gravity
// acceleration = force(time, position) / mass;
// time += timestep;
// position += timestep * (velocity + timestep * acceleration / 2);
// newAcceleration = force(time, position) / mass;
// velocity += timestep * (acceleration + newAcceleration) / 2;

sf::Vector2f gravityAccelTree(const World& world, const TreeNode& node, const Rock& a)
{
    sf::Vector2f pos_vec = node.center_mass - a.pos;
    float dist2 = pos_vec.x * pos_vec.x + pos_vec.y * pos_vec.y;
    if (dist2 < 0.00001) {return {0.0f, 0.0f};}  // if on top of COM (or is same as a), do nothing
    float dist = sqrt(dist2);
    if ((node.nodeWidth() / dist) < world.theta) {
        // use aggregrate mass
        float grav_a = world.gravity * node.total_mass / dist2;
        return {(pos_vec.x * grav_a) / dist, (pos_vec.y * grav_a) / dist};
    } else if (node.hasChildren()) {
        // use children
        sf::Vector2f acc_a {0.0,0.0};
        for (const auto& child : node.children) {
            acc_a += gravityAccelTree(world, child, a);
        }
        return acc_a;
    } else if (!node.element) {
        return {0.0f, 0.0f};
    } else {
        // single element @ node
        if (world.ignoreShortDistGrav && dist < (a.radius + node.element->radius)) {
            return {0.0f, 0.0f};
        }
        float grav_a = world.gravity * node.total_mass / dist2;
        return {(pos_vec.x * grav_a) / dist, (pos_vec.y * grav_a) / dist};
    }
}

void processCollisionTree(const World& world, const TreeNode& node, Rock& a)
{
    if (node.element) {
        if (node.element <= &a) return; // prevents repeating pairs
        if (isColliding(a, *node.element)) {
            updateForCollision(a, *node.element);
            return;
        }
    }
    sf::Vector2f pos_v = node.center - a.pos;
    float dist2 = pos_v.x * pos_v.x + pos_v.y * pos_v.y;
    // actual max from center inside a node is 1/sqrt(2) * node width
    // yet to make math faster, just using width as worst case
    float sum_radius = node.max_radius + node.nodeWidth() + a.radius;
    if (dist2 > (sum_radius * sum_radius)) {
        // means far enough away can ignore
        return;
    } else if (node.hasChildren()) {
        for (const auto& child : node.children) {
            processCollisionTree(world, child, a);
        }
    } 
}

// Use simpler check to see if node is close enough to worry about
void processCollisionTree2(const World& world, const TreeNode& node, Rock& a)
{
    if (node.element) {
        if (node.element <= &a) return; // prevents repeating pairs
        if (isColliding(a, *node.element)) {
            updateForCollision(a, *node.element);
            return;
        }
    }
    float sr = node.max_radius + a.radius;
    if (a.pos.x < (node.left-sr) || a.pos.x > (node.right+sr) || a.pos.y < (node.bottom-sr) || a.pos.y > (node.top+sr)) {
        // means far enough away can ignore
        return;
    } else if (node.hasChildren()) {
        for (const auto& child : node.children) {
            processCollisionTree2(world, child, a);
        }
    } 
}

// Idea: could we do parallel check for collisions, then when done, process 
// Actual collisions on single thread
// Testing new idea
void checkForCollisions(World& world, const TreeNode& node, Rock& a)
{
    if (node.element) {
        if (node.element <= &a) return; // prevents repeating pairs
        if (isColliding(a, *node.element)) {
            world.collisions.enqueue({&a,node.element});
            return;
        }
    }
    float sr = node.max_radius + a.radius;
    if (a.pos.x < (node.left-sr) || a.pos.x > (node.right+sr) || a.pos.y < (node.bottom-sr) || a.pos.y > (node.top+sr)) {
        // means far enough away can ignore
        return;
    } else if (node.hasChildren()) {
        int result = 0;
        for (const auto& child : node.children) {
            checkForCollisions(world, child, a);
        }
    } 
}

}  // namespace

//
// General Functions
//

void addRandomRocks(World& world, size_t numRocks, RockConfig rockConfig)
{
    world.rocks.reserve(world.rocks.size() + numRocks);
    world.shapes.reserve(world.shapes.size() + numRocks);
    for (size_t i = 0; i<numRocks; ++i) {
        Rock rock = newRandomRock(rockConfig);
        world.rocks.push_back(rock);
        world.shapes.push_back(shapeFor(rock));
    }
}

void addSatRocks(World& world)
{
    world.rocks.push_back(Rock {.pos = {0,0}, .vel = {0,0}, .radius = 20.0f});
    for (size_t i = 4; i < 10; ++i) {
        world.rocks.push_back(Rock {.pos = {i*5.0f,0}, .vel = {0, 4.0}, .radius = 2.0});
    }
    for (auto& rock: world.rocks) {
        world.shapes.push_back(shapeFor(rock));
    }
}

void deleteAllRocks(World& world)
{
    world.rocks = {};
    world.shapes = {};
}

//
// Entity Systems
//

void updateTreeSystem(World& world)
{
    world.rootTree = TreeNode(world.worldExtent);
    for (auto& rock : world.rocks) {
        world.rootTree.insert(&rock);
    }
}

// This is bottleneck, see new par version below
void updateCollisionSystemTree(World& world)
{
    for (Rock& rock : world.rocks) {
        processCollisionTree2(world, world.rootTree, rock);
    }
}

// Much faster!!
void updateCollisionSystemPar(World& world)
{
    // util::Timer timer;
    tbb::parallel_for_each(world.rocks, [&world](Rock& a) {
        checkForCollisions(world, world.rootTree, a);
    });
    CollidingPair cp;
    while (world.collisions.try_dequeue(cp)) {
       updateForCollision(*cp.first, *cp.second);
    }
}


void updateGravitySystemTree(World& world, float timestep)
{
    tbb::parallel_for_each(world.rocks, [timestep, &world](Rock& a) {
        a.vel += (gravityAccelTree(world, world.rootTree, a) * timestep);
    });
}

void updateRockPositionSystem(World& world, float timeStep)
{
    for (auto& rock : world.rocks) {
        rock.pos += rock.vel * timeStep;
    }
}

void updateShapeSystem(World& world)
{
    for (size_t i = 0; i < world.shapes.size(); ++i) {
        world.shapes[i].setPosition(world.rocks[i].pos.x, -world.rocks[i].pos.y);
        world.shapes[i].setFillColor(colorFromVelocity(world.rocks[i].vel,
                                                       world.velColorExtent));
    }
}

//
// Visualization
//

sf::CircleShape shapeFor(const Rock& rock)
{
    sf::CircleShape circle;
    circle.setRadius(rock.radius);
    circle.setOrigin(rock.radius, rock.radius);
    circle.setPosition({rock.pos.x, -rock.pos.y});
    return circle;
}

void draw(const World& world)
{
    for (auto& shape : world.shapes) {
        world.window->draw(shape);
    }
}
