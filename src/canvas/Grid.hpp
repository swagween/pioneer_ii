
#pragma once

#include "Tile.hpp"
#include <stdio.h>
#include <vector>
#include <cmath>

namespace pi {

class Grid {
  public:
	Grid(sf::Vector2<uint32_t> d);
	sf::Vector2<uint32_t> dimensions{};
	float spacing{};
	std::vector<Tile> cells{};
};

} // namespace pi
