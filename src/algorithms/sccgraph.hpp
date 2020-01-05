#ifndef VG_SCCGRAPH_HPP_INCLUDED
#define VG_SCCGRAPH_HPP_INCLUDED


#define debug
#ifdef debug
#include <iostream>
#endif /* debug */

#include <unordered_map>
#include <vector>
#include <unordered_set>
#include <functional>

#include "../../deps/handlegraph/handle_graph.hpp"
#include "../../deps/handlegraph/util.hpp"

using namespace std;
using namespace handlegraph;

using sccid_t = uint64_t;

struct SCCEdge {
    sccid_t scc1;
    sccid_t scc2;
    bool from_left;
    bool to_right;
};

class SCCGraph : public HandleGraph {
    private:
        unordered_map<sccid_t, vector<SCCEdge>> edges;
        sccid_t min_node;
        sccid_t max_node;

    public:
        SCCGraph(const HandleGraph& g, const vector<unordered_set<nid_t>> sccs);

        /// Method to check if a node exists by ID
        bool has_node(nid_t node_id) const;
    
        /// Look up the handle for the node with the given ID in the given orientation
        handle_t get_handle(const nid_t& node_id, bool is_reverse = false) const;
        
        /// Get the ID from a handle
        nid_t get_id(const handle_t& handle) const;
        
        /// Get the orientation of a handle
        bool get_is_reverse(const handle_t& handle) const;
        
        /// Invert the orientation of a handle (potentially without getting its ID)
        handle_t flip(const handle_t& handle) const;
        
        /// Get the length of a node
        size_t get_length(const handle_t& handle) const;
        
        /// Get the sequence of a node, presented in the handle's local forward
        /// orientation.
        std::string get_sequence(const handle_t& handle) const;
        
        /// Return the number of nodes in the graph
        size_t get_node_count() const;
        
        /// Return the smallest ID in the graph, or some smaller number if the
        /// smallest ID is unavailable. Return value is unspecified if the graph is empty.
        nid_t min_node_id() const;
        
        /// Return the largest ID in the graph, or some larger number if the
        /// largest ID is unavailable. Return value is unspecified if the graph is empty.
        nid_t max_node_id() const;

    protected:
        
        /// Loop over all the handles to next/previous (right/left) nodes. Passes
        /// them to a callback which returns false to stop iterating and true to
        /// continue. Returns true if we finished and false if we stopped early.
        bool follow_edges_impl(const handle_t& handle, bool go_left, const std::function<bool(const handle_t&)>& iteratee) const;
        
        /// Loop over all the nodes in the graph in their local forward
        /// orientations, in their internal stored order. Stop if the iteratee
        /// returns false. Can be told to run in parallel, in which case stopping
        /// after a false return value is on a best-effort basis and iteration
        /// order is not defined. Returns true if we finished and false if we 
        /// stopped early.
        bool for_each_handle_impl(const std::function<bool(const handle_t&)>& iteratee, bool parallel = false) const;
};

#endif /* VG_SCCGRAPH_HPP_INCLUDED */