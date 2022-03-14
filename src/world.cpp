#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <cassert>
#include "util.h"
#include "world.hpp"

void addRandomRocks(World& world, size_t numRocks, RockConfig rockConfig)
{
    world.rocks.reserve(world.rocks.size() + numRocks);
    world.shapes.reserve(world.shapes.size() + numRocks);
    for (size_t i = 0; i<numRocks; i++) {
        Rock rock = newRandomRock(rockConfig);
        world.rocks.push_back(rock);
        world.shapes.push_back(shapeFor(rock));
    }
}

void addSatRocks(World& world)
{
    world.rocks.push_back(Rock {.pos = {0,0}, .vel = {0,0}, .radius = 20.0f});
    for (size_t i = 4; i < 10; i++) {
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

// See
// https://gamedev.stackexchange.com/questions/15708/how-can-i-implement-gravity
// for another approach

std::pair<sf::Vector2f, sf::Vector2f>
gravityAccelComponents(const Rock& a, const Rock& b, const float gConst)
{
    sf::Vector2f pos_a = b.pos - a.pos;
    float dist2 = pos_a.x * pos_a.x + pos_a.y * pos_a.y;

    if (dist2 == 0.0) return {{0,0}, {0,0}};
    float dist = sqrt(dist2);

    float g_a = gConst * b.mass() / dist2;
    sf::Vector2f acc_a {(pos_a.x * g_a) / dist, (pos_a.y * g_a) / dist};

    sf::Vector2f pos_b = -pos_a;
    float g_b = gConst * a.mass() / dist2;
    sf::Vector2f acc_b {(pos_b.x * g_b) / dist, (pos_b.y * g_b) / dist};
    
    return {acc_a, acc_b};
}

std::pair<sf::Vector2f, sf::Vector2f>
gravityAccelComponentsOld(const Rock& a, const Rock& b, const float gConst)
{
    float distance2 = (a.pos.x - b.pos.x) * (a.pos.x - b.pos.x) +
        (a.pos.y - b.pos.y) * (a.pos.y - b.pos.y);
    float acc = gConst / distance2;
    float t_acc_a = acc * b.mass();  // total acceleration on a
    float t_acc_b = acc * a.mass();  // total acceleration on b
    // Now break down totals into x & y components
    sf::Vector2f a_vec = b.pos - a.pos;
    sf::Vector2f b_vec = a.pos - b.pos;
    float a_radians = atan2(a_vec.y, a_vec.x);
    float b_radians = atan2(b_vec.y, b_vec.x);
    sf::Vector2f acc_a {static_cast<float>(cos(a_radians)*t_acc_a), static_cast<float>(sin(a_radians)*t_acc_a)};
    sf::Vector2f acc_b {static_cast<float>(cos(b_radians)*t_acc_b), static_cast<float>(sin(b_radians)*t_acc_b)};
    return {acc_a, acc_b};
}


void testGrav()
{
    Rock a {.pos = {0,0}, .radius = 10.0f};
    Rock b {.pos = {0,0}, .radius = 1.0f};
    auto [g_a, g_b] = gravityAccelComponentsOld(a, b, 0.6f);
    std::cout << "a: " << g_a.x << ", " << g_a.y << "\n";
    std::cout << "b: " << g_b.x << ", " << g_b.y << "\n";
    auto [g_a_o, g_b_o] = gravityAccelComponents(a, b, 0.6f);
    std::cout << "a_fast: " << g_a_o.x << ", " << g_a_o.y << "\n";
    std::cout << "b_fast: " << g_b_o.x << ", " << g_b_o.y << "\n";
}

//
// Entity Systems
//

void updateCollisionSystem(World& world)
{
    util::for_distinct_pairs(world.rocks, [](Rock& a, Rock& b){
        if (isColliding(a, b)) { updateForCollision(a, b); };
    });
}

void updateGravitySystem(World& world, float timestep)
{
    util::for_distinct_pairs(world.rocks, [timestep, &world](Rock& a, Rock& b){
        auto [a_acc, b_acc] = gravityAccelComponents(a, b, world.gravity);
        a.vel += (a_acc * timestep);
        b.vel += (b_acc * timestep);
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
    for (size_t i = 0; i < world.shapes.size(); i++) {
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
