#pragma once

#include <semifunctional/container/static_vector.hpp>

#include <cstddef>
#include <array>
#include <string_view>
#include <type_traits>


namespace sf {
    struct Node {
        std::string_view name;
        int x, y;
        static_vector<std::size_t, 4> neighbours = {};
        
        constexpr bool operator==(const Node& o) const { return this == &o; }
    };
    
    template <std::size_t Nodes> using Graph = std::array<Node, Nodes>;
    
    
    template <typename> struct array_size {};
    template <typename T, std::size_t N> struct array_size<std::array<T, N>> { constexpr static std::size_t value = N; };
    
    
    constexpr inline auto make_graph(auto nodes, auto edges) {
        return create_filled_array<Node, array_size<decltype(nodes)>::value>([&](std::size_t i) {
            Node n = nodes[i];
            
            for (const auto& edge : edges) {
                if (edge.first == i) n.neighbours.push_back(edge.second);
                if (edge.second == i) n.neighbours.push_back(edge.first);
            }
            
            return n;
        });
    }
}