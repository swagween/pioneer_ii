//
//  Fill.cpp
//  Pioneer-Lab
//
//  Created by Alex Frasca on 10/3/20.
//

#include "Tool.hpp"
#include "../util/ServiceLocator.hpp"

namespace tool {

void Fill::handle_events(canvas::Canvas& canvas, sf::Event& e) {
    if(in_bounds(canvas.dimensions) && ready) {
        fill_section(canvas.tile_val_at(scaled_position.x, scaled_position.y, pi::svc::active_layer), tile, scaled_position.x, scaled_position.y, canvas);
    }
    update();
}

void Fill::handle_keyboard_events(canvas::Canvas& canvas, sf::Keyboard::Key& key) {
    
}

void Fill::update() {
    tool::Tool::update();
}

void Fill::render(sf::RenderWindow& win, sf::Vector2<float> offset) {
    
}

void Fill::fill_section(const uint8_t prev_val, const uint8_t new_val, uint32_t i, uint32_t j, canvas::Canvas& canvas) {
    if(i < 0 || i >= canvas.dimensions.x || j < 0 || j >= canvas.dimensions.y) {
        return;
    } else if(canvas.tile_val_at(i, j, pi::svc::active_layer) != prev_val) {
        return;
    } else if(canvas.tile_val_at(i, j, pi::svc::active_layer) == new_val) {
        return;
    } else {
        
        canvas.edit_tile_at(i, j, new_val, pi::svc::active_layer);
        
        fill_section(prev_val, new_val, i + 1, j, canvas);
        fill_section(prev_val, new_val, i - 1, j, canvas);
        fill_section(prev_val, new_val, i, j + 1, canvas);
        fill_section(prev_val, new_val, i, j - 1, canvas);
        
    }
}

void Fill::store_tile(int index) {
    tile = index;
}

}
