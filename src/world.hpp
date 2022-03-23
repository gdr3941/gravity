#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include "rock.hpp"
#include "tree.hpp"

//
// Simulation World that holds Entities and Config
//

struct World {
    std::vector<Rock> rocks;  // abstract objects in world
    std::vector<sf::CircleShape> shapes;  // screen object cache
    sf::RenderWindow* window;
    float gravity {6.67408e-2f};
    bool ignoreShortDistGrav {true};
    float velColorExtent {20.0f};  // Vel for full red color
    float worldExtent {1000.0f};  // Max extent of world +/-
    TreeNode rootTree;

    explicit World(sf::RenderWindow* window)
        : window {window}, rootTree {TreeNode(worldExtent)} {};
};

void addRandomRocks(World& world, size_t numRocks, RockConfig rockConfig);

void addSatRocks(World& world);

void deleteAllRocks(World& world);

//
// Entity Systems
//

void updateTreeSystem(World& world);

void updateCollisionSystem(World& world);

void updateGravitySystem(World& world, float timestep);

void updateGravitySystemPar(World& world, float timestep);

void updateRockPositionSystem(World& world, float timeStep);

void updateShapeSystem(World& world);

//
// Visualization
//

sf::CircleShape shapeFor(const Rock& rock);

void draw(const World& world);
