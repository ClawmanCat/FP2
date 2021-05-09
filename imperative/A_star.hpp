#pragma once

#include <imperative/graph.hpp>
#include <imperative/cost_function.hpp>

#include <vector>
#include <unordered_map>
#include <map>
#include <set>
#include <queue>
#include <algorithm>
#include <limits>


namespace imp {
    class score_comparator {
    public:
        explicit score_comparator(std::unordered_map<node*, float>* scores) : scores(scores) {}
        
        bool operator()(node* a, node* b) const {
            return scores->at(a) < scores->at(b);
        }
    private:
        std::unordered_map<node*, float>* scores;
    };
    
    
    inline std::vector<node*> reconstruct_path(const std::unordered_map<node*, node*>& came_from, node* current) {
        std::vector<node*> result { current };
        
        typename std::unordered_map<node*, node*>::const_iterator it;
        while (it = came_from.find(current), it != came_from.end()) {
            current = it->second;
            result.push_back(current);
        }
        
        std::reverse(result.begin(), result.end());
        return result;
    }
    
    
    inline std::vector<node*> A_star(graph& g, node* from, node* to, cost_function& h, cost_function& d) {
        const float infinity = std::numeric_limits<float>::infinity();
    
        
        std::unordered_map<node*, node*> came_from;
        std::unordered_map<node*, float> fscore, gscore;
        std::multiset<node*, score_comparator> discovered { score_comparator { &fscore } };
        
        
        for (auto& node : g.nodes) gscore[node.get()] = fscore[node.get()] = infinity;
        gscore[from] = 0;
        fscore[from] = h.cost(from, to);
        
        discovered.insert(from);
        
        
        while (!discovered.empty()) {
            node* current = discovered.extract(discovered.begin()).value();
            if (current == to) return reconstruct_path(came_from, current);
        
            
            for (node* neighbour : current->neighbours) {
                float tentative_gscore = gscore[current] + d.cost(current, neighbour);
                
                
                if (tentative_gscore < gscore[neighbour]) {
                    came_from[neighbour] = current;
                    gscore[neighbour] = tentative_gscore;
                    
                    discovered.erase(neighbour);
                    fscore[neighbour] = tentative_gscore + h.cost(neighbour, to);
                    discovered.insert(neighbour);
                }
            }
        }
        
        
        return {};
    }
}