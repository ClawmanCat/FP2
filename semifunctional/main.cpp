#include <semifunctional/A_star.hpp>
#include <semifunctional/graph.hpp>
#include <constexpr_math.hpp>
#include <define_graph.hpp>

#include <cstddef>
#include <iostream>


#define FP2_NODE_DEF(index, name, x, y, ...)                        \
Node { #name, x, y, {} } FP2_MAYBE_COMMA(index)


#define FP2_EDGE_DEF_IMPL(Rep, Args, Elem)                          \
std::pair {                                                         \
    BOOST_PP_CAT(indexof_, Elem),                                   \
    BOOST_PP_TUPLE_ELEM(0, Args)                                    \
},


#define FP2_EDGE_DEF(index, name, x, y, ...)                        \
BOOST_PP_LIST_FOR_EACH(                                             \
    FP2_EDGE_DEF_IMPL,                                              \
    (index, name, x, y),                                            \
    BOOST_PP_VARIADIC_TO_LIST(__VA_ARGS__)                          \
)


#define FP2_INDEX_DEF(index, name, x, y, ...)                       \
constexpr std::size_t indexof_##name = index;


using namespace sf;

int main(int argc, char** argv) {
    FP2_GRAPH_DEF(FP2_INDEX_DEF);
    
    
    constexpr static auto nodes = std::array { FP2_GRAPH_DEF(FP2_NODE_DEF) };
    constexpr static auto edges = std::array { FP2_GRAPH_DEF(FP2_EDGE_DEF) };
    
    constexpr static auto g = make_graph(nodes, edges);
    
    
    constexpr auto distance = [](const Node& a, const Node& b) {
        auto square = lam(auto x, x * x);
        return constexpr_sqrt<float>(square(b.x - a.x) + square(b.y - a.y));
    };
    
    
    constexpr auto path = A_star(
        g,
        g[indexof_N1],
        g[indexof_N16],
        distance,
        distance
    );
    
    
    for (const auto& [i, node] : views::enumerate(*path)) {
        std::cout << (**node).name;
        if (i + 1 < path->size()) std::cout << " -> ";
    }
}