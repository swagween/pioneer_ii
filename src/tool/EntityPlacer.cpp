
#include "Tool.hpp"

namespace pi {

	void EntityPlacer::handle_events(Canvas& canvas, sf::Event& e) {
		if (in_bounds(canvas.dimensions) && ready) {
			bool anim_available{ true };
			bool crit_available{ true };
			for(auto& animator : canvas.animators) {
				if (animator.position == scaled_position - current_animator.dimensions + sf::Vector2<uint32_t>(1, 1)) { anim_available = false; }
			}
			for(auto& critter : canvas.critters) {
				if (critter.position == scaled_position) { crit_available = false; }
			}
			switch (ent_type) {
			case ENTITY_TYPE::PORTAL: canvas.portals.push_back(current_portal); break;
			case ENTITY_TYPE::INSPECTABLE: canvas.inspectables.push_back(current_inspectable); break;
			case ENTITY_TYPE::BED: canvas.beds.push_back(Bed(scaled_position)); break;
			case ENTITY_TYPE::PLATFORM: canvas.platforms.push_back(current_platform); break;
			case ENTITY_TYPE::PLAYER_PLACER: canvas.player_start = scaled_position; break;
			case ENTITY_TYPE::SAVE_POINT:
				canvas.save_point.position = scaled_position;
				canvas.save_point.placed = true;
				break;
			case ENTITY_TYPE::CRITTER:
				if (crit_available) { canvas.critters.push_back(current_critter); }
				break;
			case ENTITY_TYPE::ANIMATOR:
				if (anim_available) { canvas.animators.push_back(current_animator); }
				break;
			case ENTITY_TYPE::CHEST:
				canvas.chests.push_back(current_chest); break;
			case ENTITY_TYPE::SWITCH_BUTTON:
				canvas.switch_buttons.push_back(current_switch); break;
			case ENTITY_TYPE::SWITCH_BLOCK:
				if (!canvas.has_switch_block_at(current_switch_block.position)) { canvas.switch_blocks.push_back(current_switch_block); }
				break;

				// for erasing existing entities
			case ENTITY_TYPE::ENTITY_EDITOR:
				std::erase_if(canvas.animators, [this](auto& c) { return c.position == scaled_position; });
				std::erase_if(canvas.critters, [this](auto& c) { return c.position == scaled_position; });
				std::erase_if(canvas.inspectables, [this](auto& c) { return c.position == scaled_position; });
				std::erase_if(canvas.portals, [this](auto& c) { return c.position == scaled_position; });
				std::erase_if(canvas.platforms, [this](auto& c) { return c.position == scaled_position; });
				std::erase_if(canvas.chests, [this](auto& c) { return c.position == scaled_position; });
				std::erase_if(canvas.switch_blocks, [this](auto& c) { return c.position == scaled_position; });
				std::erase_if(canvas.switch_buttons, [this](auto& c) { return c.position == scaled_position; });
				std::erase_if(canvas.beds, [this](auto& c) { return c.position == scaled_position; });
				break;
			}
			if (ent_type != ENTITY_TYPE::ANIMATOR && ent_type != ENTITY_TYPE::CRITTER && ent_type != ENTITY_TYPE::ENTITY_EDITOR && ent_type != ENTITY_TYPE::PLATFORM && ent_type != ENTITY_TYPE::SWITCH_BLOCK) { trigger_switch = true; }
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
		current_portal.position = scaled_position - current_portal.dimensions + sf::Vector2<uint32_t>(1, 1);
		current_inspectable.position = scaled_position - current_inspectable.dimensions + sf::Vector2<uint32_t>(1, 1);
		current_animator.position = scaled_position - current_animator.dimensions + sf::Vector2<uint32_t>(1, 1);
		current_critter.position = scaled_position;
		current_platform.position = scaled_position;
		current_chest.position = scaled_position;
		current_switch.position = scaled_position;
		current_switch_block.position = scaled_position;
	}

	void EntityPlacer::render(sf::RenderWindow& win, sf::Vector2<float> offset) {
		sf::RectangleShape box{};

		box.setOutlineColor(sf::Color{ 200, 200, 200, 80 });
		box.setFillColor(sf::Color{ 100, 190, 190, 80 });
		box.setOutlineThickness(-2);
		box.setSize({ CELL_SIZE, CELL_SIZE });

		if (ent_type == ENTITY_TYPE::PORTAL) {
			for (uint32_t i = 0; i < current_portal.dimensions.x; ++i) {
				for (uint32_t j = 0; j < current_portal.dimensions.y; ++j) {
					box.setPosition(current_portal.position.x * CELL_SIZE + i * CELL_SIZE + offset.x, current_portal.position.y * CELL_SIZE + j * CELL_SIZE + offset.y);
					win.draw(box);
				}
			}
		} else if(ent_type == ENTITY_TYPE::ANIMATOR) {
			for (uint32_t i = 0; i < current_animator.dimensions.x; ++i) {
				for (uint32_t j = 0; j < current_animator.dimensions.y; ++j) {
					box.setPosition(current_animator.position.x * CELL_SIZE + i * CELL_SIZE + offset.x, current_animator.position.y * CELL_SIZE + j * CELL_SIZE + offset.y);
					win.draw(box);
				}
			}
		} else {
			box.setPosition(scaled_position.x * CELL_SIZE + offset.x, scaled_position.y * CELL_SIZE + offset.y);
			win.draw(box);
		}
	}

	void EntityPlacer::store_portal(sf::Vector2<uint32_t> dim, bool activate_on_contact, int src_id, int dest_id) {
		current_portal = Portal(dim, activate_on_contact, src_id, dest_id);
	}

	void EntityPlacer::store_inspectable(sf::Vector2<uint32_t> dim, bool activate_on_contact, std::string msg) {
		current_inspectable = Inspectable(dim, activate_on_contact, msg);
	}

	void EntityPlacer::store_critter(CRITTER_TYPE type) {
		current_critter = Critter(type);
	}

	void EntityPlacer::store_animator(sf::Vector2<uint32_t> dim, int id, bool automatic, bool foreground) {
		current_animator = Animator(dim, id, automatic, foreground);
	}

	void EntityPlacer::store_tile(int index) {}

}
