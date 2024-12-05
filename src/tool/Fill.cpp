//

#include "Tool.hpp"

namespace pi {

void Fill::handle_events(Canvas& canvas, sf::Event& e) {
	if (in_bounds(canvas.dimensions) && ready) { fill_section(canvas.tile_val_at(scaled_position.x, scaled_position.y, canvas.active_layer), tile, scaled_position.x, scaled_position.y, canvas); }
	update();
}

void Fill::handle_keyboard_events(Canvas& canvas, sf::Keyboard::Key& key) {}

void Fill::update() { Tool::update(); }

void Fill::render(sf::RenderWindow& win, sf::Vector2<float> offset) {}

void Fill::fill_section(uint8_t const prev_val, uint8_t const new_val, uint32_t i, uint32_t j, Canvas& canvas) {
	if (i < 0 || i >= canvas.dimensions.x || j < 0 || j >= canvas.dimensions.y) {
		return;
	} else if (canvas.tile_val_at(i, j, canvas.active_layer) != prev_val) {
		return;
	} else if (canvas.tile_val_at(i, j, canvas.active_layer) == new_val) {
		return;
	} else {

		canvas.edit_tile_at(i, j, new_val, canvas.active_layer);

		fill_section(prev_val, new_val, i + 1, j, canvas);
		fill_section(prev_val, new_val, i - 1, j, canvas);
		fill_section(prev_val, new_val, i, j + 1, canvas);
		fill_section(prev_val, new_val, i, j - 1, canvas);
	}
}

void Fill::store_tile(int index) { tile = index; }

void Fill::clear() {}

} // namespace pi
