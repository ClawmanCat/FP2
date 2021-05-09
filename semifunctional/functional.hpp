#pragma once

#include <cstddef>
#include <functional>
#include <array>


#define fwd(x) std::forward<decltype(x)>(x)
#define lam(var, ...) [&](var) { return __VA_ARGS__; }
#define generic(fn) [](auto&&... args) { return fn(fwd(args)...); }

#define compare_as(name, ...) [&](const auto& a, const auto& b) {       \
    auto value_of = [&](const auto& name) { __VA_ARGS__; };             \
    return value_of(a) < value_of(b);                                   \
}


namespace sf {
    constexpr inline auto identity = [](const auto& x) -> decltype(auto) { return x; };
    
    
    template <typename T>
    constexpr auto convert(void) {
        return [](auto&& elem) { return T { fwd(elem) }; };
    }
    
    
    // Creates a function that produces the given value.
    constexpr auto produce(auto&& value) {
        return [value = fwd(value)] (auto...) { return value; };
    }
    
    
    // Creates a function which either returns the result of the second or the third predicate,
    // depending on the first predicate.
    constexpr auto pick(auto condition, auto if_true, auto if_false) {
        return [=](auto&&... args) -> decltype(auto) {
            return condition(args...) ? if_true(fwd(args)...) : if_false(fwd(args)...);
        };
    }
    
    
    // Creates a function which checks if a value is equal to the pre-provided value.
    constexpr auto equal(auto&& value) {
        return [value = fwd(value)](const auto& other) { return other == value; };
    }
    
    
    // Given two fields a and b, creates a function that returns the value in the field that is not equal to the provided value.
    constexpr decltype(auto) get_other(auto field_a, auto field_b, auto&& value) {
        return [=, value = fwd(value)](const auto& obj) -> decltype(auto) {
            return obj.*field_a == value ? obj.*field_b : obj.*field_a;
        };
    }
    
    
    constexpr auto non_equal(auto&& value) {
        return [value = fwd(value)](const auto& other) { return other == value; };
    }
    
    
    constexpr auto conjunction(auto a, auto b) {
        return [=](auto&&... args) { return a(args...) && b(args...); };
    }
    
    
    constexpr auto disjunction(auto a, auto b) {
        return [=](auto&&... args) { return a(args...) || b(args...); };
    }
    
    
    // Constructs an array filled with the result of invoking pred for each element.
    template <typename T, std::size_t N, typename Pred>
    constexpr std::array<T, N> create_filled_array(Pred pred) {
        return [&] <std::size_t... Is> (std::index_sequence<Is...>) {
            return std::array<T, N> { pred(Is)... };
        }(std::make_index_sequence<N>());
    }
}