//
//  Tool.cpp
//  Pioneer-Lab
//
//  Created by Alex Frasca on 10/3/20.
//

#include "Tool.hpp"

namespace pi {

bool Tool::in_bounds(sf::Vector2<uint32_t>& bounds) const { return scaled_position.x >= 0 && scaled_position.x < bounds.x && scaled_position.y >= 0 && scaled_position.y < bounds.y; }

bool Tool::is_paintable() const { return type == ToolType::brush || type == ToolType::fill; }

void Tool::update() {
	uint32_t posx = (uint32_t)position.x / CELL_SIZE;
	uint32_t posy = (uint32_t)position.y / CELL_SIZE;
	scaled_position = sf::Vector2<uint32_t>{posx, posy};
}

} // namespace pi
