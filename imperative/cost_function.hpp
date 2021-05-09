#pragma once

#include <imperative/graph.hpp>
#include <imperative/common.hpp>


namespace imp {
    struct cost_function {
        virtual ~cost_function(void) = default;
        virtual float cost(const node* a, const node* b) = 0;
    };
    
    
    struct distance_based_cost : public cost_function {
        float cost(const node* a, const node* b) override {
            return distance(a->position, b->position);
        }
    };
}