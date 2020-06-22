#include "decomposition_tree.hpp"

#ifdef DEBUG_DECOMP_TREE
#include <iostream>
#endif /* DEBUG_DECOMP_TREE */

DecompositionNode::DecompositionNode(nid_t nid_, decomp_node_t type_, 
    bool is_reverse_)
    : nid(nid_)
    , type(type_)
    , is_reverse(is_reverse_)
{}

void DecompositionNode::reverse() {
    // If this is a derived node with a chain, perform a linked-list reverse.
    if (type == decomp_node_t::Chain) {
        DecompositionNode* previous = nullptr;
        DecompositionNode* conductor = child_head;
        DecompositionNode* temp; 
        while (conductor != nullptr) {
            temp = conductor->sibling;     // Keep track of the next node.
            conductor->sibling = previous; // Reverse the conductor's direction.
            previous = conductor;          // Set the new previous to conductor.
            conductor = temp;              // Shift conductor forward. 
        }
        // Flip head and tail pointers.
        temp = child_head;
        child_head = child_tail;
        child_tail = temp;
    }

    // If this is derived node, the children must be reversed too.
    if (type == decomp_node_t::Chain || type == decomp_node_t::Split) {
        for (auto& child : children) {
            child->reverse();
        }
    }

    // Flip the orientation status of this decomposition node.
    is_reverse = !is_reverse;
}

void DecompositionNode::add_child(DecompositionNode* child) {
    // Add child to unordered list of children.
    children.push_back(child);

    // Set this as a parent of the child.
    child->parents.push_back(this);
}

void DecompositionNode::push_front(DecompositionNode* child) {
    // Add child to the unordered list of children.
    add_child(child);

    // If there aren't any children, set new node as head and tail.
    if (child_head == nullptr) {
        child_head = child;
        child_tail = child;
        return;
    }
    
    // Set new node as head.
    child->sibling = child_head;
    child_head = child;
}

void DecompositionNode::push_back(DecompositionNode* child) {
    // Add child to the unordered list of children.
    add_child(child);

    // If there aren't any chlidren, set new node as head and tail.
    if (child_tail == nullptr) {
        child_head = child;
        child_tail = child;
        return;
    }

    // Set new node as tail.
    child_tail->sibling = child;
    child_tail = child;
}

DecompositionNode* create_chain_node(nid_t nid, DecompositionNode* first_node,
    DecompositionNode* second_node
) {
    // Get new derived chain node.
    DecompositionNode* new_node = new DecompositionNode(nid, decomp_node_t::Chain);

    // If the first node is a chain node, it could be used to create a longer chain.
    if (first_node->type == decomp_node_t::Chain) {
        // Copy children chain over to the new node.
        DecompositionNode* conductor = first_node->child_head;
        DecompositionNode* next;
        while (conductor != nullptr) {
            next = conductor->sibling;
            new_node->push_back(conductor);
            conductor = next;
        }
        // Free the original parent node.
        delete first_node;
    // Otherwise just put the split/source node as the first child.
    } else {
        new_node->push_back(first_node);
    }

    // If the second node is a chain node ...
    if (second_node->type == decomp_node_t::Chain) {
        // Copy children over to the new node
        DecompositionNode* conductor = second_node->child_head;
        DecompositionNode* next;
        while (conductor != nullptr) {
            next = conductor->sibling;
            new_node->push_back(conductor);
            conductor = next;
        }
        // Free the original parent node.
        delete second_node;
    // Otherwise just put the split/source node as the second child.
    } else {
        new_node->push_back(second_node);
    }
    
    return new_node;
}

void free_tree(DecompositionNode *node) {
    switch (node->type) {
        case Source: {
            delete node;
            break;
        }
        case Chain: {
            DecompositionNode* conductor = node->child_head;
            DecompositionNode* next;
            while (conductor != nullptr) {
                next = conductor->sibling;
                free_tree(conductor);
                conductor = next;
            }
            delete node;
            break;
        }
        case Split: {
            for (auto& child : node->children) {
                delete child;
            }
            break;
        }
    }
}

#ifdef DEBUG_DECOMP_TREE
inline void print_depth(int depth) {
    for (int i = 0; i < depth; i++) {
        std::cout << "| ";
    }
}

void print_tree(DecompositionNode* node, int depth) {
    print_depth(depth);
    switch (node->type) {
        case Source: {
            std::cout << "Source Node: " << node->nid << (node->is_reverse ? "r" : "") << std::endl;
            break;
        }
        case Chain: {
            std::cout << "Chain Node: " << node->nid << (node->is_reverse ? "r" : "") << std::endl;
            DecompositionNode* conductor = node->child_head;
            while (conductor != nullptr) {
                print_tree(conductor, depth + 1);
                conductor = conductor->sibling;
            }
            break;
        }
        case Split: {
            std::cout << "Split Node: " << node->nid << (node->is_reverse ? "r" : "") << std::endl;
            for (auto& child : node->children) {
                print_tree(child, depth + 1);
            }
            break;
        }
    }
}
#endif /* DEBUG_DECOMP_TREE */
