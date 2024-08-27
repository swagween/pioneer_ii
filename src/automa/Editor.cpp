//
//  Editor.cpp
//  automa
//
//  Created by Alex Frasca on 12/26/22.
//

#include <algorithm>
#include <filesystem>
#include "../../demo/src/setup/Game.hpp"
#include "../util/Lookup.hpp"
#include "GameState.hpp"

namespace pi {

namespace automa {

void Editor::init(std::string const& load_path) {
	folderpath = load_path + "../../../level/";
	filepath = load_path;
	map.load(load_path);

	room = {};
	demopath = {};
	std::filesystem::path room_dir = load_path;
	room = room_dir.filename().string();
	demopath = "../../../demo/resources/level/" + room;
	std::string demo_resources = "../../../demo/resources/image";
	std::cout << "\ndemopath: " << demopath;
	room_id = map.room_id;

	tool_texture.loadFromFile(load_path + "../../../gui/tools.png");
	large_animator_textures.loadFromFile(demo_resources + "/animators/large_animators_01.png");
	large_animator_thumbs.loadFromFile(load_path + "../../../animators/large_animator_thumbs.png");
	small_animator_textures.loadFromFile(load_path + "../../../animators/small_animators_01.png");
	small_animator_thumbs.loadFromFile(demo_resources + "/animators/small_animator_thumbs.png");
	enemy_thumbnails.loadFromFile(load_path + "../../../enemies/thumbnails.png");
	t_chest.loadFromFile(load_path + "../../../entity/chest.png");
	t_npc.loadFromFile(load_path + "../../../entity/npc.png");

	map.map_states.back().layers.at(canvas::MIDDLEGROUND).active = true;

	sprites.current_enemy.setTexture(enemy_thumbnails);
	sprites.enemy_thumb.setTexture(enemy_thumbnails);
	sprites.tool.setTexture(tool_texture);
	sprites.large_animator.setTexture(large_animator_textures);
	sprites.large_animator_thumb.setTexture(large_animator_thumbs);
	sprites.small_animator.setTexture(small_animator_textures);
	sprites.small_animator_thumb.setTexture(small_animator_thumbs);

	for (int i = 0; i < canvas::NUM_STYLES; ++i) {
		char const* next = lookup::get_style_string.at(static_cast<canvas::STYLE>(i));
		styles[i] = next;
	}
	for (int i = 0; i < canvas::NUM_BGS; ++i) {
		char const* next = lookup::get_backdrop_string.at(static_cast<canvas::BACKDROP>(i));
		bgs[i] = next;
	}
	for (int i = 0; i < canvas::NUM_STYLES; ++i) {
		tileset_textures.push_back(sf::Texture());
		std::string style = lookup::get_style_string.at(static_cast<canvas::STYLE>(i));
		tileset_textures.back().loadFromFile(load_path + "../../../tile/" + style + "_tiles.png");
	}

	for (int i = 0; i < 16; ++i) {
		for (int j = 0; j < 16; ++j) {
			// do all tilesets in this loop
			sprites.tileset.setTexture(tileset_textures.at(static_cast<int>(map.style)));
		}
	}
	setTilesetTexture(tileset_textures.at(static_cast<int>(map.style)));
	box.setOutlineColor(sf::Color{200, 200, 200, 20});
	box.setOutlineThickness(-2);
	box.setSize({canvas::CELL_SIZE, canvas::CELL_SIZE});

	player_box.setFillColor(sf::Color{100, 200, 100, 10});
	player_box.setOutlineColor(sf::Color{100, 200, 100, 70});
	player_box.setOutlineThickness(-2);
	player_box.setSize({canvas::CELL_SIZE, canvas::CELL_SIZE});

	gridbox.setFillColor(sf::Color::Transparent);
	gridbox.setOutlineColor(sf::Color{240, 230, 255, 20});
	gridbox.setOutlineThickness(-1);
	gridbox.setSize({canvas::CELL_SIZE, canvas::CELL_SIZE});

	portalbox.setFillColor(sf::Color{120, 220, 200, 128});
	portalbox.setOutlineColor(sf::Color{240, 230, 255, 180});
	portalbox.setOutlineThickness(-1);
	portalbox.setSize({canvas::CELL_SIZE, canvas::CELL_SIZE});

	chestbox.setFillColor(sf::Color{220, 220, 80, 128});
	chestbox.setOutlineColor(sf::Color{40, 30, 255, 180});
	chestbox.setOutlineThickness(-3);
	chestbox.setSize({canvas::CELL_SIZE, canvas::CELL_SIZE});

	savebox.setFillColor(sf::Color{220, 20, 220, 128});
	savebox.setOutlineColor(sf::Color{240, 230, 255, 180});
	savebox.setOutlineThickness(-1);
	savebox.setSize({canvas::CELL_SIZE, canvas::CELL_SIZE});

	inspbox.setFillColor(sf::Color{220, 120, 100, 128});
	inspbox.setOutlineColor(sf::Color{240, 230, 255, 180});
	inspbox.setOutlineThickness(-1);
	inspbox.setSize({canvas::CELL_SIZE, canvas::CELL_SIZE});

	
	platextent.setFillColor(sf::Color::Transparent);
	platextent.setOutlineColor(sf::Color{240, 230, 55, 80});
	platextent.setOutlineThickness(-2);
	platbox.setFillColor(sf::Color{220, 120, 100, 128});
	platbox.setOutlineColor(sf::Color{240, 230, 255, 180});
	platbox.setOutlineThickness(-4);

	target.setFillColor(sf::Color{110, 90, 200, 80});
	target.setOutlineColor(sf::Color{240, 230, 255, 100});
	target.setOutlineThickness(-2);
	target.setSize({canvas::CELL_SIZE, canvas::CELL_SIZE});

	backdrop.setFillColor(sf::Color{60, 40, 60, 40});
	backdrop.setOutlineColor(sf::Color{240, 230, 255, 40});
	backdrop.setOutlineThickness(4);
	backdrop.setSize(map.real_dimensions);

	setTilesetTexture(tileset_textures.at(static_cast<int>(map.style)));
	std::cout << lookup::get_style_string.at(map.style) << "\n";
}

void Editor::setTilesetTexture(sf::Texture& new_tex) { sprites.tileset.setTexture(new_tex); }

void Editor::handle_events(sf::Event& event, sf::RenderWindow& win) {

	ImGui::IsAnyItemHovered() || ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) || ImGui::IsAnyItemActive() ? svc::windowHoveredLocator.get() = true : svc::windowHoveredLocator.get() = false;

	svc::current_tool.get()->ready = !svc::windowHoveredLocator.get();
	svc::secondary_tool.get()->ready = !svc::windowHoveredLocator.get();
	if (event.type == sf::Event::EventType::KeyPressed) {
		svc::current_tool.get()->ready = false;
		svc::secondary_tool.get()->ready = false;
		if (event.key.code == sf::Keyboard::C) { svc::current_tool.get()->handle_keyboard_events(map, event.key.code); }
		if (event.key.code == sf::Keyboard::V) {
			svc::current_tool.get()->handle_keyboard_events(map, event.key.code);
			map.save_state();
		}
		if (event.key.code == sf::Keyboard::Q) { svc::current_tool.get()->handle_keyboard_events(map, event.key.code); }
		if (event.key.code == sf::Keyboard::LControl) { svc::current_tool.get()->handle_keyboard_events(map, event.key.code); }
	}
	if (event.type == sf::Event::EventType::KeyReleased) {
		svc::current_tool.get()->ready = false;
		svc::secondary_tool.get()->ready = false;
	}
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		map.save_state();
		svc::current_tool.get()->handle_events(map, event);
		svc::current_tool.get()->active = true;
	} else {
		svc::current_tool.get()->active = false;
		svc::current_tool.get()->just_clicked = true;
		svc::current_tool.get()->clicked_position = {0.0f, 0.0f};
		svc::current_tool.get()->scaled_clicked_position = {0, 0};
		svc::current_tool.get()->relative_position = {0.0f, 0.0f};
	}
	if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
		svc::secondary_tool.get()->handle_events(map, event);
		svc::secondary_tool.get()->active = true;
	} else {
		svc::secondary_tool.get()->active = false;
		svc::secondary_tool.get()->just_clicked = true;
		svc::secondary_tool.get()->clicked_position = {0.0f, 0.0f};
		svc::secondary_tool.get()->scaled_clicked_position = {0, 0};
		svc::secondary_tool.get()->relative_position = {0.0f, 0.0f};
	}
	// select tool gets special treatment, because it can be used without the mouse being pressed (copy/paste)
	if (svc::current_tool.get()->type == tool::TOOL_TYPE::SELECT) { svc::current_tool.get()->handle_events(map, event); }
}

void Editor::logic() {
	if (svc::current_tool->trigger_switch) { svc::current_tool = std::move(std::make_unique<tool::Hand>()); }
	svc::current_tool->update();
	svc::secondary_tool->update();
	if (svc::current_tool.get()->type == tool::TOOL_TYPE::HAND && svc::current_tool.get()->active) {
		svc::cameraLocator.get().physics.position += svc::current_tool.get()->relative_position;
		svc::cameraLocator.get().physics.velocity = {0.0f, 0.0f};
		svc::cameraLocator.get().physics.acceleration = {0.0f, 0.0f};
		svc::current_tool.get()->relative_position = {0.0f, 0.0f};
	} else if (svc::secondary_tool.get()->type == tool::TOOL_TYPE::HAND && svc::secondary_tool.get()->active) {
		svc::cameraLocator.get().physics.position += svc::secondary_tool.get()->relative_position;
		svc::cameraLocator.get().physics.velocity = {0.0f, 0.0f};
		svc::cameraLocator.get().physics.acceleration = {0.0f, 0.0f};
		svc::secondary_tool.get()->relative_position = {0.0f, 0.0f};
	}
	svc::cameraLocator.get().update();
}

void Metagrid::gui_render(sf::RenderWindow& win) {
	bool* debug{};
	float const PAD = 10.0f;
	static int corner = 0;
	ImGuiIO& io = ImGui::GetIO();
	io.FontGlobalScale = 1.0;

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

	ImGuiViewport const* viewport = ImGui::GetMainViewport();
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
	if (ImGui::Begin("Debug Mode", debug, window_flags)) {
		ImGui::Text("Pioneer (beta version 1.0.0) - Metagrid");
		ImGui::Separator();
		if (ImGui::IsMousePosValid()) {
			ImGui::Text("Mouse Position: (%.1f,%.1f)", io.MousePos.x, io.MousePos.y);
		} else {
			ImGui::Text("Mouse Position: <invalid>");
		}
		prev_window_size = ImGui::GetWindowSize();
		ImGui::End();
	}
	ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
	work_size = viewport->WorkSize;
	window_pos = {window_pos.x, window_pos.y + prev_window_size.y + PAD};
	window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
	ImGui::SetNextWindowPos(window_pos);
	if (ImGui::Begin("Info", debug, window_flags)) {
		prev_window_size = ImGui::GetWindowSize();
		prev_window_pos = ImGui::GetWindowPos();

		ImGui::End();
	}
}

void Editor::render(sf::RenderWindow& win) {
	backdrop.setPosition(svc::cameraLocator.get().physics.position.x, svc::cameraLocator.get().physics.position.y);
	backdrop.setSize(map.real_dimensions);
	win.draw(backdrop);
	if (!map.map_states.empty()) {
		for (auto& layer : map.map_states.back().layers) {
			box.setFillColor(sf::Color{static_cast<uint8_t>(layer.render_order * 30), 230, static_cast<uint8_t>(255 - layer.render_order * 30), 40});
			for (auto& cell : layer.grid.cells) {
				if (cell.value == 0) { continue; }
				if (layer.render_order == svc::active_layer || show_all_layers) {
					sprites.tileset.setTextureRect(sf::IntRect{get_tile_coord(cell.value), {32, 32}});
					sprites.tileset.setPosition(cell.position.x + svc::cameraLocator.get().physics.position.x, cell.position.y + svc::cameraLocator.get().physics.position.y);
					win.draw(sprites.tileset);
				} else {
					box.setPosition(cell.position.x + svc::cameraLocator.get().physics.position.x, cell.position.y + svc::cameraLocator.get().physics.position.y);
					win.draw(box);
				}
			}
		}
	}
	for (auto& portal : map.portals) {
		for (int i = 0; i < portal.dimensions.x; ++i) {
			for (int j = 0; j < portal.dimensions.y; ++j) {
				portalbox.setPosition((portal.position.x + i) * canvas::CELL_SIZE + svc::cameraLocator.get().physics.position.x, (portal.position.y + j) * canvas::CELL_SIZE + svc::cameraLocator.get().physics.position.y);
				win.draw(portalbox);
			}
		}
	}

	for (auto& inspectable : map.inspectables) {
		inspbox.setPosition((inspectable.position.x) * canvas::CELL_SIZE + svc::cameraLocator.get().physics.position.x, (inspectable.position.y) * canvas::CELL_SIZE + svc::cameraLocator.get().physics.position.y);
		win.draw(inspbox);
	}

	for (auto& block : map.switch_blocks) {
		inspbox.setPosition((block.position.x) * canvas::CELL_SIZE + svc::cameraLocator.get().physics.position.x, (block.position.y) * canvas::CELL_SIZE + svc::cameraLocator.get().physics.position.y);
		win.draw(inspbox);
	}
	for (auto& button : map.switch_buttons) {
		inspbox.setPosition((button.position.x) * canvas::CELL_SIZE + svc::cameraLocator.get().physics.position.x, (button.position.y) * canvas::CELL_SIZE + svc::cameraLocator.get().physics.position.y);
		win.draw(inspbox);
	}

	for (auto& chest : map.chests) {
		chestbox.setPosition((chest.position.x) * canvas::CELL_SIZE + svc::cameraLocator.get().physics.position.x, (chest.position.y) * canvas::CELL_SIZE + svc::cameraLocator.get().physics.position.y);
		win.draw(chestbox);
	}

	if (map.save_point.placed) {
		savebox.setPosition((map.save_point.position.x) * canvas::CELL_SIZE + svc::cameraLocator.get().physics.position.x, (map.save_point.position.y) * canvas::CELL_SIZE + svc::cameraLocator.get().physics.position.y);
		win.draw(savebox);
	}
	

	for (auto& platform : map.platforms) {
		auto f_extent = platform.extent * 32.f;
		platextent.setSize({f_extent, f_extent});
		platextent.setPosition((platform.position.x) * canvas::CELL_SIZE + svc::cameraLocator.get().physics.position.x + platform.dimensions.x * 16.f,
							   (platform.position.y) * canvas::CELL_SIZE + svc::cameraLocator.get().physics.position.y + platform.dimensions.y * 16.f);
		platbox.setSize({platform.dimensions.x * 32.f, platform.dimensions.y * 32.f});
		platbox.setPosition((platform.position.x) * canvas::CELL_SIZE + svc::cameraLocator.get().physics.position.x, (platform.position.y) * canvas::CELL_SIZE + svc::cameraLocator.get().physics.position.y);
		win.draw(platextent);
		win.draw(platbox);
	}

	for (auto& critter : map.critters) {
		int idx = critter.id;
		sprites.current_enemy.setTextureRect(sf::IntRect({(idx * 16) % (64), (idx / 4) * 16}, {16, 16}));
		sprites.current_enemy.setScale({2.0f, 2.0f});
		sprites.current_enemy.setPosition((critter.position.x) * canvas::CELL_SIZE + svc::cameraLocator.get().physics.position.x, (critter.position.y) * canvas::CELL_SIZE + svc::cameraLocator.get().physics.position.y);
		win.draw(sprites.current_enemy);
	}

	for (auto& animator : map.animators) {
		sf::Vector2<float> anim_pos = {animator.position.x * canvas::CELL_SIZE + svc::cameraLocator.get().physics.position.x, animator.position.y * canvas::CELL_SIZE + svc::cameraLocator.get().physics.position.y};
		box.setPosition(anim_pos.x, anim_pos.y);
		if (show_all_layers) {
			auto which = static_cast<int>(animator.id / 100);
			auto lookup = static_cast<int>(animator.id % 100);
			if (which == 1) {
				win.draw(box);
				sprites.large_animator.setTextureRect(sf::IntRect{{lookup * 64, 0}, {64, 64}});
				sprites.large_animator.setPosition(anim_pos.x, anim_pos.y);
				sprites.large_animator.setScale({1.0f, 1.0f});
				win.draw(sprites.large_animator);
			} else {
				win.draw(box);
				sprites.small_animator.setTextureRect(sf::IntRect{{lookup * 32, 0}, {32, 32}});
				sprites.small_animator.setPosition(anim_pos.x, anim_pos.y);
				win.draw(sprites.small_animator);
			}
		} else if ((svc::active_layer < 4 && animator.foreground) || (svc::active_layer >= 4 && !animator.foreground)) {
			win.draw(box);
		}
	}

	// player start
	player_box.setPosition(static_cast<sf::Vector2<float>>(pi::svc::playerStartLocator.get()) * (float)CELL_SIZE + svc::cameraLocator.get().physics.position);
	win.draw(player_box);

	if (show_grid && !map.map_states.empty()) {
		if (map.map_states.back().layers.empty()) { return; }
		for (auto& cell : map.map_states.back().layers.back().grid.cells) {
			if (cell.position.x + svc::cameraLocator.get().bounding_box.left < 0) { continue; }
			if (cell.position.x + svc::cameraLocator.get().bounding_box.left > screen_dimensions.x) { continue; }
			if (cell.position.y + svc::cameraLocator.get().bounding_box.top < 0) { continue; }
			if (cell.position.y + svc::cameraLocator.get().bounding_box.top > screen_dimensions.y) { continue; }
			gridbox.setPosition(cell.position.x + svc::cameraLocator.get().physics.position.x, cell.position.y + svc::cameraLocator.get().physics.position.y);
			win.draw(gridbox);
		}
	}

	if (svc::current_tool.get()->ready && svc::current_tool.get()->in_bounds(map.dimensions) &&
		(svc::current_tool.get()->type == tool::TOOL_TYPE::BRUSH || svc::current_tool.get()->type == tool::TOOL_TYPE::FILL || svc::current_tool.get()->type == tool::TOOL_TYPE::ERASE ||
		 svc::current_tool.get()->type == tool::TOOL_TYPE::ENTITY_PLACER)) {
		for (int i = 0; i < svc::current_tool.get()->size; i++) {
			for (int j = 0; j < svc::current_tool.get()->size; j++) {
				target.setPosition((svc::current_tool.get()->scaled_position.x - i) * canvas::CELL_SIZE + svc::cameraLocator.get().physics.position.x,
								   (svc::current_tool.get()->scaled_position.y - j) * canvas::CELL_SIZE + svc::cameraLocator.get().physics.position.y);
				win.draw(target);
			}
		}
	}

	svc::current_tool.get()->render(win, svc::cameraLocator.get().physics.position);
	float tool_x = svc::current_tool.get()->position.x + svc::cameraLocator.get().physics.position.x;
	float tool_y = svc::current_tool.get()->position.y + svc::cameraLocator.get().physics.position.y;
	if (!svc::windowHoveredLocator.get()) {
		sprites.tool.setTextureRect({{(int)svc::current_tool.get()->type * 32, 0}, {32, 32}});
		sprites.tool.setPosition(tool_x, tool_y);
		win.draw(sprites.tool);
	}
}

void Editor::gui_render(sf::RenderWindow& win) {
	bool* debug{};
	float const PAD = 10.0f;
	static int corner = 0;
	ImGuiIO& io = ImGui::GetIO();
	io.FontGlobalScale = 1.0;

	svc::current_tool.get()->position = sf::Vector2<float>{io.MousePos.x, io.MousePos.y} - svc::cameraLocator.get().physics.position;
	svc::secondary_tool.get()->position = sf::Vector2<float>{io.MousePos.x, io.MousePos.y} - svc::cameraLocator.get().physics.position;

	if (svc::current_tool.get()->type == tool::TOOL_TYPE::SELECT && !svc::windowHoveredLocator.get()) {
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::Text("Press `C` to copy selection.");
		ImGui::Text("Press `V` to paste selection.");
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}

	if (svc::current_tool.get()->type == tool::TOOL_TYPE::ENTITY_PLACER && !svc::windowHoveredLocator.get()) {
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		if (svc::current_tool.get()->ent_type == tool::ENTITY_TYPE::PORTAL) {
			ImGui::Text("Click to place portal to room %i%s", svc::current_tool.get()->current_portal.destination_map_id, ".");
			ImGui::Text("Portal size: width: %i, height: %i", svc::current_tool.get()->current_portal.dimensions.x, svc::current_tool.get()->current_portal.dimensions.y);
			ImGui::Text("Activate on contact: %s", svc::current_tool.get()->current_portal.activate_on_contact ? "Yes" : "No");
			ImGui::TextUnformatted("Press 'Q' to cancel.");
		} else if (svc::current_tool.get()->ent_type == tool::ENTITY_TYPE::INSPECTABLE) {
			if (!svc::current_tool.get()->current_inspectable.suites.empty()) {
				if (!svc::current_tool.get()->current_inspectable.suites.back().empty()) { ImGui::Text("Place an inspectable with message: %s", svc::current_tool.get()->current_inspectable.suites.back().back().c_str()); }
			}
			ImGui::Text("and with key: %s", svc::current_tool.get()->current_inspectable.key.c_str());
			ImGui::Text("Activate on contact: %s", svc::current_tool.get()->current_inspectable.activate_on_contact ? "Yes" : "No");
			ImGui::TextUnformatted("Press 'Q' to cancel.");
		} else if (svc::current_tool.get()->ent_type == tool::ENTITY_TYPE::CRITTER) {
			ImGui::Text(lookup::get_critter_string.at(svc::current_tool.get()->current_critter.type));
			ImGui::Text("id: %i", svc::current_tool.get()->current_critter.id);
		} else if (svc::current_tool.get()->ent_type == tool::ENTITY_TYPE::ENTITY_EDITOR) {
			ImGui::Text("Erase Entity");
		} else {
			ImGui::Text("Place Entity");
		}
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}

	// Main Menu
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::Button("New")) { ImGui::OpenPopup("New File"); }
			// Always center this window when appearing
			ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
			ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

			if (ImGui::BeginPopupModal("New File", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
				ImGui::Text("Please enter a new room name.");
				ImGui::Text("Convention is all caps, snake-case, and of the format `STYLE_NAME_INDEX`.");
				ImGui::Separator();
				ImGui::NewLine();
				static char buffer[128] = "";

				ImGui::InputTextWithHint("Folder Name", "DOJO_CORRIDOR_01", buffer, IM_ARRAYSIZE(buffer));
				ImGui::Separator();
				ImGui::NewLine();

				ImGui::Text("Please specify the dimensions of the level in chunks (16x16 tiles). Please note, these dimensions cannot be changed once the level is created.");
				ImGui::Separator();
				ImGui::NewLine();

				static int width{0};
				static int height{0};
				static int room_id{0};

				ImGui::InputInt("Width", &width);
				ImGui::Separator();
				ImGui::NewLine();

				ImGui::InputInt("Height", &height);
				ImGui::Separator();
				ImGui::NewLine();

				ImGui::InputInt("Room ID", &room_id);
				ImGui::Separator();
				ImGui::NewLine();

				if (ImGui::Button("Close")) ImGui::CloseCurrentPopup();
				ImGui::SameLine();
				if (ImGui::Button("Create")) {

					// set new bg to current one to make it easier to design multiple rooms
					static int style_current = static_cast<int>(map.style);
					static int bg_current = static_cast<int>(map.bg);

					map = canvas::Canvas({static_cast<uint32_t>(width * canvas::CHUNK_SIZE), static_cast<uint32_t>(height * canvas::CHUNK_SIZE)});
					map.style = static_cast<canvas::STYLE>(style_current);
					setTilesetTexture(tileset_textures.at(style_current));
					map.bg = static_cast<canvas::BACKDROP>(bg_current);
					filepath = folderpath + buffer;
					map.save(filepath);
					map.save(demopath);
					map.load(filepath);
					map.room_id = room_id;
					std::filesystem::path room_dir = filepath;
					room = room_dir.filename().string();
					room_id = map.room_id;
					demopath = "../../../demo/resources/level/" + room;
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}

			if (ImGui::Button("Open")) {

				char filename[MAX_PATH];
				OPENFILENAME ofn;
				ZeroMemory(&filename, sizeof(filename));
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = NULL; // If you have a window to center over, put its HANDLE here
				ofn.lpstrFilter = "Json Files\0*.json\0Any File\0*.*\0Folders\0\0";
				ofn.lpstrFile = filename;
				ofn.nMaxFile = MAX_PATH;
				ofn.lpstrTitle = "Select a [meta.json] to load.";
				ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;

				if (GetOpenFileNameA(&ofn)) {
					std::string loadpath = filename;
					std::string mapdata = "/meta.json";
					std::string loaddir = loadpath.substr(0, loadpath.size() - mapdata.size());
					map.load(loaddir);
					filepath = loaddir;
					setTilesetTexture(tileset_textures.at(static_cast<int>(map.style)));
					std::filesystem::path room_dir = loaddir;
					room = room_dir.filename().string();
					room_id = map.room_id;
					demopath = "../../../demo/resources/level/" + room;
				} else {
					switch (CommDlgExtendedError()) {
					case CDERR_DIALOGFAILURE: std::cout << "CDERR_DIALOGFAILURE\n"; break;
					case CDERR_FINDRESFAILURE: std::cout << "CDERR_FINDRESFAILURE\n"; break;
					case CDERR_INITIALIZATION: std::cout << "CDERR_INITIALIZATION\n"; break;
					case CDERR_LOADRESFAILURE: std::cout << "CDERR_LOADRESFAILURE\n"; break;
					case CDERR_LOADSTRFAILURE: std::cout << "CDERR_LOADSTRFAILURE\n"; break;
					case CDERR_LOCKRESFAILURE: std::cout << "CDERR_LOCKRESFAILURE\n"; break;
					case CDERR_MEMALLOCFAILURE: std::cout << "CDERR_MEMALLOCFAILURE\n"; break;
					case CDERR_MEMLOCKFAILURE: std::cout << "CDERR_MEMLOCKFAILURE\n"; break;
					case CDERR_NOHINSTANCE: std::cout << "CDERR_NOHINSTANCE\n"; break;
					case CDERR_NOHOOK: std::cout << "CDERR_NOHOOK\n"; break;
					case CDERR_NOTEMPLATE: std::cout << "CDERR_NOTEMPLATE\n"; break;
					case CDERR_STRUCTSIZE: std::cout << "CDERR_STRUCTSIZE\n"; break;
					case FNERR_BUFFERTOOSMALL: std::cout << "FNERR_BUFFERTOOSMALL\n"; break;
					case FNERR_INVALIDFILENAME: std::cout << "FNERR_INVALIDFILENAME\n"; break;
					case FNERR_SUBCLASSFAILURE: std::cout << "FNERR_SUBCLASSFAILURE\n"; break;
					default: std::cout << "You cancelled.\n";
					}
				}
			}

			if (ImGui::Button("Save")) {
				map.save(filepath);
				map.save(demopath);
				ImGui::OpenPopup("Notice");
			}
			if (ImGui::Button("Save As")) { ImGui::OpenPopup("Save As"); }
			if (ImGui::BeginPopupModal("Save As", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
				ImGui::Text("Please enter a unique room name.");
				ImGui::Text("Convention is all caps, snake-case, and of the format `STYLE_NAME_INDEX`.");
				ImGui::Separator();
				ImGui::NewLine();
				static char buffer[128] = "";

				ImGui::InputTextWithHint("Folder Name", "DOJO_CORRIDOR_01", buffer, IM_ARRAYSIZE(buffer));
				ImGui::Separator();
				ImGui::NewLine();

				if (ImGui::Button("Close")) ImGui::CloseCurrentPopup();
				ImGui::SameLine();
				if (ImGui::Button("Create")) {
					std::string savepath = folderpath + buffer;
					map.save(savepath);
					map.save(demopath);
					//                    lookup::room_name_lookup.insert( {map.room_id, buffer} );
					ImGui::CloseCurrentPopup();
				}

				ImGui::TextUnformatted(buffer);

				ImGui::EndPopup();
			}
			if (ImGui::BeginPopupModal("Notice", NULL, ImGuiWindowFlags_Modal)) {
				ImGui::Text("File saved successfully.");
				if (ImGui::Button("Close")) ImGui::CloseCurrentPopup();
				ImGui::EndPopup();
			}
			if (ImGui::BeginPopupModal("Notice: Error", NULL, ImGuiWindowFlags_Modal)) {
				ImGui::Text("ERROR: File failed to save.");
				if (ImGui::Button("Close")) ImGui::CloseCurrentPopup();
				ImGui::EndPopup();
			}
			ImGui::EndMenu();
		}
		// none of the below have been implemented and will likely be removed
		if (ImGui::BeginMenu("Edit")) {
			if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
			ImGui::Separator();
			if (ImGui::MenuItem("Cut", "CTRL+X")) {}
			if (ImGui::MenuItem("Copy", "CTRL+C")) {}
			if (ImGui::MenuItem("Paste", "CTRL+V")) {}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

	ImGuiViewport const* viewport = ImGui::GetMainViewport();
	ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar
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
	if (show_overlay) {
		if (ImGui::Begin("Debug Mode", debug, window_flags)) {
			ImGui::Text("Pioneer (beta version 1.0.0) - Level Editor");
			ImGui::Separator();
			if (ImGui::IsMousePosValid()) {
				ImGui::Text("Mouse Position: (%.1f,%.1f)", io.MousePos.x, io.MousePos.y);
			} else {
				ImGui::Text("Mouse Position: <invalid>");
			}
			ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
			if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags)) {
				if (ImGui::BeginTabItem("General")) {
					ImGui::Text("Any Window Hovered: ");
					ImGui::SameLine();
					if (svc::windowHoveredLocator.get()) {
						ImGui::Text("Yes");
					} else {
						ImGui::Text("No");
					};

					/*constexpr auto text = R"({ "foo": "party", "universe": 42 })";
					auto const json = dj::Json::parse(text);
					ImGui::Text("djson test: %i", json[1]);*/
					ImGui::Text("Room ID: %u", map.room_id);
					//                    ImGui::Text("Room Name: %s", lookup::room_name_lookup.at(map.room_id).c_str());
					ImGui::Text("Camera Position: (%.1f,%.1f)", svc::cameraLocator.get().physics.position.x, svc::cameraLocator.get().physics.position.y);
					ImGui::Text("Active Layer: %i", svc::active_layer);
					ImGui::Text("Num Layers: %lu", map.map_states.back().layers.size());
					ImGui::Text("Stored Tile Value: %u", svc::current_tool.get()->tile);
					if (svc::current_tool.get()->in_bounds(map.dimensions)) {
						ImGui::Text("Tile Value at Mouse Pos: %u", map.tile_val_at(svc::current_tool.get()->scaled_position.x, svc::current_tool.get()->scaled_position.y, svc::active_layer));
					} else {
						ImGui::Text("Tile Value at Mouse Pos: <invalid>");
					}
					ImGui::Separator();
					ImGui::Text("Current State: %s", lookup::get_state_string.at(state).c_str());
					ImGui::Text("Current Style: %s", lookup::get_style_string.at(map.style));
					ImGui::Text("Current Backdrop: %s", lookup::get_backdrop_string.at(map.bg));
					ImGui::Text("Number of Portals: %i", map.portals.size());
					ImGui::Text("Number of Inspectables: %i", map.inspectables.size());
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Tool")) {
					ImGui::Text("Tool Position: (%.1f,%.1f)", svc::current_tool.get()->position.x, svc::current_tool.get()->position.y);
					ImGui::Text("Clicked Position: (%.1f,%.1f)", svc::current_tool.get()->clicked_position.x, svc::current_tool.get()->clicked_position.y);
					ImGui::Text("Relative Position: (%.1f,%.1f)", svc::current_tool.get()->relative_position.x, svc::current_tool.get()->relative_position.y);
					ImGui::Text("Tool Position (scaled): (%i,%i)", svc::current_tool.get()->scaled_position.x, svc::current_tool.get()->scaled_position.y);
					ImGui::Text("Tool Clicked Position (scaled): (%i,%i)", svc::current_tool.get()->scaled_clicked_position.x, svc::current_tool.get()->scaled_clicked_position.y);
					ImGui::Text("Tool in Bounds: ");
					ImGui::SameLine();
					if (svc::current_tool.get()->in_bounds(map.dimensions)) {
						ImGui::Text("Yes");
					} else {
						ImGui::Text("No");
					};
					ImGui::Text("Tool Ready: ");
					ImGui::SameLine();
					if (svc::current_tool.get()->ready) {
						ImGui::Text("Yes");
					} else {
						ImGui::Text("No");
					};
					ImGui::Text("Tool Active: ");
					ImGui::SameLine();
					if (svc::current_tool.get()->active) {
						ImGui::Text("Yes");
					} else {
						ImGui::Text("No");
					};

					ImGui::Text("Current Tool: %s", lookup::get_tool_string.at(svc::current_tool->type).c_str());
					ImGui::Text("Secondary Tool: %s", lookup::get_tool_string.at(svc::secondary_tool->type).c_str());
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Resources")) {
					ImGui::Text("Size of Canvas (Bytes): %lu", sizeof(map));
					ImGui::Text("Size of Tileset (Bytes): %lu", sizeof(tileset_textures));
					ImGui::Text("Size of Filepath (Bytes): %lu", sizeof(filepath));
					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
			}

			prev_window_size = ImGui::GetWindowSize();
			ImGui::End();
		}
	}
	ImGui::SetNextWindowBgAlpha(0.95f); // Transparent background
	work_size = viewport->WorkSize;
	window_pos = {window_pos.x, window_pos.y + prev_window_size.y + PAD};
	window_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;
	ImGui::SetNextWindowSizeConstraints(ImVec2{work_size.x / 12, work_size.y / 4 - prev_window_size.y - PAD * 4}, ImVec2{work_size.x / 2, work_size.y - prev_window_size.y - PAD * 4});
	ImGui::SetNextWindowPos(window_pos);
	if (ImGui::Begin("Tile Palette", debug, window_flags)) {
		prev_window_size = ImGui::GetWindowSize();
		prev_window_pos = ImGui::GetWindowPos();
		int num_cols = 16;
		int num_rows = 16;
		for (int i = 0; i < num_rows; i++) {
			for (int j = 0; j < num_cols; j++) {
				auto idx = j + i * num_cols;
				auto new_j = j * 32;
				auto new_i = i * 32;
				ImGui::PushID(idx);
				sprites.tileset.setTextureRect(sf::IntRect({{new_j, new_i}, {32, 32}}));
				if (ImGui::ImageButton(sprites.tileset, 2)) {
					svc::current_tool.get()->store_tile(idx);
					svc::selectedBlockLocator.get() = idx;
					if (!svc::current_tool.get()->is_paintable()) { svc::current_tool = std::move(std::make_unique<tool::Brush>()); }
				}
				ImGui::PopID();
				ImGui::SameLine();
			}
			ImGui::NewLine();
		}
		ImGui::End();
	}
	ImGui::SetNextWindowBgAlpha(0.95f); // Transparent background
	work_size = viewport->WorkSize;
	window_pos = {window_pos.x, window_pos.y + prev_window_size.y + PAD};
	window_flags = ImGuiWindowFlags_NoCollapse;
	ImGui::SetNextWindowPos(window_pos);
	if (ImGui::Begin("Enemies", debug, window_flags)) {
		prev_window_size = ImGui::GetWindowSize();
		prev_window_pos = ImGui::GetWindowPos();
		int num_cols = 4;
		int num_rows = 4;
		for (int i = 0; i < num_rows; i++) {
			for (int j = 0; j < num_cols; j++) {
				auto idx = j + i * num_cols;
				ImGui::PushID(idx);
				sprites.enemy_thumb.setTextureRect(sf::IntRect({{j * 16, i * 16}, {16, 16}}));
				sprites.enemy_thumb.setScale({2.0f, 2.0f});
				if (ImGui::ImageButton(sprites.enemy_thumb, 2)) {
					svc::current_tool = std::move(std::make_unique<tool::EntityPlacer>());
					svc::current_tool.get()->ent_type = tool::ENTITY_TYPE::CRITTER;
					svc::current_tool.get()->current_critter = canvas::Critter(static_cast<canvas::CRITTER_TYPE>(idx));
					svc::current_tool.get()->current_critter.id = idx;
				}
				ImGui::PopID();
				ImGui::SameLine();
			}
			ImGui::NewLine();
		}
		ImGui::End();
	}
	ImGui::SetNextWindowBgAlpha(0.95f); // Transparent background
	work_size = viewport->WorkSize;
	window_pos = {window_pos.x, window_pos.y + prev_window_size.y + PAD};
	window_flags = ImGuiWindowFlags_NoCollapse;
	ImGui::SetNextWindowSizeConstraints(ImVec2{work_size.x / 12, work_size.y / 4 - prev_window_size.y - PAD * 4}, ImVec2{work_size.x / 2, work_size.y - prev_window_size.y - PAD * 4});
	ImGui::SetNextWindowPos(window_pos);
	if (ImGui::Begin("Entities", debug, window_flags)) {
		prev_window_size = ImGui::GetWindowSize();
		prev_window_pos = ImGui::GetWindowPos();

		if (ImGui::Button("Animator")) { ImGui::OpenPopup("Select Animator"); }
		if (ImGui::BeginPopupModal("Select Animator", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::Text("2x2:");
			int num_cols = 8;
			int num_rows = 2;
			for (int i = 0; i < num_rows; i++) {
				for (int j = 0; j < num_cols; j++) {
					ImGui::PushID(j + i * num_cols);
					auto idx = j + i * num_cols;
					sprites.large_animator.setTextureRect(sf::IntRect({{idx * 64, 0}, {64, 64}}));
					sprites.large_animator.setScale({0.5f, 0.5f});
					if (ImGui::ImageButton(sprites.large_animator, 2)) {
						svc::current_tool = std::move(std::make_unique<tool::EntityPlacer>());
						svc::current_tool.get()->ent_type = tool::ENTITY_TYPE::ANIMATOR;
						svc::current_tool.get()->current_animator = canvas::Animator(sf::Vector2<uint32_t>{(uint32_t)2, (uint32_t)2}, idx + large_index_multiplier, false, svc::active_layer >= 4); // change booleans here later
						ImGui::CloseCurrentPopup();
					}
					ImGui::PopID();
					ImGui::SameLine();
				}
				ImGui::NewLine();
			}
			ImGui::Text("1x1:");
			num_cols = 16;
			num_rows = 2;
			for (int i = 0; i < num_rows; i++) {
				for (int j = 0; j < num_cols; j++) {
					ImGui::PushID(j + i * num_cols);
					auto idx = j + i * num_cols;
					sprites.small_animator.setTextureRect(sf::IntRect({{idx * 32, 0}, {32, 32}}));
					if (ImGui::ImageButton(sprites.small_animator, 2)) {
						svc::current_tool = std::move(std::make_unique<tool::EntityPlacer>());
						svc::current_tool.get()->ent_type = tool::ENTITY_TYPE::ANIMATOR;
						svc::current_tool.get()->current_animator = canvas::Animator(sf::Vector2<uint32_t>{(uint32_t)1, (uint32_t)1}, idx + small_index_multiplier, false, svc::active_layer >= 4); // change booleans here later
						ImGui::CloseCurrentPopup();
					}
					ImGui::PopID();
					ImGui::SameLine();
				}
				ImGui::NewLine();
			}
			if (ImGui::Button("Close")) { ImGui::CloseCurrentPopup(); }
			ImGui::EndPopup();
		}
	
		if (ImGui::Button("Bed")) { 
				svc::current_tool = std::move(std::make_unique<tool::EntityPlacer>());
				svc::current_tool.get()->ent_type = tool::ENTITY_TYPE::BED;
		}
		if (ImGui::Button("Automatic Animators")) { ImGui::OpenPopup("Animator Specifications"); }
		if (ImGui::BeginPopupModal("Animator Specifications", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {

			static int id{};
			static int style{};
			static bool foreground{};
			ImGui::InputInt("Texture Lookup", &id);
			ImGui::InputInt("Style", &style);

			if (ImGui::Button("Create")) {
				// switch to entity tool, and store the specified portal for placement
				svc::current_tool = std::move(std::make_unique<tool::EntityPlacer>());
				svc::current_tool.get()->ent_type = tool::ENTITY_TYPE::ANIMATOR;
				svc::current_tool.get()->current_animator = canvas::Animator(sf::Vector2<uint32_t>{(uint32_t)1, (uint32_t)1}, id, true, false, style);
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Close")) { ImGui::CloseCurrentPopup(); }
			ImGui::EndPopup();
		}
		if (ImGui::Button("Portal")) { ImGui::OpenPopup("Portal Dimensions"); }
		if (ImGui::BeginPopupModal("Portal Dimensions", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {

			static int width{0};
			static int height{0};
			static int destination{0};
			static bool activate_on_contact{false};
			static bool locked{false};
			static int key_id{};

			ImGui::InputInt("Width", &width);
			ImGui::Separator();
			ImGui::NewLine();

			ImGui::InputInt("Height", &height);
			ImGui::Separator();
			ImGui::NewLine();

			ImGui::InputInt("Destination Room ID", &destination);
			ImGui::SameLine();
			help_marker("Must be an existing room before being activated in-game. By convention, choose a three-digit number where the first digit indicates the region.");
			ImGui::Separator();
			ImGui::NewLine();

			ImGui::Checkbox("Activate on contact?", &activate_on_contact);
			ImGui::SameLine();
			help_marker("If left unchecked, the player will have to inspect the portal to activate it.");
			ImGui::Separator();
			ImGui::NewLine();

			ImGui::Checkbox("Locked?", &locked);
			ImGui::SameLine();
			ImGui::InputInt("Key ID", &key_id);
			ImGui::Separator();
			ImGui::NewLine();

			if (ImGui::Button("Create")) {
				// switch to entity tool, and store the specified portal for placement
				svc::current_tool = std::move(std::make_unique<tool::EntityPlacer>());
				svc::current_tool.get()->ent_type = tool::ENTITY_TYPE::PORTAL;
				svc::current_tool.get()->current_portal = canvas::Portal(sf::Vector2<uint32_t>{(uint32_t)width, (uint32_t)height}, activate_on_contact, map.room_id, destination, locked, key_id);
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Close")) { ImGui::CloseCurrentPopup(); }
			ImGui::EndPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Inspectable")) { ImGui::OpenPopup("Inspectable Message"); }
		if (ImGui::BeginPopupModal("Inspectable Message", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {

			static bool activate_on_contact{false};
			static char keybuffer[128] = "";
			static char msgbuffer[512] = "";

			ImGui::InputTextWithHint("Key", "Title (invisible in-game; must be unique per room)", keybuffer, IM_ARRAYSIZE(keybuffer));
			ImGui::Separator();
			ImGui::NewLine();

			ImGui::InputTextWithHint("Message", "Type message here...", msgbuffer, IM_ARRAYSIZE(msgbuffer));
			ImGui::Separator();
			ImGui::NewLine();

			ImGui::Checkbox("Activate on contact?", &activate_on_contact);
			ImGui::SameLine();
			help_marker("If left unchecked, the player will have to inspect the portal to activate it.");

			if (ImGui::Button("Create")) {
				// switch to entity tool, and store the specified portal for placement
				svc::current_tool = std::move(std::make_unique<tool::EntityPlacer>());
				svc::current_tool.get()->ent_type = tool::ENTITY_TYPE::INSPECTABLE;
				svc::current_tool.get()->current_inspectable = canvas::Inspectable(sf::Vector2<uint32_t>{1, 1}, activate_on_contact, keybuffer);
				svc::current_tool.get()->current_inspectable.suites.push_back(std::vector<std::string>{msgbuffer}); 
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Close")) { ImGui::CloseCurrentPopup(); }
			ImGui::EndPopup();
		}
		if (ImGui::Button("Platform")) { ImGui::OpenPopup("Platform Specifications"); }
		if (ImGui::BeginPopupModal("Platform Specifications", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {

			static int x{};
			static int y{};
			static int extent{};
			static int style{};
			static char typebuffer[128] = "";
			static float start{};

			ImGui::InputInt("X Dimensions", &x);
			ImGui::InputInt("Y Dimensions", &y);
			ImGui::SameLine();
			ImGui::Separator();
			ImGui::NewLine();

			ImGui::InputInt("Extent", &extent);
			ImGui::SameLine();
			help_marker("In units of 32x32.");
			ImGui::Separator();
			ImGui::NewLine();

			ImGui::InputInt("Style", &style);
			ImGui::SameLine();
			help_marker("Must not exceed number of styles.");
			ImGui::Separator();
			ImGui::NewLine();

			ImGui::InputTextWithHint("Type", "Must exist in platforms.json", typebuffer, IM_ARRAYSIZE(typebuffer));
			ImGui::Separator();
			ImGui::NewLine();

			ImGui::InputFloat("Start", &start);
			ImGui::SameLine();
			help_marker("Must be a value between 0 and 1.");
			ImGui::Separator();
			ImGui::NewLine();

			if (ImGui::Button("Create")) {
				// switch to entity tool, and store the specified portal for placement
				svc::current_tool = std::move(std::make_unique<tool::EntityPlacer>());
				svc::current_tool.get()->ent_type = tool::ENTITY_TYPE::PLATFORM;
				svc::current_tool.get()->current_platform = canvas::Platform(sf::Vector2<uint32_t>{0, 0}, sf::Vector2<uint32_t>{(uint32_t)x, (uint32_t)y}, extent, style, typebuffer, start);
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Close")) { ImGui::CloseCurrentPopup(); }
			ImGui::EndPopup();
		}
		if (ImGui::Button("Chest")) { ImGui::OpenPopup("Chest Specifications"); }
		if (ImGui::BeginPopupModal("Chest Specifications", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {

			static int item_id{};
			static int id{};
			static int type{};
			static int amount{};
			static float rarity{};

			ImGui::SameLine();
			ImGui::Separator();
			ImGui::NewLine();

			ImGui::InputInt("Type", &type);
			ImGui::SameLine();
			help_marker("0 for gun, 1 for item, 2 for chest");
			ImGui::Separator();
			ImGui::NewLine();

			ImGui::InputInt("Amount", &amount);
			ImGui::SameLine();
			help_marker("Number of orbs to drop. Only for type == 2.");
			ImGui::Separator();
			ImGui::NewLine();

			ImGui::InputFloat("Rarity", &rarity);
			ImGui::Separator();
			ImGui::NewLine();

			ImGui::InputInt("Item ID", &item_id);
			ImGui::SameLine();
			help_marker("For guns and items only.");
			ImGui::Separator();
			ImGui::NewLine();

			ImGui::InputInt("ID", &id);
			ImGui::SameLine();
			help_marker("Currently not used, just put a random number.");
			ImGui::Separator();
			ImGui::NewLine();

			if (ImGui::Button("Create")) {
				// switch to entity tool, and store the specified portal for placement
				svc::current_tool = std::move(std::make_unique<tool::EntityPlacer>());
				svc::current_tool.get()->ent_type = tool::ENTITY_TYPE::CHEST;
				svc::current_tool.get()->current_chest = canvas::Chest(id, item_id, type, rarity, amount);
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Close")) { ImGui::CloseCurrentPopup(); }
			ImGui::EndPopup();
		}
		if (ImGui::Button("Switch")) { ImGui::OpenPopup("Switch Specifications"); }
		if (ImGui::BeginPopupModal("Switch Specifications", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {

			static int button_id{};
			static int type{};

			ImGui::SameLine();
			ImGui::Separator();
			ImGui::NewLine();

			ImGui::InputInt("Type", &type);
			ImGui::SameLine();
			help_marker("0 for toggler, 1 for permanent, 2 for movable");
			ImGui::Separator();
			ImGui::NewLine();

			ImGui::InputInt("ID", &button_id);
			ImGui::SameLine();
			help_marker("To match with blocks");
			ImGui::Separator();
			ImGui::NewLine();

			if (ImGui::Button("Create")) {
				// switch to entity tool, and store the specified portal for placement
				svc::current_tool = std::move(std::make_unique<tool::EntityPlacer>());
				svc::current_tool.get()->ent_type = tool::ENTITY_TYPE::SWITCH_BUTTON;
				svc::current_tool.get()->current_switch = canvas::SwitchButton(button_id, type);
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Close")) { ImGui::CloseCurrentPopup(); }
			ImGui::EndPopup();
		}
		if (ImGui::Button("Switch Block")) { ImGui::OpenPopup("Switch Block Specifications"); }
		if (ImGui::BeginPopupModal("Switch Block Specifications", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {

			static int button_id{};
			static int type{};

			ImGui::SameLine();
			ImGui::Separator();
			ImGui::NewLine();

			ImGui::InputInt("Type", &type);
			ImGui::SameLine();
			help_marker("0 for toggler, 1 for permanent, 2 for movable");
			ImGui::Separator();
			ImGui::NewLine();

			ImGui::InputInt("ID", &button_id);
			ImGui::SameLine();
			help_marker("To match with blocks");
			ImGui::Separator();
			ImGui::NewLine();

			if (ImGui::Button("Create")) {
				// switch to entity tool, and store the specified portal for placement
				svc::current_tool = std::move(std::make_unique<tool::EntityPlacer>());
				svc::current_tool.get()->ent_type = tool::ENTITY_TYPE::SWITCH_BLOCK;
				svc::current_tool.get()->current_switch_block = canvas::SwitchBlock(button_id, type);
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Close")) { ImGui::CloseCurrentPopup(); }
			ImGui::EndPopup();
		}
		if (ImGui::Button("Save Point")) {
			svc::current_tool = std::move(std::make_unique<tool::EntityPlacer>());
			svc::current_tool.get()->ent_type = tool::ENTITY_TYPE::SAVE_POINT;
		}
		if (ImGui::Button("Player Placer")) {
			svc::current_tool = std::move(std::make_unique<tool::EntityPlacer>());
			svc::current_tool.get()->ent_type = tool::ENTITY_TYPE::PLAYER_PLACER;
		}

		ImGui::End();
	}

	ImGui::SetNextWindowBgAlpha(0.65f); // Transparent background
	work_pos = viewport->WorkPos;		// Use work area to avoid menu-bar/task-bar, if any!
	work_size = viewport->WorkSize;
	window_pos.x = work_pos.x + work_size.x - PAD;
	window_pos.y = work_pos.y + PAD;
	window_pos_pivot.x = 1.0f;
	window_pos_pivot.y = 0.0f;
	window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
	if (ImGui::Begin("Settings", debug, window_flags)) {
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
		window_flags |= ImGuiWindowFlags_MenuBar;
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
		ImGui::BeginChild("ChildR", ImVec2(320, 72), true, window_flags);
		ImGui::BeginMenuBar();
		if (ImGui::BeginMenu("Tools")) { ImGui::EndMenu(); }

		ImGui::EndMenuBar();
		for (int i = 0; i < NUM_TOOLS; i++) {
			ImGui::PushID(i);
			sprites.tool.setTextureRect(sf::IntRect({{i * 32, 0}, {32, 32}}));
			ImGui::ImageButton(sprites.tool, 2);
			if (ImGui::IsItemHovered()) {
				ImGui::BeginTooltip();
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
				ImGui::Text(lookup::get_tool_string.at((tool::TOOL_TYPE)i).c_str());
				ImGui::PopTextWrapPos();
				ImGui::EndTooltip();
			}
			if (ImGui::IsItemClicked()) {
				switch ((tool::TOOL_TYPE)i) {
				case tool::TOOL_TYPE::BRUSH: svc::current_tool = std::move(std::make_unique<tool::Brush>()); break;
				case tool::TOOL_TYPE::FILL: svc::current_tool = std::move(std::make_unique<tool::Fill>()); break;
				case tool::TOOL_TYPE::SELECT: svc::current_tool = std::move(std::make_unique<tool::SelectionRectangular>()); break;
				case tool::TOOL_TYPE::ERASE: svc::current_tool = std::move(std::make_unique<tool::Erase>()); break;
				case tool::TOOL_TYPE::HAND: svc::current_tool = std::move(std::make_unique<tool::Hand>()); break;
				case tool::TOOL_TYPE::ENTITY_PLACER:
					svc::current_tool = std::move(std::make_unique<tool::EntityPlacer>());
					svc::current_tool.get()->ent_type = tool::ENTITY_TYPE::ENTITY_EDITOR;
					break;
				default: svc::current_tool = std::move(std::make_unique<tool::Hand>()); break;
				}
			}
			ImGui::PopID();
			ImGui::SameLine();
		}

		ImGui::EndChild();
		ImGui::PopStyleVar();

		ImGui::Separator();
		ImGui::Text("Brush Size");
		ImGui::SliderInt("##brushsz", &svc::current_tool.get()->size, 1, 16);

		ImGui::Separator();
		ImGui::Text("Current Block:");
		sprites.tileset.setTextureRect(sf::IntRect({get_tile_coord(svc::selectedBlockLocator.get()), {32, 32}}));
		ImGui::Image(sprites.tileset);

		ImGui::Text("Current Entity:");
		switch (svc::current_tool.get()->ent_type) {
		case tool::ENTITY_TYPE::PORTAL: ImGui::Text("Portal"); break;
		case tool::ENTITY_TYPE::INSPECTABLE: ImGui::Text("Inspectable"); break;
		case tool::ENTITY_TYPE::ANIMATOR: ImGui::Text("Animator"); break;
		case tool::ENTITY_TYPE::CRITTER: ImGui::Text("Critter"); break;
		case tool::ENTITY_TYPE::PLAYER_PLACER: ImGui::Text("Player Placer"); break;

		case tool::ENTITY_TYPE::SAVE_POINT: ImGui::Text("Save Point"); break;
		case tool::ENTITY_TYPE::CHEST: ImGui::Text("Chest"); break;
		case tool::ENTITY_TYPE::ENTITY_EDITOR: ImGui::Text("Entity Editor"); break;
		}

		ImGui::Separator();

		ImGui::Text("Canvas Settings");
		ImGui::Separator();
		ImGui::NewLine();

		ImGui::Checkbox("Show Grid", &show_grid);
		ImGui::Checkbox("Show All Layers", &show_all_layers);

		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
		ImGui::BeginChild("ChildS", ImVec2(320, 172), true, window_flags);
		ImGui::BeginMenuBar();
		if (ImGui::BeginMenu("Current Layer")) { ImGui::EndMenu(); }
		ImGui::EndMenuBar();
		for (int i = 0; i < 8; ++i) {
			if (ImGui::Selectable(lookup::layer_name.at(i), svc::active_layer == i)) { svc::active_layer = i; }
		}
		ImGui::EndChild();
		ImGui::PopStyleVar();

		ImGui::Text("General Settings");
		ImGui::Separator();
		ImGui::NewLine();

		ImGui::Checkbox("Show Overlay", &show_overlay);

		ImGui::NewLine();
		ImGui::Separator();
		ImGui::Text("Actions");
		ImGui::Separator();
		ImGui::NewLine();
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.5, 0.6f, 0.5f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.5, 0.7f, 0.6f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.5, 0.8f, 0.7f));

		ImGui::Text("Demo Room: %s", room.c_str());
		if (ImGui::Button("Launch Demo")) {
			trigger_demo = true;
			map.save(filepath);
			map.save(demopath);
		};
		ImGui::PopStyleColor(3);
		ImGui::NewLine();
		if (ImGui::Button("Export Layer to .png")) {
			screencap.create(win.getSize().x, win.getSize().y);
			export_layer_texture();
		};
		if (ImGui::Button("Undo")) { map.undo(); }
		ImGui::SameLine();
		if (ImGui::Button("Redo")) { map.redo(); }
		ImGui::Text("Map States Size: %i", map.map_states.size());
		ImGui::Text("Redo Queue Size: %i", map.redo_states.size());
		ImGui::NewLine();

		ImGui::Text("Set Scene Style: ");
		int style_current = static_cast<int>(map.style);
		if (ImGui::Combo("##scenestyle", &style_current, styles, IM_ARRAYSIZE(styles))) {
			map.style = static_cast<canvas::STYLE>(style_current);
			setTilesetTexture(tileset_textures.at(style_current));
		}
		ImGui::Text("Set Scene Background: ");
		int bg_current = static_cast<int>(map.bg);
		if (ImGui::Combo("##scenebg", &bg_current, bgs, IM_ARRAYSIZE(bgs))) { map.bg = static_cast<canvas::BACKDROP>(bg_current); }
		if (ImGui::Button("Clear Layer")) {
			map.map_states.push_back(map.map_states.back());
			map.map_states.back().layers.at(svc::active_layer).clear();
		}
		if (ImGui::Button("Clear All Layers")) {
			map.map_states.push_back(map.map_states.back());
			for (auto& layer : map.map_states.back().layers) { layer.clear(); }
		}
		if (ImGui::Button("Clear Entire Canvas")) {
			map.map_states.push_back(map.map_states.back());
			for (auto& layer : map.map_states.back().layers) { layer.clear(); }
			map.portals.clear();
			map.inspectables.clear();
			map.critters.clear();
			map.animators.clear();
			map.platforms.clear();
			map.chests.clear();
			map.switch_blocks.clear();
			map.switch_buttons.clear();
			map.save_point.placed = false;
		}
		prev_window_size = ImGui::GetWindowSize();
		prev_window_pos = ImGui::GetWindowPos();
		ImGui::End();
	}
}

void Editor::help_marker(char const* desc) {
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered()) {
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

void Editor::export_layer_texture() {
	screencap.clear(sf::Color::Transparent);
	screencap.create(map.real_dimensions.x, map.real_dimensions.y);
	for (int i = 0; i <= svc::active_layer; ++i) {
		for (auto& cell : map.map_states.back().layers.at(i).grid.cells) {
			if (cell.value > 0) {
				int x_coord = (cell.value % 16) * TILE_WIDTH;
				int y_coord = std::floor(cell.value / 16) * TILE_WIDTH;
				tile_sprite.setTexture(tileset_textures.at(static_cast<int>(map.style)));
				tile_sprite.setTextureRect(sf::IntRect({x_coord, y_coord}, {32, 32}));
				tile_sprite.setPosition(cell.position);
				screencap.draw(tile_sprite);
			}
		}
	}
	std::time_t const now = std::time(nullptr);
	std::string filedate = std::asctime(std::localtime(&now));
	std::erase_if(filedate, [](auto const& c) { return c == ':' || isspace(c); });
	std::string filename = "screenshot_" + filedate + ".png";
	if (screencap.getTexture().copyToImage().saveToFile(filename)) { std::cout << "screenshot saved to " << filename << std::endl; }
}

sf::Vector2<int> Editor::get_tile_coord(int lookup) {
	sf::Vector2<int> ret{};
	ret.x = lookup % 16;
	ret.y = std::floor(lookup / 16);
	return ret * 32;
}

void GameState::launch_demo(char** argv, int room_id, std::filesystem::path path, sf::Vector2<float> player_position) {
	ImGui::SFML::Shutdown();
	fornani::Game demo{argv};
	std::cout << "Editor path: " << path.string() << "\n";
	demo.run(true, room_id, path, player_position);
}

} // namespace automa

} // namespace pi
