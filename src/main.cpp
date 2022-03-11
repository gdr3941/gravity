//
// Gravity Simulator
//
// [ ] = Zoom In and Out,  Arrows = Move viewport, 0 = recenter
//

#include <fmt/core.h>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include "SFML/Window/Window.hpp"
#include <range/v3/all.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include "util.h"
#include "rock.hpp"
#include "world.hpp"

constexpr float kGravity = 6.67408e-2f;

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
// Run Loop
//

void run()
{
    fmt::print("Gravity Simulator\n");
    sf::RenderWindow window(sf::VideoMode(1500, 1500), "Gravity");
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
        updateGravitySystem(world, kGravity, delta);
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
