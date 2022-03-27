#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <cmath>
#include <doctest/doctest.h>
#include "../src/tree.hpp"

TEST_CASE("vectors can be sized and resized") {
    std::vector<int> v(5);

    REQUIRE(v.size() == 5);
    REQUIRE(v.capacity() >= 5);

    SUBCASE("adding to the vector increases it's size") {
        v.push_back(1);

        CHECK(v.size() == 6);
        CHECK(v.capacity() >= 6);
    }
    SUBCASE("reserving increases just the capacity") {
        v.reserve(6);

        CHECK(v.size() == 5);
        CHECK(v.capacity() >= 6);
    }
}

TEST_CASE("Tree Tests") {
    TreeNode t(0.0, 1.0, 0.0, 1.0);
    REQUIRE(!t.element);
    REQUIRE(!t.hasChildren());
    Rock r = Rock {.pos = {0.1, 0.1}, .radius = 2.0f};
    t.insert(&r);
    REQUIRE(t.element == &r);
    REQUIRE(!t.hasChildren());
    REQUIRE(t.total_mass == mass(r));
    REQUIRE(t.center_mass == r.pos);
    REQUIRE(t.max_radius == r.radius);
    Rock r2 = Rock {.pos = {0.6, 0.6}, .radius = 4.0f};
    t.insert(&r2);
    REQUIRE(t.element == nullptr);
    REQUIRE(t.hasChildren());
    TreeNode* r_node = t.getChild(r.pos);
    TreeNode* r2_node = t.getChild(r2.pos);
    REQUIRE(r_node != r2_node);
    REQUIRE(r_node != &t);
    REQUIRE(r_node->element == &r);
    REQUIRE(t.max_radius == 4.0f);
    REQUIRE(!r_node->hasChildren());
    REQUIRE(r2_node->element == &r2);
    REQUIRE(!r2_node->hasChildren());
    REQUIRE(r2_node->total_mass == mass(r2));
    REQUIRE(r2_node->center_mass == r2.pos);
    TreeNode* r3_node = t.getChild({0.2, 0.6});
    REQUIRE(r3_node->element == nullptr);
    REQUIRE(!r3_node->hasChildren());
    float total_mass = mass(r) + mass(r2);
    REQUIRE(fabs(t.total_mass - total_mass) < 0.001);
    sf::Vector2f com = (mass(r)/total_mass) * r.pos + (mass(r2)/total_mass) * r2.pos;
    REQUIRE(fabs(t.center_mass.x - com.x) < 0.001);
    // Test when rocks are directly on top of each other
    // Rock r4 {.pos = {0.1, 0.1}, .radius = 2.0f};
    // auto r4_node = t.insert(&r4);
    // REQUIRE(r4_node->element == &r4);
}
