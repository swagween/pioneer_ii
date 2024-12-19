
#include "Tool.hpp"

namespace pi {

	void EntityPlacer::handle_events(Canvas& canvas, sf::Event& e) {
		if (in_bounds(canvas.dimensions) && ready && canvas.editable()) {
			bool anim_available{ true };
			bool crit_available{ true };
			for(auto& animator : canvas.entities.variables.animators) {
				if (animator.position == scaled_position() - current_animator.dimensions + sf::Vector2<uint32_t>(1, 1)) { anim_available = false; }
			}
			for(auto& critter : canvas.entities.variables.critters) {
				if (critter.position == scaled_position()) { crit_available = false; }
			}
			switch (ent_type) {
			case ENTITY_TYPE::PORTAL: canvas.entities.variables.portals.push_back(current_portal); break;
			case ENTITY_TYPE::INSPECTABLE: canvas.entities.variables.inspectables.push_back(current_inspectable); break;
			case ENTITY_TYPE::BED: canvas.entities.variables.beds.push_back(Bed(scaled_position())); break;
			case ENTITY_TYPE::INTERACTIVE_SCENERY: canvas.entities.variables.interactive_scenery.push_back(current_interactive_scenery); break;
			case ENTITY_TYPE::SCENERY: canvas.entities.variables.scenery.push_back(current_scenery); break;
			case ENTITY_TYPE::PLATFORM: canvas.entities.variables.platforms.push_back(current_platform); break;
			case ENTITY_TYPE::PLAYER_PLACER: canvas.entities.variables.player_start = scaled_position(); break;
			case ENTITY_TYPE::SAVE_POINT:
				canvas.entities.variables.save_point.position = scaled_position();
				canvas.entities.variables.save_point.placed = true;
				break;
			case ENTITY_TYPE::CRITTER:
				if (crit_available) { canvas.entities.variables.critters.push_back(current_critter); }
				break;
			case ENTITY_TYPE::ANIMATOR:
				if (anim_available) { canvas.entities.variables.animators.push_back(current_animator); }
				break;
			case ENTITY_TYPE::CHEST:
				canvas.entities.variables.chests.push_back(current_chest); break;
			case ENTITY_TYPE::SWITCH_BUTTON:
				canvas.entities.variables.switch_buttons.push_back(current_switch); break;
			case ENTITY_TYPE::SWITCH_BLOCK:
				if (!canvas.entities.has_switch_block_at(current_switch_block.position)) { canvas.entities.variables.switch_blocks.push_back(current_switch_block); }
				break;

				// for erasing existing entities
			case ENTITY_TYPE::ENTITY_EDITOR:
				std::erase_if(canvas.entities.variables.animators, [this](auto& c) { return c.position == scaled_position(); });
				std::erase_if(canvas.entities.variables.critters, [this](auto& c) { return c.position == scaled_position(); });
				std::erase_if(canvas.entities.variables.inspectables, [this](auto& c) { return c.position == scaled_position(); });
				std::erase_if(canvas.entities.variables.portals, [this](auto& c) { return c.position == scaled_position(); });
				std::erase_if(canvas.entities.variables.platforms, [this](auto& c) { return c.position == scaled_position(); });
				std::erase_if(canvas.entities.variables.chests, [this](auto& c) { return c.position == scaled_position(); });
				std::erase_if(canvas.entities.variables.switch_blocks, [this](auto& c) { return c.position == scaled_position(); });
				std::erase_if(canvas.entities.variables.switch_buttons, [this](auto& c) { return c.position == scaled_position(); });
				std::erase_if(canvas.entities.variables.beds, [this](auto& c) { return c.position == scaled_position(); });
				std::erase_if(canvas.entities.variables.interactive_scenery, [this](auto& c) { return c.position == scaled_position(); });
				std::erase_if(canvas.entities.variables.scenery, [this](auto& c) { return c.position == scaled_position(); });
				break;
			}
			if (ent_type != ENTITY_TYPE::ANIMATOR && ent_type != ENTITY_TYPE::CRITTER && ent_type != ENTITY_TYPE::ENTITY_EDITOR && ent_type != ENTITY_TYPE::PLATFORM && ent_type != ENTITY_TYPE::SWITCH_BLOCK &&
				ent_type != ENTITY_TYPE::INTERACTIVE_SCENERY && ent_type != ENTITY_TYPE::SCENERY) {
				trigger_switch = true;
			}
		}
		update();
	}

	void EntityPlacer::handle_keyboard_events(Canvas& canvas, sf::Keyboard::Key& key) {
		if (key == sf::Keyboard::Q) {
			trigger_switch = true;
		}
	}

	void EntityPlacer::update() {
		Tool::update();
		current_portal.position = scaled_position() - current_portal.dimensions + sf::Vector2<uint32_t>(1, 1);
		current_inspectable.position = scaled_position() - current_inspectable.dimensions + sf::Vector2<uint32_t>(1, 1);
		current_animator.position = scaled_position() - current_animator.dimensions + sf::Vector2<uint32_t>(1, 1);
		current_critter.position = scaled_position();
		current_platform.position = scaled_position();
		current_chest.position = scaled_position();
		current_switch.position = scaled_position();
		current_switch_block.position = scaled_position();
		current_interactive_scenery.position = scaled_position();
		current_scenery.position = scaled_position();
	}

	void EntityPlacer::render(Canvas& canvas, sf::RenderWindow& win, sf::Vector2<float> offset, bool transformed) {
		sf::RectangleShape box{};

		box.setOutlineColor(sf::Color{ 200, 200, 200, 80 });
		box.setFillColor(sf::Color{ 100, 190, 190, 80 });
		box.setOutlineThickness(-2);
		box.setSize({ canvas.f_cell_size(), canvas.f_cell_size() });

		if (ent_type == ENTITY_TYPE::PORTAL) {
			for (uint32_t i = 0; i < current_portal.dimensions.x; ++i) {
				for (uint32_t j = 0; j < current_portal.dimensions.y; ++j) {
					box.setPosition(current_portal.position.x * canvas.f_cell_size() + i * canvas.f_cell_size() + offset.x, current_portal.position.y * canvas.f_cell_size() + j * canvas.f_cell_size() + offset.y);
					win.draw(box);
				}
			}
		} else if(ent_type == ENTITY_TYPE::ANIMATOR) {
			for (uint32_t i = 0; i < current_animator.dimensions.x; ++i) {
				for (uint32_t j = 0; j < current_animator.dimensions.y; ++j) {
					box.setPosition(current_animator.position.x * canvas.f_cell_size() + i * canvas.f_cell_size() + offset.x, current_animator.position.y * canvas.f_cell_size() + j * canvas.f_cell_size() + offset.y);
					win.draw(box);
				}
			}
		} else {
			box.setPosition(scaled_position().x * canvas.f_cell_size() + offset.x, scaled_position().y * canvas.f_cell_size() + offset.y);
			win.draw(box);
		}
	}

	void EntityPlacer::store_portal(sf::Vector2<uint32_t> dim, bool activate_on_contact, int src_id, int dest_id) {
		current_portal = Portal(dim, activate_on_contact, src_id, dest_id);
	}

	void EntityPlacer::store_inspectable(sf::Vector2<uint32_t> dim, bool activate_on_contact, std::string msg) {
		current_inspectable = Inspectable(dim, activate_on_contact, msg);
	}

	void EntityPlacer::store_critter(int type) {
		current_critter = Critter(type);
	}

	void EntityPlacer::store_animator(sf::Vector2<uint32_t> dim, int id, bool automatic, bool foreground) {
		current_animator = Animator(dim, id, automatic, foreground);
	}

	void EntityPlacer::store_tile(int index) {}

	void EntityPlacer::clear() {}

}
