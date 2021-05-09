#pragma once

#include <metaprogram/pack/pack.hpp>
#include <semifunctional/common.hpp>
#include <semifunctional/functional.hpp>
#include <semifunctional/monad/maybe.hpp>

#include <array>
#include <iterator>


namespace sf {
    template <typename T, std::size_t Alloc>
    class static_vector {
    public:
        constexpr static_vector(void) = default;
        
        template <typename... Args> requires (
            // Prevent this constructor from shadowing the move / copy constructors.
            !std::is_same_v<typename meta::pack<bare<Args>...>::head, static_vector<T, Alloc>>
        ) constexpr static_vector(Args&&... elems) {
            (push_back(elems), ...);
        }
        
        
        constexpr auto begin(void) { return elements.begin(); }
        constexpr auto end(void) { return elements.begin() + current_size; }
    
        constexpr auto begin(void) const { return elements.begin(); }
        constexpr auto end(void) const { return elements.begin() + current_size; }
        
        
        using size_type       = std::size_t;
        using difference_type = std::ptrdiff_t;
        using value_type      = Maybe<T>;
        using reference       = Maybe<T>&;
        using pointer         = Maybe<T>*;
        using iterator        = decltype(std::declval<std::array<value_type, Alloc>>().begin());
        using const_iterator  = decltype(std::declval<std::array<value_type, Alloc>>().cbegin());
        
        
        constexpr T& push_back(const T& v) {
            // Note: out-of-bounds checks are already handled by std::array.
            elements[current_size] = v;
            return *elements[current_size++];
        }
    
        constexpr T& push_back(T&& v) {
            elements[current_size] = std::move(v);
            return *elements[current_size++];
        }
        
        constexpr void pop_back(void) {
            elements[--current_size] = std::nullopt;
        }
        
        constexpr std::size_t size(void) const {
            return current_size;
        }
        
        constexpr bool empty(void) const {
            return current_size == 0;
        }
        
        constexpr bool full(void) const {
            return current_size == Alloc;
        }
        
        constexpr T& operator[](std::size_t index) {
            // Out-of-bounds access will trigger compile time error.
            if (elements[index]) return *elements[index];
        }
    
        constexpr const T& operator[](std::size_t index) const {
            if (elements[index]) return *elements[index];
        }
    private:
        std::array<Maybe<T>, Alloc> elements = create_filled_array<Maybe<T>, Alloc>(produce(std::nullopt));
        std::size_t current_size = 0;
    };
}