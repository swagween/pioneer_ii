//
//  EntityPlacer.cpp
//  Pioneer-Lab
//
//  Created by Alex Frasca on 10/3/20.
//

#include "Tool.hpp"
#include "../util/ServiceLocator.hpp"

namespace tool {

	void EntityPlacer::handle_events(canvas::Canvas& canvas, sf::Event& e) {
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
			case ENTITY_TYPE::PORTAL:       canvas.portals.push_back(current_portal); break;
			case ENTITY_TYPE::INSPECTABLE:  canvas.inspectables.push_back(current_inspectable); break;
			case ENTITY_TYPE::PLATFORM:		 canvas.platforms.push_back(current_platform); break;
			case ENTITY_TYPE::PLAYER_PLACER:  pi::svc::playerStartLocator.get() = scaled_position; break;
			case ENTITY_TYPE::CRITTER:		if (crit_available) { canvas.critters.push_back(current_critter); } break;
			case ENTITY_TYPE::ANIMATOR:     if (anim_available) { canvas.animators.push_back(current_animator); } break;

				//for editing existing entities
			case ENTITY_TYPE::ENTITY_EDITOR:
				std::erase_if(canvas.animators,		[this](auto&& const c) { return c.position == scaled_position;  });
				std::erase_if(canvas.critters,		[this](auto&& const c) { return c.position == scaled_position;  });
				std::erase_if(canvas.inspectables,	[this](auto&& const c) { return c.position == scaled_position;  });
				std::erase_if(canvas.portals,		[this](auto&& const c) { return c.position == scaled_position;  });
				std::erase_if(canvas.platforms,		[this](auto&& const c) { return c.position == scaled_position;  });
				break;
			}
			if (ent_type != ENTITY_TYPE::ANIMATOR && ent_type != ENTITY_TYPE::CRITTER && ent_type != ENTITY_TYPE::ENTITY_EDITOR && ent_type != ENTITY_TYPE::PLATFORM) { trigger_switch = true; }
		}
		update();
	}

	void EntityPlacer::handle_keyboard_events(canvas::Canvas& canvas, sf::Keyboard::Key& key) {
		if (key == sf::Keyboard::Q) {
			trigger_switch = true;
		}
	}

	void EntityPlacer::update() {
		tool::Tool::update();
		current_portal.position = scaled_position - current_portal.dimensions + sf::Vector2<uint32_t>(1, 1);
		current_inspectable.position = scaled_position - current_inspectable.dimensions + sf::Vector2<uint32_t>(1, 1);
		current_animator.position = scaled_position - current_animator.dimensions + sf::Vector2<uint32_t>(1, 1);
		current_critter.position = scaled_position;
		current_platform.position = scaled_position;
	}

	void EntityPlacer::render(sf::RenderWindow& win, sf::Vector2<float> offset) {
		sf::RectangleShape box{};

		box.setOutlineColor(sf::Color{ 200, 200, 200, 80 });
		box.setFillColor(sf::Color{ 100, 190, 190, 80 });
		box.setOutlineThickness(-2);
		box.setSize({ canvas::CELL_SIZE, canvas::CELL_SIZE });

		if (ent_type == ENTITY_TYPE::PORTAL) {
			for (int i = 0; i < current_portal.dimensions.x; ++i) {
				for (int j = 0; j < current_portal.dimensions.y; ++j) {
					box.setPosition(current_portal.position.x * canvas::CELL_SIZE + i * canvas::CELL_SIZE + offset.x, current_portal.position.y * canvas::CELL_SIZE + j * canvas::CELL_SIZE + offset.y);
					win.draw(box);
				}
			}
		} else if(ent_type == ENTITY_TYPE::ANIMATOR) {
			for (int i = 0; i < current_animator.dimensions.x; ++i) {
				for (int j = 0; j < current_animator.dimensions.y; ++j) {
					box.setPosition(current_animator.position.x * canvas::CELL_SIZE + i * canvas::CELL_SIZE + offset.x, current_animator.position.y * canvas::CELL_SIZE + j * canvas::CELL_SIZE + offset.y);
					win.draw(box);
				}
			}
		} else {
			box.setPosition(scaled_position.x * canvas::CELL_SIZE + offset.x, scaled_position.y * canvas::CELL_SIZE + offset.y);
			win.draw(box);
		}
	}

	void EntityPlacer::store_portal(sf::Vector2<uint32_t> dim, bool activate_on_contact, int src_id, int dest_id) {
		current_portal = canvas::Portal(dim, activate_on_contact, src_id, dest_id);
	}

	void EntityPlacer::store_inspectable(sf::Vector2<uint32_t> dim, bool activate_on_contact, std::string msg) {
		current_inspectable = canvas::Inspectable(dim, activate_on_contact, msg);
	}

	void EntityPlacer::store_critter(canvas::CRITTER_TYPE type) {
		current_critter = canvas::Critter(type);
	}

	void EntityPlacer::store_animator(sf::Vector2<uint32_t> dim, int id, bool automatic, bool foreground) {
		current_animator = canvas::Animator(dim, id, automatic, foreground);
	}

	void EntityPlacer::store_tile(int index) {}

}
