#pragma once

#include <metaprogram/common.hpp>
#include <constexpr_math.hpp>


namespace meta::detail {
    template <typename First, typename Second> struct euclidean_distance {
        constexpr static auto square(auto val) { return val * val; }
        
        constexpr static std::size_t value = constexpr_sqrt<double>(
            square(First::position.x - Second::position.x) +
            square(First::position.y - Second::position.y)
        );
    };
    
    
    template <
        template <typename From, typename To> typename Distance = euclidean_distance
    > struct distance_based_cost {
        template <typename Edge> struct type {
            constexpr static std::size_t value = Distance<typename Edge::from, typename Edge::to>::value;
        };
    };
}