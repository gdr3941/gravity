#include <fmt/core.h>
#include <fmt/ranges.h>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <vector>
#include "SFML/System/Vector2.hpp"
#include "SFML/Window/Window.hpp"
#include "range/v3/algorithm/for_each.hpp"
#include <range/v3/algorithm.hpp>
#include <functional>
#include "util.h"

using namespace std::placeholders;
using std::cout;

namespace r = ranges;

// Initial Extents for new Rocks
constexpr float kInitialPosExtent = 10.0f;
constexpr float kInitialVelExtent = 2.0f;
constexpr float kInitialRadiusMix = 0.01f;
constexpr float kInitialRadiusMax = 2.0f; 

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

using Rocks = std::vector<Rock>;

std::ostream& operator<<(std::ostream& out, const Rocks& rocks)
{
    for(const auto& rock : rocks) {
        out << rock;
    }
    return out;
}

struct World {
    Rocks rocks;
};

Rocks createNewRocks(size_t num)
{
    Rocks rocks(num);
    for (auto& rock : rocks) {
        rock.pos.x = util::f_rand(-kInitialPosExtent, kInitialPosExtent);
        rock.pos.y = util::f_rand(-kInitialPosExtent, kInitialPosExtent);
        rock.vel.x = util::f_rand(-kInitialVelExtent, kInitialVelExtent);
        rock.vel.y = util::f_rand(-kInitialVelExtent, kInitialVelExtent);
        rock.radius = util::f_rand(kInitialRadiusMix, kInitialRadiusMax);
    }
    return rocks;
}

void updatePositions(Rocks& rocks, float timeStep)
{
    for (auto& rock : rocks) {
        rock.pos += rock.vel * timeStep;
    }
}

void run()
{
    fmt::print("Gravity Simulator\n");
    sf::RenderWindow window(sf::VideoMode(800, 600), "Gravity");
    window.setFramerateLimit(60);

    Rocks rocks {createNewRocks(10)};
    cout << rocks;

    updatePositions(rocks, 1.0f);
    fmt::print("---\n");
    cout << rocks;
    
    sf::Clock clock;
    while (window.isOpen()) {
        sf::Event event;
        if (event.type == sf::Event::Closed) { window.close(); }
        while (window.pollEvent(event)) {
        }
        float delta = clock.restart().asSeconds();
        if (delta > 1.0) std::cout << "Bad!!\n";
        // Recalc state here based on time delta
        window.clear();
        // draw new state here
        window.display();
    }
}

int main()
{
    run();
}
