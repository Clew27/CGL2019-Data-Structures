#include "strongly_connected_components.hpp"

//#define debug
#include <unordered_map>

using namespace std;

// recursion-free version of Tarjan's strongly connected components algorithm
// https://en.wikipedia.org/wiki/Tarjan%27s_strongly_connected_components_algorithm
// Generalized to bidirected graphs as described (confusingly) in
// "Decomposition of a bidirected graph into strongly connected components and
// its signed poset structure", by Kazutoshi Ando, Satoru Fujishige, and Toshio
// Nemoto. http://www.sciencedirect.com/science/article/pii/0166218X95000683

// The best way to think about that paper is that the edges are vectors in a
// vector space with number of dimensions equal to the number of nodes in the
// graph, and an edge attaching to the end a node is the positive unit vector in
// its dimension, and an edge attaching to the start of node is the negative
// unit vector in its dimension.

// The basic idea is that you just consider the orientations as different nodes,
// and the edges as existing between both pairs of orientations they connect,
// and do connected components on that graph. Since we don't care about
// "consistent" or "inconsistent" strongly connected components, we just put a
// node in a component if either orientation is in it. But bear in mind that
// both orientations of a node might not actually be in the same strongly
// connected component in a bidirected graph, so now the components may overlap.
vector<unordered_set<nid_t>> strongly_connected_components(const HandleGraph* handle_graph) {
    
    // What node visit step are we on?
    int64_t index = 0;
    // What's the search root from which a node was reached?
    unordered_map<handle_t, handle_t> roots;
    // At what index step was each node discovered?
    unordered_map<handle_t, int64_t> discover_idx;
    // We need our own copy of the DFS stack
    vector<handle_t> stack;
    // And our own set of nodes already on the stack
    unordered_set<handle_t> on_stack;
    // What components did we find? Because of the way strongly connected
    // components generalizes, both orientations of a node always end up in the
    // same component.
    vector<unordered_set<nid_t>> components;
    
    // A single node ID from each component we've already added, which we use
    // to deduplicate the results
    // TODO: why do we produce duplicate components in the first place?
    unordered_set<nid_t> already_used;
    
    dfs(*handle_graph,
    [&](const handle_t& trav) {
        // When a NodeTraversal is first visited

        // It is its own root
        roots[trav] = trav;
        // We discovered it at this step
        discover_idx[trav] = index++;
        // And it's on the stack
        stack.push_back(trav);
        on_stack.insert(trav);
    },
    [&](const handle_t& trav) {
        // When a NodeTraversal is done being recursed into

        // Go through all the NodeTraversals reachable reading onwards from this traversal.
        handle_graph->follow_edges(trav, false, [&](const handle_t& next) {

            if (on_stack.count(next)) {
                // If any of those NodeTraversals are on the stack already

                auto& node_root = roots[trav];
                auto& next_root = roots[next];
                // Adopt the root of the NodeTraversal that was discovered first.
                roots[trav] = discover_idx[node_root] < discover_idx[next_root] ?
                node_root : next_root;
            }
            return true;
        });
        
        if (roots[trav] == trav) {
            // If we didn't find a better root

            handle_t other;
            bool is_duplicate = false;
            unordered_set<nid_t> component;
            do
            {
                // Grab everything that was put on the DFS stack below us
                // and put it in our component.
                other = stack.back();
                stack.pop_back();
                on_stack.erase(other);
                
                nid_t node_id = handle_graph->get_id(other);
                
                if (already_used.count(node_id)) {
                    is_duplicate = true;
                    break;
                }
                
                component.insert(node_id);
            } while (other != trav);
            
            if (!is_duplicate) {
                // use one node ID to mark this component as finished
                already_used.insert(*component.begin());
                // add it to the return valuse
                components.emplace_back(move(component));
            }
        }
    },
    vector<handle_t>(), unordered_set<handle_t>());
    
    return components;
}
