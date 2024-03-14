//
//  Tool.cpp
//  Pioneer-Lab
//
//  Created by Alex Frasca on 10/3/20.
//

#include "Tool.hpp"
#include "../util/ServiceLocator.hpp"

namespace tool {

bool Tool::in_bounds(sf::Vector2<uint32_t>& bounds) const {
    return scaled_position.x >= 0 && scaled_position.x < bounds.x && scaled_position.y >= 0 && scaled_position.y < bounds.y;
}

bool Tool::is_paintable() const {
    return type == TOOL_TYPE::BRUSH || type == TOOL_TYPE::FILL;
}

void Tool::update() {
    uint32_t posx = (uint32_t)position.x/canvas::CELL_SIZE;
    uint32_t posy = (uint32_t)position.y/canvas::CELL_SIZE;
    scaled_position = sf::Vector2<uint32_t>{posx, posy};
    tile = pi::svc::selectedBlockLocator.get();
}



}
