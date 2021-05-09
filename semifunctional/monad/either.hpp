#pragma once

#include <semifunctional/common.hpp>
#include <metaprogram/pack/pack.hpp>
#include <metaprogram/metafunctional.hpp>

#include <variant>


namespace sf {
    template <typename... Ts> using Either = std::variant<Ts...>;
    
    
    // Combines multiple functions into a single overloaded callable.
    template <typename... Fns>
    struct Visitor : public Fns... {
        constexpr Visitor(Fns&&... fns) : Fns(fwd(fns))... {}
        using Fns::operator()...;
    };
    
    
    // Gets the options list from an either type as a pack.
    template <typename> struct get_options {};
    template <typename... O> struct get_options<Either<O...>> { using type = meta::pack<O...>; };
    
    template <typename T> using get_options_t = typename get_options<T>::type;
    
    
    namespace detail {
        template <typename Pred, typename T>
        using either_transformed_t = std::conditional_t<
            std::is_invocable_v<Pred, T>,
            std::invoke_result_t<Pred, T>,
            T
        >;
        
        
        template <typename OldPack, typename NewPack>
        struct either_transform {
            using old_t = typename OldPack::template expand_inside<Either>;
            using new_t = typename NewPack::template expand_inside<Either>;
            
            
            template <typename Pred>
            constexpr static new_t cast(old_t&& v, Pred p) {
                return std::visit([&] (auto&& arg) -> new_t {
                    if constexpr (std::is_invocable_v<Pred, decltype(arg)>) {
                        return p(fwd(arg));
                    } else {
                        return arg;
                    }
                }, fwd(v));
            }
        };
    }
    
    
    // Either<X, Y, Z> | Fn<X -> W> -> Either<W, Y, Z>. Function may be overloaded for multiple types.
    template <typename E, typename Pred>
    constexpr auto operator|(E&& either, Pred pred) {
        using new_options_t = typename get_options_t<bare<E>>
            // For every T in Options, decltype(Pred(T)) if Pred is invocable with T, else T.
            ::template expand_outside<
                meta::bind<Pred>::template front<detail::either_transformed_t>
            >;
        
        return detail::either_transform<get_options_t<bare<E>>, new_options_t>::cast(fwd(either), pred);
    }
    
    
    // Either<X, Y, Z> >> Fn<X -> void> -> void.
    // Equivalent to std::visit, but can be chained and is a no-op if Pred does not accept the contained type.
    template <typename E, typename Pred>
    constexpr auto operator>>(E&& either, Pred pred) {
        return std::visit(
            [&] (auto&& v) {
                if constexpr (std::is_invocable_v<Pred, decltype(v)>) pred(fwd(v));
            },
            either
        );
    };
}