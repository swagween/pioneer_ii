//
//  Layer.cpp
//  Pioneer-Lab
//
//  Created by Alex Frasca on 9/30/20.
//

#include "Layer.hpp"

namespace canvas {

void Layer::clear() {
    for(auto& cell : grid.cells) {
        cell.value = 0;
    }
}

void Layer::erase() {
    grid.cells.clear();
}

}
