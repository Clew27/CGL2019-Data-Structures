#include <iostream>
#include <sstream>
#include <string>

#include "../../src/BidirectedGraph.hpp"
#include "../../src/BidirectedGraphBuilder.hpp"
#include "../../src/algorithms/find_balanced_bundles.hpp"
#include "../../src/algorithms/bundle.hpp"

using namespace std;

std::string node_to_str(const handle_t& handle, const HandleGraph& g) {
    stringstream ss;
    ss << "Node " << g.get_id(handle);
    if (g.get_is_reverse(handle)) ss << " reversed";
    return ss.str();
}

void print_bundle(BidirectedGraph& g, Bundle& bundle) {
    cout << "Left" << endl;
    bundle.get_bundleside(true).traverse_bundle([&](const handle_t& l_handle) {
        cout << node_to_str(l_handle, g) << endl;
        return true;
    });
    cout << "Right" << endl;
    bundle.get_bundleside(false).traverse_bundle([&](const handle_t& r_handle) {
        cout << node_to_str(r_handle, g) << endl;
        return true;
    }); 
    cout << "Is a trivial bundle:  " << ((bundle.is_trivial()) ? "true" : "false") << endl;
    cout << "Has reversed node(s): " << ((bundle.has_reversed_node()) ? "true" : "false") << endl;
}

/// Usage:
/// Call BundleTest binary with the test graph json files as the arguments
/// Ex: ./BundleTest test/bundle_test_graph/00_trivial.json
int main(int argc, char* argv[]) {
    int exit_code = EXIT_SUCCESS;

    for (int i = 1; i < argc; i++) {
        cout << i << "\t" << argv[i] << endl;
        BidirectedGraph g = BidirectedGraphBuilder::build_graph(argv[i]);
        auto bundles = find_balanced_bundles(g);
        for (auto bundle : bundles) {
            print_bundle(g, bundle);
        }
        cout << "Nodes: ";
        g.for_each_handle([&](const handle_t& handle) {
            cout << g.get_id(handle) << " ";
        });
        cout << endl;
#ifdef DEBUG_BIDIRECTED_GRAPH
        cout << "Edges:" << endl;
        g.print_edges();
#endif /* DEBUG_BIDIRECTED_GRAPH */
    }

    return exit_code;
}