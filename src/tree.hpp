#pragma once

#include <vector>
#include <SFML/Graphics.hpp>
#include "rock.hpp"

/// Tree to hold rocks
/// Can be empty, have 1 rock element, or have children
struct TreeNode {
    float left {0.0f}; // inclusive
    float right {0.0f}; // exclusive
    float bottom {0.0f}; // inclusive
    float top {0.0f}; // exclusive
    sf::Vector2f center {0.0f, 0.0f};
    sf::Vector2f center_mass;
    float total_mass {0.0f};
    float max_radius {0.0f}; // largest radius of element / children
    Rock* element {nullptr};
    std::vector<TreeNode> children;

    explicit TreeNode(float extent) : TreeNode(-extent, extent, -extent, extent) {}

    explicit TreeNode(float left, float right, float bottom, float top)
        : left {left}, right {right}, bottom {bottom}, top {top}, 
        center {left + (right - left) / 2.0f, bottom + (right - left) / 2.0f} {}

    inline float nodeWidth() const { return right - left;}

    inline bool contains(sf::Vector2f pos) const {
        return (pos.x >= left && pos.x < right && pos.y >= bottom && pos.y < top);
    }

    bool hasChildren() const { return children.size() > 0; }

    void createChildren() {
        float x_mid = left + (right-left) / 2.0f;
        float y_mid = bottom + (top-bottom) / 2.0f;
        children.reserve(4);
        children.push_back(TreeNode(x_mid, right, y_mid, top)); // upper right = 0
        children.push_back(TreeNode(x_mid, right, bottom, y_mid)); // lower right = 1
        children.push_back(TreeNode(left, x_mid, bottom, y_mid)); // lower left = 2
        children.push_back(TreeNode(left, x_mid, y_mid, top)); // upper left = 3
    }

    TreeNode* getChild(sf::Vector2f pos) {
        for (auto& child : children) {
            if (child.contains(pos)) return &child;
        }
        return nullptr;
    }

    void insert(Rock* rock) {
        if (hasChildren()) {
            if (TreeNode* target = getChild(rock->pos); target) {
                target->insert(rock);
                total_mass += rock->mass;
                center_mass += (rock->mass / total_mass) * (rock->pos - center_mass);
                if (rock->radius > max_radius) max_radius = rock->radius;
            } 
        } else if (!element) {
            element = rock;
            center_mass = rock->pos;
            total_mass = rock->mass;
            max_radius = rock->radius;
        } else {
            if (rock->pos == element->pos) {
                std::cout<< "Warning: trying to add rock to tree at same point\n" <<
                    "Pos: " << rock->pos.x << "," << rock->pos.y << "\n";
                abort();
            }
            createChildren();
            center_mass = {0.0f, 0.0f};
            total_mass = 0.0f;
            max_radius = 0.0f;
            insert(element);
            element = nullptr;
            insert(rock);
        }
    }
};
