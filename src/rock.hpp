//
// Rock - Abstract Entity in Simulation
//

#pragma once

#include <iostream>
#include <vector>
#include "SFML/System/Vector2.hpp"

constexpr float kInitialPosExtent = 45.0f;
constexpr float kInitialVelExtent = 10.0f;
constexpr float kInitialRadiusMin = 1.0f;
constexpr float kInitialRadiusMax = 6.0f;

struct Rock {
    sf::Vector2f pos {0,0};
    sf::Vector2f vel {0,0};
    float radius {0.0f};

    float mass() const;
};

Rock newRandomRock();

std::ostream& operator<<(std::ostream& out, const Rock& r);
