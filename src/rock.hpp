#pragma once

#include <iostream>
#include "SFML/System/Vector2.hpp"

//
// Rock - Abstract Entity in Simulation
//

struct Rock {
    sf::Vector2f pos {0,0};
    sf::Vector2f vel {0,0};
    float radius {0.0f};
    float mass {0.0f};
};

struct RockConfig {
    float posExtent {45.0f};
    float velExtent {10.0f};
    float radiusMin {1.0f};
    float radiusMax {6.0f};
};

Rock newRandomRock(const RockConfig& config);

bool isColliding(const Rock& a, const Rock& b);

void updateForCollision(Rock& a, Rock& b);

std::ostream& operator<<(std::ostream& out, const Rock& r);
