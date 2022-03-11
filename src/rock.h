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
};

Rock newRandomRock();

float mass(const Rock& rock);

float distanceBetween(const sf::Vector2f& a, const sf::Vector2f& b);

/// Returns true if in contact and still moving towards one another
bool isColliding(const Rock& a, const Rock& b);


/// Update velocity vectors from a collision to bounce away
void updateForCollision(Rock& a, Rock& b);

std::pair<sf::Vector2f, sf::Vector2f>
gravityAccelComponents(const Rock& a, const Rock& b, const float gConst);

std::ostream& operator<<(std::ostream& out, const Rock& r);
