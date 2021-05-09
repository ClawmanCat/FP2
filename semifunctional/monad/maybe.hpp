#pragma once

#include <semifunctional/common.hpp>

#include <optional>


namespace sf {
    template <typename T> using Maybe = std::optional<T>;
    
    
    template <typename T> struct is_maybe           { constexpr static bool value = false; };
    template <typename T> struct is_maybe<Maybe<T>> { constexpr static bool value = true;  };
    
    template <typename T> constexpr static bool is_maybe_v = is_maybe<bare<T>>::value;
    
    
    // Maybe<X> >>= Fn<X -> Y> -> Maybe<Y>. Will move if possible.
    // Explicit check for invocation with M is required since any type that accepts M will also accept T,
    // unless T is explicitly default constructable.
    template <typename M, typename Pred, typename T = typename bare<M>::value_type>
    requires (std::is_invocable_v<Pred, T> && !std::is_invocable_v<Pred, M> && is_maybe_v<M>)
    constexpr auto operator>>(M&& opt, Pred fn) {
        using result_t = std::invoke_result_t<Pred, T>;
    
        return opt.has_value()
               ? (Maybe<result_t>) fn((reference_as_t<T, decltype(opt)>) *opt)
               : Maybe<result_t>(std::nullopt);
    }
    
    
    // Maybe<X> >>= Fn<Maybe<X> -> Y> -> Y. Will move if possible.
    template <typename M, typename Pred, typename T = typename bare<M>::value_type>
    requires (std::is_invocable_v<Pred, M> && is_maybe_v<M>)
    constexpr auto operator>>(M&& opt, Pred fn) {
        return fn(fwd(opt));
    }
    
    
    // Maybe<X> -> X. If Maybe<X> is empty, the default is used. Will move if possible.
    template <typename T> class Else {
    public:
        explicit constexpr Else(const T& v) : v(v) {}
        explicit constexpr Else(T&& v) : v(std::move(v)) {}
        
        constexpr T operator()(const Maybe<T>& m) const {
            return m.has_value() ? *m : v;
        }
        
        constexpr T operator()(Maybe<T>&& m) const {
            return m.has_value() ? std::move(*m) : std::move(v);
        }
    
    private:
        T v;
    };
}