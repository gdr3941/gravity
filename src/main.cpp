#include <fmt/core.h>
#include <fmt/ranges.h>
#include <sys/types.h>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <vector>
#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/Shape.hpp"
#include "SFML/System/Vector2.hpp"
#include "SFML/Window/Window.hpp"
#include <range/v3/all.hpp>
#include <functional>
#include "range/v3/algorithm/generate.hpp"
#include "util.h"

using namespace std::placeholders;
using std::cout;

namespace r = ranges;

constexpr size_t kNumRocks = 10;
constexpr float kInitialPosExtent = 45.0f;
constexpr float kInitialVelExtent = 20.0f;
constexpr float kInitialRadiusMin = 1.0f;
constexpr float kInitialRadiusMax = 5.0f;
constexpr float kInitialViewportScale = 10.0f;

struct Rock {
    sf::Vector2f pos {};
    sf::Vector2f vel {};
    float radius {0.0f};
};

std::ostream& operator<<(std::ostream& out, const Rock& r)
{
    return out << "Rock: pos: " << r.pos.x << "," << r.pos.y
               << " vel: " << r.vel.x << "," << r.vel.y
               << " radius: " << r.radius << "\n";
}

Rock createRandomRock()
{
    Rock rock;
    rock.pos.x = util::f_rand(-kInitialPosExtent, kInitialPosExtent);
    rock.pos.y = util::f_rand(-kInitialPosExtent, kInitialPosExtent);
    rock.vel.x = util::f_rand(-kInitialVelExtent, kInitialVelExtent);
    rock.vel.y = util::f_rand(-kInitialVelExtent, kInitialVelExtent);
    rock.radius = util::f_rand(kInitialRadiusMin, kInitialRadiusMax);
    return rock;
}

std::ostream& operator<<(std::ostream& out, const std::vector<Rock>& rocks)
{
    for(const auto& rock : rocks) {
        out << rock;
    }
    return out;
}

struct World {
    std::vector<Rock> rocks;  // abstract objects in world
    std::vector<sf::CircleShape> shapes;  // screen object cache
    float viewportScale {kInitialViewportScale};  // scale factor from game to screen size
    sf::RenderWindow* window;
};

sf::CircleShape createShape()
{
    sf::CircleShape shape;
    shape.setFillColor(sf::Color(255,0,0));
    return shape;
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
    auto winSize = world.window->getSize();
    sf::Vector2u winCenter = {winSize.x / 2, winSize.y / 2};
    for (size_t i = 0; i < world.shapes.size(); i++) {
        // using sf::View would enable doing this automatically for the view
        auto screenRadius = world.rocks[i].radius * world.viewportScale;
        world.shapes[i].setRadius(screenRadius);
        world.shapes[i].setOrigin(screenRadius, screenRadius);
        world.shapes[i].setPosition(
            (world.rocks[i].pos.x * world.viewportScale) + winCenter.x,
            winCenter.y - (world.rocks[i].pos.y * world.viewportScale));
    }
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

World createWorld(size_t numRocks, sf::RenderWindow* win)
{
    World world;
    world.window = win;
    world.rocks = std::vector<Rock>(numRocks);
    r::generate(world.rocks, createRandomRock);
    world.shapes = std::vector<sf::CircleShape>(numRocks);
    r::generate(world.shapes, createShape);
    updateShapeSystem(world); // set shapes positions based on current rocks
    return world;
}

void run()
{
    fmt::print("Gravity Simulator\n");
    sf::RenderWindow window(sf::VideoMode(1000, 1000), "Gravity");
    window.setFramerateLimit(60);

    World world {createWorld(kNumRocks, &window)};
    cout << world.rocks;

    sf::Clock clock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) { window.close(); }
        }
        float delta = clock.restart().asSeconds();
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
