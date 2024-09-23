
#include "Map.hpp"
#include <imgui.h>
#include "../entities/player/Player.hpp"
#include "../gui/InventoryWindow.hpp"
#include "../gui/Portrait.hpp"
#include "../service/ServiceProvider.hpp"
#include "../setup/EnumLookups.hpp"

#include "../utils/Math.hpp"

namespace world {

Map::Map(automa::ServiceProvider& svc, player::Player& player, gui::Console& console) : player(&player), enemy_catalog(svc), save_point(svc), transition(svc, 96), m_services(&svc), m_console(&console) {}

void Map::load(automa::ServiceProvider& svc, int room_number, bool soft) {

	// for debugging
	center_box.setSize(svc.constants.f_screen_dimensions * 0.5f);
	flags.state.reset(LevelState::game_over);
	if (!player->is_dead()) { svc.state_controller.actions.reset(automa::Actions::death_mode); }
	spawn_counter.start();

	svc.state_flags.reset(automa::StateFlags::hide_hud);

	int ctr{};
	for (auto& room : svc.data.map_jsons) {
		if (room.id == room_number) { room_lookup = ctr; }
		++ctr;
	}
	auto const& metadata = svc.data.map_jsons.at(room_lookup).metadata;
	auto const& tiles = svc.data.map_jsons.at(room_lookup).tiles;
	inspectable_data = svc.data.map_jsons.at(room_lookup).inspectable_data;

	auto const& meta = metadata["meta"];
	room_id = meta["room_id"].as<int>();
	metagrid_coordinates.x = meta["metagrid"][0].as<int>();
	metagrid_coordinates.y = meta["metagrid"][1].as<int>();
	dimensions.x = meta["dimensions"][0].as<int>();
	dimensions.y = meta["dimensions"][1].as<int>();
	chunk_dimensions.x = meta["chunk_dimensions"][0].as<int>();
	chunk_dimensions.y = meta["chunk_dimensions"][1].as<int>();
	real_dimensions = {(float)dimensions.x * svc.constants.cell_size, (float)dimensions.y * svc.constants.cell_size};

	if (!soft) {
		if (meta["cutscene_on_entry"]["flag"].as_bool()) {
			auto ctype = meta["cutscene_on_entry"]["type"].as<int>();
			auto cid = meta["cutscene_on_entry"]["id"].as<int>();
			auto csource = meta["cutscene_on_entry"]["source"].as<int>();
			auto cutscene = util::QuestKey{ctype, cid, csource};
			svc.quest.process(svc, cutscene);
			if (svc.quest.get_progression(fornani::QuestType::cutscene, 6001) > 0) { std::cout << "cutscene requested!\n"; }
			cutscene_catalog.push_cutscene(svc, *this, *m_console, cid);
		}
		if (meta["music"].is_string()) {
			svc.music.load(meta["music"].as_string());
			svc.music.play_looped(10);
		}

		auto style_value = meta["style"].as<int>();
		style_label = svc.data.map_styles["styles"][style_value]["label"].as_string();
		style_id = svc.data.map_styles["styles"][style_value]["id"].as<int>();
		if (svc.greyblock_mode()) { style_id = 20; }
		native_style_id = svc.data.map_styles["styles"][style_value]["id"].as<int>();
		background = std::make_unique<bg::Background>(svc, meta["background"].as<int>());
		styles.breakables = meta["styles"]["breakables"].as<int>();
		styles.pushables = meta["styles"]["pushables"].as<int>();

		for (auto& entry : metadata["npcs"].array_view()) {
			sf::Vector2<float> pos{};
			pos.x = entry["position"][0].as<float>();
			pos.y = entry["position"][1].as<float>();
			auto id = entry["id"].as<int>();
			npcs.push_back(npc::NPC(svc, id));
			auto npc_state = svc.quest.get_progression(fornani::QuestType::npc, id);
			for (auto& convo : entry["suites"][npc_state].array_view()) { npcs.back().push_conversation(convo.as_string()); }
			npcs.back().set_position_from_scaled(pos);
			if ((bool)entry["background"].as_bool()) { npcs.back().push_to_background(); }
		}
		for (auto& entry : metadata["chests"].array_view()) {
			sf::Vector2<float> pos{};
			pos.x = entry["position"][0].as<float>();
			pos.y = entry["position"][1].as<float>();
			chests.push_back(entity::Chest(svc, entry["id"].as<int>()));
			chests.back().set_item(entry["item_id"].as<int>());
			chests.back().set_amount(entry["amount"].as<int>());
			chests.back().set_rarity(entry["rarity"].as<float>());
			if (entry["type"].as<int>() == 1) { chests.back().set_type(entity::ChestType::gun); }
			if (entry["type"].as<int>() == 2) { chests.back().set_type(entity::ChestType::orbs); }
			if (entry["type"].as<int>() == 3) { chests.back().set_type(entity::ChestType::item); }
			chests.back().set_position_from_scaled(pos);
		}

		for (auto& entry : metadata["animators"].array_view()) {
			sf::Vector2<int> scaled_dim{};
			sf::Vector2<int> scaled_pos{};
			scaled_pos.x = entry["position"][0].as<int>();
			scaled_pos.y = entry["position"][1].as<int>();
			scaled_dim.x = entry["dimensions"][0].as<int>();
			scaled_dim.y = entry["dimensions"][1].as<int>();
			auto automatic = (bool)entry["automatic"].as_bool();
			auto astyle = (bool)entry["style"].as<int>();
			auto lg = scaled_dim.x == 2;
			auto foreground = (bool)entry["foreground"].as_bool();
			auto aid = entry["id"].as<int>();
			auto a = entity::Animator(svc, scaled_pos, aid, lg, automatic, foreground, astyle);
			animators.push_back(a);
		}
		for (auto& entry : metadata["beds"].array_view()) {
			sf::Vector2<float> pos{};
			pos.x = entry["position"][0].as<float>() * svc.constants.cell_size;
			pos.y = entry["position"][1].as<float>() * svc.constants.cell_size;
			beds.push_back(entity::Bed(svc, pos, room_lookup));
		}
		for (auto& entry : metadata["inspectables"].array_view()) {
			sf::Vector2<uint32_t> dim{};
			sf::Vector2<uint32_t> pos{};
			auto key = entry["key"].as_string();
			pos.x = entry["position"][0].as<int>();
			pos.y = entry["position"][1].as<int>();
			dim.x = entry["dimensions"][0].as<int>();
			dim.y = entry["dimensions"][1].as<int>();
			auto alt = entry["alternates"].as<int>();
			auto native = entry["native_id"].as<int>();
			auto aoc = static_cast<bool>(entry["activate_on_contact"].as_bool());
			inspectables.push_back(entity::Inspectable(svc, dim, pos, key, room_id, alt, native, aoc));
			if (svc.data.inspectable_is_destroyed(inspectables.back().get_id())) { inspectables.back().destroy(); }
		}

		for (auto& entry : metadata["enemies"].array_view()) {
			int id{};
			sf::Vector2<float> pos{};
			pos.x = entry["position"][0].as<float>();
			pos.y = entry["position"][1].as<float>();
			enemy_catalog.push_enemy(svc, *this, *m_console, entry["id"].as<int>());
			enemy_catalog.enemies.back()->set_position_from_scaled({pos * svc.constants.cell_size});
			enemy_catalog.enemies.back()->get_collider().physics.zero();
		}
		for (auto& entry : metadata["destroyers"].array_view()) {
			sf::Vector2<int> pos{};
			pos.x = entry["position"][0].as<int>();
			pos.y = entry["position"][1].as<int>();
			auto quest_id = entry["quest_id"].as<int>();
			destroyers.push_back(Destroyable(svc, pos, quest_id));
		}
	}

	for (auto& entry : metadata["portals"].array_view()) {
		sf::Vector2<uint32_t> pos{};
		sf::Vector2<uint32_t> dim{};
		pos.x = entry["position"][0].as<int>();
		pos.y = entry["position"][1].as<int>();
		dim.x = entry["dimensions"][0].as<int>();
		dim.y = entry["dimensions"][1].as<int>();
		auto src_id = entry["source_id"].as<int>();
		auto dest_id = entry["destination_id"].as<int>();
		auto aoc = static_cast<bool>(entry["activate_on_contact"].as_bool());
		auto locked = static_cast<bool>(entry["locked"].as_bool());
		auto already_open = static_cast<bool>(entry["already_open"].as_bool());
		auto key_id = entry["key_id"].as<int>();
		portals.push_back(entity::Portal(svc, dim, pos, src_id, dest_id, aoc, locked, already_open, key_id));
		portals.back().update(svc);
	}

	auto const& savept = metadata["save_point"];
	auto save_id = svc.state_controller.save_point_id;
	save_point.id = savept.contains("position") ? room_id : -1;
	save_point.scaled_position.x = savept["position"][0].as<int>();
	save_point.scaled_position.y = savept["position"][1].as<int>();

	for (auto& entry : metadata["platforms"].array_view()) {
		sf::Vector2<float> dim{};
		sf::Vector2<float> pos{};
		pos.x = entry["position"][0].as<float>();
		pos.y = entry["position"][1].as<float>();
		dim.x = entry["dimensions"][0].as<float>();
		dim.y = entry["dimensions"][1].as<float>();
		pos *= svc.constants.cell_size;
		dim *= svc.constants.cell_size;
		auto start = entry["start"].as<float>();
		start = std::clamp(start, 0.f, 1.f);
		auto type = entry["type"].as_string();
		platforms.push_back(Platform(svc, pos, dim, entry["extent"].as<float>(), type, start, entry["style"].as<int>()));
	}
	for (auto& entry : metadata["switch_blocks"].array_view()) {
		sf::Vector2<float> pos{};
		pos.x = entry["position"][0].as<float>();
		pos.y = entry["position"][1].as<float>();
		pos *= svc.constants.cell_size;
		auto type = entry["type"].as<int>();
		auto button_id = entry["button_id"].as<int>();
		switch_blocks.push_back(SwitchBlock(svc, pos, button_id, type));
	}
	for (auto& entry : metadata["switches"].array_view()) {
		sf::Vector2<float> pos{};
		pos.x = entry["position"][0].as<float>();
		pos.y = entry["position"][1].as<float>();
		pos *= svc.constants.cell_size;
		auto type = entry["type"].as<int>();
		auto button_id = entry["button_id"].as<int>();
		switch_buttons.push_back(std::make_unique<SwitchButton>(svc, pos, button_id, type, *this));
	}

	generate_collidable_layer();
	if (!soft) {
		generate_layer_textures(svc);

		player->map_reset();

		transition.end();
		loading.start(4);
	}
}

void Map::update(automa::ServiceProvider& svc, gui::Console& console, gui::InventoryWindow& inventory_window) {
	auto& layers = svc.data.get_layers(room_id);
	loading.update();
	//if (loading.running()) { generate_layer_textures(svc); } // band-aid fix for weird artifacting for 1x1 levels
	flags.state.reset(LevelState::camera_shake);

	for (auto& cutscene : cutscene_catalog.cutscenes) { cutscene->update(svc, console, *this, *player); }

	if (flags.state.test(LevelState::spawn_enemy)) {
		for (auto& spawn : enemy_spawns) {
			enemy_catalog.push_enemy(*m_services, *this, *m_console, spawn.id, true);
			enemy_catalog.enemies.back()->set_position(spawn.pos);
			enemy_catalog.enemies.back()->get_collider().physics.zero();
			effects.push_back(entity::Effect(*m_services, spawn.pos + enemy_catalog.enemies.back()->get_collider().dimensions * 0.5f, {}, 0, 4));
		}
		enemy_spawns.clear();
		flags.state.reset(LevelState::spawn_enemy);
	}

	inventory_window.update(svc, *player, *this);

	player->collider.reset();
	for (auto& a : player->antennae) { a.collider.reset(); }
	player->ledge_height = player->collider.detect_ledge_height(*this);
	if (off_the_bottom(player->collider.physics.position) && loading.is_complete()) {
		player->hurt(64.f);
		player->freeze_position();
	}

	for (auto& grenade : active_grenades) {
		if (player->shielding() && player->controller.get_shield().sensor.within_bounds(grenade.bounding_box)) {
			player->controller.get_shield().damage();
			grenade.physics.velocity *= -1.f;
		}
		if (grenade.detonated() && grenade.sensor.within_bounds(player->hurtbox)) { player->hurt(grenade.get_damage()); }
		for (auto& enemy : enemy_catalog.enemies) {
			if (grenade.detonated() && grenade.sensor.within_bounds(enemy->get_collider().hurtbox)) {
				enemy->hurt();
				enemy->health.inflict(grenade.get_damage());
				enemy->health_indicator.add(grenade.get_damage());
				if (enemy->just_died() && enemy->spawn_loot()) {
					svc.stats.enemy.enemies_killed.update();
					active_loot.push_back(item::Loot(svc, enemy->get_attributes().drop_range, enemy->get_attributes().loot_multiplier, enemy->get_collider().bounding_box.position));
					svc.soundboard.flags.frdog.set(audio::Frdog::death);
				}
			}
		}
	}

	// i need to refactor this...
	for (auto& index : collidable_indeces) {
		auto& cell = layers.at(MIDDLEGROUND).grid.cells.at(index);
		// damage player if spikes
		if (cell.is_spike() && player->collider.hurtbox.overlaps(cell.bounding_box)) { player->hurt(1); }
		if (cell.is_death_spike() && player->collider.hurtbox.overlaps(cell.bounding_box)) { player->hurt(64); }

		for (auto& grenade : active_grenades) {
			for (auto& breakable : breakables) {
				if (grenade.detonated() && grenade.sensor.within_bounds(breakable.get_bounding_box())) { breakable.destroy(); }
			}
		}
		for (auto& proj : active_projectiles) {

			// should be, simply:
			// cell.update(svc, player, proj, *this);
			// or something similar

			if (!nearby(cell.bounding_box, proj.bounding_box)) {
				continue;
			} else {
				cell.collision_check = true;
				if ((proj.bounding_box.overlaps(cell.bounding_box) && cell.is_occupied())) {
					if (!cell.is_collidable()) { continue; }
					if (!proj.stats.transcendent) {
						if (!proj.destruction_initiated()) {
							effects.push_back(entity::Effect(svc, proj.destruction_point + proj.physics.position, {}, proj.effect_type(), 2));
							if (proj.direction.lr == dir::LR::neutral) { effects.back().rotate(); }
						}
						proj.destroy(false);
					}
					if (proj.stats.spring && cell.is_hookable()) {
						if (proj.hook.grapple_flags.test(arms::GrappleState::probing)) {
							proj.hook.spring.set_anchor(cell.middle_point());
							proj.hook.grapple_triggers.set(arms::GrappleTriggers::found);
						}
						handle_grappling_hook(svc, proj);
					}
				}
			}
		}
	}

	manage_projectiles(svc);

	for (auto& proj : active_projectiles) {
		if (proj.state.test(arms::ProjectileState::destruction_initiated)) { continue; }
		for (auto& platform : platforms) { platform.on_hit(svc, *this, proj); }
		for (auto& breakable : breakables) { breakable.on_hit(svc, *this, proj); }
		for (auto& pushable : pushables) { pushable.on_hit(svc, *this, proj); }
		for (auto& destroyer : destroyers) { destroyer.on_hit(svc, *this, proj); }
		for (auto& block : switch_blocks) { block.on_hit(svc, *this, proj); }
		for (auto& enemy : enemy_catalog.enemies) { enemy->on_hit(svc, *this, proj); }

		if (player->shielding() && player->controller.get_shield().sensor.within_bounds(proj.bounding_box)) { player->controller.get_shield().damage(proj.stats.base_damage * player->player_stats.shield_dampen); }
		if (proj.bounding_box.overlaps(player->hurtbox) && proj.team != arms::TEAMS::NANI) {
			player->hurt(proj.stats.base_damage);
			proj.destroy(false);
		}
	}
	for (auto& enemy : enemy_catalog.enemies) {
		enemy->unique_update(svc, *this, *player);
		enemy->post_update(svc, *this, *player);
	}

	for (auto& loot : active_loot) { loot.update(svc, *this, *player); }
	for (auto& grenade : active_grenades) { grenade.update(svc, *player, *this); }
	for (auto& emitter : active_emitters) { emitter.update(svc, *this); }
	for (auto& chest : chests) { chest.update(svc, *this, console, *player); }
	for (auto& npc : npcs) { npc.update(svc, *this, console, *player); }
	for (auto& portal : portals) { portal.handle_activation(svc, *player, console, room_id, transition); }
	for (auto& inspectable : inspectables) { inspectable.update(svc, *player, console, inspectable_data); }
	for (auto& animator : animators) { animator.update(svc, *player); }
	for (auto& effect : effects) { effect.update(svc, *this); }
	for (auto& platform : platforms) { platform.update(svc, *this, *player); }
	for (auto& spawner : spawners) { spawner.update(svc, *this); }
	for (auto& switch_block : switch_blocks) { switch_block.update(svc, *this, *player); }
	for (auto& switch_button : switch_buttons) { switch_button->update(svc, *this, *player); }
	for (auto& destroyer : destroyers) { destroyer.update(svc, *this, *player); }
	for (auto& bed : beds) { bed.update(svc, *this, console, *player, transition); }
	for (auto& breakable : breakables) {
		breakable.update(svc);
		breakable.handle_collision(player->collider);
	}
	for (auto& pushable : pushables) { pushable.update(svc, *this, *player); }
	for (auto& spike : spikes) { spike.handle_collision(player->collider); }
	player->collider.detect_map_collision(*this);
	transition.update(*player);
	if (player->collider.collision_depths) { player->collider.collision_depths.value().update(); }
	if (save_point.id != -1) { save_point.update(svc, *player, console); }

	std::erase_if(effects, [](auto& e) { return e.done(); });
	console.update(svc);

	player->collider.reset_ground_flags();

	// check if player died
	if (!flags.state.test(LevelState::game_over) && player->death_animation_over() && svc.death_mode() && loading.is_complete()) {
		// std::cout << "Launch death console.\n";
		console.set_source(svc.text.basic);
		console.load_and_launch("death");
		flags.state.set(LevelState::game_over);
		svc.music.load("mortem");
		svc.music.play_looped(10);
		svc.soundboard.turn_off();
		svc.stats.player.death_count.update();
	}

	// demo only
	if (svc.state_controller.actions.consume(automa::Actions::end_demo)) { end_demo.start(); }
	end_demo.update();
	if (end_demo.get_cooldown() == 1) {
		m_console->set_source(svc.text.basic);
		m_console->load_and_launch("end_demo");
	}
	if (svc.state_controller.actions.test(automa::Actions::print_stats) && console.is_complete()) { svc.state_controller.actions.set(automa::Actions::trigger); }
	// demo only

	if (svc.state_controller.actions.test(automa::Actions::retry)) { flags.state.set(LevelState::game_over); }
	if (console.is_complete() && flags.state.test(LevelState::game_over)) {
		transition.start();
		if (transition.is_done()) {
			player->start_over();
			svc.state_controller.actions.set(automa::Actions::player_death);
			svc.state_controller.actions.set(automa::Actions::trigger);
		}
	}

	console.clean_off_trigger();
	inventory_window.clean_off_trigger();
	inventory_window.info.clean_off_trigger();
}

void Map::render(automa::ServiceProvider& svc, sf::RenderWindow& win, sf::Vector2<float> cam) {
	auto& layers = svc.data.get_layers(room_id);
	// check for a switch to greyblock mode
	if (svc.debug_flags.test(automa::DebugFlags::greyblock_trigger)) {
		style_id = style_id == 20 ? native_style_id : 20;
		generate_layer_textures(svc);
		svc.debug_flags.reset(automa::DebugFlags::greyblock_trigger);
	}

	for (auto& portal : portals) { portal.render(svc, win, cam); }
	for (auto& chest : chests) { chest.render(svc, win, cam); }
	for (auto& npc : npcs) {
		if (!npc.background()) { npc.render(svc, win, cam); }
	}
	for (auto& emitter : active_emitters) { emitter.render(svc, win, cam); }
	for (auto& grenade : active_grenades) { grenade.render(svc, win, cam); }
	player->render(svc, win, cam);
	for (auto& enemy : enemy_catalog.enemies) {
		if (!enemy->is_foreground()) {
			enemy->render(svc, win, cam);
			enemy->unique_render(svc, win, cam);
		}
	}
	for (auto& enemy : enemy_catalog.enemies) { }
	for (auto& proj : active_projectiles) { proj.render(svc, *player, win, cam); }
	for (auto& loot : active_loot) { loot.render(svc, win, cam); }
	for (auto& platform : platforms) { platform.render(svc, win, cam); }
	for (auto& breakable : breakables) { breakable.render(svc, win, cam); }
	for (auto& pushable : pushables) { pushable.render(svc, win, cam); }
	for (auto& destroyer : destroyers) { destroyer.render(svc, win, cam); }
	for (auto& spike : spikes) { spike.render(svc, win, cam); }
	for (auto& switch_block : switch_blocks) { switch_block.render(svc, win, cam); }
	for (auto& switch_button : switch_buttons) { switch_button->render(svc, win, cam); }
	for (auto& bed : beds) { bed.render(svc, win, cam); }

	if (save_point.id != -1) { save_point.render(svc, win, cam); }

	// map foreground tiles
	for (int i = 4; i < NUM_LAYERS; ++i) {
		if (svc.greyblock_mode() && i != 4) { continue; }
		layer_textures.at(i).display();
		layer_sprite.setTexture(layer_textures.at(i).getTexture());
		layer_sprite.setPosition(-cam);
		win.draw(layer_sprite);
	}

	//foreground enemies
	for (auto& enemy : enemy_catalog.enemies) {
		if (enemy->is_foreground()) {
			enemy->render(svc, win, cam);
			enemy->unique_render(svc, win, cam);
		}
	}

	for (auto& effect : effects) { effect.render(svc, win, cam); }

	player->render_indicators(svc, win, cam);
	for (auto& enemy : enemy_catalog.enemies) { enemy->render_indicators(svc, win, cam); }
	for (auto& enemy : enemy_catalog.enemies) { enemy->gui_render(svc, win, cam); }

	if (svc.greyblock_mode()) {
		for (auto& index : collidable_indeces) {
			auto& cell = layers.at(MIDDLEGROUND).grid.cells.at(index);
			cell.drawbox.setPosition(cell.position - cam);
			if (cell.ramp_adjacent()) { cell.drawbox.setOutlineColor(sf::Color::Red); }
			win.draw(cell.drawbox);
		}
	}

	if (real_dimensions.y < svc.constants.screen_dimensions.y) {
		float ydiff = (svc.constants.screen_dimensions.y - real_dimensions.y) / 2;
		borderbox.setFillColor(svc.styles.colors.ui_black);
		borderbox.setSize({(float)svc.constants.screen_dimensions.x, ydiff});
		borderbox.setPosition(0.0f, 0.0f);
		win.draw(borderbox);

		borderbox.setPosition(0.0f, real_dimensions.y + ydiff);
		win.draw(borderbox);
	}
	if (real_dimensions.x < svc.constants.screen_dimensions.x) {
		float xdiff = (svc.constants.screen_dimensions.x - real_dimensions.x) / 2;
		borderbox.setFillColor(svc.styles.colors.ui_black);
		borderbox.setSize({xdiff, (float)svc.constants.screen_dimensions.y});
		borderbox.setPosition(0.0f, 0.0f);
		win.draw(borderbox);

		borderbox.setPosition(real_dimensions.x + xdiff, 0.0f);
		win.draw(borderbox);
	}

	for (auto& animator : animators) {
		if (animator.foreground()) { animator.render(svc, win, cam); }
	}
	for (auto& inspectable : inspectables) { inspectable.render(svc, win, cam); }

	if (svc.greyblock_mode()) {
		center_box.setPosition(0.f, 0.f);
		center_box.setFillColor(sf::Color(80, 80, 80, 60));
		win.draw(center_box);
		center_box.setPosition(svc.constants.f_screen_dimensions * 0.5f);
		win.draw(center_box);
		center_box.setFillColor(sf::Color(100, 100, 100, 60));
		center_box.setPosition(svc.constants.f_screen_dimensions.x * 0.5f, 0.f);
		win.draw(center_box);
		center_box.setPosition(0.f, svc.constants.f_screen_dimensions.y * 0.5f);
		win.draw(center_box);
	}
}

void Map::render_background(automa::ServiceProvider& svc, sf::RenderWindow& win, sf::Vector2<float> cam) {
	if (!svc.greyblock_mode()) {
		background->render(svc, win, cam, real_dimensions);
		for (int i = 0; i < 4; ++i) {
			layer_textures.at(i).display();
			layer_sprite.setTexture(layer_textures.at(i).getTexture());
			layer_sprite.setPosition(-cam);
			win.draw(layer_sprite);
			if (i == 0) {
				for (auto& npc : npcs) {
					if (npc.background()) { npc.render(svc, win, cam); }
				}
			}
		}
		for (auto& switch_block : switch_blocks) { switch_block.render(svc, win, cam, true); }
	} else {
		sf::RectangleShape box{};
		box.setPosition(0, 0);
		box.setFillColor(svc.styles.colors.black);
		box.setSize({(float)svc.constants.screen_dimensions.x, (float)svc.constants.screen_dimensions.y});
		win.draw(box);
	}
	for (auto& animator : animators) {
		if (!animator.foreground()) { animator.render(svc, win, cam); }
	}
}

void Map::render_console(automa::ServiceProvider& svc, gui::Console& console, sf::RenderWindow& win) {
	if (console.flags.test(gui::ConsoleFlags::active)) { console.render(win); }
	console.write(win, false);
}

void Map::spawn_projectile_at(automa::ServiceProvider& svc, arms::Weapon& weapon, sf::Vector2<float> pos) {
	if (weapon.attributes.grenade) { active_grenades.push_back(arms::Grenade(svc, pos, weapon.firing_direction)); }
	active_projectiles.push_back(weapon.projectile);
	active_projectiles.back().set_sprite(svc);
	active_projectiles.back().set_position(pos);
	active_projectiles.back().seed(svc);
	active_projectiles.back().update(svc, *player);
	active_projectiles.back().sync_position();
	if (active_projectiles.back().stats.boomerang) { active_projectiles.back().set_boomerang_speed(); }
	if (active_projectiles.back().stats.spring) {
		active_projectiles.back().set_hook_speed();
		active_projectiles.back().hook.grapple_flags.set(arms::GrappleState::probing);
	}

	active_emitters.push_back(vfx::Emitter(svc, weapon.barrel_point, weapon.emitter_dimensions, weapon.emitter_type, weapon.emitter_color, weapon.firing_direction));
}

void Map::spawn_enemy(int id, sf::Vector2<float> pos) {
	enemy_spawns.push_back({pos, id});
	spawn_counter.update();
	flags.state.set(LevelState::spawn_enemy);
}

void Map::manage_projectiles(automa::ServiceProvider& svc) {
	for (auto& proj : active_projectiles) {
		proj.update(svc, *player);
		if (proj.whiffed() && !proj.poofed() && !proj.made_contact()) {
			effects.push_back(entity::Effect(svc, proj.physics.position, proj.physics.velocity * 0.1f, proj.effect_type(), 8));
			proj.state.set(arms::ProjectileState::poof);
		}
	}
	for (auto& emitter : active_emitters) { emitter.update(svc, *this); }

	std::erase_if(active_projectiles, [](auto const& p) { return p.state.test(arms::ProjectileState::destroyed); });
	std::erase_if(active_grenades, [](auto const& g) { return g.detonated(); });
	std::erase_if(active_emitters, [](auto const& p) { return p.done(); });
	std::erase_if(breakables, [](auto const& b) { return b.destroyed(); });
	std::erase_if(inspectables, [](auto const& i) { return i.destroyed(); });
	std::erase_if(destroyers, [](auto const& d) { return d.detonated(); });

	if (player->arsenal) {
		if (player->fire_weapon()) {
			svc.stats.player.bullets_fired.update();
			sf::Vector2<float> tweak = player->controller.facing_left() ? sf::Vector2<float>{0.f, 0.f} : sf::Vector2<float>{-3.f, 0.f};
			spawn_projectile_at(svc, player->equipped_weapon(), player->equipped_weapon().barrel_point + tweak);
			++player->equipped_weapon().active_projectiles;
			player->equipped_weapon().shoot();
			if (!player->equipped_weapon().attributes.automatic) { player->controller.set_shot(false); }
		}
	}
}

void Map::generate_collidable_layer(bool live) {
	auto& layers = m_services->data.get_layers(room_id);
	auto pushable_offset = sf::Vector2<float>{1.f, 0.f};
	layers.at(MIDDLEGROUND).grid.check_neighbors();
	for (auto& cell : layers.at(MIDDLEGROUND).grid.cells) {
		if ((!cell.surrounded && cell.is_occupied() && !cell.is_special())) { collidable_indeces.push_back(cell.one_d_index); }
		if (live) { continue; }
		if (cell.is_breakable()) { breakables.push_back(Breakable(*m_services, cell.position, styles.breakables)); }
		if (cell.is_pushable()) { pushables.push_back(Pushable(*m_services, cell.position + pushable_offset, styles.pushables, cell.value - 227)); }
		if (cell.is_spike()) { spikes.push_back(Spike(*m_services, cell.position, cell.value)); }
		if (cell.is_spawner()) { spawners.push_back(Spawner(*m_services, cell.position, 5)); }
	}
}

void Map::generate_layer_textures(automa::ServiceProvider& svc) {
	auto& layers = svc.data.get_layers(room_id);
	for (auto& layer : layers) {
		layer_textures.at((int)layer.render_order).create(layer.grid.dimensions.x * svc.constants.i_cell_size, layer.grid.dimensions.y * svc.constants.i_cell_size);
		layer_textures.at((int)layer.render_order).clear(sf::Color::Transparent);
		for (auto& cell : layer.grid.cells) {
			if (cell.is_occupied() && !cell.is_special()) {
				auto x_coord = static_cast<int>((cell.value % svc.constants.tileset_scaled.x) * svc.constants.i_cell_size);
				auto y_coord = static_cast<int>(std::floor(cell.value / svc.constants.tileset_scaled.x) * svc.constants.i_cell_size);
				tile_sprite.setTexture(svc.assets.tilesets.at(style_id));
				tile_sprite.setTextureRect(sf::IntRect({x_coord, y_coord}, {svc.constants.i_cell_size, svc.constants.i_cell_size}));
				tile_sprite.setPosition(cell.position);
				layer_textures.at((int)layer.render_order).draw(tile_sprite);
			}
		}
	}
}

bool Map::check_cell_collision(shape::Collider collider) {
	auto& layers = m_services->data.get_layers(room_id);
	for (auto& index : collidable_indeces) {
		auto& cell = layers.at(MIDDLEGROUND).grid.cells.at(index);
		if (!nearby(cell.bounding_box, collider.bounding_box)) {
			continue;
		} else {
			// check vicinity so we can escape early
			if (!collider.vicinity.overlaps(cell.bounding_box)) {
				continue;
			} else if (!cell.is_solid()) {
				continue;
			} else {
				if (cell.value > 0 && collider.predictive_combined.SAT(cell.bounding_box)) { return true; }
			}
		}
	}
	return false;
}

void Map::handle_grappling_hook(automa::ServiceProvider& svc, arms::Projectile& proj) {
	// do this first block once
	if (proj.hook.grapple_triggers.test(arms::GrappleTriggers::found) && !proj.hook.grapple_flags.test(arms::GrappleState::anchored) && !proj.hook.grapple_flags.test(arms::GrappleState::snaking)) {
		proj.hook.spring.set_bob(player->apparent_position);
		proj.hook.grapple_triggers.reset(arms::GrappleTriggers::found);
		proj.hook.grapple_flags.set(arms::GrappleState::anchored);
		proj.hook.grapple_flags.reset(arms::GrappleState::probing);
		proj.hook.spring.set_force(proj.stats.spring_constant);
		proj.hook.spring.variables.physics.acceleration += player->collider.physics.acceleration;
		proj.hook.spring.variables.physics.velocity += player->collider.physics.velocity;
	}
	if (player->controller.hook_held() && proj.hook.grapple_flags.test(arms::GrappleState::anchored)) {
		proj.hook.spring.variables.physics.acceleration += player->collider.physics.acceleration;
		proj.hook.spring.variables.physics.acceleration.x += player->controller.horizontal_movement();
		proj.lock_to_anchor();
		proj.hook.spring.update(svc);

		// update rest length
		auto next_length = proj.stats.spring_slack * abs(player->collider.physics.position.y - proj.hook.spring.get_anchor().y);
		next_length = std::clamp(next_length, lookup::min_hook_length, lookup::max_hook_length);
		proj.hook.spring.set_rest_length(next_length);

		// break out if player is far away from bob. we don't want the player to teleport.
		auto distance = util::magnitude(player->collider.physics.position - proj.hook.spring.get_bob());
		if (distance > 32.f) { proj.hook.break_free(*player); }

		// handle map collisions while anchored
		player->collider.predictive_combined.set_position(proj.hook.spring.variables.physics.position);
		if (check_cell_collision(player->collider)) {
			player->collider.physics.zero();
		} else {
			player->collider.physics.position = proj.hook.spring.variables.physics.position - player->collider.dimensions / 2.f;
		}
		player->collider.sync_components();
	} else if (proj.hook.grapple_flags.test(arms::GrappleState::anchored)) {
		proj.hook.break_free(*player);
	}

	if (player->controller.released_hook() && !proj.hook.grapple_flags.test(arms::GrappleState::snaking)) { proj.hook.break_free(*player); }
}

void Map::shake_camera() { flags.state.set(LevelState::camera_shake); }

void Map::clear() {
	dimensions = {};
	portals.clear();
	platforms.clear();
	breakables.clear();
	pushables.clear();
	spikes.clear();
	destroyers.clear();
	switch_blocks.clear();
	switch_buttons.clear();
	chests.clear();
	npcs.clear();
}

std::vector<Layer>& Map::get_layers() { return m_services->data.get_layers(room_id); }

sf::Vector2<float> Map::get_spawn_position(int portal_source_map_id) {
	for (auto& portal : portals) {
		if (portal.get_source() == portal_source_map_id) { return (portal.position); }
	}
	return Vec(300.f, 390.f);
}

bool Map::nearby(shape::Shape& first, shape::Shape& second) const {
	return abs(first.position.x + first.dimensions.x * 0.5f - second.position.x) < lookup::unit_size_f * collision_barrier && abs(first.position.y - second.position.y) < lookup::unit_size_f * collision_barrier;
}

} // namespace world
