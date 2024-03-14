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

namespace canvas {

class Clipboard {
    
public:
    
    Clipboard();
    
    void write_to_clipboard(std::vector<uint8_t>& selection);
    void clear_clipboard();
    uint8_t get_value_at(uint32_t i, uint32_t j);
    
    sf::Vector2<uint32_t> dimensions{};
    std::vector<uint8_t> cell_values;
    
};

}
