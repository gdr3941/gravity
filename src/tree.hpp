#pragma once

#include <optional>
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
    sf::Vector2f center_mass;
    float total_mass {0.0f};
    Rock* element {nullptr};
    std::vector<TreeNode> children;

    TreeNode() : TreeNode(0.0,0.0,0.0,0.0) {}
    
    explicit TreeNode(float left, float right, float bottom, float top)
        : left {left}, right {right}, bottom {bottom}, top {top} {}

    inline float nodeWidth() { return right - left;}

    inline bool contains(sf::Vector2f pos) const {
        return (pos.x >= left && pos.x < right && pos.y >= bottom && pos.y < top);
    }

    bool hasChildren() { return children.size() > 0; }

    void createChildren() {
        float x_mid = left + (right-left) / 2.0f;
        float y_mid = bottom + (top-bottom) / 2.0f;
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

    TreeNode* insert(Rock* rock) {
        float rockMass = mass(*rock); 
        if (hasChildren()) {
            if (TreeNode* target = getChild(rock->pos); target) {
                TreeNode* finalNode = target->insert(rock);
                total_mass += rockMass;
                center_mass += (rockMass / total_mass) * (rock->pos - center_mass);
                return finalNode;
            } else {
                return nullptr;
            }
        } else if (!element) {
            element = rock;
            center_mass = rock->pos;
            total_mass = rockMass;
            return this;
        } else {
            createChildren();
            center_mass = {0.0f, 0.0f};
            total_mass = 0.0f;
            insert(element);
            element = nullptr;
            return insert(rock);
        }
        return nullptr;
    }
};
