#ifndef VG_ALGORITHMS_BUNDLE_TREE_HPP_INCLUDED
#define VG_ALGORITHMS_BUNDLE_TREE_HPP_INCLUDED

#define DEBUG_DECOMP_TREE

#include "handle.hpp"
#include <utility>
#include <vector>
#include <unordered_map>

#ifdef DEBUG_DECOMP_TREE
#include <unordered_set>
#endif /* DEBUG_DECOMP_TREE */

enum decomp_node_t {
    Source,  // This node represents a node in the original graph.
    Epsilon, // This is a special subset of the Source node that represents an
             // edge (created via rule 1).
    Chain,   // This type of derived node represents a chain of children 
    Split    // This type of derived node is split into multiple children that 
             // are independent of each other.
};

// A POD that represents a node in the decompositon tree.
struct DecompositionNode {
    // Id of the source/derived node this decomposition node represents.
    nid_t nid; 
    // Direction this decomposition node is oriented in.
    // This is important to determine if the blue edges need to be reversed
    // (only relevant for intermediate nodes).
    bool is_reverse; 
    // The type of decompostion node. 
    decomp_node_t type; 
    // Keeps track of self-cycles/inversions.
    // Self-inversion is a pair of relative left and relative right.
    // Relative left = g->get_handle(nid, !is_reverse)
    bool scycle = false; // Self-cycle.
    bool sinv[2] = {false, false}; // Self inversions (left, right).

    // Decomposition node's relationship with other nodes.
    // The parent of this node if it isn't a R1 type node.
    DecompositionNode* parent = nullptr;

    // A blue edge. A pointer to the sibling to the "right".
    DecompositionNode* sibling = nullptr; 
    // Red edges. An unordered list of children nodes in the tree.
    std::vector<DecompositionNode*> children; 

    // Head and tail of children chain.
    DecompositionNode* child_head = nullptr;
    DecompositionNode* child_tail = nullptr;

    // Node id, Node type, is_reverse (optional)
    DecompositionNode(nid_t nid_, decomp_node_t type_, bool is_reverse_ = false);
    
    // Reverses the chain connecting the children and also the children themselves.
    void reverse();

    // Add unordered child (assumes this will only be run once per child per parent).
    void add_child(DecompositionNode* child);

    // Pushes a child node to the beginning of the chain.
    void push_front(DecompositionNode* child);

    // Pushes a child node to the end of the chain.
    void push_back(DecompositionNode* child); 
};

// Assigns the two ordered child nodes to a parent chain node.
// These child nodes are assumed to be the root node of their respective "trees".
// If a child node is also a chain node, its children will be moved over to the
// new node and the original parent will be freed.
// TODO: More elegant version that takes variadic arguments.
DecompositionNode* create_chain_node(nid_t nid, DecompositionNode* first_node,
    DecompositionNode* second_node);

// Finds the common ancestor between two decomposition nodes. If nothing is found,
// a nullptr is returned.
// TODO: Change naive algorithm to a RMQ solver. 
// With a sparse table, time complexity of a query will be O(1) and
// mem complexity will be O(nlogn).
DecompositionNode* find_lca(DecompositionNode* n1, DecompositionNode* n2);

// Frees decomposition tree given root.
void free_tree(DecompositionNode* node);

#ifdef DEBUG_DECOMP_TREE
class DecompositionTreePrinter {
private:
    // Prints decomposition tree with "| " indicating depth.
    void print_tree(DecompositionNode* node, int depth);
public:
    // Prints information about a node.
    void print_node(DecompositionNode* node);
    // Prints decomposition tree with "| " indicating depth.
    void print_tree(DecompositionNode* node);
};

#endif /* DEBUG_DECOMP_TREE */
#endif /* VG_ALGORITHMS_BUNDLE_TREE_HPP_INCLUDED */
