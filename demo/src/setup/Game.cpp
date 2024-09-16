#include "Game.hpp"
#include "WindowManager.hpp"
#include <steam/steam_api.h>
#include <ctime>

namespace fornani {

Game::Game(char** argv, WindowManager& window) : player(services) {
	services.window = &window;
	services.constants.screen_dimensions = window.screen_dimensions;
	// data
	services.data = data::DataManager(services);
	services.data.finder.setResourcePath(argv);
	services.data.finder.set_scene_path(argv);
	services.data.load_data();
	// controls
	services.data.load_controls(services.controller_map);
	// text
	services.text.finder.setResourcePath(argv);
	services.text.load_data();
	// image
	services.assets.finder.setResourcePath(argv);
	services.assets.import_textures();
	// sounds
	services.music.finder.setResourcePath(argv);
	services.assets.load_audio();
	playtest.m_musicplayer = true;
	services.music.turn_on();
	// player
	player.init(services);

	// state manager
	game_state.set_current_state(std::make_unique<automa::MainMenu>(services, player, "main"));
	game_state.get_current_state().init(services, 100);

	background.setSize(static_cast<sf::Vector2<float>>(services.constants.screen_dimensions));
	background.setPosition(0, 0);
	background.setFillColor(services.styles.colors.ui_black);
}

void Game::run(bool demo, int room_id, std::filesystem::path levelpath, sf::Vector2<float> player_position) {

	if (services.window->fullscreen()) { services.app_flags.set(automa::AppFlags::fullscreen); }
	flags.set(GameFlags::standard_display);

	measurements.win_size.x = services.window->get().getSize().x;
	measurements.win_size.y = services.window->get().getSize().y;

	// for editor demo. should be excluded for releases.
	if (demo) {
		services.debug_flags.set(automa::DebugFlags::demo_mode);
		flags.set(GameFlags::in_game);
		game_state.get_current_state().target_folder.paths.scene = levelpath;
		services.music.turn_off();
		services.data.load_blank_save(player);
		game_state.set_current_state(std::make_unique<automa::Dojo>(services, player, "dojo"));
		// TODO: fix this
		game_state.get_current_state().init(services, room_id, levelpath.filename().string());
		services.state_controller.demo_level = room_id;
		//
		services.state_controller.player_position = player_position;
		player.set_position(player_position);
	} else {
		game_state.get_current_state().target_folder.paths.scene = services.data.finder.scene_path;
		game_state.get_current_state().target_folder.paths.region = services.data.finder.scene_path + "/firstwind";
	}

	std::cout << "> Success\n";

	while (services.window->get().isOpen()) {

		if (services.state_controller.actions.test(automa::Actions::shutdown)) { break; }
		if (services.death_mode()) { flags.reset(GameFlags::in_game); }

		services.ticker.start_frame();

		// game loop
		sf::Clock deltaClock{};

		// SFML event variable
		auto event = sf::Event{};

		bool valid_event{};
		// check window events
		while (services.window->get().pollEvent(event)) {
			player.animation.state = {};
			if (event.key.code == sf::Keyboard::F2) { valid_event = false; }
			if (event.key.code == sf::Keyboard::F3) { valid_event = false; }
			if (event.key.code == sf::Keyboard::Slash) { valid_event = false; }
			switch (event.type) {
			case sf::Event::Closed: goto shutdown;
			case sf::Event::Resized:
				if (flags.test(GameFlags::standard_display)) {
					sf::Vector2u display_dimensions{static_cast<unsigned>(sf::VideoMode::getDesktopMode().width), static_cast<unsigned>(sf::VideoMode::getDesktopMode().height)};
					auto aspect_ratio = static_cast<float>(services.constants.aspect_ratio.x) / static_cast<float>(services.constants.aspect_ratio.y);
					auto display_ratio = static_cast<float>(display_dimensions.x) / static_cast<float>(display_dimensions.y);
					auto vertical = display_ratio < aspect_ratio;
					auto letterbox = std::min(display_ratio, aspect_ratio) / std::max(display_ratio, aspect_ratio);
					if (vertical) {
						services.window->get().setSize({static_cast<unsigned>(sf::VideoMode::getDesktopMode().width), static_cast<unsigned>(sf::VideoMode::getDesktopMode().height * letterbox)});
					} else {
						services.window->get().setSize({static_cast<unsigned>(sf::VideoMode::getDesktopMode().width * letterbox), static_cast<unsigned>(sf::VideoMode::getDesktopMode().height)});
					}
					flags.reset(GameFlags::standard_display);
				} else {
					services.window->get().setSize(measurements.win_size);
					flags.set(GameFlags::standard_display);
				}
				services.window->set_screencap();
				while (services.window->get().pollEvent(event)) {} // have to clear the event queue because Window::setSize() is considered a resize event
				break;
			case sf::Event::KeyPressed:
				if (event.key.code == sf::Keyboard::F2) { valid_event = false; }
				if (event.key.code == sf::Keyboard::F3) { valid_event = false; }
				if (event.key.code == sf::Keyboard::Slash) { valid_event = false; }
				if (event.key.code == sf::Keyboard::Unknown) { valid_event = false; }
				if (event.key.code == sf::Keyboard::D) {
					// debug() ? services.debug_flags.reset(automa::DebugFlags::imgui_overlay) : services.debug_flags.set(automa::DebugFlags::imgui_overlay);
					// services.assets.sharp_click.play();
					// services.state_controller.actions.set(automa::Actions::print_stats);
				}
				if (event.key.code == sf::Keyboard::Q) {
					// game_state.set_current_state(std::make_unique<automa::MainMenu>(services, player, "main"));
					// flags.set(GameFlags::in_game);
				}
				if (event.key.code == sf::Keyboard::P) {
					if (flags.test(GameFlags::playtest)) {
						flags.reset(GameFlags::playtest);
						services.assets.menu_back.play();
					} else {
						flags.set(GameFlags::playtest);
						services.assets.menu_next.play();
					}
				}
				if (event.key.code == sf::Keyboard::Equal) { take_screenshot(services.window->screencap); }
				if (event.key.code == sf::Keyboard::H) {
					// services.debug_flags.set(automa::DebugFlags::greyblock_trigger);
					// services.debug_flags.test(automa::DebugFlags::greyblock_mode) ? services.debug_flags.reset(automa::DebugFlags::greyblock_mode) : services.debug_flags.set(automa::DebugFlags::greyblock_mode);
				}
				break;
			case sf::Event::KeyReleased:
				if (event.key.code == sf::Keyboard::F2) { valid_event = false; }
				if (event.key.code == sf::Keyboard::F3) { valid_event = false; }
				if (event.key.code == sf::Keyboard::Slash) { valid_event = false; }
				if (event.key.code == sf::Keyboard::Unknown) { valid_event = false; }
				break;
			default: break;
			}
			game_state.get_current_state().handle_events(services, event);
			if (valid_event) { ImGui::SFML::ProcessEvent(event); }
			valid_event = true;
		}

		SteamAPI_RunCallbacks();

		// game logic and rendering
		services.music.update();
		bool has_focus = services.window->get().hasFocus();
		services.ticker.tick([this, has_focus, &services = services] {
			services.controller_map.update(has_focus);
			game_state.get_current_state().tick_update(services);
		});
		game_state.get_current_state().frame_update(services);
		game_state.process_state(services, player, *this);
		if (services.state_controller.actions.consume(automa::Actions::screenshot)) { take_screenshot(services.window->screencap); }

		ImGui::SFML::Update(services.window->get(), deltaClock.restart());
		services.window->screencap.update(services.window->get());

		// ImGui stuff
		if (services.debug_flags.test(automa::DebugFlags::imgui_overlay)) { debug_window(services.window->get()); }
		if (flags.test(GameFlags::playtest)) { playtester_portal(services.window->get()); }

		// my renders
		services.window->get().clear();
		services.window->get().draw(background);

		game_state.get_current_state().render(services, services.window->get());

		ImGui::SFML::Render(services.window->get());
		services.window->get().display();

		services.ticker.end_frame();
	}

shutdown:
	// shutdown
	// explicitly delete music player since it can't be deleted after AssetManager
	services.music.stop();
	ImGui::SFML::Shutdown();
}

void Game::debug_window(sf::RenderWindow& window) {

	bool* debug{};
	float const PAD = 10.0f;
	static int corner = 1;
	ImGuiIO& io = ImGui::GetIO();
	io.FontGlobalScale = 1.0;
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
	if (corner != -1) {
		ImGuiViewport const* viewport = ImGui::GetMainViewport();
		ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
		ImVec2 work_size = viewport->WorkSize;
		ImVec2 window_pos, window_pos_pivot;
		window_pos.x = (corner & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
		window_pos.y = (corner & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
		window_pos_pivot.x = (corner & 1) ? 1.0f : 0.0f;
		window_pos_pivot.y = (corner & 2) ? 1.0f : 0.0f;
		ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
		window_flags |= ImGuiWindowFlags_NoMove;
		ImGui::SetNextWindowBgAlpha(0.65f); // Transparent background
		if (ImGui::Begin("Debug Mode", debug, window_flags)) {
			ImGui::Text("Debug Window\n"
						"For Nani (beta version 1.0.0)");
			ImGui::Text("Window Focused: ");
			ImGui::SameLine();
			if (window.hasFocus()) {
				ImGui::Text("Yes");
			} else {
				ImGui::Text("No");
			}
			if (!window.hasFocus()) { window.RenderTarget::setActive(); }
			ImGui::Separator();
			ImGui::Text("Screen Dimensions X: %u", services.constants.screen_dimensions.x);
			ImGui::Text("Screen Dimensions Y: %u", services.constants.screen_dimensions.y);
			if (ImGui::IsMousePosValid()) {
				ImGui::Text("Mouse Position: (%.1f,%.1f)", io.MousePos.x, io.MousePos.y);
			} else {
				ImGui::Text("Mouse Position: <invalid>");
			}
			ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
			if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags)) {
				if (ImGui::BeginTabItem("Time")) {
					ImGui::Separator();
					ImGui::Text("Ticker");
					ImGui::Text("dt: %.8f", services.ticker.dt.count());
					ImGui::Text("New Time: %.4f", services.ticker.new_time);
					ImGui::Text("Current Time: %.4f", services.ticker.current_time);
					ImGui::Text("Accumulator: %.4f", services.ticker.accumulator.count());
					ImGui::Separator();
					ImGui::Text("Seconds Passed: %.2f", services.ticker.total_seconds_passed.count());
					ImGui::Text("Milliseconds Passed: %.0f", services.ticker.total_milliseconds_passed.count());
					ImGui::Text("Ticks Per Frame: %.2f", services.ticker.ticks_per_frame);
					ImGui::Text("Frames Per Second: %.2f", services.ticker.fps);
					ImGui::Separator();
					ImGui::SliderFloat("Tick Rate (ms): ", &services.ticker.tick_rate, 0.0001f, 0.02f, "%.5f");
					ImGui::SliderFloat("Tick Multiplier: ", &services.ticker.tick_multiplier, 0.f, 64.f, "%.2f");
					if (ImGui::Button("Reset")) {
						services.ticker.tick_rate = 0.016f;
						services.ticker.tick_multiplier = 16;
					}
					ImGui::Separator();
					ImGui::Text("Stopwatch");
					ImGui::Text("average time: %.4f", services.stopwatch.get_snapshot());

					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Console")) {
					ImGui::Separator();

					ImGui::Text("Console Active : %s", game_state.get_current_state().console.flags.test(gui::ConsoleFlags::active) ? "Yes" : "No");
					ImGui::Text("Console Writing? : %s", game_state.get_current_state().console.writer.writing() ? "Yes" : "No");
					ImGui::Text("Console Writing Done? : %s", game_state.get_current_state().console.writer.complete() ? "Yes" : "No");
					ImGui::Text("Console Select Mode? : %s", game_state.get_current_state().console.flags.test(gui::ConsoleFlags::selection_mode) ? "Yes" : "No");
					ImGui::Text("Writer Select Mode? : %s", game_state.get_current_state().console.writer.selection_mode() ? "Yes" : "No");
					ImGui::Text("Prompt? : %s", game_state.get_current_state().console.writer.current_message().prompt ? "Yes" : "No");
					ImGui::Text("Message Target : %i", game_state.get_current_state().console.writer.current_message().target);
					ImGui::Text("Response Target : %i", game_state.get_current_state().console.writer.current_response().target);
					ImGui::Text("Current Selection : %i", game_state.get_current_state().console.writer.get_current_selection());
					ImGui::Text("Current Suite Set : %i", game_state.get_current_state().console.writer.get_current_suite_set());
					ImGui::Separator();
					ImGui::Text("Player Transponder Skipping : %s", player.transponder.skipped_ahead() ? "Yes" : "No");
					ImGui::Text("Player Transponder Exited : %s", player.transponder.exited() ? "Yes" : "No");
					ImGui::Text("Player Transponder Requested Next : %s", player.transponder.requested_next() ? "Yes" : "No");
					ImGui::Separator();
					ImGui::Text("Player Restricted? : %s", player.controller.restricted() ? "Yes" : "No");
					ImGui::Text("Player Inspecting? : %s", player.controller.inspecting() ? "Yes" : "No");
					ImGui::Separator();
					ImGui::SliderInt("Text Size", &game_state.get_current_state().console.writer.text_size, 6, 64);

					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Input State")) {
					for (auto action = config::DigitalAction::platformer_left; action < config::DigitalAction::COUNT; action = static_cast<config::DigitalAction>(static_cast<int>(action) + 1)) {
						ImGui::Text(services.controller_map.digital_action_name(action).data());
						ImGui::SameLine();
						auto status = services.controller_map.digital_action_status(action);
						if (status.triggered) {
							ImGui::Text("Triggered");
						} else if (status.held) {
							ImGui::Text("Held");
						} else if (status.released) {
							ImGui::Text("Released");
						} else {
							ImGui::Text("-");
						}
					}
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Audio")) {
					ImGui::Separator();
					ImGui::Text("Music Player");
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Player")) {
					if (ImGui::BeginTabBar("PlayerTabBar", tab_bar_flags)) {
						if (ImGui::BeginTabItem("Texture")) {
							if (ImGui::Button("Default")) { player.texture_updater.switch_to_palette(services.assets.t_palette_nani); }
							if (ImGui::Button("Divine")) { player.texture_updater.switch_to_palette(services.assets.t_palette_nanidiv); }
							if (ImGui::Button("Night")) { player.texture_updater.switch_to_palette(services.assets.t_palette_naninight); }
							ImGui::EndTabItem();
						}
						if (ImGui::BeginTabItem("Physics and Collision")) {
							ImGui::Text("Player Pos: (%.4f,%.4f)", player.collider.physics.position.x, player.collider.physics.position.y);
							ImGui::Text("Player Vel: (%.4f,%.4f)", player.collider.physics.velocity.x, player.collider.physics.velocity.y);
							ImGui::Text("Player Acc: (%.4f,%.4f)", player.collider.physics.acceleration.x, player.collider.physics.acceleration.y);
							ImGui::Separator();
							ImGui::Text("Player Grounded: %s", player.grounded() ? "Yes" : "No");
							ImGui::Separator();
							ImGui::Text("Ledge Height: %i", player.ledge_height);
							ImGui::Separator();
							ImGui::Text("Collision Depths:");
							if (player.collider.collision_depths) {
								ImGui::Text("Top Depth: %.4f", player.collider.collision_depths.value().top_depth());
								ImGui::Text("Bottom Depth: %.4f", player.collider.collision_depths.value().bottom_depth());
								ImGui::Text("Left Depth: %.4f", player.collider.collision_depths.value().left_depth());
								ImGui::Text("Right Depth: %.4f", player.collider.collision_depths.value().right_depth());
							}
							ImGui::Separator();
							ImGui::Text("Right Collision: %s", player.collider.flags.collision.test(shape::Collision::has_right_collision) ? "Yes" : "No");
							ImGui::Text("Left Collision: %s", player.collider.flags.collision.test(shape::Collision::has_left_collision) ? "Yes" : "No");
							ImGui::Text("Top Collision: %s", player.collider.flags.collision.test(shape::Collision::has_top_collision) ? "Yes" : "No");
							ImGui::Text("Bottom Collision: %s", player.collider.flags.collision.test(shape::Collision::has_bottom_collision) ? "Yes" : "No");
							ImGui::Separator();
							ImGui::Text("Dash Cancel Collision: %s", player.collider.flags.dash.test(shape::Dash::dash_cancel_collision) ? "Yes" : "No");

							ImGui::EndTabItem();
						}
						if (ImGui::BeginTabItem("Movement")) {
							ImGui::Text("Direction LR	: %s", player.controller.direction.print_lr().c_str());
							ImGui::Text("Direction UND	: %s", player.controller.direction.print_und().c_str());
							ImGui::Separator();
							ImGui::Text("Horizontal Movement.: %.4f", player.controller.horizontal_movement());
							ImGui::Text("Vertical Movement...: %.4f", player.controller.vertical_movement());
							ImGui::Separator();
							ImGui::Text("Controller");
							ImGui::Text("Move Left : %s", player.controller.get_controller_state(player::ControllerInput::move_x) < 0.f ? "Yes" : "No");
							ImGui::Text("Move Right : %s", player.controller.get_controller_state(player::ControllerInput::move_x) > 0.f ? "Yes" : "No");
							ImGui::Text("Inspecting : %s", player.controller.get_controller_state(player::ControllerInput::inspect) > 0.f ? "Yes" : "No");
							ImGui::Text("Facing : %s", player.print_direction(true).c_str());
							ImGui::EndTabItem();
						}
						if (ImGui::BeginTabItem("Jump")) {
							ImGui::Text("Jump Request : %i", player.controller.get_jump().get_request());
							ImGui::Text("Jump Cooldown : %i", player.controller.get_jump().get_cooldown());
							ImGui::Text("Jump Released : %s", player.controller.get_jump().released() ? "Yes" : "No");
							ImGui::Text("Can Jump : %s", player.controller.get_jump().can_jump() ? "Yes" : "No");
							ImGui::Text("Controller Jumping? : %s", player.controller.get_jump().jumping() ? "Yes" : "No");
							ImGui::Text("Jump Began? : %s", player.controller.get_jump().began() ? "Yes" : "No");
							ImGui::Text("Collider Jumping? : %s", player.collider.flags.movement.test(shape::Movement::jumping) ? "Yes" : "No");
							ImGui::EndTabItem();
						}
						if (ImGui::BeginTabItem("Dash")) {
							ImGui::Text("Dash Value : %f", player.controller.dash_value());
							ImGui::Text("Dash Request : %i", player.controller.get_dash_request());
							ImGui::Text("Dash Count : %i", player.controller.get_dash_count());
							ImGui::Text("Can Dash? : %s", player.controller.can_dash() ? "Yes" : "No");
							ImGui::EndTabItem();
						}
						if (ImGui::BeginTabItem("Animation")) {
							ImGui::Text("Animation: %s", player.animation.animation.label.data());
							ImGui::Separator();
							ImGui::Text("Current Frame: %i", player.animation.animation.frame.get_count());
							ImGui::Text("Loop: %i", player.animation.animation.loop.get_count());
							ImGui::Text("Frame Timer: %i", player.animation.animation.frame_timer.get_cooldown());
							ImGui::Text("Complete? %s", player.animation.animation.complete() ? "Yes" : "No");
							ImGui::Text("One Off? %s", player.animation.animation.params.num_loops > -1 ? "Yes" : "No");
							ImGui::Text("Repeat Last Frame? %s", player.animation.animation.params.repeat_last_frame ? "Yes" : "No");
							ImGui::Text("Idle Timer: %i", player.animation.idle_timer.get_count());
							ImGui::Separator();
							ImGui::Text("idle...: %s", player.animation.state == player::AnimState::idle ? "flag set" : "");
							ImGui::Text("run....: %s", player.animation.state == player::AnimState::run ? "flag set" : "");
							ImGui::Text("stop...: %s", player.animation.state == player::AnimState::stop ? "flag set" : "");
							ImGui::Text("turn...: %s", player.animation.state == player::AnimState::turn ? "flag set" : "");
							ImGui::Text("hurt...: %s", player.animation.state == player::AnimState::hurt ? "flag set" : "");
							ImGui::Text("shpturn: %s", player.animation.state == player::AnimState::sharp_turn ? "flag set" : "");
							ImGui::Text("rise...: %s", player.animation.state == player::AnimState::rise ? "flag set" : "");
							ImGui::Text("suspend: %s", player.animation.state == player::AnimState::suspend ? "flag set" : "");
							ImGui::Text("fall...: %s", player.animation.state == player::AnimState::fall ? "flag set" : "");
							ImGui::Text("land...: %s", player.animation.state == player::AnimState::land ? "flag set" : "");
							ImGui::Text("dash...: %s", player.animation.state == player::AnimState::dash ? "flag set" : "");
							ImGui::Text("sprint.: %s", player.animation.state == player::AnimState::sprint ? "flag set" : "");
							ImGui::Text("wlslide: %s", player.animation.state == player::AnimState::wallslide ? "flag set" : "");
							ImGui::Text("inspect: %s", player.animation.state == player::AnimState::inspect ? "flag set" : "");
							ImGui::Text("die....: %s", player.animation.state == player::AnimState::die ? "flag set" : "");
							ImGui::EndTabItem();
						}
						if (ImGui::BeginTabItem("Catalog")) {
							ImGui::Text("Player Stats");
							ImGui::SliderInt("Max Orbs", &player.player_stats.max_orbs, 99, 99999);
							ImGui::SliderInt("Orbs", &player.player_stats.orbs, 0, 99999);
							ImGui::Text("Shield: %f", player.controller.get_shield().health.get_hp());
							ImGui::Separator();
							ImGui::Text("Inventory");
							for (auto& item : player.catalog.categories.inventory.items) {
								ImGui::Text(item.label.data());
								ImGui::SameLine();
								ImGui::Text(" : %i", item.get_quantity());
							}
							ImGui::Separator();
							ImGui::Text("Abilities");
							ImGui::Text("Dash: ");
							ImGui::SameLine();
							player.catalog.categories.abilities.has_ability(player::Abilities::dash) ? ImGui::Text("Enabled") : ImGui::Text("Disabled");
							if (ImGui::Button("Give Dash")) { player.catalog.categories.abilities.give_ability(player::Abilities::dash); }
							if (ImGui::Button("Remove Dash")) { player.catalog.categories.abilities.remove_ability(player::Abilities::dash); }
							ImGui::Text("Wallslide: ");
							ImGui::SameLine();
							player.catalog.categories.abilities.has_ability(player::Abilities::wall_slide) ? ImGui::Text("Enabled") : ImGui::Text("Disabled");
							if (ImGui::Button("Give Wallslide")) { player.catalog.categories.abilities.give_ability(player::Abilities::wall_slide); }
							if (ImGui::Button("Remove Wallslide")) { player.catalog.categories.abilities.remove_ability(player::Abilities::wall_slide); }
							ImGui::Text("Shield: ");
							ImGui::SameLine();
							player.catalog.categories.abilities.has_ability(player::Abilities::shield) ? ImGui::Text("Enabled") : ImGui::Text("Disabled");
							if (ImGui::Button("Give Shield")) { player.catalog.categories.abilities.give_ability(player::Abilities::shield); }
							if (ImGui::Button("Remove Shield")) { player.catalog.categories.abilities.remove_ability(player::Abilities::shield); }
							ImGui::Text("Double Jump: ");
							ImGui::SameLine();
							player.catalog.categories.abilities.has_ability(player::Abilities::double_jump) ? ImGui::Text("Enabled") : ImGui::Text("Disabled");
							if (ImGui::Button("Give Double Jump")) { player.catalog.categories.abilities.give_ability(player::Abilities::double_jump); }
							if (ImGui::Button("Remove Double Jump")) { player.catalog.categories.abilities.remove_ability(player::Abilities::double_jump); }
							ImGui::EndTabItem();
						}
						if (ImGui::BeginTabItem("Parameter Tweaking")) {
							ImGui::Text("Vertical Movement");
							ImGui::SliderFloat("GRAVITY", &player.physics_stats.grav, 0.f, 0.8f, "%.5f");
							ImGui::SliderFloat("JUMP VELOCITY", &player.physics_stats.jump_velocity, 0.5f, 12.0f, "%.5f");
							ImGui::SliderFloat("JUMP RELEASE MULTIPLIER", &player.physics_stats.jump_release_multiplier, 0.005f, 1.f, "%.5f");
							ImGui::SliderFloat("MAX Y VELOCITY", &player.physics_stats.maximum_velocity.y, 1.0f, 60.0f);

							ImGui::Separator();
							ImGui::Text("Horizontal Movement");
							ImGui::SliderFloat("AIR MULTIPLIER", &player.physics_stats.air_multiplier, 0.0f, 5.0f);
							ImGui::SliderFloat("GROUND FRICTION", &player.physics_stats.ground_fric, 0.800f, 1.000f);
							ImGui::SliderFloat("AIR FRICTION", &player.physics_stats.air_fric, 0.800f, 1.000f);
							ImGui::SliderFloat("GROUND SPEED", &player.physics_stats.x_acc, 0.0f, 3.f);
							ImGui::SliderFloat("SPRINT MULTIPLIER", &player.physics_stats.sprint_multiplier, 1.0f, 4.0f);
							ImGui::SliderFloat("MAX X VELOCITY", &player.physics_stats.maximum_velocity.x, 1.0f, 10.0f);

							ImGui::Separator();
							ImGui::Text("Abilities");
							ImGui::SliderFloat("Dash Speed", &player.physics_stats.dash_speed, 1.0f, 30.0f);
							ImGui::SliderFloat("Vertical Dash Multiplier", &player.physics_stats.vertical_dash_multiplier, 0.0f, 10.0f);
							ImGui::SliderFloat("Dash Dampen", &player.physics_stats.dash_dampen, 0.7f, 2.0f);
							ImGui::SliderFloat("Wallslide Speed", &player.physics_stats.wallslide_speed, 0.2f, 3.0f);

							ImGui::Separator();
							if (ImGui::Button("Save Parameters")) { services.data.save_player_params(player); }
							ImGui::EndTabItem();
						}
						if (ImGui::BeginTabItem("Misc")) {

							ImGui::Text("Alive? %s", player.alive() ? "Yes" : "No");

							ImGui::Text("Invincibility Counter: %i", player.counters.invincibility);
							ImGui::Text("Spike Trigger: %s", player.collider.spike_trigger ? "True" : "False");
							ImGui::Text("On Ramp: %s", player.collider.on_ramp() ? "True" : "False");

							ImGui::Text("Grounded: %s", player.grounded() ? "Yes" : "No");
							ImGui::EndTabItem();
						}
						ImGui::EndTabBar();
					}
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Weapon")) {
					if (ImGui::Button("Toggle Weapons")) {
						if (player.arsenal.value().empty()) {
							player.arsenal.value().push_to_loadout(0);
							player.arsenal.value().push_to_loadout(1);
							player.arsenal.value().push_to_loadout(2);
							player.arsenal.value().push_to_loadout(3);
							player.arsenal.value().push_to_loadout(4);
							player.arsenal.value().push_to_loadout(5);
						} else {
							player.arsenal.value().clear();
						}
					}

					ImGui::Separator();
					ImGui::Text("Grappling Hook:");
					ImGui::Text("Hook held: %s", player.controller.hook_held() ? "Yes" : "No");
					ImGui::Text("Direction: %s", player.equipped_weapon().projectile.hook.probe_direction.print_intermediate().c_str());

					ImGui::Separator();

					ImGui::Text("Firing Direction %s", player.equipped_weapon().firing_direction.print_lr().c_str());
					ImGui::Text("Firing Direction %s", player.equipped_weapon().firing_direction.print_und().c_str());

					ImGui::Text("Cooling Down? %s", player.equipped_weapon().cooling_down() ? "Yes" : "No");
					ImGui::Text("Cooldown Time %i", player.equipped_weapon().cooldown.get_cooldown());
					ImGui::Text("Active Projectiles: %i", player.equipped_weapon().active_projectiles);

					ImGui::Separator();
					ImGui::Text("Equipped Weapon: %s", player.equipped_weapon().label.data());
					ImGui::Text("UI color: %i", (int)player.equipped_weapon().attributes.ui_color);
					ImGui::Text("Sprite Dimensions X: %i", player.equipped_weapon().sprite_dimensions.x);
					ImGui::Text("Sprite Dimensions Y: %i", player.equipped_weapon().sprite_dimensions.y);
					ImGui::Text("Barrel Point X: %.1f", player.equipped_weapon().barrel_point.x);
					ImGui::Text("Barrel Point Y: %.1f", player.equipped_weapon().barrel_point.y);
					ImGui::Separator();
					ImGui::Text("Weapon Stats: ");
					ImGui::Indent();
					ImGui::Text("Rate: (%i)", player.equipped_weapon().attributes.rate);
					ImGui::Text("Cooldown: (%i)", player.equipped_weapon().attributes.cooldown_time);
					ImGui::Text("Recoil: (%.2f)", player.equipped_weapon().attributes.recoil);

					ImGui::Separator();
					ImGui::Unindent();
					ImGui::Text("Projectile Stats: ");
					ImGui::Indent();
					ImGui::Text("Damage: (%f)", player.equipped_weapon().projectile.stats.base_damage);
					ImGui::Text("Range: (%i)", player.equipped_weapon().projectile.stats.range);
					ImGui::Text("Speed: (%.2f)", player.equipped_weapon().projectile.stats.speed);
					ImGui::Text("Velocity: (%.4f,%.4f)", player.equipped_weapon().projectile.physics.velocity.x, player.equipped_weapon().projectile.physics.velocity.y);
					ImGui::Text("Position: (%.4f,%.4f)", player.equipped_weapon().projectile.physics.position.x, player.equipped_weapon().projectile.physics.position.y);
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("General")) {

					ImGui::Text("Draw Calls: %u", trackers.draw_calls);
					trackers.draw_calls = 0;

					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Resources")) {
					ImGui::Text("Size of Asset Manager (Bytes): %lu", sizeof(services.assets));
					ImGui::Text("Size of Data Manager (Bytes): %lu", sizeof(services.data));
					ImGui::Text("Size of Player (Bytes): %lu", sizeof(player));
					ImGui::Text("Size of TextureUpdater (Bytes): %lu", sizeof(player.texture_updater));
					ImGui::Text("Size of Collider (Bytes): %lu", sizeof(player.collider));
					ImGui::Text("Size of Arsenal (Bytes): %lu", sizeof(player.arsenal));
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("State")) {
					ImGui::Separator();
					ImGui::Text("State");
					ImGui::Text("Current State: ");
					ImGui::SameLine();
					/*if (ImGui::Button("Under")) {
						services.assets.click.play();
						game_state.set_current_state(std::make_unique<automa::Dojo>(services, player, "dojo"));
						game_state.get_current_state().init(services, "/level/UNDER_LEDGE_01");
						player.set_position({player::PLAYER_START_X, player::PLAYER_START_Y});
					}
					if (ImGui::Button("Minigus")) {
						services.assets.click.play();
						game_state.set_current_state(std::make_unique<automa::Dojo>(services, player, "dojo"));
						game_state.get_current_state().init(services, "/level/FIRSTWIND_DECK_01");
						player.set_position({32 * 4, 32 * 8});
					}
					if (ImGui::Button("Corridor 4")) {
						services.assets.click.play();
						game_state.set_current_state(std::make_unique<automa::Dojo>(services, player, "dojo"));
						game_state.get_current_state().init(services, "/level/FIRSTWIND_CORRIDOR_04");
						player.set_position({player::PLAYER_START_X * 2.f, player::PLAYER_START_Y * 2.f});
					}
					if (ImGui::Button("1x1 TEST")) {
						services.assets.click.play();
						game_state.set_current_state(std::make_unique<automa::Dojo>(services, player, "dojo"));
						game_state.get_current_state().init(services, "/level/1x1_TEST");
						player.set_position({player::PLAYER_START_X * 2.f + 128, player::PLAYER_START_Y * 2.f});
					}
					if (ImGui::Button("House")) {
						services.assets.click.play();
						game_state.set_current_state(std::make_unique<automa::Dojo>(services, player, "dojo"));
						game_state.get_current_state().init(services, "/level/UNDER_HUT_01");

						player.set_position({100, 160});
					}
					if (ImGui::Button("Night Plat")) {
						services.assets.click.play();
						game_state.set_current_state(std::make_unique<automa::Dojo>(services, player, "dojo"));
						game_state.get_current_state().init(services, "/level/NIGHT_PLATFORMS_01");

						player.set_position({100, 160});
					}
					if (ImGui::Button("Base")) {
						services.assets.click.play();
						game_state.set_current_state(std::make_unique<automa::Dojo>(services, player, "dojo"));
						game_state.get_current_state().init(services, "/level/BASE_LIVING_01");

						player.set_position({25 * 32, 10 * 32});
					}
					if (ImGui::Button("Mayhem")) {
						services.assets.click.play();
						game_state.set_current_state(std::make_unique<automa::Dojo>(services, player, "dojo"));
						game_state.get_current_state().init(services, "/level/NIGHT_MAYHEM_01");

						player.set_position({28 * 32, 8 * 32});
					}
					if (ImGui::Button("Abandoned Course")) {
						services.assets.click.play();
						game_state.set_current_state(std::make_unique<automa::Dojo>(services, player, "dojo"));
						game_state.get_current_state().init(services, "/level/ABANDONED_COURSE_01");

						player.set_position({15 * 32 * 4, 8 * 32});
					}
					if (ImGui::Button("Abandoned Passage")) {
						services.assets.click.play();
						game_state.set_current_state(std::make_unique<automa::Dojo>(services, player, "dojo"));
						game_state.get_current_state().init(services, "/level/ABANDONED_PASSAGE_01");

						player.set_position({16 * 32 * 6, 8 * 32});
					}
					if (ImGui::Button("Breakable Test")) {
						services.assets.click.play();
						game_state.set_current_state(std::make_unique<automa::Dojo>(services, player, "dojo"));
						game_state.get_current_state().init(services, "/level/BREAKABLE_TEST_01");

						player.set_position({20 * 32, 8 * 32});
					}
					if (ImGui::Button("Skycorps")) {
						services.assets.click.play();
						game_state.set_current_state(std::make_unique<automa::Dojo>(services, player, "dojo"));
						game_state.get_current_state().init(services, "/level/SKYCORPS_YARD_01");

						player.set_position({28 * 32, 8 * 32});
					}
					if (ImGui::Button("Sky")) {
						services.assets.click.play();
						game_state.set_current_state(std::make_unique<automa::Dojo>(services, player, "dojo"));
						game_state.get_current_state().init(services, "/level/SKY_CHAMBER_01");
						player.set_position({7 * 32, 16 * 32});
					}

					if (ImGui::Button("Corridor 2")) {
						services.assets.click.play();
						game_state.set_current_state(std::make_unique<automa::Dojo>(services, player, "dojo"));
						game_state.get_current_state().init(services, "/level/FIRSTWIND_CORRIDOR_02");
						player.set_position({7 * 32, 7 * 32});
					}
					if (ImGui::Button("Shadow")) {
						services.assets.click.play();
						game_state.set_current_state(std::make_unique<automa::Dojo>(services, player, "dojo"));
						game_state.get_current_state().init(services, "/level/SHADOW_DOJO_01");
						player.set_position({4 * 32, 11 * 32});
					}
					if (ImGui::Button("Stone")) {
						services.assets.click.play();
						game_state.set_current_state(std::make_unique<automa::Dojo>(services, player, "dojo"));
						game_state.get_current_state().init(services, "/level/STONE_CORRIDOR_01");
						player.set_position({10 * 32, 16 * 32});
					}
					if (ImGui::Button("Overturned")) {
						services.assets.click.play();
						game_state.set_current_state(std::make_unique<automa::Dojo>(services, player, "dojo"));
						game_state.get_current_state().init(services, "/level/OVERTURNED_DOJO_01");
						player.set_position({4 * 32, 11 * 32});
					}
					if (ImGui::Button("Glade")) {
						services.assets.click.play();
						game_state.set_current_state(std::make_unique<automa::Dojo>(services, player, "dojo"));
						game_state.get_current_state().init(services, "/level/OVERTURNED_GLADE_01");
						player.set_position({8 * 32, 8 * 32});
					}
					if (ImGui::Button("Woodshine")) {
						services.assets.click.play();
						game_state.set_current_state(std::make_unique<automa::Dojo>(services, player, "dojo"));
						game_state.get_current_state().init(services, "/level/WOODSHINE_VILLAGE_01");
						player.set_position({32, 1280});
					}
					if (ImGui::Button("Collision Room")) {
						services.assets.click.play();
						game_state.set_current_state(std::make_unique<automa::Dojo>(services, player, "dojo"));
						game_state.get_current_state().init(services, "/level/SKY_COLLISIONROOM_01");
						player.set_position({5 * 32, 5 * 32});
					}
					if (ImGui::Button("Grub Dojo")) {
						services.assets.click.play();
						game_state.set_current_state(std::make_unique<automa::Dojo>(services, player, "dojo"));
						game_state.get_current_state().init(services, "/level/GRUB_DOJO_01");
						player.set_position({3 * 32, 8 * 32});
					}
					if (ImGui::Button("Firstwind Dojo")) {
						services.assets.click.play();
						game_state.set_current_state(std::make_unique<automa::Dojo>(services, player, "dojo"));
						game_state.get_current_state().init(services, "/level/BASE_TEST_03");
						player.set_position({3 * 32, 8 * 32});
					}*/
					/*if (ImGui::Button("Atrium")) {
						services.assets.click.play();
						game_state.set_current_state(std::make_unique<automa::Dojo>(services, player, "dojo"));
						game_state.get_current_state().init(services, services.assets.resource_path + "/level/FIRSTWIND_ATRIUM_01");
					}
					if (ImGui::Button("Hangar")) {
						services.assets.click.play();
						game_state.set_current_state(std::make_unique<automa::Dojo>(services, player, "dojo"));
						game_state.get_current_state().init(services, services.assets.resource_path + "/level/FIRSTWIND_HANGAR_01");
						player.set_position({ 3080, 790 });
					}
					if (ImGui::Button("Corridor 3")) {
						services.assets.click.play();
						game_state.set_current_state(std::make_unique<automa::Dojo>(services, player, "dojo"));
						game_state.get_current_state().init(services, services.assets.resource_path + "/level/FIRSTWIND_CORRIDOR_03");
						player.set_position({ 2327, 360 });
					}
					if(ImGui::Button("Lab")) {
						services.assets.click.play();
						game_state.set_current_state(std::make_unique<automa::Dojo>(services, player, "dojo"));
						game_state.get_current_state().init(services, services.assets.resource_path + "/level/TOXIC_LAB_01");
					}*/
					/*if (ImGui::Button("Toxic")) {
						services.assets.click.play();
						game_state.set_current_state(std::make_unique<automa::Dojo>(services, player, "dojo"));
						game_state.get_current_state().init(services, "/level/TOXIC_ARENA_01");
						player.set_position({player::PLAYER_START_X, player::PLAYER_START_Y});
						player.collider.physics.zero();
						player.set_position({34, 484});
					}
					if (ImGui::Button("Grub")) {
						services.assets.click.play();
						game_state.set_current_state(std::make_unique<automa::Dojo>(services, player, "dojo"));
						game_state.get_current_state().init(services, "/level/GRUB_TUNNEL_01");
						player.set_position({224, 290});
					}*/
					/*if(ImGui::Button("Night")) {
						services.assets.click.play();
						game_state.set_current_state(std::make_unique<automa::Dojo>(services, player, "dojo"));
						game_state.get_current_state().init(services, services.assets.resource_path + "/level/NIGHT_CRANE_01");
						player.set_position({50, 50});
						player.assign_texture(services.assets.t_nani_dark);
					}*/
					/*if (ImGui::Button("Night 2")) {
						services.assets.click.play();
						game_state.set_current_state(std::make_unique<automa::Dojo>(services, player, "dojo"));
						game_state.get_current_state().init(services, "/level/NIGHT_CATWALK_01");
						player.set_position({50, 50});
					}*/
					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
			}
		}
		ImVec2 prev_size = ImGui::GetWindowSize();
		ImGui::End();
	}
}

void Game::playtester_portal(sf::RenderWindow& window) {
	if (!flags.test(GameFlags::playtest)) { return; }
	// if (flags.test(GameFlags::in_game)) { return; }
	bool* b_debug{};
	float const PAD = 10.0f;
	static int corner = 1;
	ImGuiIO& io = ImGui::GetIO();
	io.FontGlobalScale = 1.0;
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
	if (corner != -1) {
		ImGuiViewport const* viewport = ImGui::GetMainViewport();
		ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
		ImVec2 work_size = viewport->WorkSize;
		ImVec2 window_pos, window_pos_pivot;
		window_pos.x = (corner & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
		window_pos.y = (corner & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
		window_pos_pivot.x = (corner & 1) ? 1.0f : 0.0f;
		window_pos_pivot.y = (corner & 2) ? 1.0f : 0.0f;
		ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
		window_flags |= ImGuiWindowFlags_NoMove;
		ImGui::SetNextWindowBgAlpha(0.65f); // Transparent background
		if (ImGui::Begin("Playtester Portal", b_debug, window_flags)) {
			ImGui::Text("Playtester Portal");
			ImGui::Separator();
			ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
			if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags)) {
				if (ImGui::BeginTabItem("General")) {
					ImGui::Text("Region: %s", game_state.get_current_state().target_folder.paths.region.string().c_str());
					ImGui::Text("Room: %s", game_state.get_current_state().target_folder.paths.room.string().c_str());
					ImGui::Text("demo mode: %s", services.demo_mode() ? "Enabled" : "Disabled");
					if (ImGui::Button("Toggle Demo Mode")) { services.debug_flags.test(automa::DebugFlags::demo_mode) ? services.debug_flags.reset(automa::DebugFlags::demo_mode) : services.debug_flags.set(automa::DebugFlags::demo_mode); }
					if (ImGui::Button("Toggle Greyblock Mode")) {
						services.debug_flags.set(automa::DebugFlags::greyblock_trigger);
						services.debug_flags.test(automa::DebugFlags::greyblock_mode) ? services.debug_flags.reset(automa::DebugFlags::greyblock_mode) : services.debug_flags.set(automa::DebugFlags::greyblock_mode);
					}
					ImGui::Separator();
					ImGui::Text("Player");
					ImGui::Text("World Grounded? %s", player.collider.perma_grounded() ? "Yes" : "No");
					ImGui::Text("Horizontal Movement: %f", player.controller.horizontal_movement());
					ImGui::Text("Coyote Time: %i", player.controller.get_jump().get_coyote());
					ImGui::Text("Push Time: %i", player.cooldowns.push.get_cooldown());
					ImGui::Text("Can Doublejump? %s", player.controller.get_jump().can_doublejump() ? "Yes" : "No");
					ImGui::Text("Jump Count: %i", player.controller.get_jump().get_count());
					ImGui::Text("Downhill? %s", player.collider.downhill() ? "Yes" : "No");
					ImGui::Text("X Velocity: %.2f", player.collider.physics.velocity.x);
					ImGui::Text("Y Velocity: %.2f", player.collider.physics.velocity.y);

					ImGui::Separator();
					ImGui::Text("Ticker");
					ImGui::Text("dt: %.8f", services.ticker.dt.count());
					ImGui::Separator();
					ImGui::Text("Seconds Passed: %.2f", services.ticker.total_seconds_passed.count());
					ImGui::Text("Seconds Passed In-Game: %.2f", services.ticker.in_game_seconds_passed.count());
					ImGui::Text("Milliseconds Passed: %.0f", services.ticker.total_milliseconds_passed.count());
					ImGui::Text("Ticks Per Frame: %.2f", services.ticker.ticks_per_frame);
					ImGui::Text("Frames Per Second: %.2f", services.ticker.fps);
					ImGui::Separator();
					if (ImGui::SliderFloat("DeltaTime Scalar", &services.ticker.dt_scalar, 0.0f, 2.f, "%.3f")) { services.ticker.scale_dt(); };
					if (ImGui::Button("Reset")) { services.ticker.reset_dt(); }
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Input")) {
					ImGui::Text("Current Input Device: %s", "TODO"); // XXX services.controller_map.is_gamepad() ? "Gamepad" : "Keyboard
					ImGui::Text("Gamepad Status: %s", services.controller_map.gamepad_connected() ? "Connected" : "Disconnected");
					ImGui::Text("Gamepad Enabled? %s", services.controller_map.is_gamepad_input_enabled() ? "Yes" : "No");
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Tutorial")) {
					ImGui::Text("Jump..: %s", player.tutorial.flags.test(text::TutorialFlags::jump) ? "Yes" : "No");
					ImGui::Text("Shoot.: %s", player.tutorial.flags.test(text::TutorialFlags::shoot) ? "Yes" : "No");
					ImGui::Text("Sprint: %s", player.tutorial.flags.test(text::TutorialFlags::sprint) ? "Yes" : "No");
					ImGui::Text("Inv...: %s", player.tutorial.flags.test(text::TutorialFlags::inventory) ? "Yes" : "No");
					ImGui::Text("Map...: %s", player.tutorial.flags.test(text::TutorialFlags::map) ? "Yes" : "No");
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Transponder")) {
					ImGui::Text("Voice Shipment..: %i", player.transponder.shipments.voice.get_residue());
					ImGui::Text("Emotion Shipment: %i", player.transponder.shipments.emotion.get_residue());
					ImGui::Text("Item Shipment...: %i", player.transponder.shipments.item.get_residue());
					ImGui::Text("Quest Shipment..: %i", player.transponder.shipments.quest.get_residue());
					ImGui::Separator();
					ImGui::Text("MiniMap Scale..: %f", game_state.get_current_state().inventory_window.minimap.get_scale());
					ImGui::Text("MiniMap X Pos..: %f", game_state.get_current_state().inventory_window.minimap.get_position().x);
					ImGui::Text("MiniMap Y Pos..: %f", game_state.get_current_state().inventory_window.minimap.get_position().y);
					ImGui::Separator();
					ImGui::Text("MiniMap Center X Pos..: %f", game_state.get_current_state().inventory_window.minimap.get_center_position().x);
					ImGui::Text("MiniMap Center Y Pos..: %f", game_state.get_current_state().inventory_window.minimap.get_center_position().y);
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Music")) {
					ImGui::Separator();
					ImGui::Checkbox("Music Player", &playtest.m_musicplayer);
					ImGui::Text("Volume: ");
					ImGui::SameLine();
					ImGui::SliderFloat("##vol", &services.music.volume.multiplier, 0.f, 1.f, "%.3f");
					if (playtest.m_musicplayer && services.music.global_off()) {
						services.music.turn_on();
					} else if (!playtest.m_musicplayer && !services.music.global_off()) {
						services.music.turn_off();
					}
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Story")) {
					ImGui::Separator();
					ImGui::Text("Quest Progress:");
					ImGui::Text("Bit: %i", services.quest.get_progression(QuestType::npc, 20));
					ImGui::Text("Bryn's Notebook: %i", services.quest.get_progression(QuestType::inspectable, 1));
					ImGui::Text("Boiler: %i", services.quest.get_progression(QuestType::inspectable, 110));
					ImGui::Separator();
					ImGui::Text("Stats:");
					ImGui::Text("Death count: %i", services.stats.player.death_count.get_count());
					ImGui::Text("Bullets fired: %i", services.stats.player.bullets_fired.get_count());
					ImGui::Text("Guns collected: %i", services.stats.player.guns_collected.get_count());
					ImGui::Text("Items collected: %i", services.stats.player.items_collected.get_count());
					ImGui::Text("Orbs collected: %i", services.stats.treasure.total_orbs_collected.get_count());
					ImGui::Text("Blue orbs collected: %i", services.stats.treasure.blue_orbs.get_count());
					ImGui::Text("Highest indicator amount: %i", services.stats.treasure.highest_indicator_amount.get_count());
					ImGui::Text("Rooms discovered: %i", services.stats.world.rooms_discovered.get_count());
					ImGui::Text("Enemies killed: %i", services.stats.enemy.enemies_killed.get_count());
					ImGui::EndTabItem();
				}
				if (flags.test(GameFlags::in_game)) {

					if (ImGui::BeginTabItem("State")) {
						if (ImGui::Button("Main Menu")) {
							game_state.set_current_state(std::make_unique<automa::MainMenu>(services, player, "main"));
							flags.reset(GameFlags::in_game);
						}
						if (ImGui::Button("Fall")) {
							services.assets.click.play();
							game_state.set_current_state(std::make_unique<automa::Dojo>(services, player, "dojo"));
							game_state.get_current_state().init(services, 122);
							player.set_position({32 * 3, 32 * 3});
						}
						if (ImGui::Button("Minigus")) {
							services.assets.click.play();
							game_state.set_current_state(std::make_unique<automa::Dojo>(services, player, "dojo"));
							game_state.get_current_state().init(services, 115);
							player.set_position({32 * 3, 32 * 8});
						}
						if (ImGui::Button("Hangar")) {
							services.assets.click.play();
							game_state.set_current_state(std::make_unique<automa::Dojo>(services, player, "dojo"));
							game_state.get_current_state().init(services, 112);
							player.set_position({32 * 2, 32 * 8});
						}
						if (ImGui::Button("Canopy")) {
							services.assets.click.play();
							game_state.set_current_state(std::make_unique<automa::Dojo>(services, player, "dojo"));
							game_state.get_current_state().init(services, 224);
							player.set_position({32 * 4, 32 * 8});
						}
						if (ImGui::Button("Hideout")) {
							services.assets.click.play();
							game_state.set_current_state(std::make_unique<automa::Dojo>(services, player, "dojo"));
							game_state.get_current_state().init(services, 125);
							player.set_position({32 * 8, 32 * 2});
						}
						if (ImGui::Button("Shaft")) {
							services.assets.click.play();
							game_state.set_current_state(std::make_unique<automa::Dojo>(services, player, "dojo"));
							game_state.get_current_state().init(services, 107);
							player.set_position({32 * 6, 32 * 4});
						}
						if (ImGui::Button("Atrium 1")) {
							services.assets.click.play();
							game_state.set_current_state(std::make_unique<automa::Dojo>(services, player, "dojo"));
							game_state.get_current_state().init(services, 102);
							player.set_position({32 * 45, 32 * 56});
						}
						if (ImGui::Button("Corridor 2")) {
							services.assets.click.play();
							game_state.set_current_state(std::make_unique<automa::Dojo>(services, player, "dojo"));
							game_state.get_current_state().init(services, 104);
							player.set_position({7 * 32, 7 * 32});
						}
						if (ImGui::Button("Arena")) {
							services.assets.click.play();
							game_state.set_current_state(std::make_unique<automa::Dojo>(services, player, "dojo"));
							game_state.get_current_state().init(services, 121);
							player.set_position({3 * 32, 9 * 32});
						}
						if (ImGui::Button("Bunker")) {
							services.assets.click.play();
							game_state.set_current_state(std::make_unique<automa::Dojo>(services, player, "dojo"));
							game_state.get_current_state().init(services, 124);
							player.set_position({4 * 32, 3 * 32});
						}
						if (ImGui::Button("Cargo")) {
							services.assets.click.play();
							game_state.set_current_state(std::make_unique<automa::Dojo>(services, player, "dojo"));
							game_state.get_current_state().init(services, 103);
							player.set_position({7 * 32, 7 * 32});
						}
						if (ImGui::Button("Prison")) {
							services.assets.click.play();
							game_state.set_current_state(std::make_unique<automa::Dojo>(services, player, "dojo"));
							game_state.get_current_state().init(services, 100);
							player.set_position({7 * 32, 7 * 32});
						}
						if (ImGui::Button("Lab")) {
							services.assets.click.play();
							game_state.set_current_state(std::make_unique<automa::Dojo>(services, player, "dojo"));
							game_state.get_current_state().init(services, 110);
							player.set_position({7 * 32, 9 * 32});
						}
						ImGui::Text("Test Levels:");
						if (ImGui::Button("Junkyard")) {
							services.assets.click.play();
							game_state.set_current_state(std::make_unique<automa::Dojo>(services, player, "dojo"));
							game_state.get_current_state().init(services, 3001);
							player.set_position({4 * 32, 9 * 32});
						}
						if (ImGui::Button("Bridge")) {
							services.assets.click.play();
							game_state.set_current_state(std::make_unique<automa::Dojo>(services, player, "dojo"));
							game_state.get_current_state().init(services, 6001);
							player.set_position({4 * 32, 9 * 32});
						}
						ImGui::EndTabItem();
					}
					if (ImGui::BeginTabItem("Player")) {
						if (ImGui::BeginTabBar("PlayerTabBar", tab_bar_flags)) {
							if (ImGui::BeginTabItem("Weapon")) {
								ImGui::Text("Player has arsenal? %s", player.arsenal ? "Yes" : "No");
								ImGui::Text("Loadout Size: %i", player.arsenal ? player.arsenal.value().size() : 0);
								playtest_sync();
								ImGui::Checkbox("Bryn's Gun", &playtest.weapons.bryn);
								toggle_weapon(playtest.weapons.bryn, 0);
								ImGui::Checkbox("Plasmer", &playtest.weapons.plasmer);
								toggle_weapon(playtest.weapons.plasmer, 1);
								ImGui::Checkbox("Tomahawk", &playtest.weapons.tomahawk);
								toggle_weapon(playtest.weapons.tomahawk, 3);
								ImGui::Checkbox("Grappling Hook", &playtest.weapons.grapple);
								toggle_weapon(playtest.weapons.grapple, 4);
								ImGui::Checkbox("Grenade Launcher", &playtest.weapons.grenade);
								toggle_weapon(playtest.weapons.grenade, 5);
								ImGui::Checkbox("Staple Gun", &playtest.weapons.staple_gun);
								toggle_weapon(playtest.weapons.staple_gun, 8);
								ImGui::Checkbox("Indie", &playtest.weapons.indie);
								toggle_weapon(playtest.weapons.indie, 9);
								ImGui::Checkbox("Gnat", &playtest.weapons.gnat);
								toggle_weapon(playtest.weapons.gnat, 10);
								ImGui::Separator();

								if (ImGui::Button("Clear Loadout")) {
									if (player.arsenal) { player.arsenal = {}; }
									playtest.weapons = {};
								}
								ImGui::EndTabItem();
							}
							if (ImGui::BeginTabItem("Catalog")) {
								ImGui::Separator();
								ImGui::Text("Inventory");
								for (auto& item : player.catalog.categories.inventory.items) {
									ImGui::Text(item.label.data());
									ImGui::SameLine();
									ImGui::Text(" : %i", item.get_quantity());
								}
								ImGui::Separator();

								ImGui::Text("Abilities");
								ImGui::Checkbox("Dash", &playtest.b_dash);
								ImGui::Checkbox("Shield", &playtest.b_shield);
								ImGui::Checkbox("Wallslide", &playtest.b_wallslide);
								ImGui::Checkbox("Double Jump", &playtest.b_doublejump);
								playtest.b_dash ? player.catalog.categories.abilities.give_ability(player::Abilities::dash) : player.catalog.categories.abilities.remove_ability(player::Abilities::dash);
								playtest.b_shield ? player.catalog.categories.abilities.give_ability(player::Abilities::shield) : player.catalog.categories.abilities.remove_ability(player::Abilities::shield);
								playtest.b_wallslide ? player.catalog.categories.abilities.give_ability(player::Abilities::wall_slide) : player.catalog.categories.abilities.remove_ability(player::Abilities::wall_slide);
								playtest.b_doublejump ? player.catalog.categories.abilities.give_ability(player::Abilities::double_jump) : player.catalog.categories.abilities.remove_ability(player::Abilities::double_jump);

								ImGui::EndTabItem();
							}
							if (ImGui::BeginTabItem("Parameters")) {
								ImGui::Text("Vertical Movement");
								ImGui::SliderFloat("GRAVITY", &player.physics_stats.grav, 0.f, 2.8f, "%.5f");
								ImGui::SliderFloat("JUMP VELOCITY", &player.physics_stats.jump_velocity, 0.5f, 18.0f, "%.5f");
								ImGui::SliderFloat("JUMP RELEASE MULTIPLIER", &player.physics_stats.jump_release_multiplier, 0.005f, 1.f, "%.5f");
								ImGui::SliderFloat("MAX Y VELOCITY", &player.physics_stats.maximum_velocity.y, 1.0f, 60.0f);

								ImGui::Separator();
								ImGui::Text("Horizontal Movement");
								ImGui::SliderFloat("AIR MULTIPLIER", &player.physics_stats.air_multiplier, 0.0f, 5.0f);
								ImGui::SliderFloat("GROUND FRICTION", &player.physics_stats.ground_fric, 0.800f, 1.000f);
								ImGui::SliderFloat("AIR FRICTION", &player.physics_stats.air_fric, 0.800f, 1.000f);
								ImGui::SliderFloat("GROUND SPEED", &player.physics_stats.x_acc, 0.0f, 3.f);
								ImGui::SliderFloat("SPRINT MULTIPLIER", &player.physics_stats.sprint_multiplier, 1.0f, 4.0f);
								ImGui::SliderFloat("MAX X VELOCITY", &player.physics_stats.maximum_velocity.x, 1.0f, 10.0f);

								ImGui::Separator();
								ImGui::Text("Abilities");
								ImGui::SliderFloat("Dash Speed", &player.physics_stats.dash_speed, 1.0f, 30.0f);
								ImGui::SliderFloat("Vertical Dash Multiplier", &player.physics_stats.vertical_dash_multiplier, 0.0f, 10.0f);
								ImGui::SliderFloat("Dash Dampen", &player.physics_stats.dash_dampen, 0.7f, 2.0f);
								ImGui::SliderFloat("Wallslide Speed", &player.physics_stats.wallslide_speed, 0.2f, 3.0f);

								ImGui::Separator();
								if (ImGui::Button("Save Parameters")) { services.data.save_player_params(player); }
								ImGui::EndTabItem();
							}
							ImGui::EndTabBar();
						}
						ImGui::EndTabItem();
					}
				}
				ImGui::EndTabBar();
			}
		}
		ImVec2 prev_size = ImGui::GetWindowSize();
		ImGui::End();
	}
}

void Game::take_screenshot(sf::Texture& screencap) {
	std::time_t const now = std::time(nullptr);

	std::time_t time = std::time({});
	char time_string[std::size("yyyy-mm-ddThh:mm:ssZ")];
	std::strftime(std::data(time_string), std::size(time_string), "%FT%TZ", std::gmtime(&time));
	std::string time_str = time_string;

	std::erase_if(time_str, [](auto const& c) { return c == ':' || isspace(c); });
	std::string filename = "screenshot_" + time_str + ".png";
	if (screencap.copyToImage().saveToFile(filename)) { std::cout << "screenshot saved to " << filename << std::endl; }
}

bool Game::debug() { return services.debug_flags.test(automa::DebugFlags::imgui_overlay); }

void Game::playtest_sync() {
	if (!player.arsenal) {
		playtest.weapons = {};
		return;
	}
	playtest.weapons.bryn = player.arsenal.value().has(0);
	playtest.weapons.plasmer = player.arsenal.value().has(1);
	playtest.weapons.tomahawk = player.arsenal.value().has(3);
	playtest.weapons.grapple = player.arsenal.value().has(4);
	playtest.weapons.grenade = player.arsenal.value().has(5);
	playtest.weapons.staple_gun = player.arsenal.value().has(8);
	playtest.weapons.indie = player.arsenal.value().has(9);
	playtest.weapons.gnat = player.arsenal.value().has(10);
}

void Game::toggle_weapon(bool flag, int id) {
	if (!player.arsenal && flag) {
		player.push_to_loadout(id);
		return;
	}
	if (player.arsenal) {
		if (flag && !player.arsenal.value().has(id)) {
			player.push_to_loadout(id);
		} else if (!flag && player.arsenal.value().has(id)) {
			player.pop_from_loadout(id);
		}
	}
}

} // namespace fornani