#include <imperative/graph.hpp>
#include <imperative/A_star.hpp>
#include <imperative/cost_function.hpp>
#include <define_graph.hpp>

#include <vector>
#include <utility>
#include <iostream>


using namespace imp;
template <typename T> using ref = std::reference_wrapper<T>;


#define FP2_NODE_DEF(index, name, x, y, ...)                                \
node& name = g.add_node(#name, { x, y });                                   \
for (auto& o : std::vector<ref<node>>{ __VA_ARGS__ }) g.add_edge(name, o);


int main(int argc, char** argv) {
    graph g;
    FP2_GRAPH_DEF(FP2_NODE_DEF);
    
    
    auto heuristic     = distance_based_cost{};
    auto traverse_cost = distance_based_cost{};
    
    
    auto path = A_star(g, &N1, &N16, heuristic, traverse_cost);
    for (std::size_t i = 0; i < path.size(); ++i) {
        std::cout << path[i]->name;
        if (i + 1 != path.size()) std::cout << " --> ";
    }
}