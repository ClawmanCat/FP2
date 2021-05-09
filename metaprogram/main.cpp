#include <metaprogram/pack/pack.hpp>
#include <metaprogram/graph/recursive_graph.hpp>
#include <metaprogram/graph/flattened_graph.hpp>
#include <metaprogram/graph/flattener.hpp>
#include <metaprogram/type_string.hpp>
#include <metaprogram/A_star/A_star.hpp>
#include <define_graph.hpp>

using namespace meta;


#define FP2_NODE_DEF(index, name, x, y, ...) \
using name = node<#name, vec2i { x, y }, __VA_ARGS__>;


int main(int argc, char** argv) {
    // We can define our graph in a directed acyclic manner to prevent circular dependencies in node definitions.
    FP2_GRAPH_DEF(FP2_NODE_DEF);
    std::cout << pretty_type_string<FP2_ROOT_NODE>() << "\n\n\n";
    
    
    // Next, we can convert this recursively defined graph to a flattened one.
    using fg = typename flattener<FP2_ROOT_NODE>::type;
    std::cout << pretty_type_string<fg>(3) << "\n\n\n";
    
    
    // Finally we can use A* to find a path in the graph.
    auto print_path = []<typename Pack>(auto self) {
        std::cout << Pack::head::name;
        
        if constexpr (Pack::size > 1) {
            std::cout << " --> ";
            self.template operator()<typename Pack::tail>(self);
        }
    };
    
    using start_node = N1;
    using dest_node  = N16;
    using path = A_star<fg, start_node, dest_node>::type;
    
    std::cout << "A* path from " << start_node::name << " to " << dest_node::name << ":\n";
    print_path.operator()<path>(print_path);
}