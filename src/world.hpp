#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#import "rock.hpp"

//
// Simulation World that holds Entities
//

struct World {
    std::vector<Rock> rocks;  // abstract objects in world
    std::vector<sf::CircleShape> shapes;  // screen object cache
    sf::RenderWindow* window;
    float gravity {6.67408e-2f};
};

World createRandomWorld(size_t numRocks, RockConfig config, sf::RenderWindow* win);

World createSatWorld(sf::RenderWindow* win);

//
// Entity Systems
//

void updateCollisionSystem(World& world);

void updateGravitySystem(World& world, float timestep);

void updateRockPositionSystem(World& world, float timeStep);

void updateShapeSystem(World& world);

//
// Visualization
//

sf::CircleShape shapeFor(const Rock& rock);

void draw(const World& world);
