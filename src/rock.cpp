#include "util.h"
#include "rock.h"

Rock newRandomRock()
{
    Rock rock;
    rock.pos.x = util::f_rand(-kInitialPosExtent, kInitialPosExtent);
    rock.pos.y = util::f_rand(-kInitialPosExtent, kInitialPosExtent);
    rock.vel.x = util::f_rand(-kInitialVelExtent, kInitialVelExtent);
    rock.vel.y = util::f_rand(-kInitialVelExtent, kInitialVelExtent);
    rock.radius = util::f_rand(kInitialRadiusMin, kInitialRadiusMax);
    return rock;
}

float mass(const Rock& rock)
{
    return rock.radius * rock.radius * rock.radius;
}

float distanceBetween(const sf::Vector2f& a, const sf::Vector2f& b)
{
    return sqrt((a.x - b.x) * (a.x - b.x) +
                (a.y - b.y) * (a.y - b.y));
}

/// Returns true if in contact and still moving towards one another
bool isColliding(const Rock& a, const Rock& b)
{
    float currentDistance = distanceBetween(a.pos, b.pos);
    if (currentDistance > (a.radius + b.radius)) return false;
    // Now check if still moving closer to one another
    // by using a very small time increment
    sf::Vector2f future_a_pos = a.pos + (a.vel * 0.001f);
    sf::Vector2f future_b_pos = b.pos + (b.vel * 0.001f);
    float futureDistance = distanceBetween(future_a_pos, future_b_pos);
    return (futureDistance < currentDistance);
}


/// Update velocity vectors from a collision to bounce away
void updateForCollision(Rock& a, Rock& b)
{
    float a_mass = a.radius * a.radius;
    float b_mass = b.radius * b.radius;
    sf::Vector2f a_new_vel = (a.vel * (a_mass - b_mass) + (2.0f * b_mass * b.vel)) / (a_mass + b_mass);
    sf::Vector2f b_new_vel = (b.vel * (b_mass - a_mass) + (2.0f * a_mass * a.vel)) / (a_mass + b_mass);
    a.vel = a_new_vel;
    b.vel = b_new_vel;
}

std::pair<sf::Vector2f, sf::Vector2f>
gravityAccelComponents(const Rock& a, const Rock& b, const float gConst)
{
    float distance2 = (a.pos.x - b.pos.x) * (a.pos.x - b.pos.x) +
        (a.pos.y - b.pos.y) * (a.pos.y - b.pos.y);
    float acc = gConst / distance2;
    float t_acc_a = acc * mass(b);  // total acceleration on a
    float t_acc_b = acc * mass(a);  // total acceleration on b
    // Now break down totals into x & y components
    sf::Vector2f a_vec = b.pos - a.pos;
    sf::Vector2f b_vec = a.pos - b.pos;
    float a_radians = atan2(a_vec.y, a_vec.x);
    float b_radians = atan2(b_vec.y, b_vec.x);
    sf::Vector2f acc_a {static_cast<float>(cos(a_radians)*t_acc_a), static_cast<float>(sin(a_radians)*t_acc_a)};
    sf::Vector2f acc_b {static_cast<float>(cos(b_radians)*t_acc_b), static_cast<float>(sin(b_radians)*t_acc_b)};
    return {acc_a, acc_b};
}

std::ostream& operator<<(std::ostream& out, const Rock& r)
{
    return out << "Rock: pos: " << r.pos.x << "," << r.pos.y
               << " vel: " << r.vel.x << "," << r.vel.y
               << " radius: " << r.radius << "\n";
    
}
