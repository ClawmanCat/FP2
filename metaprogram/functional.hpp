#pragma once

#include <metaprogram/common.hpp>


#define fn_object(fn) [](auto&&... args) { return fn(fwd(args)...); }


// Normal (i.e. not type-based) functional utilities.
namespace meta {
    // Returns true iff T is in Ts.
    template <typename T, typename... Ts>
    constexpr inline bool one_of(const T& elem, const Ts&... rest) {
        return ((elem == rest) || ...);
    }
    
    
    // Binds the given parameters to the start of the function parameter list.
    template <typename Fn, typename... Args>
    constexpr inline auto bind_front(Fn&& fn, Args&&... args) {
        return [fn = fwd(fn), ...args = fwd(args)](auto&&... rest_args) {
            return fn(args..., fwd(rest_args)...);
        };
    }
    
    
    // Binds the given parameters to the end of the function parameter list.
    template <typename Fn, typename... Args>
    constexpr inline auto bind_back(Fn&& fn, Args&&... args) {
        return [fn = fwd(fn), ...args = fwd(args)](auto&&... rest_args) {
            return fn(fwd(rest_args)..., args...);
        };
    }
    
    
    template <typename T> constexpr inline T cast(auto&& from) {
        return static_cast<T>(fwd(from));
    }
}