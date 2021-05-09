#pragma once

#include <semifunctional/common.hpp>
#include <semifunctional/functional.hpp>
#include <semifunctional/container/static_vector.hpp>


namespace sf {
    template <typename K, typename V, std::size_t Alloc, typename Compare = std::less<K>>
    class static_ordered_list {
    public:
        struct Node {
            K key;
            V value;
        
            Node *next = nullptr;
        };
        
        
        constexpr Node& insert(auto&& key, auto&& val) {
            Node& node = storage.push_back(Node { .key = fwd(key), .value = fwd(val) });
            
            // Find pointer to first node larger than new node.
            Node** p = &head;
            while (*p && comparator((**p).key, node.key)) p = &(**p).next;
            
            // The pointer pointing to the element larger than node shall point to node;
            // node shall point to the larger element.
            node.next = *p;
            *p = &node;
            
            return node;
        }
        
        
        constexpr Node& update(auto&& old_key, auto&& new_key) {
            // Find pointer to old key.
            Node** old = &head;
            while ((**old).key != old_key) old = &(**old).next;
            Node* elem = *old;
            
            // Find pointer to first node larger than new key.
            Node** p = &head;
            while (*p && comparator((**p).key, new_key)) p = &(**p).next;
    
            // If the next element is still the previously next element, don't move anything.
            if (elem->next != *p) {
                // Pointer to old_key shall point to the element larger than the old value of the node.
                *old = elem->next;
                // Changed element shall point to the element larger than its new value.
                elem->next = *p;
                // Pointer to element larger than new key shall point to updated element.
                *p = elem;
            }
            
            elem->key = fwd(new_key);
            return *elem;
        }
        
        
        struct iterator {
            using value_type        = std::pair<const K, V>;
            using difference_type   = std::ptrdiff_t;
            using reference         = std::pair<const K&, V&>;
            using const_reference   = std::pair<const K&, const V&>;
            using iterator_category = std::forward_iterator_tag;
            
            Node* ptr;
            
            constexpr iterator& operator++(void) {
                ptr = ptr->next;
                return *this;
            }
            
            constexpr reference operator*(void) { return { ptr->key, ptr->value }; }
            constexpr const_reference operator*(void) const { return { ptr->key, ptr->value }; }
            
            constexpr bool operator==(const iterator&) const = default;
        };
        
        constexpr iterator begin(void) { return { head }; }
        constexpr iterator end(void) { return { nullptr}; }
    
        
        constexpr typename iterator::reference operator[](std::size_t i) {
            auto it = begin();
            for (std::size_t j = 0; j < i; ++j) ++it;
            return *it;
        }
        
    private:
        static_vector<Node, Alloc> storage;
        Node* head = nullptr;
        Compare comparator;
    };
}