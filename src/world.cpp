#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <cassert>
#include <oneapi/tbb/parallel_for_each.h>
#include "util.h"
#include "world.hpp"

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
// System Helpers
// 

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

std::pair<sf::Vector2f, sf::Vector2f>
gravityAccelComponents(const Rock& a, const Rock& b, const float gConst, bool ignoreShortDist)
{
    // util::Timer timer;
    sf::Vector2f pos_a = b.pos - a.pos;
    float dist2 = pos_a.x * pos_a.x + pos_a.y * pos_a.y;
    if (ignoreShortDist && dist2 < ((a.radius + b.radius) * (a.radius + b.radius))) {
        // dont add gravity if overlapping to prevent overacceleration
        return {{0,0}, {0,0}};
    }
    float dist = sqrt(dist2);

    float g_a = gConst * mass(b) / dist2;
    sf::Vector2f acc_a {(pos_a.x * g_a) / dist, (pos_a.y * g_a) / dist};

    sf::Vector2f pos_b = -pos_a;
    float g_b = gConst * mass(a) / dist2;
    sf::Vector2f acc_b {(pos_b.x * g_b) / dist, (pos_b.y * g_b) / dist};
    
    return {acc_a, acc_b};
}

inline
sf::Vector2f gravityAccel(const Rock& a, const Rock& b, const float gConst, bool ignoreShortDist)
{
    // util::Timer timer;
    sf::Vector2f pos_a = b.pos - a.pos;
    float dist2 = pos_a.x * pos_a.x + pos_a.y * pos_a.y;
    if (ignoreShortDist && dist2 < ((a.radius + b.radius) * (a.radius + b.radius))) {
        // dont add gravity if overlapping to prevent overacceleration
        return {0,0};
    }
    float dist = sqrt(dist2);
    float g_a = gConst * mass(b) / dist2;
    sf::Vector2f acc_a {(pos_a.x * g_a) / dist, (pos_a.y * g_a) / dist};
    return acc_a;
}

sf::Vector2f gravityAccelTree(const World& world, const TreeNode& node, const Rock& a)
{
    sf::Vector2f distV = node.center_mass - a.pos;
    float dist2 = distV.x*distV.x + distV.y*distV.y;
    if (dist2 < 0.00001) {return {0.0f, 0.0f};}  // if on top of COM (or is same as a), do nothing
    float dist = sqrt(dist2);
    if ((node.nodeWidth() / dist) < world.theta) {
        // use aggregrate mass
        float g_a = world.gravity * node.total_mass / dist2;
        return {(distV.x * g_a) / dist, (distV.y * g_a) / dist};
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
            // dont add gravity if overlapping to prevent overacceleration
            return {0.0f, 0.0f};
        }
        float g_a = world.gravity * node.total_mass / dist2;
        return {(distV.x * g_a) / dist, (distV.y * g_a) / dist};
    }
}

//
// Entity Systems
//

void updateTreeSystem(World& world)
{
    world.rootTree = TreeNode(world.worldExtent);
    for (auto& rock : world.rocks) {
        // std::cout << "system insert rock: " << rock.pos.x << "," << rock.pos.y << "\n";
        world.rootTree.insert(&rock);
    }
}

void updateCollisionSystem(World& world)
{
    util::for_distinct_pairs(world.rocks, [](Rock& a, Rock& b){
        if (isColliding(a, b)) { updateForCollision(a, b); };
    });
}

void updateGravitySystem(World& world, float timestep)
{
    util::for_distinct_pairs(world.rocks, [timestep, &world](Rock& a, Rock& b){
        auto [a_acc, b_acc] = gravityAccelComponents(a, b, world.gravity, world.ignoreShortDistGrav);
        a.vel += (a_acc * timestep);
        b.vel += (b_acc * timestep);
    });
}

void updateGravitySystemPar(World& world, float timestep)
{
    tbb::parallel_for_each(world.rocks, [timestep, &world](Rock& a) {
        sf::Vector2f acc;
        for (const Rock& b : world.rocks) {
            if (&a != &b) {
                acc += gravityAccel(a, b, world.gravity, world.ignoreShortDistGrav);
            }
        }
        a.vel += (acc * timestep);
    });
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
