#pragma once

#include <cstddef>


template <typename Fn, typename DFn, typename T>
constexpr inline auto newtons_method(Fn fn, DFn derivative, T guess, std::size_t iterations) {
    for (std::size_t i = 0; i < iterations; ++i) {
        guess -= fn(guess) / derivative(guess);
    }
    
    return guess;
}


template <typename T> requires std::is_floating_point_v<T>
constexpr inline T constexpr_sqrt(T value) {
    if (value <= T(0)) return T(0);
    
    return newtons_method(
        [&](T guess) { return guess * guess - value; },
        [&](T guess) { return 2 * guess; },
        value,
        sizeof(T) * 6
    );
}


template <typename T>
constexpr inline T constexpr_pow(T value, std::size_t exp) {
    T result = 1;
    
    while (exp > 0) {
        if (exp & 1) result *= value;
        
        exp >>= 1;
        value *= value;
    }
    
    return result;
}