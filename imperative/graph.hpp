#pragma once

#include <imperative/common.hpp>

#include <vector>
#include <memory>
#include <string>


namespace imp {
    struct node {
        std::string name;
        vec2i position;
        
        std::vector<node*> neighbours;
        
        
        node(void) = default;
        node(std::string&& name, vec2i pos) : name(std::move(name)), position(pos) {}
    };
    

struct graph {
std::vector<std::unique_ptr<node>> nodes;


node& add_node(std::string&& name, vec2i where) {
nodes.emplace_back(
std::make_unique<node>(std::move(name), where)
);

return *nodes.back();
}


void add_edge(node& from, node& to) {
from.neighbours.push_back(&to);
to.neighbours.push_back(&from);
}
};
}