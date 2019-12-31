#include "bundle.hpp"
#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <unordered_map>
#include "../../deps/handlegraph/util.hpp"

using namespace handlegraph;

/***********************************************
 * Custom structures implementation
 ***********************************************/

/// Counter structure
struct Count {
    struct value_type { template<typename T> value_type(const T&) {} };
    void push_back(const value_type&) { count++; }
    size_t count = 0;
};

/***********************************************
 * BundleSide implementation
 ***********************************************/

bool BundleSide::add_node(const handle_t& node) {
    if (is_bundle_freed) {
        return true;
    }

    return bundle_set.insert(node).second;
}

void BundleSide::cache(HandleGraph& g) {
    for (const handle_t& node : bundle_set) {
        bundle_vector.push_back(node);
        bundle_vector_reversed.push_back(g.get_handle(g.get_id(node), !g.get_is_reverse(node)));
    }

    // Should prompt C++ to clear the memory
    bundle_set = std::unordered_set<handle_t>();
    // Else this is the method I came up with
    /*
    bundle_set.clear();
    bundle_set.rehash(1);
    */

    /// Sort for set intersection
    std::sort(bundle_vector.begin(), bundle_vector.end(), 
        [](handle_t& h1, handle_t& h2) {return as_integer(h1) < as_integer(h2);});
    std::sort(bundle_vector_reversed.begin(), bundle_vector_reversed.end(),
        [](handle_t& h1, handle_t& h2) {return as_integer(h1) < as_integer(h2);});

    is_bundle_freed = true;
}

void BundleSide::traverse_bundle(const std::function<void(const handle_t&)>& iteratee) {    
    if (is_bundle_freed) {
        for (const handle_t& node : bundle_vector) {
            iteratee(node);
        }
    } else {
        for (const handle_t& node : bundle_set) {
            iteratee(node);
        }
    }
}

int BundleSide::size() {
    if (is_bundle_freed) {
        return bundle_vector.size();
    }
    return bundle_set.size();
}

Adjacency get_adjacency_type(const bundle_vector_t side1, const bundle_vector_t side2) {
    Count c;
    set_intersection(side1.begin(), side1.end(), side2.begin(), side2.end(), std::back_insert_iterator(c), 
        [](const handle_t h1, const handle_t h2) -> bool {
            return as_integer(h1) < as_integer(h2);
        }
    );

    size_t count = c.count;

    if (side1.size() == count && side2.size() == count) {
        return Adjacency::Strong;
    } else if (count > 0) {
        return Adjacency::Weak;
    }
    return Adjacency::None;
}

Adjacency BundleSide::get_adjacency(const BundleSide& other) const {
    if (!this->is_bundle_freed || !other.is_bundle_freed) {
        throw std::invalid_argument("Invalid BundleSide in adjacency");
    }

    Adjacency res;
    if ((res = get_adjacency_type(this->bundle_vector, other.bundle_vector)) != Adjacency::None ||
        (res = get_adjacency_type(this->bundle_vector, other.bundle_vector_reversed)) != Adjacency::None ||
        (res = get_adjacency_type(this->bundle_vector_reversed, other.bundle_vector)) != Adjacency::None ||
        (res = get_adjacency_type(this->bundle_vector_reversed, other.bundle_vector_reversed)) != Adjacency::None
    ) {
        return res;
    }
    return Adjacency::None;
}

/***********************************************
 * Bundle implementation
 ***********************************************/
bool Bundle::add_node(const handle_t& node, bool is_left) {
    if (is_left) {
        return internal_bundle.first.add_node(node);
    } else {
        return internal_bundle.second.add_node(node);
    }
}

void Bundle::add_init_node(const handle_t& node, bool is_left) {
    add_node(node, is_left);
}

int Bundle::get_bundleside_size(bool is_left) {
    return get_bundleside(is_left).size();
}

BundleSide Bundle::get_bundleside(bool is_left) {
    if (is_left) {
        return internal_bundle.first;
    }
    return internal_bundle.second;
}

void Bundle::freeze(HandleGraph& g) {
    internal_bundle.first.cache(g);
    internal_bundle.second.cache(g);
}

bool Bundle::is_trivial() {
    return is_bundle_trivial; 
}

void Bundle::set_trivial(bool is_bundle_trivial_) {
    this->is_bundle_trivial = is_bundle_trivial_;
}

bool Bundle::has_reversed_node() {
    return has_reversed;
}

void Bundle::set_has_reversed_node(bool has_reversed_) {
    this->has_reversed = has_reversed_;
}