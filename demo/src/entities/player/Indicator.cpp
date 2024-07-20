#include "Indicator.hpp"
#include "../../gui/Console.hpp"
#include "../../service/ServiceProvider.hpp"
#include "Player.hpp"

namespace player {

Indicator::Indicator(automa::ServiceProvider& svc) {}

void Indicator::init(automa::ServiceProvider& svc, int id) {
	meta.id = id;
	font.loadFromFile(svc.text.title_font);
	font.setSmooth(false);
	label.setCharacterSize(16);
	label.setLetterSpacing(0.6f);
	label.setFont(font);
	if (meta.id == 0) { color_fade = vfx::ColorFade({svc.styles.colors.ui_white, svc.styles.colors.red, svc.styles.colors.dark_fucshia}, 16, addition_time); }
	if (meta.id == 1) { color_fade = vfx::ColorFade({svc.styles.colors.ui_white, svc.styles.colors.goldenrod, svc.styles.colors.dark_orange}, 16, addition_time); }
	float fric{0.85f};
	gravitator = vfx::Gravitator(position, sf::Color::Transparent, 1.f);
	gravitator.collider.physics = components::PhysicsComponent(sf::Vector2<float>{fric, fric}, 2.0f);
}

void Indicator::shift() { position += {0.f, -20.f}; }

void Indicator::update(automa::ServiceProvider& svc, sf::Vector2<float> pos) {
	gravitator.update(svc);
	gravitator.set_target_position(pos);
	if (addition_limit.get_cooldown() == fadeout_time) { fadeout.start(fadeout_time); }
	if (fadeout.running()) { gravitator.set_target_position(pos + sf::Vector2<float>{60.f, 0.f}); }
	addition_limit.update();
	fadeout.update();
	color_fade.update();
	if (addition_limit.is_complete()) { variables.amount = 0; }
	position = gravitator.collider.physics.position + offset;
}

void Indicator::render(automa::ServiceProvider& svc, sf::RenderWindow& win, sf::Vector2<float> cam) {
	if (svc.greyblock_mode()) {
		return;
	} else if (!addition_limit.is_complete()) {
		label.setPosition(position + shadow - cam);
		meta.id == 0 ? label.setFillColor(svc.styles.colors.ui_black) : label.setFillColor(svc.styles.colors.dark_fucshia);
		win.draw(label);
		label.setFillColor(color_fade.color());
		label.setPosition(position - cam);
		win.draw(label);
	}
}

void Indicator::add(float amount) {
	variables.amount += amount;
	std::string sign = variables.amount >= 0 ? "+" : "";
	auto round = static_cast<int>(variables.amount);
	label.setString(sign + std::to_string(round));
	addition_limit.start(addition_time);
	color_fade.start();
}

void Indicator::set_position(sf::Vector2<float> pos) {
	position = pos;
	gravitator.set_position(pos);
}

} // namespace player
