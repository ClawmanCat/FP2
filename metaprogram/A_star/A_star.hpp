#pragma once

#include <metaprogram/common.hpp>
#include <metaprogram/functional.hpp>
#include <metaprogram/metafunctional.hpp>
#include <metaprogram/pack/pack.hpp>
#include <metaprogram/A_star/heuristic.hpp>
#include <metaprogram/A_star/node_score.hpp>
#include <metaprogram/A_star/kv_pair.hpp>


namespace meta {
    template <
        typename Graph,
        typename From,
        typename To,
        template <typename A, typename B> typename Heuristic = detail::euclidean_distance,
        template <typename Edge> typename TraversalCost = detail::distance_based_cost<>::template type,
        // Recursive parameters
        typename Discovered = pack<From>,
        typename CameFrom = pack<>,
        typename GScore = typename Graph::nodes
            ::template expand_outside<make_score<max_score>::template type>
            ::template expand_outside<update_score<From, 0>::template type>,
        typename FScore = typename Graph::nodes
            ::template expand_outside<make_score<max_score>::template type>
            ::template expand_outside<update_score<From, Heuristic<From, To>::value>::template type>
    > struct A_star {
        // If Node is discovered, returns the score of node, otherwise returns max value.
        template <typename NS> struct fscore_cost_fn {
            constexpr static std::size_t value =
                (Discovered::template contains<typename NS::node>) ? NS::score : max_score;
        };
    
    
        // Given a node, filters a pack to all neighbours of that node.
        template <typename Node> struct neighbour_filter {
            template <typename KeepNode> struct edge_filter {
                template <typename Edge> struct type {
                    constexpr static bool value =
                        std::is_same_v<KeepNode, typename Edge::from> ||
                        std::is_same_v<KeepNode, typename Edge::to>;
                };
            };
        
        
            template <typename KeepNode> struct type {
                using connecting_edge = typename Graph::edges
                    ::template filter<edge_filter<Node>::template type>
                    ::template filter<edge_filter<KeepNode>::template type>
                    ::unique;
            
                constexpr static bool value = !std::is_same_v<Node, KeepNode> && connecting_edge::size > 0;
            };
        };
        
        // Gets the edge connecting two nodes.
        template <typename A, typename B> using connecting_edge =
            typename neighbour_filter<A>::template type<B>::connecting_edge::head;
    
    
        // Given the current node, calculates the tentative gscore of a given neighbour of that node.
        template <typename Current> struct tentative_gscore {
            template <typename Node> using type = node_score<
                Node,
                GScore::template find<equals_node<Current>::template type>::score +
                TraversalCost<connecting_edge<Current, Node>>::value
            >;
        };
    
    
        // Filters a pack on nodes whose gscore was improved.
        template <typename NodeScore> struct improved_score_filter {
            template <typename NS> using pred = std::is_same<typename NS::node, typename NodeScore::node>;
            
            constexpr static bool value =
                NodeScore::score <
                GScore::template find<pred>::score;
        };
        
        template <typename Node> using improved_score_filter_for_node = improved_score_filter<
            typename GScore::template find<equals_node<Node>::template type>
        >;
        
        
        // Adds the heuristic score of a given node to its GScore.
        template <typename NodeScore> using add_heuristic_score = node_score<
            typename NodeScore::node,
            NodeScore::score + Heuristic<typename NodeScore::node, To>::value
        >;
        
        
        // Reconstructs a path back from the end node to the start node.
        template <typename Current, typename Path = pack<>>
        constexpr static auto reconstruct_path(void) {
            using keys = typename CameFrom::template expand_outside<get_key>;
            
            if constexpr (!std::is_same_v<Current, From> && keys::template contains<Current>) {
                return reconstruct_path<
                    typename find_value<CameFrom, Current>::type::value,
                    typename Path::template prepend<Current>
                >();
            } else {
                return typename Path::template prepend<Current>{};
            }
        }
        
        
        constexpr static auto astar_impl(void) {
            if constexpr (Discovered::size == 0) return null_type{};
            
            else {
                // Acquire the node in Discovered with the lowest FScore.
                // We can create a predicate for comparing scores which assigns max score to elements not in Discovered,
                // and then fold that over the FScore pack to get the lowest element.
                using current = typename FScore
                    ::template left_fold<
                        compare_by<fscore_cost_fn>::template type
                    >
                    ::node;
                
                
                // If we've found the destination, reconstruct the path and return it.
                if constexpr (std::is_same_v<current, To>) {
                    return reconstruct_path<current>();
                }
                
                else {
                    // Get all neighbours of current by filtering through the edge set.
                    using neighbours = typename Graph::nodes
                        ::template filter<neighbour_filter<current>::template type>;
                    
                    // And calculate their new GScores.
                    using new_neighbour_gscores = typename neighbours
                        ::template expand_outside<tentative_gscore<current>::template type>
                        ::template filter<improved_score_filter>;
                    
                    // And store the neighbours whose score improved.
                    using changed_neighbours = typename new_neighbour_gscores
                        ::template expand_outside<get_node>;
                    
                    
                    // Update scores of changed neighbours.
                    using new_gscore = typename GScore
                        ::template remove_if<improved_score_filter>
                        ::template append_pack<new_neighbour_gscores>;
                    
                    
                    using new_fscore = typename FScore
                        ::template remove_if<improved_score_filter>
                        ::template append_pack<
                            typename new_neighbour_gscores::template expand_outside<add_heuristic_score>
                        >;
                    
                    
                    // Update discovered nodes.
                    using new_discovered = typename Discovered
                        ::template remove_type<current>
                        ::template append_pack<changed_neighbours>
                        ::unique;
                    
                    
                    // Update traversal history.
                    using new_camefrom = typename CameFrom
                        ::template tf_filter<
                            get_key,
                            invert<improved_score_filter_for_node>::template type
                        >
                        ::template append_pack<
                            typename neighbours::template expand_outside<
                                bind<current>::template back<kv_pair>::template type
                            >
                        >;
                    
                    
                    // Recursively call with updated values.
                    return A_star<
                        Graph,
                        From,
                        To,
                        Heuristic,
                        TraversalCost,
                        new_discovered,
                        new_camefrom,
                        new_gscore,
                        new_fscore
                    >::astar_impl();
                }
            }
        }
        
        using type = decltype(astar_impl());
    };
}