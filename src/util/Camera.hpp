//
//  Camera.hpp
//

#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>

namespace pi {

float const CAM_FRICTION = 0.9f;
float const CAM_MASS = 1.0f;
float const CAM_GRAV = 0.003f;

int const CX_OFFSET = 60;
int const CY_OFFSET = 60;

float const CAM_BOUNDARY = 2048.0f;

sf::Vector2<uint32_t> const aspect_ratio{3840, 2160};
sf::Vector2<uint32_t> const screen_dimensions{aspect_ratio.x / 2, aspect_ratio.y / 2};

class Camera {
  public:
	Camera() { bounding_box = sf::Rect<float>({0.0f, 0.0f}, {(float)screen_dimensions.x, (float)screen_dimensions.y}); }

	void update() {
		bounding_box.left = position.x;
		bounding_box.top = position.y;
	}

	void move(sf::Vector2<float> distance) { position += distance; }
	void set_position(sf::Vector2<float> new_pos) { position = new_pos; }

	sf::Vector2<float> position{};
	sf::Rect<float> bounding_box{};
};

} // namespace pi