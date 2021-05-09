#pragma once

#include <metaprogram/common.hpp>
#include <metaprogram/pack/pack.hpp>


namespace meta {
    struct vec2i { int x = 0, y = 0; };
    
    
    // A graph can be defined recursively by constructing each node from its outgoing connections.
    // Because a type's definition cannot depend upon itself, this must always result in a directional, acyclic graph.
    // To generate a non-directional and possibly cyclic graph, such a graph can be flattened into a set of nodes and edges.
    //
    // This node class also includes a name to easily identify it, and a position for use with the A* heuristic.
    // These are not required for the flattener, and any other node type could be created and used instead to make a flattened graph.
    // Such a node type would still work with the A* algorithm, as long as a heuristic is provided that can work with it.
    template <string_arg Name, vec2i Pos, typename... Children> struct node {
        constexpr static inline std::string_view name { Name.c_string };
        constexpr static inline vec2i position { Pos };
        
        using children = pack<Children...>;
    };
}