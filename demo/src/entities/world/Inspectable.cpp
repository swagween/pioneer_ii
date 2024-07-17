
#include "Inspectable.hpp"
#include "../player/Player.hpp"
#include "../../gui/Console.hpp"
#include "../../service/ServiceProvider.hpp"

namespace entity {

Inspectable::Inspectable(automa::ServiceProvider& svc, Vecu16 dim, Vecu16 pos, std::string_view key, int room_id, int alternates) : scaled_dimensions(dim), scaled_position(pos), key(key), alternates(alternates) {
	dimensions = static_cast<Vec>(dim * svc.constants.u32_cell_size);
	position = static_cast<Vec>(pos * svc.constants.u32_cell_size);
	bounding_box = shape::Shape(dimensions);
	bounding_box.set_position(position);
	sprite.setTexture(svc.assets.t_inspectable);
	animation.end();
	id = key.data() + std::to_string(room_id);
	iid = room_id;
}

void Inspectable::update(automa::ServiceProvider& svc, player::Player& player, gui::Console& console, dj::Json& set) {
	position = static_cast<Vec>(scaled_position * svc.constants.u32_cell_size);
	dimensions = static_cast<Vec>(scaled_dimensions * svc.constants.u32_cell_size);
	bounding_box.set_position(position);
	activated = false;
	animation.update();

	//check for quest-based alternates
	auto quest_status = svc.quest.get_progression(fornani::QuestType::inspectable, iid);
	if (quest_status > 0) { current_alt = quest_status; }

	if (bounding_box.overlaps(player.collider.hurtbox)) {
		if (!flags.test(InspectableFlags::hovered)) { flags.set(InspectableFlags::hovered_trigger); }
		flags.set(InspectableFlags::hovered);
		if (player.controller.inspecting()) { activated = true; }
	} else {
		flags.reset(InspectableFlags::hovered);
	}
	if (activated) {
		console.set_source(set);
		if (alternates < 1) { console.load_and_launch(key); } else {
			std::string new_key = key + std::to_string(current_alt);
			//std::cout << "New key: " << new_key << "\n";
			//std::cout << "Current alt: " << current_alt << "\n";
			console.load_and_launch(new_key);
		}
	}
	if (flags.test(InspectableFlags::hovered) && flags.consume(InspectableFlags::hovered_trigger) && animation.complete()) {
		animation.set_params(params);
	}
	if (console.get_key() == key) { flags.set(InspectableFlags::engaged); }
	if (flags.test(InspectableFlags::engaged)) {
		if (player.transponder.shipments.quest.get_residue() == 9) {
			flags.set(InspectableFlags::destroy);
			svc.data.destroy_inspectable(id);
		}
	}
	if (console.is_complete()) { flags.reset(InspectableFlags::engaged); }
}

void Inspectable::render(automa::ServiceProvider& svc, sf::RenderWindow& win, Vec campos) {
	sf::RectangleShape box{};
	auto u = 0;
	auto v = animation.get_frame() * 32;
	sprite.setPosition(position + offset - campos);
	sprite.setTextureRect(sf::IntRect{{u, v}, {32, 32}});

	if (svc.greyblock_mode()) {
		if (activated) {
			box.setFillColor(sf::Color{80, 180, 120, 100});
		} else {
			box.setFillColor(sf::Color{180, 120, 80, 100});
		}
		box.setOutlineColor(sf::Color::White);
		box.setOutlineThickness(-1);
		box.setPosition(bounding_box.position - campos);
		box.setSize(dimensions);
		win.draw(box);
	} else {
		win.draw(sprite);
	}
}

} // namespace entity
