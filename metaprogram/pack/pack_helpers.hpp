#pragma once

#include <metaprogram/common.hpp>

#include <boost/preprocessor.hpp>


namespace meta::detail {
    // Base case: sizeof(Ts) == 0.
    template <
        template <typename...> typename Pack,
        typename... Ts
    > struct splitter {
        using head = null_type;
        using tail = Pack<>;
    };
    
    
    // Specialization: sizeof(Ts) >= 1.
    template <
        template <typename...> typename Pack,
        typename T0,
        typename... Ts
    > struct splitter<Pack, T0, Ts...> {
        using head = T0;
        using tail = Pack<Ts...>;
    };
    
    
    // We cannot instantiate an object of type T if T is incomplete,
    // but we can always instantiate a T* and then get the type of that.
    #define deptr_type(fn, ...) \
    std::remove_pointer_t<decltype(fn __VA_OPT__(<) __VA_ARGS__ __VA_OPT__(>) ())>
}