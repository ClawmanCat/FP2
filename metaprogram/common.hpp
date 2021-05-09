#pragma once

#define meta ranges_meta
#include <range/v3/all.hpp>
#undef meta

#include <ctre.hpp>

#include <cstddef>
#include <algorithm>
#include <type_traits>
#include <iostream>
#include <string_view>
#include <string>
#include <cctype>


#define fwd(x) std::forward<decltype(x)>(x)


namespace meta {
    using namespace std::string_literals;
    using namespace std::string_view_literals;
    namespace views = ranges::views;
    
    
    // An empty class to be used in place of another type when no type is available.
    // e.g. pack::head on an empty pack.
    struct null_type {};
    
    
    // A wrapper for std::integral_constant which deduces the type parameter.
    // This can be used to store a value as a type.
    template <auto Val> using value = std::integral_constant<decltype(Val), Val>;
    
    
    // Wrappers for C-style function pointers.
    template <typename Ret, typename... Args> using fn = Ret(*)(Args...);
    
    template <typename Cls, typename Ret, typename... Args> using mem_fn = Ret(Cls::*)(Args...);
    template <typename Cls, typename Ret, typename... Args> using const_mem_fn = Ret(Cls::*)(Args...) const;
    
    template <typename Cls, typename T> using mem_var = T(Cls::*);
    
    
    // Wrapper for reference-to-array types.
    template <typename T, std::size_t N> using array_ref = T(&)[N];
    
    
    // A string type that can be used as a template parameter.
    template <std::size_t N> struct string_arg {
        constexpr string_arg (array_ref<const char, N> c_string) {
            std::copy_n(c_string, N, this->c_string);
        }
        
        char c_string[N];
    };
}