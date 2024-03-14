//
//  Grid.hpp
//  squid
//
//  Created by Alex Frasca on 12/26/22.
//
#pragma once

#include "Tile.hpp"
#include <stdio.h>
#include <vector>
#include <cmath>

namespace canvas {

class Grid {
public:
    
    Grid();
    Grid(sf::Vector2<uint32_t> d);
    
    sf::Vector2<uint32_t> dimensions{};
    float spacing{};
    
    std::vector<Tile> cells{};
    
    void initialize();
    void update();
    
    void pushCells(int i);
    
    
    TILE_TYPE lookupType(int idx);
    
};

}
