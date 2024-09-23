//
//  Brush.cpp
//  Pioneer-Lab
//
//  Created by Alex Frasca on 10/3/20.
//

#include "Tool.hpp"

namespace pi {

void Brush::handle_events(Canvas& canvas, sf::Event& e) {
	if (in_bounds(canvas.dimensions) && ready) {
		for (int i = 0; i < size; i++) {
			for (int j = 0; j < size; j++) { canvas.edit_tile_at(scaled_position.x - i, scaled_position.y - j, tile, canvas.active_layer); }
		}
	}
	update();
}

void Brush::handle_keyboard_events(Canvas& canvas, sf::Keyboard::Key& key) {}

void Brush::update() { Tool::update(); }

void Brush::render(sf::RenderWindow& win, sf::Vector2<float> offset) {}

void Brush::store_tile(int index) { tile = index; }

} // namespace pi
