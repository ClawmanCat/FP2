#pragma once

#include <metaprogram/common.hpp>


namespace meta {
    constexpr static std::size_t max_score = std::numeric_limits<std::size_t>::max();
    
    
    template <typename Node, std::size_t Score>
    struct node_score {
        using node = Node;
        constexpr static std::size_t score = Score;
    };
    
    
    template <typename NS> using get_node  = typename NS::node;
    template <typename NS> using get_score = value<NS::score>;
    
    template <typename A, typename B> using node_eq = std::is_same<typename A::node, typename B::node>;
    
    
    template <std::size_t Score> struct make_score {
        template <typename Node> using type = node_score<Node, Score>;
    };
    
    
    template <typename Node, std::size_t Score> struct update_score {
        template <typename NodeScore> using type = std::conditional_t<
            std::is_same_v<Node, typename NodeScore::node>,
            node_score<Node, Score>,
            NodeScore
        >;
    };
    
    
    template <typename Node> struct equals_node {
        template <typename NS> using type = std::is_same<Node, typename NS::node>;
    };
}