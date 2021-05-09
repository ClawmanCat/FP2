#pragma once

#include <semifunctional/common.hpp>
#include <semifunctional/graph.hpp>
#include <semifunctional/functional.hpp>
#include <semifunctional/container/static_vector.hpp>
#include <semifunctional/container/static_ordered_list.hpp>
#include <semifunctional/monad/maybe.hpp>

#include <limits>
#include <array>
#include <iterator>


namespace sf {
    constexpr inline float infinity = std::numeric_limits<float>::infinity();
    template <std::size_t Nodes> using Path = static_vector<const Node*, Nodes>;
    
    
    template <std::size_t Nodes> struct Indexer {
        explicit constexpr Indexer(const std::array<Node, Nodes>& nodes) : nodes(nodes) {}
        
        const std::array<Node, Nodes>& nodes;
        
        constexpr std::size_t operator()(const Node& node) const {
            // This could be done with a better time complexity in various ways,
            // but the constexpr requirement severely limits what is possible.
            // e.g. we cannot simply map Node* => std::size_t and then do a binary search,
            // since comparing pointers is not a constant expression, and neither is casting the pointer to an integer.
            // The best way would probably to manually implement a RB-tree or similar on top of static_vector,
            // but that is outside the scope of this assignment.
            auto it = std::find(nodes.begin(), nodes.end(), node);
            return std::distance(nodes.begin(), it);
        }
    };
    
    
    template <std::size_t Nodes> constexpr Path<Nodes> reconstruct_path(
        const Graph<Nodes>& graph,
        const auto& came_from,
        const Node& current
    ) {
        Indexer indexof { graph };
        
        auto result = came_from[indexof(current)]
            >> lam(const Node* next, reconstruct_path(graph, came_from, *next))
            >> Else { Path<Nodes>{} };
        
        result.push_back(&current);
        return result;
    }
    
    
    template <std::size_t Nodes, typename Heuristic, typename DistanceFn, std::size_t MaxConnections = 4>
    consteval Maybe<Path<Nodes>> A_star_impl(
        const Graph<Nodes>& graph,
        const Node& from,
        const Node& to,
        const Heuristic& h,
        const DistanceFn& d,
        std::array<Maybe<const Node*>, Nodes>& came_from,
        std::array<bool, Nodes>& discovered,
        std::array<float, Nodes>& fscore,
        std::array<float, Nodes>& gscore
    ) {
        Indexer indexof { graph };
    
    
        if (std::all_of(discovered.begin(), discovered.end(), equal(false))) {
            // There are no more nodes to expand and there is no path.
            return std::nullopt;
        }
        
        
        // See comment for Indexer::operator()
        const Node* next_ptr = nullptr;
        float next_score = infinity;
        
        for (std::size_t i = 0; i < discovered.size(); ++i) {
            if (discovered[i] && fscore[i] < next_score) {
                next_ptr   = &graph[i];
                next_score = fscore[i];
            }
        }
        
        const auto& next = *next_ptr;
        
        // Cleaner solution, but not marked constexpr.
        /* auto next = graph[ranges::min_element(
		    discovered | views::filter(identity) | views::enumerate | views::keys,
		    compare_as(x, fscore[x])
	    )];*/
        
        
        discovered[indexof(next)] = false;
        if (next == to) return reconstruct_path(graph, came_from, next);
        
        
        for (const auto& maybe_index : next.neighbours) {
            if (!maybe_index) continue;
            auto index = *maybe_index;
            
            const auto& neighbour = graph[index];
            float tentative_gscore = gscore[indexof(next)] + d(neighbour, to);
            
            if (tentative_gscore < gscore[index]) {
                gscore[index] = tentative_gscore;
                fscore[index] = tentative_gscore + h(neighbour, to);
                discovered[index] = true;
                came_from[index] = &next;
            }
        }
        
        return A_star_impl(
            graph,
            from, to,
            h, d,
            came_from,
            discovered,
            fscore, gscore
        );
    }
    
    
    template <std::size_t Nodes, typename Heuristic, typename DistanceFn, std::size_t MaxConnections = 4>
    consteval Maybe<static_vector<const Node*, Nodes>> A_star(
        const Graph<Nodes>& graph,
        const Node& from,
        const Node& to,
        const Heuristic& h,
        const DistanceFn& d
    ) {
        Indexer indexof { graph };
    
        auto came_from = create_filled_array<Maybe<const Node*>, Nodes>(produce(std::nullopt));
        auto discovered = create_filled_array<bool, Nodes>(produce(false));
        auto fscore = create_filled_array<float, Nodes>(produce(infinity));
        auto gscore = create_filled_array<float, Nodes>(produce(infinity));
        
        fscore[indexof(from)] = h(from, to);
        gscore[indexof(from)] = 0;
        discovered[indexof(from)] = true;
    
        return A_star_impl(
            graph,
            from, to,
            h, d,
            came_from,
            discovered,
            fscore, gscore
        );
    }
}