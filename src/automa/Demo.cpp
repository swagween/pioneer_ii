//
//  Demo.cpp
//  automa
//
//  Created by Alex Frasca on 12/26/22.
//

#include "GameState.hpp"
#include "../util/Lookup.hpp"

namespace pi {

	namespace automa {

		Dojo::Dojo() {
			state = automa::STATE::DOJO;
			::svc::cameraLocator.get().set_position({ 1, 1 });
			::svc::playerLocator.get().set_position({ 360, 500 });
		}
		void Dojo::init(const std::string& load_path) {
			state = automa::STATE::DOJO;
			folderpath = load_path + "../";
			filepath = load_path;
			map.load(load_path);
			::svc::playerLocator.get().behavior.current_state = behavior::Behavior(behavior::idle);
			::svc::playerLocator.get().flags.input.reset(Input::inspecting);
			tileset = ::svc::assetLocator.get().tilesets.at(::lookup::get_style_id.at(map.style));
			for (int i = 0; i < 16; ++i) {
				for (int j = 0; j < 16; ++j) {
					tileset_sprites.push_back(sf::Sprite());
					tileset_sprites.back().setTexture(tileset);
					tileset_sprites.back().setTextureRect(sf::IntRect({ j * TILE_WIDTH, i * TILE_WIDTH }, { TILE_WIDTH, TILE_WIDTH }));


					::svc::assetLocator.get().sp_bryn_test.setTextureRect(sf::IntRect({ 0, 0 }, { 128, 256 }));
					::svc::assetLocator.get().sp_ui_test.setTextureRect(sf::IntRect({ 0, 0 }, { 420, 128 }));
				}
			}
			::svc::playerLocator.get().collider.physics.zero();
			::svc::playerLocator.get().flags.state.set(State::alive);
			bool found_one = false;
			for (auto& portal : map.portals) {
				if (portal.destination_map_id == ::svc::stateControllerLocator.get().source_id) {
					found_one = true;
					sf::Vector2<float> spawn_position{ portal.position.x + std::floor(portal.dimensions.x / 2), portal.position.y + portal.dimensions.y - PLAYER_HEIGHT };
					::svc::playerLocator.get().set_position(spawn_position);
					::svc::cameraLocator.get().center(spawn_position);
					::svc::cameraLocator.get().physics.position = spawn_position - sf::Vector2<float>(::svc::cameraLocator.get().bounding_box.width / 2, ::svc::cameraLocator.get().bounding_box.height / 2);
				}
			}
			if (!found_one) {
				::svc::playerLocator.get().set_position(sf::Vector2<float>(200.f, 390.f));
			}

			::svc::playerLocator.get().weapons_hotbar = { arms::WEAPON_TYPE::BRYNS_GUN };

		}

		void Dojo::setTilesetTexture(sf::Texture& t) {
			tileset_sprites.clear();
			for (int i = 0; i < 16; ++i) {
				for (int j = 0; j < 16; ++j) {
					tileset_sprites.push_back(sf::Sprite());
					tileset_sprites.back().setTexture(t);
					tileset_sprites.back().setTextureRect(sf::IntRect({ j * TILE_WIDTH, i * TILE_WIDTH }, { TILE_WIDTH, TILE_WIDTH }));
				}
			}
		}

		void Dojo::handle_events(sf::Event& event, sf::RenderWindow& win) {
			if (event.type == sf::Event::EventType::KeyPressed) {
				if (event.key.code == sf::Keyboard::Z || event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::Right) {
					::svc::playerLocator.get().flags.input.set(Input::exit_request);
				}
			}
			if (event.type == sf::Event::EventType::KeyReleased) {
				if (event.key.code == sf::Keyboard::Z || event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::Right) {
					::svc::playerLocator.get().flags.input.reset(Input::exit_request);
					::svc::playerLocator.get().unrestrict_inputs();
					::svc::playerLocator.get().flags.input.reset(Input::inspecting);
					::svc::playerLocator.get().flags.input.reset(Input::inspecting_trigger);
				}
			}
			::svc::playerLocator.get().handle_events(event);
			if (event.type == sf::Event::EventType::KeyPressed) {
				if (event.key.code == sf::Keyboard::H) {
					::svc::globalBitFlagsLocator.get().set(::svc::global_flags::greyblock_state);
					::svc::globalBitFlagsLocator.get().set(::svc::global_flags::greyblock_trigger);
				}
			}
			if (event.type == sf::Event::EventType::KeyPressed) {
				if (event.key.code == sf::Keyboard::LControl) {
					map.show_minimap = !map.show_minimap;
				}
			}
			if (event.type == sf::Event::EventType::KeyPressed) {
				if (event.key.code == sf::Keyboard::D) {
				}
			}
		}

		void Dojo::logic() {
			::svc::cameraLocator.get().previous_position = ::svc::cameraLocator.get().position;
			map.update();
			hud.update();
			::svc::cameraLocator.get().center(::svc::playerLocator.get().anchor_point);
			::svc::cameraLocator.get().update();
			::svc::cameraLocator.get().restrict_movement(map.real_dimensions);
			if (map.real_dimensions.x < ::cam::screen_dimensions.x) { ::svc::cameraLocator.get().fix_vertically(map.real_dimensions); }
			if (map.real_dimensions.y < ::cam::screen_dimensions.y) { ::svc::cameraLocator.get().fix_horizontally(map.real_dimensions); }
			::svc::playerLocator.get().update(::svc::clockLocator.get().elapsed_time);
			for (auto& critter : map.critters) { critter->update(); critter->unique_update(); critter->flags.shot = false; }
			::svc::assetLocator.get().three_pipes.setVolume(::svc::assetLocator.get().music_vol);

			::svc::cameraLocator.get().position = ::svc::cameraLocator.get().physics.position;
			::svc::cameraLocator.get().observed_velocity.x = ::svc::cameraLocator.get().position.x - ::svc::cameraLocator.get().previous_position.x;
			::svc::cameraLocator.get().observed_velocity.y = ::svc::cameraLocator.get().position.y - ::svc::cameraLocator.get().previous_position.y;
		}

		void Dojo::render(sf::RenderWindow& win) {
			sf::Vector2<float> camvel = ::svc::cameraLocator.get().physics.velocity;
			sf::Vector2<float> camoffset = ::svc::cameraLocator.get().physics.position + camvel;
			map.render_background(win, tileset_sprites, ::svc::cameraLocator.get().physics.position);



			map.render(win, tileset_sprites, ::svc::cameraLocator.get().physics.position);
			if (::svc::globalBitFlagsLocator.get().test(::svc::global_flags::greyblock_state)) { ::svc::playerLocator.get().collider.render(win, ::svc::cameraLocator.get().physics.position); }
			hud.render(win);

			map.render_console(win);

			::svc::assetLocator.get().sp_ui_test.setPosition(20, ::cam::screen_dimensions.y - 148);
			::svc::assetLocator.get().sp_bryn_test.setPosition(20, ::cam::screen_dimensions.y - 276);
			//        win.draw(svc::assetLocator.get().sp_ui_test);
			//        win.draw(svc::assetLocator.get().sp_bryn_test);

			map.transition.render(win);

			if (::svc::globalBitFlagsLocator.get().test(::svc::global_flags::greyblock_trigger)) {
				if (::svc::globalBitFlagsLocator.get().test(::svc::global_flags::greyblock_state)) {
					tileset = ::svc::assetLocator.get().tilesets.at(::lookup::get_style_id.at(::lookup::STYLE::PROVISIONAL));
					setTilesetTexture(tileset);
				} else {
					tileset = ::svc::assetLocator.get().tilesets.at(::lookup::get_style_id.at(map.style));
					setTilesetTexture(tileset);
				}
			}

		}

		void Dojo::gui_render(sf::RenderWindow& win) {
			bool* debug{};
			const float PAD = 10.0f;
			static int corner = 0;
			ImGuiIO& io = ImGui::GetIO();
			io.FontGlobalScale = 1.0;

			ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
			ImVec2 work_size = viewport->WorkSize;
			ImVec2 window_pos, window_pos_pivot;
			ImVec2 prev_window_pos{};
			ImVec2 prev_window_size{};
			window_pos.x = (corner & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
			window_pos.y = (corner & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
			window_pos_pivot.x = (corner & 1) ? 1.0f : 0.0f;
			window_pos_pivot.y = (corner & 2) ? 1.0f : 0.0f;
			ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
			window_flags |= ImGuiWindowFlags_NoMove;

			ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
			if (ImGui::Begin("Demo Mode", debug, window_flags)) {
				ImGui::Text("Pioneer (beta version 1.0.0) - Fornani Demo");
				ImGui::Separator();
				if (ImGui::IsMousePosValid()) {
					ImGui::Text("Mouse Position: (%.1f,%.1f)", io.MousePos.x, io.MousePos.y);
				} else {
					ImGui::Text("Mouse Position: <invalid>");
				}
				ImGui::Text("filepath: %s", filepath.c_str());

				prev_window_size = ImGui::GetWindowSize();
				ImGui::End();
			}
			ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
			work_size = viewport->WorkSize;
			window_pos = { window_pos.x, window_pos.y + prev_window_size.y + PAD };
			window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
			ImGui::SetNextWindowPos(window_pos);
			if (ImGui::Begin("Info", debug, window_flags)) {
				prev_window_size = ImGui::GetWindowSize();
				prev_window_pos = ImGui::GetWindowPos();
				ImGui::Text("Press `ESC` to exit demo.");
				ImGui::End();
			}
			ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
			work_size = viewport->WorkSize;
			window_pos = { window_pos.x, window_pos.y + prev_window_size.y + PAD };
			window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
			ImGui::SetNextWindowPos(window_pos);
			ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
			if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags)) {
				if (ImGui::BeginTabItem("Time")) {
					ImGui::Separator();
					ImGui::Text("Time");
					ImGui::Text("Timer Elapsed Time: %.5f", ::svc::clockLocator.get().elapsed_time.count());
					ImGui::Text("Timer Time (seconds): %.1f", ::svc::clockLocator.get().seconds);
					ImGui::Text("Time Step (milliseconds): %.1f", ::svc::clockLocator.get().dt.count());
					ImGui::Text("Accumulator: %.1f", ::svc::clockLocator.get().accumulator);
					ImGui::Text("FPS: %.1f", ::svc::clockLocator.get().FPS);

					ImGui::SliderFloat("Tick Rate: ", &::svc::clockLocator.get().rate, 0.00050f, 0.0080f, "%.8f");
					if (ImGui::Button("Reset")) { ::svc::clockLocator.get().rate = 0.001; }
					ImGui::Separator();
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Player")) {
					ImGui::Text("Player Adjustments");
					ImGui::SliderInt("Max HP", &::svc::playerLocator.get().player_stats.max_health, 3, 12);
					ImGui::SliderInt("HP", &::svc::playerLocator.get().player_stats.health, 3, 12);
					ImGui::SliderInt("Max Orbs", &::svc::playerLocator.get().player_stats.max_orbs, 99, 99999);
					ImGui::SliderInt("Orbs", &::svc::playerLocator.get().player_stats.orbs, 0, 99999);

					ImGui::SliderFloat("GRAVITY", &::svc::playerLocator.get().stats.PLAYER_GRAV, 0.000f, 0.008f);
					ImGui::SliderFloat("PLAYER MASS", &::svc::playerLocator.get().stats.PLAYER_MASS, 0.1f, 2.0f);
					ImGui::SliderFloat("AIR MANEUVERABILITY", &::svc::playerLocator.get().stats.AIR_MULTIPLIER, 0.0f, 5.0f);
					ImGui::SliderFloat("TERMINAL VELOCITY", &::svc::playerLocator.get().stats.TERMINAL_VELOCITY, 1.0f, 8.0f);

					ImGui::Text("Friction Multipliers");
					ImGui::SliderFloat("GROUND FRICTION", &::svc::playerLocator.get().stats.PLAYER_GROUND_FRIC, 0.900f, 1.000f);
					ImGui::SliderFloat("HORIZONTAL AIR FRICTION", &::svc::playerLocator.get().stats.PLAYER_HORIZ_AIR_FRIC, 0.900f, 1.000f);
					ImGui::SliderFloat("VERTICAL AIR FRICTION", &::svc::playerLocator.get().stats.PLAYER_VERT_AIR_FRIC, 0.900f, 1.000f);
					ImGui::NewLine();

					ImGui::SliderFloat("GROUND SPEED", &::svc::playerLocator.get().stats.X_ACC, 0.0f, 0.2f);
					ImGui::SliderFloat("AIR SPEED", &::svc::playerLocator.get().stats.X_ACC_AIR, 0.0f, 0.2f);
					ImGui::SliderFloat("JUMP HEIGHT", &::svc::playerLocator.get().stats.JUMP_MAX, 0.15f, 0.5f);
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("General")) {
					if (ImGui::Button("Toggle Greyblock Mode")) {
						if (::svc::globalBitFlagsLocator.get().test(::svc::global_flags::greyblock_state)) {
							::svc::globalBitFlagsLocator.get().reset(::svc::global_flags::greyblock_state);
							::svc::globalBitFlagsLocator.get().set(::svc::global_flags::greyblock_trigger);
						} else {
							::svc::globalBitFlagsLocator.get().set(::svc::global_flags::greyblock_state);
							::svc::globalBitFlagsLocator.get().set(::svc::global_flags::greyblock_trigger);
						}

					}
					ImGui::Text("Greyblock Mode : %s", ::svc::globalBitFlagsLocator.get().test(::svc::global_flags::greyblock_state) ? "On" : "Off");
					ImGui::Separator();

					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
			}
		};



	}

}
