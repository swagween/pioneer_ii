
#include "Tool.hpp"

namespace pi {

bool Tool::in_bounds(sf::Vector2<uint32_t>& bounds) const { return scaled_position().x >= 0 && scaled_position().x < bounds.x && scaled_position().y >= 0 && scaled_position().y < bounds.y; }

bool Tool::is_paintable() const { return type == ToolType::brush || type == ToolType::fill; }

void Tool::update() {}

void Tool::set_position(sf::Vector2<float> to_position) { position = to_position; }

void Tool::set_window_position(sf::Vector2<float> to_position) { window_position = to_position; }

void Tool::reset() {
	active = false;
	just_clicked = true;
}

void Tool::activate() { active = true; }

} // namespace pi
