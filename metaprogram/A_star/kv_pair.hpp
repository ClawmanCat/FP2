#pragma once

#include <metaprogram/common.hpp>


namespace meta {
    template <typename K, typename V>
    struct kv_pair {
        using key = K;
        using value = V;
    };
    
    
    template <typename KV> using get_key = typename KV::key;
    template <typename KV> using get_value = typename KV::value;
    
    
    template <typename Pack, typename K> struct find_value {
        template <typename KV> using key_filter = std::is_same<typename KV::key, K>;
        
        using type = typename Pack::template filter<key_filter>::head;
    };
}