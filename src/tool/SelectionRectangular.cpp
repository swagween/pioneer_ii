
#include "Tool.hpp"

namespace pi {

void SelectionRectangular::handle_events(Canvas& canvas, sf::Event& e) {
	sf::Vector2<uint32_t> dim = {static_cast<uint32_t>(canvas.dimensions.x + 1), static_cast<uint32_t>(canvas.dimensions.y + 1)};
	if (in_bounds(dim) && ready) {
		if (active) {
			if (just_clicked) {
				clicked_position = position;
				scaled_clicked_position.x = static_cast<uint32_t>(clicked_position.x / f_cell_size);
				scaled_clicked_position.y = static_cast<uint32_t>(clicked_position.y / f_cell_size);
				selection = SelectBox(scaled_clicked_position, {0, 0});
				just_clicked = false;
			}
			// I'm being long-winded here to provision for the mouse
			// being dragged above or left of the clicked position
			sf::Vector2<uint32_t> adjustment{};
			if (scaled_position.x > scaled_clicked_position.x) {
				adjustment.x = scaled_position.x - selection.position.x;
			} else {
				adjustment.x = scaled_clicked_position.x - scaled_position.x;
				selection.position.x = scaled_position.x;
			}
			if (scaled_position.y > scaled_clicked_position.y) {
				adjustment.y = scaled_position.y - selection.position.y;
			} else {
				adjustment.y = scaled_clicked_position.y - scaled_position.y;
				selection.position.y = scaled_position.y;
			}
			selection.adjust(adjustment);
		}
	}
	update();
}

void SelectionRectangular::handle_keyboard_events(Canvas& canvas, sf::Keyboard::Key& key) {
	if (key == sf::Keyboard::C) { copy(canvas); }
	if (key == sf::Keyboard::V) {
		if (!clipboard.cell_values.empty()) { paste(canvas); }
	}
}

void SelectionRectangular::update() { Tool::update(); }

void SelectionRectangular::render(sf::RenderWindow& win, sf::Vector2<float> offset) {
	sf::RectangleShape box{};

	box.setOutlineColor(sf::Color{200, 200, 200, 80});
	box.setFillColor(sf::Color{150, 190, 110, 80});
	box.setOutlineThickness(-2);
	box.setSize({CELL_SIZE, CELL_SIZE});

	for (uint32_t i = 0; i < selection.dimensions.x; ++i) {
		for (uint32_t j = 0; j < selection.dimensions.y; ++j) {
			box.setPosition(selection.position.x * CELL_SIZE + i * CELL_SIZE + offset.x, selection.position.y * CELL_SIZE + j * CELL_SIZE + offset.y);
			win.draw(box);
		}
	}
}

void SelectionRectangular::store_tile(int index) {}

void SelectionRectangular::copy(Canvas& canvas) {
	clipboard.clear_clipboard();
	for (uint32_t i = 0; i < selection.dimensions.x; ++i) {
		for (uint32_t j = 0; j < selection.dimensions.y; ++j) { clipboard.cell_values.push_back(canvas.tile_val_at(selection.position.x + i, selection.position.y + j, canvas.active_layer)); }
	}
}

void SelectionRectangular::paste(Canvas& canvas) {
	canvas.save_state(*this);
	for (uint32_t i = 0; i < selection.dimensions.x; ++i) {
		for (uint32_t j = 0; j < selection.dimensions.y; ++j) {
			if (scaled_position.x + i < canvas.dimensions.x && scaled_position.y + j < canvas.dimensions.y) {
				canvas.edit_tile_at(scaled_position.x + i, scaled_position.y + j, clipboard.cell_values.at(j + i * selection.dimensions.y), canvas.active_layer);
			}
		}
	}
}

} // namespace pi
