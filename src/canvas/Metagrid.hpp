//
//  Metagrid.hpp
//  Pioneer-Lab
//
//  Created by Alex Frasca on 9/30/20.
//

#pragma once

#include <SFML/Graphics.hpp>
#include "../canvas/Grid.hpp"
#include <stdio.h>

namespace meta {

// the `Chunk` struct in the target system will contain knowledge of its neighbors

struct Chunk {
    sf::Vector2<uint32_t> position{}; // in the grid, not in the window
    uint32_t chunk_id{}; // unique to the chunk
    uint32_t room_id{}; // should match an existing level
    bool vacant{};
};

// a grid for storing all levels, where rooms have knowledge of adjacent rooms.
// the handling of this behavior is not in the scope of Pioneer, as this will
// only be relevant for scene transitions. Pioneer is responsible only for storing
// chunk and room ids appropriately.

class Metagrid {
    
public:
    
    Metagrid(sf::Vector2<uint32_t> dim);
    
    void add_room_at(sf::Vector2<uint32_t> pos, sf::Vector2<uint32_t> dim, uint32_t rm_id);
    void remove_room(uint32_t rm_id);
    
    sf::Vector2<uint32_t> dimensions{};
    std::vector<Chunk> chunks{};
    
};


}
