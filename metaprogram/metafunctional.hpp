#pragma once

#include <metaprogram/common.hpp>


// Functional utilities that act on types rather than values.
namespace meta {
    // Wrap a template value in a type so it can be used as a trait.
    // C++ unfortunately does not allow template template value parameters,
    // so this must be done with a macro.
    #define value_as_type(name, templval)       \
    template <typename... Ts> struct name {     \
        using type = value<templval<Ts...>>;    \
    };
    
    
    // Binds types to the front or back of a variadic argument list.
    template <typename... Ts> struct bind {
        template <template <typename...> typename P> struct front {
            template <typename... Xs> using type = P<Ts..., Xs...>;
        };
    
        template <template <typename...> typename P> struct back {
            template <typename... Xs> using type = P<Xs..., Ts...>;
        };
    };
    
    
    // Maps type traits to their nested type / value.
    template <template <typename...> typename Trait>
    struct map_trait {
        template <typename... Ts> using type = typename Trait<Ts...>::type;
        template <typename... Ts> constexpr static auto value = Trait<Ts...>::value;
    };
    
    
    // Produces a comparator that compares the result of Pred<T>::value for its types.
    template <template <typename> typename Pred>
    struct compare_by {
        template <typename A, typename B> using type = std::conditional_t<
            (Pred<A>::value < Pred<B>::value),
            A,
            B
        >;
    };
    
    
    template <template <typename E> typename Pred>
    struct invert {
        template <typename T> struct type {
            constexpr static bool value = !Pred<T>::value;
        };
    };
    
    
    // While a template of the form <typename...> is compatible with one of the form <typename>,
    // the same is not true for template template parameters of the same format,
    // requiring us to wrap these instead.
    template <template <typename...> typename P> struct many_to_one_wrapper {
        template <typename E> using type = P<E>;
    };
}