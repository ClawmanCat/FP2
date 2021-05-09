#pragma once

#include <metaprogram/common.hpp>
#include <metaprogram/pack/pack.hpp>
#include <metaprogram/graph/recursive_graph.hpp>
#include <metaprogram/graph/flattened_graph.hpp>


namespace meta {
    // Flattens a recursively defined graph into a set of nodes and a set of edges.
    // Directional information present in the original graph is lost.
    // For the node type, the type of the recursive graph is used.
    // For the edge type, custom types can be provided. By default the edge type from flattened_graph.hpp is used.
    template <
        typename Node,
        template <typename A, typename B> typename MakeEdge = edge
    > struct flattener {
        template <typename N> using self = typename flattener<N, MakeEdge>::type;
        
        
        constexpr static auto flatten_impl(void) {
            if constexpr (Node::children::size == 0) {
                return flattened_graph<pack<Node>, pack<>>{};
            } else {
                using child_nodes = typename Node::children
                    ::template expand_outside<self>
                    ::template expand_outside<get_nodes>
                    ::flatten;
                
                using child_edges = typename Node::children
                    ::template expand_outside<self>
                    ::template expand_outside<get_edges>
                    ::flatten;
                
                using node_edges = typename Node::children
                    ::template expand_outside<bind<Node>::template front<MakeEdge>::template type>;
                    
                
                return flattened_graph<
                    typename child_nodes::template append<Node>::unique,
                    typename child_edges::template append_pack<node_edges>::unique
                >{};
            }
        }
        
        
        using type = decltype(flatten_impl());
    };
}