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

void print(const Rock& rock) {
    fmt::print("Rock: pos: {},{} vel: {},{} radius: {}\n",
               rock.pos.x, rock.pos.y,
               rock.vel.x, rock.vel.y,
               rock.radius);
}

using Rocks = std::vector<Rock>;

// struct World {
//     Rocks rocks;
// };

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

// Trying Alternate Functional vs Imperative approaches for position updates

void updatePosition(Rock& rock, float timeStep)
{
    rock.pos += rock.vel * timeStep;
}

auto updatePositionFor(float timeStep)
{
    return std::bind(updatePosition, _1, timeStep);

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
    r::for_each(rocks, print);
    // updatePositions(rocks, 1.0f);
    r::for_each(rocks, updatePositionFor(1.0f));
    fmt::print("---\n");
    r::for_each(rocks, print);
    
    
    sf::Clock clock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) { window.close(); }
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
