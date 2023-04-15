//
// Gravity Simulator
//
// [ ] = Zoom In and Out,  Arrows = Move viewport, = = recenter
//

#include <fmt/core.h>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include "SFML/Window/Window.hpp"
#include <imgui.h>
#include <imgui-SFML.h>
#include "util.h"
#include "rock.hpp"
#include "world.hpp"
#include "tree.hpp"

//
// ImGui 
//

void loadFonts()
{
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->Clear();
    #ifdef __linux__
    io.Fonts->AddFontFromFileTTF("resources/arial.ttf", 14.0f);
    #else
    io.Fonts->AddFontFromFileTTF("resources/arial.ttf", 26.0f);
    #endif
    bool success = ImGui::SFML::UpdateFontTexture();
}

void drawUI(World& world, sf::Time delta)
{
    static int addRocks {100};
    static RockConfig rockConfig;
    ImGui::SFML::Update(*world.window, delta);
    ImGui::Begin("Settings");  // begin window
    ImGui::Text("Active Rocks: %lu", world.rocks.size());
    ImGui::Text("FPS: %d", static_cast<int>(1.0/delta.asSeconds()));
    ImGui::InputFloat("Gravity", &world.gravity);
    ImGui::Checkbox("Ignore Short Distance Grav", &world.ignoreShortDistGrav);
    ImGui::DragFloat("VelColorMax", &world.velColorExtent, 0.1f, 1.0f, 30.0f);
    ImGui::InputFloat("RadiusMin", &rockConfig.radiusMin);
    ImGui::InputFloat("RadiusMax", &rockConfig.radiusMax);
    ImGui::InputFloat("PositionMax", &rockConfig.posExtent);
    ImGui::InputFloat("VelocityMax", &rockConfig.velExtent);
    ImGui::InputInt("RocksToAdd", &addRocks);
    if (ImGui::Button("Add Rocks")) {
        addRandomRocks(world, addRocks, rockConfig);
    }
    ImGui::SameLine();
    if (ImGui::Button("Add 1000")) {
        addRandomRocks(world, 1000, rockConfig);
    }
    ImGui::SameLine();
    if (ImGui::Button("Restart")) {
        deleteAllRocks(world);
        addRandomRocks(world, addRocks, rockConfig);
    }
    ImGui::End();  // end window
    ImGui::SFML::Render(*world.window);
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
            case sf::Keyboard::Equal:
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
            case sf::Keyboard::Escape:
                world.window->close();
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

void testTree()
{
    World world(nullptr);
    addRandomRocks(world, 10000, RockConfig{});
    {
        util::Timer timer;
        for (auto i = 0; i < 100; ++i) {
            TreeNode t(100.0f);
            for (auto& rock : world.rocks) {
                t.insert(&rock);
            }
        }
    }

    // 165ms on linux after adding reservce back in for children
    // 470-500 ms on mac for 10k rocks 100x
    // 382ms after removing mass calc on mac
    // 330ms after removing inline returns in tree insert
}

void run()
{
    sf::RenderWindow window(sf::VideoMode(1500, 1500), "Gravity");
    window.setFramerateLimit(60);
    sf::View view (sf::Vector2f(0,0), sf::Vector2f(200,200));
    window.setView(view);
    bool success = ImGui::SFML::Init(window);
    loadFonts();

    World world(&window);
    addRandomRocks(world, 100, RockConfig {});
    // addSatRocks(world);

    sf::Clock clock;
    while (window.isOpen()) {
        handleEvents(world);
        sf::Time delta = clock.restart();
        updateTreeSystem(world);
        updateGravitySystemTree(world, delta.asSeconds());
        // updateCollisionSystemTree(world);
        updateCollisionSystemPar(world); // This is much faster
        updateRockPositionSystem(world, delta.asSeconds());
        updateShapeSystem(world);
        window.clear();
        draw(world);
        drawUI(world, delta);
        window.display();
    }
}

int main()
{
    // testTree();
    run();
}
