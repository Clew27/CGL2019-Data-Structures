#ifndef VG_ALGORITHMS_BUNDLE_HPP_INCLUDED
#define VG_ALGORITHMS_BUNDLE_HPP_INCLUDED

#include <unordered_set>
#include <list>

#include "../../deps/handlegraph/types.hpp"
#include "../../deps/handlegraph/handle_graph.hpp"
#include "../../deps/handlegraph/iteratee.hpp"
#include "handle.hpp"

enum adjacency_t {
    None,
    Weak,
    Strong
};

/// Glorified wrapper for std::unordered_set<handle_t>
class BundleSide {
    private:
        std::unordered_set<handle_t> nodes;
        std::unordered_set<handle_t> nodes_flipped;
    public:
        /// Returns true if successful addition, false if otherwise
        bool add_node(const handle_t& handle);

        void add_init_node(const handle_t& handle) {
            add_node(handle);
        }

        size_t size() const { return nodes.size(); }
        
        std::unordered_set<handle_t>::iterator begin() {
            return nodes.begin();
        }

        std::unordered_set<handle_t>::iterator end() {
            return nodes.end();
        }

        std::unordered_set<handle_t>::const_iterator cbegin() const {
            return nodes.cbegin();
        }

        std::unordered_set<handle_t>::const_iterator cend() const {
            return nodes.cend();
        }

        void update(const HandleGraph& g);

        adjacency_t get_adjacency_type(const BundleSide& other) const;

        void reset();
        
        bool is_reversed(const handle_t& handle) const;

        bool is_member(const handle_t& handle) const;

        bool iterate_nodes(const std::function<bool(const handle_t&)>& iteratee, bool is_reversed) const;
};

/// Glorified wrapper for std::pair<BundleSide, BundleSide>
class Bundle {
    private:
        BundleSide left;
        BundleSide right;
        bool is_bundle_balanced; /// Is a balanced bundle
        bool is_bundle_trivial; /// Is a trivial bundle
        bool has_reversed; /// Has node where the orientation flips
        bool is_bundle_cyclic; /// Has self-cycle or self-inversion

        void update_bundlesides(const HandleGraph& g);

    public:
        BundleSide& get_bundleside(bool is_left) {
            return is_left ? left : right;
        }
        
        BundleSide& get_left() {
            return left;
        }

        BundleSide& get_right() {
            return right;
        }

        bool is_balanced() const { return is_bundle_balanced; }
        bool set_balanced(bool is_balanced_) { is_bundle_balanced = is_balanced_; }

        bool has_reversed_node() const { return has_reversed; }
        void set_has_reversed_node(bool has_reversed_) { has_reversed = has_reversed_; }

        bool is_trivial() const { return is_bundle_trivial; }
        bool is_cyclic() const { return is_bundle_cyclic; }

        void reset() {
            is_bundle_balanced = false;
            is_bundle_trivial = false;
            is_bundle_cyclic = false;
            has_reversed = false;
            left.reset();
            right.reset();
        }

        /// Checks properties of a bundle
        /// Properties: is_trivial, is_cyclic (has self-cycle or inversion)
        void define_properties(const HandleGraph& g);

        bool traverse_bundle(const handle_t& handle, const std::function<bool(const handle_t&)>& iteratee) const;

        /// Returns if go_left is false if calling follow_edges will traverse the bundle or not
        bool is_reversed(const handle_t& handle) const;

        adjacency_t get_adjacency_type(const Bundle& other) const;
};

/// Bundle pool object
class BundlePool {
    private:
        std::list<Bundle *> bundles;

        static BundlePool *instance;
        BundlePool() {}

    public:
        static BundlePool* get_instance() {
            if (instance == nullptr) {
                instance = new BundlePool;
            }
            return instance;
        }

        Bundle* get_bundle() {
            if (bundles.empty()) {
                return new Bundle;
            } else {
                Bundle* front = bundles.front();
                bundles.pop_front();
                return front;
            }
        }

        void return_bundle(Bundle* bundle) {
            bundle->reset();
            bundles.push_back(bundle);
        }
};

#endif /* VG_ALGORITHMS_BUNDLE_HPP_INCLUDED */