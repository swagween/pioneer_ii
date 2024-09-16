//
//  Grid.cpp
//  squid
//
//  Created by Alex Frasca on 12/26/22.
//

#include "Grid.hpp"

namespace pi {

Grid::Grid() {
    spacing = DEFAULT_SPACING;
}

Grid::Grid(sf::Vector2<uint32_t> d) : dimensions(d) {
    
    spacing = DEFAULT_SPACING;
    
    initialize();
    
}

void Grid::initialize() {
    
    cells.clear();
    
    for(int i = 0; i < dimensions.x * dimensions.y; i++) {
        
        // calculate index values
        uint32_t xidx = std::floor(i%dimensions.x);
        uint32_t yidx = std::floor(i/dimensions.x);
        
        //calculate positions with offsets
        float xpos, ypos;
        if(yidx % 2 == 0) {
            xpos = xidx*spacing;
        } else {
            xpos = xidx*spacing;
        }
        
        if(xidx % 2 == 0) {
            ypos = yidx*spacing;
        } else {
            ypos = yidx*spacing;
        }
        cells.push_back(Tile({xidx, yidx}, {xpos, ypos}, 0, TILE_NULL));
        
    }
    
}

void Grid::update() {
    
    for(int i = 0; i < dimensions.x * dimensions.y; i++) {
        
        pushCells(i);
        
    }
    
}

void Grid::pushCells(int i) {
    // calculate index values
    uint32_t xidx = std::floor(i%dimensions.x);
    uint32_t yidx = std::floor(i/dimensions.x);
    
    //calculate positions with offsets
    float xpos, ypos;
    if(yidx % 2 == 0) {
        xpos = xidx*spacing;
    } else {
        xpos = xidx*spacing;
    }
    
    if(xidx % 2 == 0) {
        ypos = yidx*spacing;
    } else {
        ypos = yidx*spacing;
    }
    
    //populate the grid
    cells.at(i).index = sf::Vector2<uint32_t>(xidx, yidx);
    cells.at(i).position = sf::Vector2<float>(xpos, ypos);
}

}
