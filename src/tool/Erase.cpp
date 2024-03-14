//
//  Erase.cpp
//  Pioneer-Lab
//
//  Created by Alex Frasca on 10/3/20.
//

#include "Tool.hpp"
#include "../util/ServiceLocator.hpp"

namespace tool {

void Erase::handle_events(canvas::Canvas& canvas, sf::Event& e) {
    if(in_bounds(canvas.dimensions) && ready) {
        for(int i = 0; i < size; i++) {
            for(int j = 0; j < size; j++) {
                canvas.edit_tile_at(scaled_position.x - i, scaled_position.y - j, 0, pi::svc::active_layer);
            }
        }
    }
    update();
}

void Erase::handle_keyboard_events(canvas::Canvas& canvas, sf::Keyboard::Key& key) {
    
}

void Erase::update() {
    tool::Tool::update();
}

void Erase::render(sf::RenderWindow& win, sf::Vector2<float> offset) {
    
}

void Erase::store_tile(int index) {
    tile = index;
}

}
