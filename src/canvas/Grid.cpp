//
//  Grid.cpp
//  squid
//
//  Created by Alex Frasca on 12/26/22.
//

#include "Grid.hpp"

namespace pi {

Grid::Grid(sf::Vector2<uint32_t> d) : dimensions(d) {
	spacing = DEFAULT_SPACING;
	for (uint32_t i = 0; i < dimensions.x * dimensions.y; i++) {
		sf::Vector2<uint32_t> idx{static_cast<uint32_t>(std::floor(i % dimensions.x)), static_cast<uint32_t>(std::floor(i / dimensions.x))};
		sf::Vector2<float> pos{idx.x * spacing, idx.y * spacing};
		cells.push_back(Tile(idx, pos, 0, TILE_NULL));
	}
}

} // namespace pi
