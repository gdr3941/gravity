#include <fmt/core.h>
#include <fmt/ranges.h>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <utility>
#include <vector>
#include "SFML/System/Vector2.hpp"
#include "SFML/Window/Window.hpp"
#include <range/v3/all.hpp>
#include <functional>
#include <algorithm>
#include <cmath>
#include "util.h"

namespace r = ranges;

constexpr size_t kNumRocks = 100;
constexpr float kInitialPosExtent = 45.0f;
constexpr float kInitialVelExtent = 20.0f;
constexpr float kInitialRadiusMin = 1.0f;
constexpr float kInitialRadiusMax = 6.0f;
constexpr float kInitialViewportScale = 3.0f;
constexpr float kGravity = 6.67408f;

//
// Rock - Abstract Entity in our Simulation
//

struct Rock {
    sf::Vector2f pos {0,0};
    sf::Vector2f vel {0,0};
    float radius {0.0f};
};

std::ostream& operator<<(std::ostream& out, const Rock& r)
{
    return out << "Rock: pos: " << r.pos.x << "," << r.pos.y
               << " vel: " << r.vel.x << "," << r.vel.y
               << " radius: " << r.radius << "\n";
}

Rock newRandomRock()
{
    Rock rock;
    rock.pos.x = util::f_rand(-kInitialPosExtent, kInitialPosExtent);
    rock.pos.y = util::f_rand(-kInitialPosExtent, kInitialPosExtent);
    rock.vel.x = util::f_rand(-kInitialVelExtent, kInitialVelExtent);
    rock.vel.y = util::f_rand(-kInitialVelExtent, kInitialVelExtent);
    rock.radius = util::f_rand(kInitialRadiusMin, kInitialRadiusMax);
    return rock;
}

//
// Simulation World that holds Entities and settings
//

struct World {
    std::vector<Rock> rocks;  // abstract objects in world
    std::vector<sf::CircleShape> shapes;  // screen object cache
    float viewportScale {kInitialViewportScale};  // scale factor from game to screen size
    sf::RenderWindow* window;

    explicit World(size_t numRocks, sf::RenderWindow* win)
        : rocks(numRocks), shapes(numRocks), viewportScale {kInitialViewportScale},
          window {win} {};
};

//
// Helpers for Systems
//

float distanceBetween(const sf::Vector2f& a, const sf::Vector2f& b)
{
    return sqrt((a.x - b.x) * (a.x - b.x) +
                (a.y - b.y) * (a.y - b.y));
}

/// Returns true if in contact and still moving towards one another
bool isColliding(const Rock& a, const Rock& b)
{
    float currentDistance = distanceBetween(a.pos, b.pos);
    if (currentDistance > (a.radius + b.radius)) return false;
    // Now check if still moving closer to one another
    // by using a very small time increment
    sf::Vector2f future_a_pos = a.pos + (a.vel * 0.001f);
    sf::Vector2f future_b_pos = b.pos + (b.vel * 0.001f);
    float futureDistance = distanceBetween(future_a_pos, future_b_pos);
    return (futureDistance < currentDistance);
}


void updateForCollision(Rock& a, Rock& b)
{
    float a_mass = a.radius * a.radius;
    float b_mass = b.radius * b.radius;
    sf::Vector2f a_new_vel = (a.vel * (a_mass - b_mass) + (2.0f * b_mass * b.vel)) / (a_mass + b_mass);
    sf::Vector2f b_new_vel = (b.vel * (b_mass - a_mass) + (2.0f * a_mass * a.vel)) / (a_mass + b_mass);
    a.vel = a_new_vel;
    b.vel = b_new_vel;
}

sf::Color colorFromVelocity(const sf::Vector2f& vel)
{
    float vel_percent = ((vel.x * vel.x + vel.y * vel.y)
                         / (2 * kInitialVelExtent * kInitialVelExtent));
    int red_level = std::clamp((int)(vel_percent * 255), 0, 255);
    return sf::Color(red_level, 0, 255 - red_level);
}

float mass(const Rock& rock)
{
    return rock.radius * rock.radius;
}

/// returns gravity acceleration components for each object due to gravitation
std::pair<sf::Vector2f, sf::Vector2f>
gravityAccelComponents(const Rock& a, const Rock& b, const float gConst)
{
    float distance2 = (a.pos.x - b.pos.x) * (a.pos.x - b.pos.x) +
        (a.pos.y - b.pos.y) * (a.pos.y - b.pos.y);
    float acc = gConst / distance2;
    float t_acc_a = acc * mass(b);  // total acceleration on a
    float t_acc_b = acc * mass(a);  // total acceleration on b
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
    Rock a {.pos = {-0.1,0.0}, .radius = 1.0f};
    Rock b {.pos = {0,0}, .radius = 100.0f};
    auto [acc_a, acc_b] = gravityAccelComponents(a, b, 6.67408e-11);
    fmt::print("a.x {} a.y {} b.x {} b.y {}", acc_a.x, acc_a.y, acc_b.x, acc_b.y);
}

//
// Entity Systems
//

void updateRockPositionSystem(World& world, float timeStep)
{
    for (auto& rock : world.rocks) {
        rock.pos += rock.vel * timeStep;
    }
}

void updateShapeSystem(World& world)
{
    // look into using a sf::view instead of my scaling below
    auto winSize = world.window->getSize();
    sf::Vector2u winCenter = {winSize.x / 2, winSize.y / 2};
    for (size_t i = 0; i < world.shapes.size(); i++) {
        auto screenRadius = world.rocks[i].radius * world.viewportScale;
        world.shapes[i].setRadius(screenRadius);
        world.shapes[i].setOrigin(screenRadius, screenRadius);
        world.shapes[i].setPosition(
            (world.rocks[i].pos.x * world.viewportScale) + winCenter.x,
            winCenter.y - (world.rocks[i].pos.y * world.viewportScale));
        world.shapes[i].setFillColor(colorFromVelocity(world.rocks[i].vel));
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
    util::for_distinct_pairs(world.rocks, [=](Rock& a, Rock& b){
        auto [a_acc, b_acc] = gravityAccelComponents(a, b, kGravity);
        a.vel += (a_acc * timestep);
        b.vel += (b_acc * timestep);
    });
}
//
// Visualization
//

void draw(const World& world)
{
    for (auto& shape : world.shapes) {
        world.window->draw(shape);
    }
}

//
// Event Handling
//

void handleEvents(World& world)
{
    sf::Event event;
    while (world.window->pollEvent(event)) {
        if (event.type == sf::Event::Closed) { world.window->close(); }
        if (event.type == sf::Event::KeyPressed) {
            switch (event.key.code) {
            case sf::Keyboard::Up:
                world.viewportScale *= 1.2;
                break;
            case sf::Keyboard::Down:
                world.viewportScale *= 0.8;
                break;
            default:
                break;
            }
        }
    }
}

//
// Initial Setup
//

World createWorld(size_t numRocks, sf::RenderWindow* win)
{
    World world(numRocks, win);
    r::generate(world.rocks, newRandomRock);
    updateShapeSystem(world); // set shapes positions based on current rocks
    return world;
}

//
// Run Loop
//

void run()
{
    fmt::print("Gravity Simulator\n");
    sf::RenderWindow window(sf::VideoMode(1000, 1000), "Gravity");
    window.setFramerateLimit(60);

    World world = createWorld(kNumRocks, &window);

    sf::Clock clock;
    while (window.isOpen()) {
        handleEvents(world);
        float delta = clock.restart().asSeconds();
        updateCollisionSystem(world);
        updateGravitySystem(world, delta);
        updateRockPositionSystem(world, delta);
        updateShapeSystem(world);
        window.clear();
        draw(world);
        window.display();
    }
}

int main()
{
    run();
}
