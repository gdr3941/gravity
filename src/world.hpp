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
    float worldExtent;  // Max extent of world +/-
    TreeStorage<TreeNode> treeStorage;
    TreeNode rootTree;

    explicit World(sf::RenderWindow* window, float worldExtent, size_t rockCapacity)
        : window {window},
          worldExtent {worldExtent},
          treeStorage {TreeStorage<TreeNode>(rockCapacity * 4)},
        rootTree {TreeNode(-worldExtent, worldExtent, -worldExtent, worldExtent)} {
        rocks.reserve(rockCapacity);
        shapes.reserve(rockCapacity);
    };
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
