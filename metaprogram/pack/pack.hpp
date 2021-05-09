#pragma once

#include <metaprogram/common.hpp>
#include <metaprogram/metafunctional.hpp>
#include <metaprogram/pack/pack_helpers.hpp>


namespace meta {
    // A pack is a list of types. By storing the element types in a single type,
    // it can be stored as an alias.
    // Furthermore, this class allows us to easily define transforms over the set of types.
    //
    // A problem when creating a class like pack is type completeness:
    // A type is not complete until its closing brace, meaning something like this is not allowed:
    //
    // consteval static auto impl(void) {
    //      if constexpr (some_cond) return self{};
    //      else return tail::my_type{};
    // }
    //
    // using my_type = decltype(impl()).
    //
    // We can get around this by returning pointers for these methods,
    // as constructing a pointer of incomplete type is allowed.
    //
    // Note: some compilers (notably Clang) get very confused about visibility when doing something like mentioned above.
    // its easier to just mark everything as public, as packs don't have any runtime state anyway.
    template <typename... Ts> struct pack {
        constexpr static std::size_t size = sizeof...(Ts);
        
        using self  = pack<Ts...>;
        using empty = pack<>;
        
        // Used to check if a given type is a pack.
        using pack_tag = void;
        
        // The first type in Ts, or null_type if size == 0.
        using head = typename detail::splitter<pack, Ts...>::head;
        // A pack of all types in Ts, except head.
        using tail = typename detail::splitter<pack, Ts...>::tail;
    
    
        // Put all the types in the pack inside some other template class.
        // e.g. pack<int, int, bool>::expand_inside<std::tuple> becomes std::tuple<int, int, bool>.
        template <template <typename...> typename P> using expand_inside = P<Ts...>;
        
        // Wrap every type in the pack in some template class, and store the result in a new pack.
        // e.g. pack<int, int, bool>::expand_outside<std::tuple> becomes pack<std::tuple<int>, std::tuple<int>, std::tuple<bool>>.
        template <template <typename...> typename P> using expand_outside = pack<P<Ts>...>;
        
        // Pack::transform<Op> is equivalent to Op<Pack>, but can be chained if Op returns a new pack.
        template <template <typename...> typename P> using transform = P<self>;
    
    
        // Prepend or append some new types to the pack.
        template <typename... Xs> using append  = pack<Ts..., Xs...>;
        template <typename... Xs> using prepend = pack<Xs..., Ts...>;
    
        // Prepend or append all the types in another pack to this pack.
        template <typename Pack> using append_pack  = typename Pack::template expand_inside<append>;
        template <typename Pack> using prepend_pack = typename Pack::template expand_inside<prepend>;
        
        
        // Returns true iff T is in Ts.
        template <typename T> consteval static bool contains_impl(void) {
            if constexpr (size == 0) return false;
            else if constexpr (std::is_same_v<T, head>) return true;
            else return tail::template contains_impl<T>();
        }
        
        template <typename T> constexpr static bool contains = contains_impl<T>();
        
        
        // Returns true iff any T in Pack is also in Ts.
        template <typename Pack> consteval static bool contains_any_impl(void) {
            if constexpr (size == 0 || Pack::size == 0) return false;
            else if constexpr (contains<typename Pack::head>) return true;
            else return contains_any<typename Pack::tail>();
        }
        
        template <typename Pack> constexpr static bool contains_any = contains_any_impl<Pack>();
    
    
        // Returns true iff every T in Pack is also in Ts.
        template <typename Pack> consteval static bool contains_all_impl(void) {
            if constexpr (Pack::size == 0) return true;
            else if constexpr (!contains<typename Pack::head>) return false;
            else return contains_all<typename Pack::tail>();
        }
    
        template <typename Pack> constexpr static bool contains_all = contains_all_impl<Pack>();
        
        
        // Returns a pack of all but the first N elements of Ts.
        // If N > sizeof(Ts) an empty pack is returned.
        template <std::size_t N> consteval static auto pop_front_impl(void) {
            if constexpr (N >= size) return (empty*){};
            else if constexpr (N == 0) return (self*){};
            else return tail::template pop_front_impl<N - 1>();
        }
        
        template <std::size_t N> using pop_front = deptr_type(pop_front_impl, N);
    
    
        // Returns a pack of all but the last N elements of Ts.
        // If N > sizeof(Ts) an empty pack is returned.
        template <std::size_t N> consteval static auto pop_back_impl(void) {
            if constexpr (N >= size) return (empty*){};
            else if constexpr (N == 0) return (self*){};
            
            else {
                using tail_t = deptr_type(tail::template pop_back_impl, N);
                return (typename pack<head>::template append_pack<tail_t>*){};
            }
        }
    
        template <std::size_t N> using pop_back = deptr_type(pop_back_impl, N);
        
        
        // Returns the Nth element of Ts, or null_type if N >= sizeof(Ts).
        template <std::size_t N> using get = typename pop_front<N>::head;
        
        // Returns the last element of Ts, or null_type is sizeof(Ts) == 0.
        using last = get<(size == 0 ? 0 : size - 1)>;
        
        
        // Constructs a pack of every element T in Ts for which Pred<T>::value is true.
        template <template <typename E> typename Pred>
        consteval static auto filter_impl(void) {
            if constexpr (size == 0) return (empty*){};
            else {
                using first = std::conditional_t<
                    Pred<head>::value,
                    pack<head>,
                    empty
                >;
                
                using rest = deptr_type(tail::template filter_impl, Pred);
                
                
                return (typename first::template append_pack<rest>*){};
            }
        }
    
        template <template <typename E> typename Pred>
        using filter = deptr_type(filter_impl, Pred);
        
        
        // Returns a pack containing all elements of Ts, except the one with index N.
        // If N >= sizeof(Ts), no elements are removed.
        template <std::size_t N> using remove_at = typename pop_back<size - N>::template append_pack<pop_front<N + 1>>;
    
        // Returns a pack containing all elements of Ts, except those that are equal to T.
        // If T is not in Ts, no elements are removed.
        template <typename T> using remove_type = filter<
            invert<
                many_to_one_wrapper<
                    bind<T>::template front<std::is_same>::template type
                >::template type
            >::template type
        >;
        
        
        // Equivalent to above, but if there are multiple occurrences of the same type in the pack,
        // only the first one is removed.
        template <typename T>
        constexpr static auto remove_first_impl(void) {
            if constexpr (size == 0) return (self*){};
            else if constexpr (std::is_same_v<head, T>) return (tail*){};
            else {
                return (typename pack<head>
                    ::template append_pack<deptr_type(tail::template remove_first_impl, T)
                >*){};
            }
        }
        
        template <typename T> using remove_first_type = deptr_type(remove_first_impl, T);
        
        
        // Filters the pack, but checks Pred<TF<T>> rather than Pred<T>.
        // The resulting pack contains untransformed elements.
        template <
            template <typename E> typename Pred,
            template <typename E> typename TF
        > struct tf_filter_binder {
            template <typename T> struct type {
                constexpr static bool value = Pred<TF<T>>::value;
            };
        };
        
        template <
            template <typename E> typename TF,
            template <typename E> typename Pred
        > using tf_filter = filter<tf_filter_binder<Pred, TF>::template type>;
        
        
        // Performs a left fold over Ts using Pred.
        // i.e. returns Pred(Pred(Pred(T0, T1), T2), T3), etc.
        // If sizeof(Ts) == 1, head is returned.
        // If sizeof(Ts) == 0, null_type is returned.
        template <template <typename L, typename R> typename Pred>
        consteval static auto left_fold_impl(void) {
            if constexpr (size == 0) return null_type{};
            else if constexpr (size == 1) return head{};
            else return Pred<
                decltype(pop_back<1>::template left_fold_impl<Pred>()),
                last
            >{};
        }
    
        template <template <typename L, typename R> typename Pred>
        using left_fold = decltype(left_fold_impl<Pred>());
        
        
        // Performs a right fold over Ts using Pred.
        // i.e. returns Pred(T0, Pred(T1, Pred(T2, T3))), etc.
        // If sizeof(Ts) == 1, head is returned.
        // If sizeof(Ts) == 0, null_type is returned.
        template <template <typename L, typename R> typename Pred>
        consteval static auto right_fold_impl(void) {
            if constexpr (size == 0) return null_type{};
            else if constexpr (size == 1) return head{};
            else return Pred<
                head,
                decltype(tail::template right_fold_impl<Pred>())
            >{};
        }
    
        template <template <typename L, typename R> typename Pred>
        using right_fold = decltype(right_fold_impl<Pred>());
        
        
        // Recursively flattens any nested packs within this pack.
        constexpr static auto flatten_impl(void) {
            if constexpr (size == 0) return (empty*){};
            else if constexpr (requires { typename head::pack_tag; }) {
                return (typename head
                    ::flatten
                    ::template append_pack<deptr_type(tail::flatten_impl)>*){};
            } else {
                return (typename pack<head>
                    ::template append_pack<deptr_type(tail::flatten_impl)>*){};
            }
        }
        
        using flatten = deptr_type(flatten_impl);
        
        
        // Given some comparator which returns the lesser of two given types,
        // sorts this pack so that lesser types appear before greater ones.
        template <template <typename A, typename B> typename Pred>
        consteval static auto sort_impl(void) {
            if constexpr (size < 2) {
                return (self*){};
            } else {
                using min_type = left_fold<Pred>;
    
                return (typename pack<min_type>
                    ::template append_pack<deptr_type(remove_first_type<min_type>::template sort_impl, Pred)>*){};
            }
        }
    
        template <template <typename A, typename B> typename Pred>
        using sort = deptr_type(sort_impl, Pred);
        
        
        // Returns the first element in Ts for which Pred::value is true,
        // or null_type if Pred::value is false for all elements.
        template <template <typename E> typename Pred>
        using find = typename filter<Pred>::head;
        
        
        // Removes all elements for which Pred::value is true.
        template <template <typename E> typename Pred>
        constexpr static auto remove_if_impl(void) {
            if constexpr (size == 0) return (empty*){};
            
            else if constexpr (Pred<head>::value) {
                return tail::template remove_if_impl<Pred>();
            } else {
                return (typename pack<head>
                    ::template append_pack<deptr_type(tail::template remove_if_impl, Pred)>*){};
            }
        }
    
        template <template <typename E> typename Pred>
        using remove_if = deptr_type(remove_if_impl, Pred);
        
        
        // Reverses the pack
        constexpr static auto reverse_impl(void) {
            if constexpr (size == 0) return (empty*){};
            else return (typename tail::reverse::template append<head>*){};
        }
        
        using reverse = deptr_type(reverse_impl);
        
        
        // Removes all duplicate elements from the pack.
        constexpr static auto unique_impl(void) {
            if constexpr (size == 0) return (empty*){};
            else {
                using tail_t = deptr_type(tail::unique_impl);
                
                if constexpr (tail_t::template contains<head>) return (tail_t*){};
                else return (typename tail_t::template prepend<head>*){};
            }
        }
        
        // unique_impl removes from the front, so reverse twice to keep the frontmost occurrence instead.
        using unique = typename deptr_type(reverse::unique_impl)::reverse;
    };
}