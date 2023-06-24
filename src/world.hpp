#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include "rock.hpp"
#include "tree.hpp"
#include <concurrentqueue/concurrentqueue.h>

//
// Simulation World that holds Entities and Config
//

using CollidingPair = std::pair<Rock*,Rock*>;
using Queue = moodycamel::ConcurrentQueue<CollidingPair>;

struct World {
    std::vector<Rock> rocks;  // abstract objects in world
    std::vector<sf::CircleShape> shapes;  // screen object cache
    sf::RenderWindow* window;
    float gravity {6.67408e-2f};
    bool ignoreShortDistGrav {true};
    float theta {0.5f}; // ratio of node size to dist to use node totals
    float velColorExtent {20.0f};  // Vel for full red color
    float worldExtent {1000.0f};  // Max extent of world +/-
    TreeNode rootTree;
    Queue collisions;

    explicit World(sf::RenderWindow* window)
        : window {window}, rootTree {TreeNode(worldExtent)}, collisions {10000} {};
};

void addRock(World& world, Rock rock);

void addRandomRocks(World& world, size_t numRocks, RockConfig rockConfig);

void addSatRocks(World& world);

void deleteAllRocks(World& world);

//
// Entity Systems
//

void updateTreeSystem(World& world);

void updateCollisionSystemPar(World& world);

void updateGravitySystemTree(World& world, float timestep);

void updateRockPositionSystem(World& world, float timeStep);

void updateShapeSystem(World& world);

//
// Visualization
//

sf::CircleShape shapeFor(const Rock& rock);

void draw(const World& world);
