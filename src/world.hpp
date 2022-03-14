#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include "rock.hpp"

//
// Simulation World that holds Entities
//

struct World {
    std::vector<Rock> rocks;  // abstract objects in world
    std::vector<sf::CircleShape> shapes;  // screen object cache
    sf::RenderWindow* window;
    float gravity {6.67408e-2f};
    float velColorExtent {20.0f};  // Vel for full red color

    explicit World(sf::RenderWindow* window): window {window} {};
};

void addRandomRocks(World& world, size_t numRocks, RockConfig rockConfig);

void addSatRocks(World& world);

void deleteAllRocks(World& world);

//
// Entity Systems
//

void updateCollisionSystem(World& world);

void updateGravitySystem(World& world, float timestep);

void updateRockPositionSystem(World& world, float timeStep);

void updateShapeSystem(World& world);

void testGrav();

//
// Visualization
//

sf::CircleShape shapeFor(const Rock& rock);

void draw(const World& world);
