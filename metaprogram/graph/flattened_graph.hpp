#pragma once

#include <metaprogram/common.hpp>
#include <metaprogram/pack/pack.hpp>


// A flattened graph is a set of nodes and edges.
// The graph is non-directional and may be cyclic.
// When a graph is in this form, it can be used with the A* algorithm.
// Any node type or edge type can be used, as long as proper metafunctions are provided for dealing with them
// in algorithms that transform them.
namespace meta {
template <typename NodePack, typename EdgePack>
struct flattened_graph {
using nodes = NodePack;
using edges = EdgePack;
};
    
    template <typename FG> using get_nodes = typename FG::nodes;
    template <typename FG> using get_edges = typename FG::edges;
    
    
    template <typename A, typename B>
    struct edge {
        using from = A;
        using to   = B;
    };
    
    template <typename E> using get_from_node = typename E::from;
    template <typename E> using get_to_node   = typename E::to;
}