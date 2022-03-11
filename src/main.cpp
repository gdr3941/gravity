//
// Gravity Simulator
//
// [ ] = Zoom In and Out,  Arrows = Move viewport, 0 = recenter
//

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
#include <imgui.h>
#include <imgui-SFML.h>
#include "util.h"
#include "rock.h"

constexpr float kGravity = 6.67408e-2f;
//
// Simulation World that holds Entities
//

struct World {
    std::vector<Rock> rocks;  // abstract objects in world
    std::vector<sf::CircleShape> shapes;  // screen object cache
    sf::RenderWindow* window;
};

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
    util::for_distinct_pairs(world.rocks, [timestep](Rock& a, Rock& b){
        auto [a_acc, b_acc] = gravityAccelComponents(a, b, kGravity);
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

//
// Event Handling
//

void scaleView(World& world, float scaleFactor)
{
    sf::View view = world.window->getView();
    view.zoom(scaleFactor);
    world.window->setView(view);
}

void moveView(World& world, sf::Vector2f move_percent)
{
    sf::View view = world.window->getView();
    sf::Vector2f size = view.getSize();
    sf::Vector2f center = view.getCenter();
    sf::Vector2f adjust_center = {size.x * move_percent.x, size.y * move_percent.y};
    view.setCenter(center += adjust_center);
    world.window->setView(view);
}

void centerView(World& world)
{
    sf::View view = world.window->getView();
    view.setCenter({0.0f,0.0f});
    world.window->setView(view);
}

void handleResize(World& world, unsigned int width, unsigned int height)
{
    sf::View view = world.window->getView();
    view.setSize(width, height);
    world.window->setView(view);
}

void handleEvents(World& world)
{
    sf::Event event;
    while (world.window->pollEvent(event)) {
        if (event.type == sf::Event::Closed) {world.window->close();}
        if (event.type == sf::Event::Resized) {
            handleResize(world, event.size.width, event.size.height);
        }
        if (event.type == sf::Event::KeyPressed) {
            switch (event.key.code) {
            case sf::Keyboard::Num0:
                centerView(world);
                break;
            case sf::Keyboard::Up:
                moveView(world, {0,-0.10});
                break;
            case sf::Keyboard::Down:
                moveView(world, {0,0.10});
                break;
            case sf::Keyboard::Left:
                moveView(world, {-0.10,0});
                break;
            case sf::Keyboard::Right:
                moveView(world, {0.10,0});
                break;
            case sf::Keyboard::RBracket:
                scaleView(world, 0.8);
                break;
            case sf::Keyboard::LBracket:
                scaleView(world, 1.2);
                break;
            default:
                break;
            }
        }
        ImGui::SFML::ProcessEvent(event);
    }
}

//
// Initial Setup
//

World createRandomWorld(size_t numRocks, sf::RenderWindow* win)
{
    World world;
    world.window = win;
    world.rocks.reserve(numRocks);
    world.shapes.reserve(numRocks);
    for (size_t i = 0; i<numRocks; i++) {
        Rock rock = newRandomRock();
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
// Run Loop
//

void run()
{
    fmt::print("Gravity Simulator\n");
    sf::RenderWindow window(sf::VideoMode(1000, 1000), "Gravity");
    window.setFramerateLimit(60);
    sf::View view (sf::Vector2f(0,0), sf::Vector2f(200,200));
    window.setView(view);
    ImGui::SFML::Init(window);

    World world = createRandomWorld(100, &window);
    // World world = createSatWorld(&window);

    sf::Clock clock;
    sf::Clock im_clock;
    char windowTitle[255] = "Imgui + SFML";
    
    while (window.isOpen()) {
        handleEvents(world);
        float delta = clock.restart().asSeconds();
        updateCollisionSystem(world);
        updateGravitySystem(world, delta);
        updateRockPositionSystem(world, delta);
        updateShapeSystem(world);
        window.clear();
        draw(world);

        ImGui::SFML::Update(window, im_clock.restart());
        ImGui::Begin("Sample window"); // begin window
        ImGui::InputText("Window title", windowTitle, 255);
        ImGui::End(); // end window
        ImGui::SFML::Render(window);

        window.display();
    }
}

int main()
{
    run();
}
