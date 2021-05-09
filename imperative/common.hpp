#pragma once

#include <cmath>


namespace imp {
    struct vec2i { int x, y; };
    
    
    inline float square(float f) {
        return f * f;
    }
    
    
    inline float distance(const vec2i& a, const vec2i& b) {
        return std::sqrt(
            square(b.x - a.x) +
            square(b.y - a.y)
        );
    }
}