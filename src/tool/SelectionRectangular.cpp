
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
			clipboard = Clipboard(selection.dimensions);
		}
	}
	update();
}

void SelectionRectangular::handle_keyboard_events(Canvas& canvas, sf::Keyboard::Key& key) {
	if (!clipboard) { return; }
	if (key == sf::Keyboard::X) { cut(canvas); }
	if (key == sf::Keyboard::C) { copy(canvas); }
	if (key == sf::Keyboard::V) {
		if (!clipboard.value().empty()) { paste(canvas); }
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

void SelectionRectangular::cut(Canvas& canvas) {
	if (!clipboard) { return; }
	clipboard.value().clear_clipboard();
	for (uint32_t i = 0; i < selection.dimensions.x; ++i) {
		for (uint32_t j = 0; j < selection.dimensions.y; ++j) {
			for (auto k{0}; k < canvas.get_layers().layers.size(); ++k) {
				if (!pervasive && k != canvas.active_layer) { continue; }
				clipboard.value().write_to_clipboard(canvas.tile_val_at(selection.position.x + i, selection.position.y + j, k), i, j, k);
				canvas.erase_at(selection.position.x + i, selection.position.y + j, k);
			}
		}
	}
}

void SelectionRectangular::copy(Canvas& canvas) {
	if (!clipboard) { return; }
	clipboard.value().clear_clipboard();
	for (uint32_t i = 0; i < selection.dimensions.x; ++i) {
		for (uint32_t j = 0; j < selection.dimensions.y; ++j) {
			for (auto k{0}; k < canvas.get_layers().layers.size(); ++k) {
				if (!pervasive && k != canvas.active_layer) { continue; }
				clipboard.value().write_to_clipboard(canvas.tile_val_at(selection.position.x + i, selection.position.y + j, k), i, j, k);
			}
		}
	}
}

void SelectionRectangular::paste(Canvas& canvas) {
	if (!clipboard) { return; }
	canvas.save_state(*this);
	for (uint32_t i = 0; i < selection.dimensions.x; ++i) {
		for (uint32_t j = 0; j < selection.dimensions.y; ++j) {
			if (scaled_position.x + i < canvas.dimensions.x && scaled_position.y + j < canvas.dimensions.y) {
				if (pervasive) {
					for (auto k{0}; k < canvas.get_layers().layers.size(); ++k) { canvas.edit_tile_at(scaled_position.x + i, scaled_position.y + j, clipboard.value().get_value_at(i, j, k), k); }
				} else {
					canvas.edit_tile_at(scaled_position.x + i, scaled_position.y + j, clipboard.value().get_value_at(i, j), canvas.active_layer);
				}
			}
		}
	}
}

void SelectionRectangular::clear() { clipboard = {}; }

} // namespace pi
