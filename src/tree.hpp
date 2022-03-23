#pragma once

#include <optional>
#include <vector>
#include <SFML/Graphics.hpp>
#include "rock.hpp"
#include <cassert>

template <class T>
class TreeStorage {
public:
    using Iterator = typename std::vector<T>::iterator;
    
    explicit TreeStorage(size_t capacity) {
        setCapacity(capacity);
    }

    // Old iterators may become invalid
    void setCapacity(size_t capacity) {
        if (elements.size() < capacity) {
            elements = std::vector<T>(capacity);
        }
        reset();
    }

    void reset() {
        iterator = elements.begin();
        iterator = begin(elements);
    }

    size_t count() {
        return (iterator - begin(elements));
    }

    Iterator push_back(T&& item) {
        if (iterator == end(elements)) {
         throw std::out_of_range("Exceeded TreeStorage size");   
        }
        *iterator = item;
        return iterator++; // increments after return
    }

private:
    std::vector<T> elements;
    Iterator iterator;
};

// Tree to hold rocks
// Can be empty, have 1 rock element, or have children
struct TreeNode {
    float left {0.0f}; // inclusive
    float right {0.0f}; // exclusive
    float bottom {0.0f}; // inclusive
    float top {0.0f}; // exclusive
    sf::Vector2f center_mass;
    float total_mass {0.0f};
    Rock* element {nullptr};
    std::optional<TreeStorage<TreeNode>::Iterator> childIter {};

    TreeNode() : TreeNode(0.0,0.0,0.0,0.0) {}
    
    explicit TreeNode(float left, float right, float bottom, float top)
        : left {left}, right {right}, bottom {bottom}, top {top} {}

    inline float nodeWidth() { return right - left;}

    inline bool contains(sf::Vector2f pos) const {
        return (pos.x >= left && pos.x < right && pos.y >= bottom && pos.y < top);
    }

    bool hasChildren() { return childIter.has_value(); }

    void createChildren(TreeStorage<TreeNode>& storage) {
        float x_mid = left + (right-left) / 2.0f;
        float y_mid = bottom + (top-bottom) / 2.0f;
        childIter = storage.push_back(TreeNode(x_mid, right, y_mid, top)); // upper right = 0
        storage.push_back(TreeNode(x_mid, right, bottom, y_mid)); // lower right = 1
        storage.push_back(TreeNode(left, x_mid, bottom, y_mid)); // lower left = 2
        storage.push_back(TreeNode(left, x_mid, y_mid, top)); // upper left = 3
    }

    TreeNode* getChild(sf::Vector2f pos, TreeStorage<TreeNode>& storage) {
        if (childIter) {
            for (auto it = childIter.value(); it < (childIter.value()+4); ++it) {
                if ((*it).contains(pos)) return &(*it);
            }
        }
        return nullptr;
    }

    TreeNode* insert(Rock* rock, TreeStorage<TreeNode>& storage) {
        std::cout << "insert rock: " << rock->pos.x << "," << rock->pos.y << "\n";
        float rockMass = mass(*rock); 
        if (hasChildren()) {
            if (TreeNode* target = getChild(rock->pos, storage); target) {
                std::cout << "Found child node that contains: " << &target << "\n";
                TreeNode* finalNode = target->insert(rock, storage);
                total_mass += rockMass;
                center_mass += (rockMass / total_mass) * (rock->pos - center_mass);
                return finalNode;
            } else {
                std::cout << "NULLPTR\n";
                return nullptr;
            }
        } else if (!element) {
            std::cout<<"added to element @ " << this << ": " << rock->pos.x << "," << rock->pos.y << "\n";;
            element = rock;
            center_mass = rock->pos;
            total_mass = rockMass;
            return this;
        } else {
            std::cout<<"creating children\n";
            if (rock->pos == element->pos) {
                std::cout<< "Warning: trying to add rock to tree at same point\n" <<
                    "Pos: " << rock->pos.x << "," << rock->pos.y << "\n";
            }
            createChildren(storage);
            center_mass = {0.0f, 0.0f};
            total_mass = 0.0f;
            insert(element, storage);
            element = nullptr;
            return insert(rock, storage);
        }
        return nullptr;
    }
};

// struct TreeNode {
//     float left {0.0f}; // inclusive
//     float right {0.0f}; // exclusive
//     float bottom {0.0f}; // inclusive
//     float top {0.0f}; // exclusive
//     sf::Vector2f center_mass;
//     float total_mass {0.0f};
//     Rock* element {nullptr};
//     std::vector<TreeNode> children;

//     TreeNode() : TreeNode(0.0,0.0,0.0,0.0) {}
    
//     explicit TreeNode(float left, float right, float bottom, float top)
//         : left {left}, right {right}, bottom {bottom}, top {top} {}

//     inline float nodeWidth() { return right - left;}

//     inline bool contains(sf::Vector2f pos) const {
//         return (pos.x >= left && pos.x < right && pos.y >= bottom && pos.y < top);
//     }

//     bool hasChildren() { return children.size() > 0; }

//     void createChildren() {
//         float x_mid = left + (right-left) / 2.0f;
//         float y_mid = bottom + (top-bottom) / 2.0f;
//         children.push_back(TreeNode(x_mid, right, y_mid, top)); // upper right = 0
//         children.push_back(TreeNode(x_mid, right, bottom, y_mid)); // lower right = 1
//         children.push_back(TreeNode(left, x_mid, bottom, y_mid)); // lower left = 2
//         children.push_back(TreeNode(left, x_mid, y_mid, top)); // upper left = 3
//     }

//     TreeNode* getChild(sf::Vector2f pos) {
//         for (auto& child : children) {
//             if (child.contains(pos)) return &child;
//         }
//         return nullptr;
//     }

//     TreeNode* insert(Rock* rock) {
//         float rockMass = mass(*rock); 
//         if (hasChildren()) {
//             if (TreeNode* target = getChild(rock->pos); target) {
//                 TreeNode* finalNode = target->insert(rock);
//                 total_mass += rockMass;
//                 center_mass += (rockMass / total_mass) * (rock->pos - center_mass);
//                 return finalNode;
//             } else {
//                 return nullptr;
//             }
//         } else if (!element) {
//             element = rock;
//             center_mass = rock->pos;
//             total_mass = rockMass;
//             return this;
//         } else {
//             createChildren();
//             center_mass = {0.0f, 0.0f};
//             total_mass = 0.0f;
//             insert(element);
//             element = nullptr;
//             return insert(rock);
//         }
//         return nullptr;
//     }
// };
