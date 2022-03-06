#include <fmt/core.h>
#include <fmt/ranges.h>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System.hpp>
#include <iostream>

void run()
{
    fmt::print("Gravity Simulator\n");
    sf::RenderWindow window(sf::VideoMode(800, 600), "Gravity");
    window.setFramerateLimit(60);

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
