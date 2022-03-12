#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#import "util.h"
#import "world.hpp"

World createRandomWorld(size_t numRocks, RockConfig config, sf::RenderWindow* win)
{
    World world;
    world.window = win;
    world.rocks.reserve(numRocks);
    world.shapes.reserve(numRocks);
    for (size_t i = 0; i<numRocks; i++) {
        Rock rock = newRandomRock(config);
        world.rocks.push_back(rock);
        world.shapes.push_back(shapeFor(rock));
    }
    return world;
}

World createSatWorld(sf::RenderWindow* win)
{
    World world;
    world.window = win;
    world.rocks.push_back(Rock {.pos = {0,0}, .vel = {0,0}, .radius = 20.0f});
    for (size_t i = 4; i < 10; i++) {
        world.rocks.push_back(Rock {.pos = {i*5.0f,0}, .vel = {0, 4.0}, .radius = 2.0});
    }
    for (auto& rock: world.rocks) {
        world.shapes.push_back(shapeFor(rock));
    }
    return world;
}

//
// System Helpers
// 

sf::Color colorFromVelocity(const sf::Vector2f& vel)
{
    float vel_percent = ((vel.x * vel.x + vel.y * vel.y)
                         / (2 * kInitialVelExtent * kInitialVelExtent));
    int red_level = std::clamp((int)(vel_percent * 255), 0, 255);
    return sf::Color(red_level, 0, 255 - red_level);
}


std::pair<sf::Vector2f, sf::Vector2f>
gravityAccelComponents(const Rock& a, const Rock& b, const float gConst)
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
    float gravity = world.gravity;
    util::for_distinct_pairs(world.rocks, [timestep, gravity](Rock& a, Rock& b){
        auto [a_acc, b_acc] = gravityAccelComponents(a, b, gravity);
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
        world.shapes[i].setFillColor(colorFromVelocity(world.rocks[i].vel));
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
