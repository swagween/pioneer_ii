//
//  Metagrid.cpp
//  Pioneer-Lab
//
//  Created by Alex Frasca on 9/30/20.
//

#include "Metagrid.hpp"
#include "../util/Lookup.hpp"

namespace meta {

Metagrid::Metagrid(sf::Vector2<uint32_t> dim) : dimensions(dim) {
    for(int i = 0; i < dimensions.x; ++i) {
        for(int j = 0; j < dimensions.y; ++j) {
            chunks.push_back(Chunk{ {(uint32_t)i, (uint32_t)j}, (uint32_t)(i + j * dimensions.y), 0, true });
        }
    }
}

void Metagrid::add_room_at(sf::Vector2<uint32_t> pos, sf::Vector2<uint32_t> dim, uint32_t rm_id) {
    for(int i = 0; i < dim.x; ++i) {
        for(int j = 0; j < dim.y; ++j) {
            chunks.at(pos.x + i + pos.y + j * dimensions.y).room_id = rm_id;
            chunks.at(pos.x + i + pos.y + j * dimensions.y).vacant = false;
        }
    }
}

void Metagrid::remove_room(uint32_t rm_id) {
    
}

}

