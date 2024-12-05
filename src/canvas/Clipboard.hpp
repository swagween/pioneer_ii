//
//  Clipboard.hpp
//  Pioneer Lab
//
//  Created by Alex Frasca on 12/4/22.
//

#pragma once

#include "Tile.hpp"
#include <vector>
#include <stdio.h>
#include <cassert>

namespace pi {

class Clipboard {

  public:
	Clipboard(sf::Vector2<uint32_t> dimensions);
	void write_to_clipboard(uint8_t value, size_t i, size_t j, size_t layer);
	void clear_clipboard();
	uint8_t get_value_at(size_t i, size_t j);
	uint8_t get_value_at(size_t i, size_t j, size_t layer);
	[[nodiscard]] auto empty() const -> bool { return cell_values.empty(); }

  private:
	sf::Vector2<uint32_t> dimensions{};
	std::vector<std::vector<uint8_t>> cell_values{};
};

} // namespace pi
