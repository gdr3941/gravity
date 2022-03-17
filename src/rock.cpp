#include "util.h"
#include "rock.hpp"

namespace {

inline float dist2(const sf::Vector2f& a, const sf::Vector2f& b)
{
    // note: hypot function was slower
    return (a.x - b.x) * (a.x - b.x) +
                (a.y - b.y) * (a.y - b.y);
}

}  // namespace

Rock newRandomRock(const RockConfig& config)
{
    Rock rock;
    rock.pos.x = util::f_rand(-config.posExtent, config.posExtent);
    rock.pos.y = util::f_rand(-config.posExtent, config.posExtent);
    rock.vel.x = util::f_rand(-config.velExtent, config.velExtent);
    rock.vel.y = util::f_rand(-config.velExtent, config.velExtent);
    rock.radius = util::f_rand(config.radiusMin, config.radiusMax);
    return rock;
}

/// Returns true if in contact and still moving towards one another
bool isColliding(const Rock& a, const Rock& b)
{
    float currentDistance2 = dist2(a.pos, b.pos);
    if (currentDistance2 > ((a.radius + b.radius) * (a.radius + b.radius))) return false;
    // Now check if still moving closer to one another
    // by using a very small time increment
    sf::Vector2f future_a_pos = a.pos + (a.vel * 0.001f);
    sf::Vector2f future_b_pos = b.pos + (b.vel * 0.001f);
    float futureDistance2 = dist2(future_a_pos, future_b_pos);
    return (futureDistance2 < currentDistance2);
}

/// Update velocity vectors from a collision to bounce away
void updateForCollision(Rock& a, Rock& b)
{
    float a_mass = mass(a);
    float b_mass = mass(b);
    sf::Vector2f a_new_vel = (a.vel * (a_mass - b_mass) + (2.0f * b_mass * b.vel)) / (a_mass + b_mass);
    sf::Vector2f b_new_vel = (b.vel * (b_mass - a_mass) + (2.0f * a_mass * a.vel)) / (a_mass + b_mass);
    a.vel = a_new_vel;
    b.vel = b_new_vel;
}

std::ostream& operator<<(std::ostream& out, const Rock& r)
{
    return out << "Rock: pos: " << r.pos.x << "," << r.pos.y
               << " vel: " << r.vel.x << "," << r.vel.y
               << " radius: " << r.radius << "\n";
    
}
