#pragma once

#include <semifunctional/functional.hpp>

#include <boost/preprocessor.hpp>
#include <range/v3/all.hpp>
#include <range/v3/view/view.hpp>

#include <optional>
#include <variant>
#include <type_traits>
#include <functional>


namespace sf {
    namespace views = ranges::views;
    
    
    template <typename T> using bare = std::remove_cvref_t<T>;
    
    
    template <typename T, typename Decay = std::remove_cvref_t<T>> concept Universal =
        std::is_same_v<T, Decay>            ||
        std::is_same_v<T, Decay&>           ||
        std::is_same_v<T, Decay&&>          ||
        std::is_same_v<T, const Decay&>;
    
    
    template <typename T, typename Ref> struct reference_as {
        using type = T;
    };
    
    template <typename T, typename Ref> struct reference_as<T, Ref&> {
        using type = T&;
    };
    
    template <typename T, typename Ref> struct reference_as<T, Ref&&> {
        using type = T&&;
    };
    
    template <bool is_const, typename T> using maybe_const = std::conditional_t<is_const, std::add_const_t<T>, T>;
    
    template <typename T, typename Ref>
    using reference_as_t = typename reference_as<T, Ref>::type;
    
    
    
}