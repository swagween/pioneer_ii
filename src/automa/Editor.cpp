#include <algorithm>
#include "../setup/ResourceFinder.hpp"
#include "../../demo/src/setup/Application.hpp"
#include "../util/Lookup.hpp"
#include "Editor.hpp"
#include "../gui/Console.hpp"
#include <filesystem>

namespace pi {

Editor::Editor(char** argv, WindowManager& window, ResourceFinder& finder) : window(&window), finder(&finder) { args = argv; }

void Editor::run() {

	// load textures
	auto& resource_path = finder->resource_path;
	int const TILE_WIDTH = 32;

	// load the tilesets!
	sf::Texture t_tiles_provisional{};
	t_tiles_provisional.loadFromFile(resource_path + "/tile/provisional_tiles.png");

	init(resource_path + "/level/BREAKABLE_TEST_01");
	setTilesetTexture(t_tiles_provisional);

	bool debug_mode = false;

	sf::RectangleShape background{};
	background.setSize(static_cast<sf::Vector2<float>>(screen_dimensions));
	background.setPosition(0, 0);
	background.setFillColor(sf::Color(40, 60, 80));
	sf::Clock delta_clock{};
	// editor loop
	while (window->get().isOpen()) {

		if (trigger_demo) {
			auto ppos = static_cast<sf::Vector2<float>>(player_start) * 32.f;
			std::cout << "Main path: " << room.data() << "\n";
			launch_demo(args, room_id, room, ppos);
			ImGui::SFML::Init(window->get());
			std::string loaddir = filepath;
			init(loaddir);
		}

		auto event = sf::Event{};

		while (window->get().pollEvent(event)) {
			ImGui::SFML::ProcessEvent(event);
			switch (event.type) {
			case sf::Event::Closed: return;
			case sf::Event::KeyPressed:
				if (event.key.code == sf::Keyboard::D) { debug_mode = !debug_mode; }
				if (event.key.code == sf::Keyboard::R) {}
				if (event.key.code == sf::Keyboard::Num1) { current_tool = std::move(std::make_unique<Hand>()); }
				if (event.key.code == sf::Keyboard::Num2) { current_tool = std::move(std::make_unique<Brush>()); }
				if (event.key.code == sf::Keyboard::Num3) { current_tool = std::move(std::make_unique<Fill>()); }
				if (event.key.code == sf::Keyboard::Num4) { current_tool = std::move(std::make_unique<SelectionRectangular>()); }
				if (event.key.code == sf::Keyboard::Escape) { return; }
				break;
			default: break;
			}
			handle_events(event, window->get());
		}

		// game logic and rendering
		logic();

		// ImGui update
		ImGui::SFML::Update(window->get(), delta_clock.restart());

		// my renders
		window->get().clear();
		window->get().draw(background);

		render(window->get());

		// draw canvas here

		ImGui::SFML::Render(window->get());
		window->get().display();
	}
}

 // end namespace

void Editor::init(std::string const& load_path) {
	folderpath = load_path + "../../../level/";
	filepath = load_path;
	map.load(load_path);
	window->get().setMouseCursorVisible(false);

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
	enemy_thumbnails.loadFromFile(load_path + "../../../enemies/thumbnails.png");

	map.map_states.back().layers.at(MIDDLEGROUND).active = true;

	sprites.current_enemy.setTexture(enemy_thumbnails);
	sprites.enemy_thumb.setTexture(enemy_thumbnails);
	sprites.tool.setTexture(tool_texture);
	sprites.large_animator.setTexture(large_animator_textures);
	sprites.large_animator_thumb.setTexture(large_animator_thumbs);
	sprites.small_animator.setTexture(small_animator_textures);
	sprites.small_animator_thumb.setTexture(small_animator_thumbs);

	for (int i = 0; i < static_cast<int>(Style::END); ++i) {
		char const* next = get_style_string.at(static_cast<Style>(i));
		styles[i] = next;
	}
	for (int i = 0; i < static_cast<int>(Backdrop::END); ++i) {
		char const* next = get_backdrop_string.at(static_cast<Backdrop>(i));
		bgs[i] = next;
	}
	for (int i = 0; i < static_cast<int>(Style::END); ++i) {
		tileset_textures.push_back(sf::Texture());
		std::string style = get_style_string.at(static_cast<Style>(i));
		tileset_textures.back().loadFromFile(load_path + "../../../tile/" + style + "_tiles.png");
	}

	sprites.tileset.setTexture(tileset_textures.at(static_cast<int>(map.style)));
	box.setOutlineColor(sf::Color{200, 200, 200, 20});
	box.setOutlineThickness(-2);
	box.setSize({CELL_SIZE, CELL_SIZE});

	player_box.setFillColor(sf::Color{100, 200, 100, 10});
	player_box.setOutlineColor(sf::Color{100, 200, 100, 70});
	player_box.setOutlineThickness(-2);
	player_box.setSize({CELL_SIZE, CELL_SIZE});

	gridbox.setFillColor(sf::Color::Transparent);
	gridbox.setOutlineColor(sf::Color{240, 230, 255, 20});
	gridbox.setOutlineThickness(-1);
	gridbox.setSize({CELL_SIZE, CELL_SIZE});

	portalbox.setFillColor(sf::Color{120, 220, 200, 128});
	portalbox.setOutlineColor(sf::Color{240, 230, 255, 180});
	portalbox.setOutlineThickness(-1);
	portalbox.setSize({CELL_SIZE, CELL_SIZE});

	chestbox.setFillColor(sf::Color{220, 220, 80, 128});
	chestbox.setOutlineColor(sf::Color{40, 30, 255, 180});
	chestbox.setOutlineThickness(-3);
	chestbox.setSize({CELL_SIZE, CELL_SIZE});

	savebox.setFillColor(sf::Color{220, 20, 220, 128});
	savebox.setOutlineColor(sf::Color{240, 230, 255, 180});
	savebox.setOutlineThickness(-1);
	savebox.setSize({CELL_SIZE, CELL_SIZE});

	inspbox.setFillColor(sf::Color{220, 120, 100, 128});
	inspbox.setOutlineColor(sf::Color{240, 230, 255, 180});
	inspbox.setOutlineThickness(-1);
	inspbox.setSize({CELL_SIZE, CELL_SIZE});

	vinebox.setOutlineColor(sf::Color{240, 230, 80, 80});
	vinebox.setOutlineThickness(-1);

	scenerybox.setOutlineColor(sf::Color{20, 20, 180, 30});
	scenerybox.setOutlineThickness(-1);
	
	platextent.setFillColor(sf::Color::Transparent);
	platextent.setOutlineColor(sf::Color{240, 230, 55, 80});
	platextent.setOutlineThickness(-2);
	platbox.setFillColor(sf::Color{220, 120, 100, 128});
	platbox.setOutlineColor(sf::Color{240, 230, 255, 180});
	platbox.setOutlineThickness(-4);

	target.setFillColor(sf::Color{110, 90, 200, 80});
	target.setOutlineColor(sf::Color{240, 230, 255, 100});
	target.setOutlineThickness(-2);
	target.setSize({CELL_SIZE, CELL_SIZE});

	backdrop.setFillColor(sf::Color{60, 40, 60, 40});
	backdrop.setOutlineColor(sf::Color{240, 230, 255, 40});
	backdrop.setOutlineThickness(4);
	backdrop.setSize(map.real_dimensions);

	setTilesetTexture(tileset_textures.at(static_cast<int>(map.style)));
	std::cout << get_style_string.at(map.style) << "\n";
}

void Editor::setTilesetTexture(sf::Texture& new_tex) { sprites.tileset.setTexture(new_tex); }

void Editor::handle_events(sf::Event& event, sf::RenderWindow& win) {

	current_tool.get()->ready = !window_hovered;
	secondary_tool.get()->ready = !window_hovered;
	if (event.type == sf::Event::EventType::KeyPressed) {
		current_tool.get()->ready = false;
		secondary_tool.get()->ready = false;
		if (control_pressed()) {
			if (event.key.code == sf::Keyboard::X) { current_tool.get()->handle_keyboard_events(map, event.key.code); }
			if (event.key.code == sf::Keyboard::C) { current_tool.get()->handle_keyboard_events(map, event.key.code); }
			if (event.key.code == sf::Keyboard::V) {
				current_tool.get()->handle_keyboard_events(map, event.key.code);
				map.save_state(*current_tool);
			}
		}
		if (event.key.code == sf::Keyboard::Q) { current_tool.get()->handle_keyboard_events(map, event.key.code); }
		if (event.key.code == sf::Keyboard::LShift || event.key.code == sf::Keyboard::RShift) { pressed_keys.set(PressedKeys::shift); }
		if (event.key.code == sf::Keyboard::LControl || event.key.code == sf::Keyboard::RControl) {
			current_tool.get()->handle_keyboard_events(map, event.key.code);
			pressed_keys.set(PressedKeys::control);
		}
		if (event.key.code == sf::Keyboard::LAlt) {
			if (current_tool->type == ToolType::brush) { current_tool = std::move(std::make_unique<Eyedropper>()); }
		}
		if (event.key.code == sf::Keyboard::Z) {
			if (control_pressed() && !shift_pressed()) { map.undo(); }
			if (control_pressed() && shift_pressed()) { map.redo(); }
		}
	}
	if (event.type == sf::Event::EventType::KeyReleased) {
		current_tool.get()->ready = false;
		secondary_tool.get()->ready = false;
		if (event.key.code == sf::Keyboard::LShift || event.key.code == sf::Keyboard::RShift) { pressed_keys.reset(PressedKeys::shift); }
		if (event.key.code == sf::Keyboard::LControl || event.key.code == sf::Keyboard::RControl) { pressed_keys.reset(PressedKeys::control); }
		if (event.key.code == sf::Keyboard::LAlt) {
			if (current_tool->type == ToolType::eyedropper) { current_tool = std::move(std::make_unique<Brush>()); }
		}
	}
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		map.save_state(*current_tool);
		current_tool.get()->handle_events(map, event);
		current_tool.get()->active = true;
		if (current_tool->type == ToolType::eyedropper) { selected_block = current_tool->tile; }
	} else {
		current_tool.get()->active = false;
		current_tool.get()->just_clicked = true;
		current_tool.get()->clicked_position = {0.0f, 0.0f};
		current_tool.get()->scaled_clicked_position = {0, 0};
		current_tool.get()->relative_position = {0.0f, 0.0f};
	}
	if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
		secondary_tool.get()->handle_events(map, event);
		secondary_tool.get()->active = true;
	} else {
		secondary_tool.get()->active = false;
		secondary_tool.get()->just_clicked = true;
		secondary_tool.get()->clicked_position = {0.0f, 0.0f};
		secondary_tool.get()->scaled_clicked_position = {0, 0};
		secondary_tool.get()->relative_position = {0.0f, 0.0f};
	}
	// select tool gets special treatment, because it can be used without the mouse being pressed (copy/paste)
	if (current_tool.get()->type == ToolType::select) { current_tool.get()->handle_events(map, event); }
}

void Editor::logic() {

	window_hovered = ImGui::IsAnyItemHovered() || ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) || ImGui::IsAnyItemActive();
	map.active_layer = active_layer;
	player_start = map.player_start;
	if (current_tool->trigger_switch) { current_tool = std::move(std::make_unique<Hand>()); }
	current_tool->update();
	secondary_tool->update();
	current_tool->tile = selected_block;
	if (current_tool.get()->type == ToolType::hand && current_tool.get()->active) {
		camera.move(current_tool.get()->relative_position);
		current_tool.get()->relative_position = {0.0f, 0.0f};
	} else if (secondary_tool.get()->type == ToolType::hand && secondary_tool.get()->active) {
		camera.move(secondary_tool.get()->relative_position);
		secondary_tool.get()->relative_position = {0.0f, 0.0f};
	}
	camera.update();
}

void Editor::render(sf::RenderWindow& win) {
	backdrop.setPosition(camera.position.x, camera.position.y);
	backdrop.setSize(map.real_dimensions);
	win.draw(backdrop);
	if (!map.map_states.empty()) {
		for (auto& layer : map.map_states.back().layers) {
			box.setFillColor(sf::Color{static_cast<uint8_t>(layer.render_order * 30), 230, static_cast<uint8_t>(255 - layer.render_order * 30), 40});
			for (auto& cell : layer.grid.cells) {
				if (cell.value == 0) { continue; }
				if (layer.render_order == active_layer || show_all_layers) {
					sprites.tileset.setTextureRect(sf::IntRect{get_tile_coord(cell.value), {32, 32}});
					sprites.tileset.setPosition(cell.position.x + camera.position.x, cell.position.y + camera.position.y);
					win.draw(sprites.tileset);
				} else {
					box.setPosition(cell.position.x + camera.position.x, cell.position.y + camera.position.y);
					win.draw(box);
				}
			}
		}
	}
	for (auto& portal : map.portals) {
		for (uint32_t i = 0; i < portal.dimensions.x; ++i) {
			for (uint32_t j = 0; j < portal.dimensions.y; ++j) {
				portalbox.setPosition((portal.position.x + i) * CELL_SIZE + camera.position.x, (portal.position.y + j) * CELL_SIZE + camera.position.y);
				win.draw(portalbox);
			}
		}
	}

	for (auto& inspectable : map.inspectables) {
		inspbox.setPosition((inspectable.position.x) * CELL_SIZE + camera.position.x, (inspectable.position.y) * CELL_SIZE + camera.position.y);
		win.draw(inspbox);
	}

	for (auto& block : map.switch_blocks) {
		inspbox.setPosition((block.position.x) * CELL_SIZE + camera.position.x, (block.position.y) * CELL_SIZE + camera.position.y);
		win.draw(inspbox);
	}

	for (auto& button : map.switch_buttons) {
		inspbox.setPosition((button.position.x) * CELL_SIZE + camera.position.x, (button.position.y) * CELL_SIZE + camera.position.y);
		win.draw(inspbox);
	}

	for (auto& chest : map.chests) {
		chestbox.setPosition((chest.position.x) * CELL_SIZE + camera.position.x, (chest.position.y) * CELL_SIZE + camera.position.y);
		win.draw(chestbox);
	}
	for (auto& scenery : map.scenery) {
		scenerybox.setPosition((scenery.position.x) * CELL_SIZE + camera.position.x, (scenery.position.y) * CELL_SIZE + camera.position.y);
		win.draw(scenerybox);
	}

	for (auto& scenery : map.interactive_scenery) {
		vinebox.setPosition((scenery.position.x) * CELL_SIZE + camera.position.x, (scenery.position.y) * CELL_SIZE + camera.position.y);
		scenery.type == 0 ? vinebox.setSize({static_cast<float>(scenery.size) * 8.f, static_cast<float>(scenery.length) * CELL_SIZE})
						  : vinebox.setSize({static_cast<float>(scenery.size) * 8.f, static_cast<float>(scenery.length) * CELL_SIZE});
		scenery.foreground ? vinebox.setFillColor(sf::Color{90, 120, 80, 88}) : vinebox.setFillColor(sf::Color{220, 120, 80, 88});
		scenery.type == 0 ? vinebox.setOutlineColor(sf::Color::White) : vinebox.setOutlineColor(sf::Color::Blue);
		win.draw(vinebox);
		if (scenery.has_platform) {
			for (auto& link : scenery.link_indeces) {
				vinebox.setSize({64, 16});
				vinebox.setOrigin({32, 0});
				vinebox.setPosition(vinebox.getPosition() + sf::Vector2<float>(0.f, CELL_SIZE * link));
				win.draw(vinebox);
			}
		}
		vinebox.setOrigin({});
	}

	if (map.save_point.placed) {
		savebox.setPosition((map.save_point.position.x) * CELL_SIZE + camera.position.x, (map.save_point.position.y) * CELL_SIZE + camera.position.y);
		win.draw(savebox);
	}

	for (auto& platform : map.platforms) {
		auto f_extent = platform.extent * 32.f;
		platextent.setSize({f_extent, f_extent});
		platextent.setPosition((platform.position.x) * CELL_SIZE + camera.position.x + platform.dimensions.x * 16.f,
							   (platform.position.y) * CELL_SIZE + camera.position.y + platform.dimensions.y * 16.f);
		platbox.setSize({platform.dimensions.x * 32.f, platform.dimensions.y * 32.f});
		platbox.setPosition((platform.position.x) * CELL_SIZE + camera.position.x, (platform.position.y) * CELL_SIZE + camera.position.y);
		win.draw(platextent);
		win.draw(platbox);
	}

	for (auto& critter : map.critters) {
		int idx = critter.id;
		sprites.current_enemy.setTextureRect(sf::IntRect({(idx * 16) % (64), (idx / 4) * 16}, {16, 16}));
		sprites.current_enemy.setScale({2.0f, 2.0f});
		sprites.current_enemy.setPosition((critter.position.x) * CELL_SIZE + camera.position.x, (critter.position.y) * CELL_SIZE + camera.position.y);
		win.draw(sprites.current_enemy);
	}

	for (auto& animator : map.animators) {
		sf::Vector2<float> anim_pos = {animator.position.x * CELL_SIZE + camera.position.x, animator.position.y * CELL_SIZE + camera.position.y};
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
		} else if ((active_layer < 4 && animator.foreground) || (active_layer >= 4 && !animator.foreground)) {
			win.draw(box);
		}
	}

	// player start
	player_box.setPosition(static_cast<sf::Vector2<float>>(player_start) * (float)CELL_SIZE + camera.position);
	win.draw(player_box);

	if (show_grid && !map.map_states.empty()) {
		if (map.map_states.back().layers.empty()) { return; }
		for (auto& cell : map.map_states.back().layers.back().grid.cells) {
			if (cell.position.x + camera.bounding_box.left < 0) { continue; }
			if (cell.position.x + camera.bounding_box.left > screen_dimensions.x) { continue; }
			if (cell.position.y + camera.bounding_box.top < 0) { continue; }
			if (cell.position.y + camera.bounding_box.top > screen_dimensions.y) { continue; }
			gridbox.setPosition(cell.position.x + camera.position.x, cell.position.y + camera.position.y);
			win.draw(gridbox);
		}
	}

	if (current_tool.get()->ready && current_tool.get()->in_bounds(map.dimensions) &&
		(current_tool.get()->type == ToolType::brush || current_tool.get()->type == ToolType::fill || current_tool.get()->type == ToolType::erase ||
		 current_tool.get()->type == ToolType::entity_placer)) {
		for (int i = 0; i < current_tool.get()->size; i++) {
			for (int j = 0; j < current_tool.get()->size; j++) {
				target.setPosition((current_tool.get()->scaled_position.x - i) * CELL_SIZE + camera.position.x,
								   (current_tool.get()->scaled_position.y - j) * CELL_SIZE + camera.position.y);
				win.draw(target);
			}
		}
	}

	current_tool.get()->render(win, camera.position);
	float tool_x = current_tool.get()->position.x + camera.position.x;
	float tool_y = current_tool.get()->position.y + camera.position.y;
	if (!window_hovered) {
		sprites.tool.setTextureRect({{static_cast<int>(current_tool.get()->type) * 32, 0}, {32, 32}});
		sprites.tool.setPosition(tool_x, tool_y);
		win.draw(sprites.tool);
	}
	gui_render(win);
}

void Editor::gui_render(sf::RenderWindow& win) {
	bool* debug{};
	float const PAD = 10.0f;
	static int corner = 0;
	ImGuiIO& io = ImGui::GetIO();
	io.FontGlobalScale = 1.0;

	current_tool.get()->position = sf::Vector2<float>{io.MousePos.x, io.MousePos.y} - camera.position;
	secondary_tool.get()->position = sf::Vector2<float>{io.MousePos.x, io.MousePos.y} - camera.position;

	if (current_tool.get()->type == ToolType::select && !window_hovered) {
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::Text("Press `Ctrl+X` to cut selection.");
		ImGui::Text("Press `Ctrl+C` to copy selection.");
		ImGui::Text("Press `Ctrl+V` to paste selection.");
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}

	if (current_tool.get()->type == ToolType::entity_placer && !window_hovered) {
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		if (current_tool.get()->ent_type == ENTITY_TYPE::PORTAL) {
			ImGui::Text("Click to place portal to room %i%s", current_tool.get()->current_portal.destination_map_id, ".");
			ImGui::Text("Portal size: width: %i, height: %i", current_tool.get()->current_portal.dimensions.x, current_tool.get()->current_portal.dimensions.y);
			ImGui::Text("Activate on contact: %s", current_tool.get()->current_portal.activate_on_contact ? "Yes" : "No");
			ImGui::TextUnformatted("Press 'Q' to cancel.");
		} else if (current_tool.get()->ent_type == ENTITY_TYPE::INSPECTABLE) {
			if (!current_tool.get()->current_inspectable.suites.empty()) {
				if (!current_tool.get()->current_inspectable.suites.back().empty()) { ImGui::Text("Place an inspectable with message: %s", current_tool.get()->current_inspectable.suites.back().back().c_str()); }
			}
			ImGui::Text("and with key: %s", current_tool.get()->current_inspectable.key.c_str());
			ImGui::Text("Activate on contact: %s", current_tool.get()->current_inspectable.activate_on_contact ? "Yes" : "No");
			ImGui::TextUnformatted("Press 'Q' to cancel.");
		} else if (current_tool.get()->ent_type == ENTITY_TYPE::CRITTER) {
			ImGui::Text("id: %i", current_tool.get()->current_critter.id);
		} else if (current_tool.get()->ent_type == ENTITY_TYPE::ENTITY_EDITOR) {
			ImGui::Text("Erase Entity");
		} else {
			ImGui::Text("Place Entity");
		}
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}

	// Main Menu
	if (ImGui::BeginMainMenuBar()) {
		bool new_popup{};
		bool save_popup{};
		bool save_as_popup{};
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("New", NULL, &new_popup)) {}

			// Always center this window when appearing
			ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
			ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
			ImGui::Separator();
			if (ImGui::MenuItem("Open")) {
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
					camera.set_position({});
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
			ImGui::Separator();
			if (ImGui::MenuItem("Save", NULL, &save_popup)) {}
			if (ImGui::MenuItem("Save As", NULL, &save_as_popup)) {}
			ImGui::EndMenu();
		}
		if (save_popup) {
			map.save(filepath);
			map.save(demopath);
			console.add_log("File saved successfully.\n");
		}
		if (new_popup) { ImGui::OpenPopup("New File"); }
		if (ImGui::BeginPopupModal("New File", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::Text("Please enter a new room name.");
			ImGui::Text("Convention is all caps, snake-case, and of the format `REGION_ROOM_NUMBER`.");
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
			static int metagrid_x{};
			static int metagrid_y{};

			ImGui::InputInt("Width", &width);
			ImGui::NewLine();

			ImGui::InputInt("Height", &height);
			ImGui::Separator();
			ImGui::NewLine();

			ImGui::InputInt("Room ID", &room_id);
			ImGui::Separator();
			ImGui::NewLine();

			ImGui::Text("Metagrid Position");
			ImGui::InputInt("X", &metagrid_x);
			ImGui::SameLine();

			ImGui::InputInt("Y", &metagrid_y);
			ImGui::Separator();
			ImGui::NewLine();

			if (ImGui::Button("Close")) { ImGui::CloseCurrentPopup(); }
			ImGui::SameLine();
			if (ImGui::Button("Create")) {

				// set new bg to current one to make it easier to design multiple rooms
				static int style_current = static_cast<int>(map.style);
				static int bg_current = static_cast<int>(map.bg);

				map = Canvas({static_cast<uint32_t>(width * CHUNK_SIZE), static_cast<uint32_t>(height * CHUNK_SIZE)});
				map.style = static_cast<Style>(style_current);
				setTilesetTexture(tileset_textures.at(style_current));
				map.bg = static_cast<Backdrop>(bg_current);
				map.metagrid_coordinates = {metagrid_x, metagrid_y};
				filepath = folderpath + buffer;
				map.save(filepath);
				map.load(filepath);
				camera.set_position({});
				map.room_id = room_id;
				std::filesystem::path room_dir = filepath;
				room = room_dir.filename().string();
				room_id = map.room_id;
				// demopath = "../../../demo/resources/level/" + room;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
		if (save_as_popup) { ImGui::OpenPopup("Save As"); }
		if (ImGui::BeginPopupModal("Save As", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::Text("Please enter a unique room name.");
			ImGui::Text("Convention is all caps, snake-case, and of the format `Style_NAME_INDEX`.");
			ImGui::Separator();
			ImGui::NewLine();
			static char buffer[128] = "";

			ImGui::InputTextWithHint("Folder Name", "DOJO_CORRIDOR_01", buffer, IM_ARRAYSIZE(buffer));
			ImGui::Separator();
			ImGui::NewLine();

			if (ImGui::Button("Close")) { ImGui::CloseCurrentPopup(); }
			ImGui::SameLine();
			if (ImGui::Button("Create")) {
				std::string savepath = folderpath + buffer;
				map.save(savepath);
				map.save(demopath);
				ImGui::CloseCurrentPopup();
			}

			ImGui::TextUnformatted(buffer);

			ImGui::EndPopup();
		}
		// none of the below have been implemented and will likely be removed
		if (ImGui::BeginMenu("Edit")) {
			if (ImGui::MenuItem("Undo", "CTRL+Z")) { map.undo(); }
			if (ImGui::MenuItem("Redo", "CTRL+SHIFT+Z")) { map.redo(); }
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Help")) {
			ImGui::Text("Eyedropper Tool: Alt");
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
					if (window_hovered) {
						ImGui::Text("Yes");
					} else {
						ImGui::Text("No");
					};

					/*constexpr auto text = R"({ "foo": "party", "universe": 42 })";
					auto const json = dj::Json::parse(text);
					ImGui::Text("djson test: %i", json[1]);*/
					ImGui::Text("Room ID: %u", map.room_id);
					//                    ImGui::Text("Room Name: %s", room_name_lookup.at(map.room_id).c_str());
					ImGui::Text("Camera Position: (%.1f,%.1f)", camera.position.x, camera.position.y);
					ImGui::Text("Active Layer: %i", active_layer);
					ImGui::Text("Num Layers: %lu", map.map_states.back().layers.size());
					ImGui::Text("Stored Tile Value: %u", current_tool.get()->tile);
					if (current_tool.get()->in_bounds(map.dimensions)) {
						ImGui::Text("Tile Value at Mouse Pos: %u", map.tile_val_at(current_tool.get()->scaled_position.x, current_tool.get()->scaled_position.y, active_layer));
					} else {
						ImGui::Text("Tile Value at Mouse Pos: <invalid>");
					}
					ImGui::Separator();
					ImGui::Text("Current Style: %s", get_style_string.at(map.style));
					ImGui::Text("Current Backdrop: %s", get_backdrop_string.at(map.bg));
					ImGui::Text("Number of Portals: %i", map.portals.size());
					ImGui::Text("Number of Inspectables: %i", map.inspectables.size());
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Tool")) {
					ImGui::Text("Tool Position: (%.1f,%.1f)", current_tool.get()->position.x, current_tool.get()->position.y);
					ImGui::Text("Clicked Position: (%.1f,%.1f)", current_tool.get()->clicked_position.x, current_tool.get()->clicked_position.y);
					ImGui::Text("Relative Position: (%.1f,%.1f)", current_tool.get()->relative_position.x, current_tool.get()->relative_position.y);
					ImGui::Text("Tool Position (scaled): (%i,%i)", current_tool.get()->scaled_position.x, current_tool.get()->scaled_position.y);
					ImGui::Text("Tool Clicked Position (scaled): (%i,%i)", current_tool.get()->scaled_clicked_position.x, current_tool.get()->scaled_clicked_position.y);
					ImGui::Text("Tool in Bounds: ");
					ImGui::SameLine();
					if (current_tool.get()->in_bounds(map.dimensions)) {
						ImGui::Text("Yes");
					} else {
						ImGui::Text("No");
					};
					ImGui::Text("Tool Ready: ");
					ImGui::SameLine();
					if (current_tool.get()->ready) {
						ImGui::Text("Yes");
					} else {
						ImGui::Text("No");
					};
					ImGui::Text("Tool Active: ");
					ImGui::SameLine();
					if (current_tool.get()->active) {
						ImGui::Text("Yes");
					} else {
						ImGui::Text("No");
					};

					ImGui::Text("Current Tool: %s", get_tool_string.at(current_tool->type).c_str());
					ImGui::Text("Secondary Tool: %s", get_tool_string.at(secondary_tool->type).c_str());
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
		if (ImGui::IsWindowHovered()) { ImGui::SetWindowFocus(); }
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
					current_tool.get()->store_tile(idx);
					selected_block = idx;
					if (!current_tool.get()->is_paintable()) { current_tool = std::move(std::make_unique<Brush>()); }
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
					current_tool = std::move(std::make_unique<EntityPlacer>());
					current_tool.get()->ent_type = ENTITY_TYPE::CRITTER;
					current_tool.get()->current_critter = Critter(static_cast<CRITTER_TYPE>(idx));
					current_tool.get()->current_critter.id = idx;
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
						current_tool = std::move(std::make_unique<EntityPlacer>());
						current_tool.get()->ent_type = ENTITY_TYPE::ANIMATOR;
						current_tool.get()->current_animator = Animator(sf::Vector2<uint32_t>{(uint32_t)2, (uint32_t)2}, idx + large_index_multiplier, false, active_layer >= 4); // change booleans here later
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
						current_tool = std::move(std::make_unique<EntityPlacer>());
						current_tool.get()->ent_type = ENTITY_TYPE::ANIMATOR;
						current_tool.get()->current_animator = Animator(sf::Vector2<uint32_t>{(uint32_t)1, (uint32_t)1}, idx + small_index_multiplier, false, active_layer >= 4); // change booleans here later
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
				current_tool = std::move(std::make_unique<EntityPlacer>());
				current_tool.get()->ent_type = ENTITY_TYPE::BED;
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
				current_tool = std::move(std::make_unique<EntityPlacer>());
				current_tool.get()->ent_type = ENTITY_TYPE::ANIMATOR;
				current_tool.get()->current_animator = Animator(sf::Vector2<uint32_t>{(uint32_t)1, (uint32_t)1}, id, true, false, style);
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
			static bool activate_on_contact{};
			static bool already_open{};
			static bool locked{};
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

			ImGui::Checkbox("Already open?", &already_open);
			ImGui::SameLine();
			help_marker("Only applies to 1x1 portals that are not activated on contact (doors). If true, the door will appear open.");
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
				current_tool = std::move(std::make_unique<EntityPlacer>());
				current_tool.get()->ent_type = ENTITY_TYPE::PORTAL;
				current_tool.get()->current_portal = Portal(sf::Vector2<uint32_t>{(uint32_t)width, (uint32_t)height}, activate_on_contact, already_open, map.room_id, destination, locked, key_id);
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
				current_tool = std::move(std::make_unique<EntityPlacer>());
				current_tool.get()->ent_type = ENTITY_TYPE::INSPECTABLE;
				current_tool.get()->current_inspectable = Inspectable(sf::Vector2<uint32_t>{1, 1}, activate_on_contact, keybuffer);
				current_tool.get()->current_inspectable.suites.push_back(std::vector<std::string>{msgbuffer}); 
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
			std::string type{};
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
			if (ImGui::BeginMenu("Platform Type")) {
				if (ImGui::MenuItem("Up-Down")) { type = "standard_up_down"; }
				if (ImGui::MenuItem("Side-to-Side")) { type = "standard_side_to_side"; }
				if (ImGui::MenuItem("Square")) { type = "standard_square"; }
				if (ImGui::MenuItem("Activated Up-Down")) { type = "standard_activated_up_down"; }
				if (ImGui::MenuItem("Activated Side-to-Side")) { type = "standard_activated_side_to_side"; }
			}
			ImGui::Separator();
			ImGui::NewLine();

			ImGui::InputFloat("Start", &start);
			ImGui::SameLine();
			help_marker("Must be a value between 0 and 1.");
			ImGui::Separator();
			ImGui::NewLine();

			if (ImGui::Button("Create")) {
				// switch to entity tool, and store the specified portal for placement
				current_tool = std::move(std::make_unique<EntityPlacer>());
				current_tool.get()->ent_type = ENTITY_TYPE::PLATFORM;
				current_tool.get()->current_platform = Platform(sf::Vector2<uint32_t>{0, 0}, sf::Vector2<uint32_t>{(uint32_t)x, (uint32_t)y}, extent, style, type, start);
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
			help_marker("1 for gun, 2 for orbs, 3 for item");
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
				current_tool = std::move(std::make_unique<EntityPlacer>());
				current_tool.get()->ent_type = ENTITY_TYPE::CHEST;
				current_tool.get()->current_chest = Chest(id, item_id, type, rarity, amount);
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Close")) { ImGui::CloseCurrentPopup(); }
			ImGui::EndPopup();
		}
		if (ImGui::Button("Vine")) { ImGui::OpenPopup("Vine Specifications"); }
		if (ImGui::BeginPopupModal("Vine Specifications", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {

			static int length{};
			static int size{};
			static bool foreground{};
			static int type{};
			static int platform{};

			ImGui::SameLine();
			ImGui::Separator();
			ImGui::NewLine();

			ImGui::InputInt("Length", &length);
			ImGui::InputInt("Size", &size);
			ImGui::InputInt("Platform Indeces", &platform);
			ImGui::SameLine();
			help_marker("Either 1 (1x1) or 2 (2x2)");
			ImGui::Checkbox("Foreground?", &foreground);
			ImGui::InputInt("Type", &type);
			ImGui::SameLine();
			help_marker("0 : vine, 1 : grass");

			ImGui::NewLine();

			if (ImGui::Button("Create")) {
				// switch to entity tool, and store the specified portal for placement
				current_tool = std::move(std::make_unique<EntityPlacer>());
				current_tool.get()->ent_type = ENTITY_TYPE::INTERACTIVE_SCENERY;
				current_tool.get()->current_interactive_scenery = InteractiveScenery(length, size, foreground, type, platform != 0, {platform});
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Close")) { ImGui::CloseCurrentPopup(); }
			ImGui::EndPopup();
		}
		if (ImGui::Button("Scenery")) { ImGui::OpenPopup("Scenery Specifications"); }
		if (ImGui::BeginPopupModal("Scenery Specifications", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {

			static int style{};
			static int layer{};
			static int variant{};

			ImGui::SameLine();
			ImGui::Separator();
			ImGui::NewLine();

			ImGui::InputInt("Style", &style);
			ImGui::InputInt("Layer", &layer);
			ImGui::InputInt("Variant", &variant);

			ImGui::NewLine();

			if (ImGui::Button("Create")) {
				// switch to entity tool, and store the specified portal for placement
				current_tool = std::move(std::make_unique<EntityPlacer>());
				current_tool.get()->ent_type = ENTITY_TYPE::SCENERY;
				current_tool.get()->current_scenery = Scenery(style, layer, variant);
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
				current_tool = std::move(std::make_unique<EntityPlacer>());
				current_tool.get()->ent_type = ENTITY_TYPE::SWITCH_BUTTON;
				current_tool.get()->current_switch = SwitchButton(button_id, type);
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
				current_tool = std::move(std::make_unique<EntityPlacer>());
				current_tool.get()->ent_type = ENTITY_TYPE::SWITCH_BLOCK;
				current_tool.get()->current_switch_block = SwitchBlock(button_id, type);
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Close")) { ImGui::CloseCurrentPopup(); }
			ImGui::EndPopup();
		}
		if (ImGui::Button("Save Point")) {
			current_tool = std::move(std::make_unique<EntityPlacer>());
			current_tool.get()->ent_type = ENTITY_TYPE::SAVE_POINT;
		}
		if (ImGui::Button("Player Placer")) {
			current_tool = std::move(std::make_unique<EntityPlacer>());
			current_tool.get()->ent_type = ENTITY_TYPE::PLAYER_PLACER;
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
				ImGui::Text(get_tool_string.at((ToolType)i).c_str());
				ImGui::PopTextWrapPos();
				ImGui::EndTooltip();
			}
			if (ImGui::IsItemClicked()) {
				switch ((ToolType)i) {
				case ToolType::brush: current_tool = std::move(std::make_unique<Brush>()); break;
				case ToolType::fill: current_tool = std::move(std::make_unique<Fill>()); break;
				case ToolType::select: current_tool = std::move(std::make_unique<SelectionRectangular>()); break;
				case ToolType::erase: current_tool = std::move(std::make_unique<Erase>()); break;
				case ToolType::hand: current_tool = std::move(std::make_unique<Hand>()); break;
				case ToolType::entity_placer:
					current_tool = std::move(std::make_unique<EntityPlacer>());
					current_tool.get()->ent_type = ENTITY_TYPE::ENTITY_EDITOR;
					break;
				default: current_tool = std::move(std::make_unique<Hand>()); break;
				}
			}
			ImGui::PopID();
			ImGui::SameLine();
		}

		ImGui::EndChild();
		ImGui::PopStyleVar();

		ImGui::Separator();
		ImGui::Text("Brush Size");
		ImGui::SliderInt("##brushsz", &current_tool.get()->size, 1, 16);
		if (current_tool->type == ToolType::select) {
			if (ImGui::Checkbox("Include All Layers in Selection", &current_tool->pervasive)) { current_tool->clear(); }
		} else {
			ImGui::NewLine();
		}
		ImGui::Separator();
		ImGui::Text("Current Block:");
		sprites.tileset.setTextureRect(sf::IntRect({get_tile_coord(selected_block), {32, 32}}));
		ImGui::Image(sprites.tileset);
		if (current_tool->type == ToolType::entity_placer) {
			ImGui::Text("Current Entity:");
			switch (current_tool.get()->ent_type) {
			case ENTITY_TYPE::PORTAL: ImGui::Text("Portal"); break;
			case ENTITY_TYPE::INSPECTABLE: ImGui::Text("Inspectable"); break;
			case ENTITY_TYPE::ANIMATOR: ImGui::Text("Animator"); break;
			case ENTITY_TYPE::CRITTER: ImGui::Text("Critter"); break;
			case ENTITY_TYPE::PLAYER_PLACER: ImGui::Text("Player Placer"); break;
			case ENTITY_TYPE::SAVE_POINT: ImGui::Text("Save Point"); break;
			case ENTITY_TYPE::CHEST: ImGui::Text("Chest"); break;
			case ENTITY_TYPE::ENTITY_EDITOR: ImGui::Text("Entity Editor"); break;
			}
		}
		if (current_tool.get()->in_bounds(map.dimensions)) {
			ImGui::Text("Tool Position : (%i,%i)", current_tool.get()->scaled_position.x, current_tool.get()->scaled_position.y);
		} else {
			ImGui::Text("Tool Position : ---");
		}

		ImGui::Separator();
		ImGui::Text("Canvas Settings");
		ImGui::Separator();

		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
		ImGui::BeginChild("ChildS", ImVec2(320, 172), true, window_flags);
		ImGui::BeginMenuBar();
		if (ImGui::BeginMenu("Current Layer")) { ImGui::EndMenu(); }
		ImGui::EndMenuBar();
		for (int i = 0; i < 8; ++i) {
			if (ImGui::Selectable(layer_name.at(i), active_layer == i)) { active_layer = i; }
		}
		ImGui::EndChild();
		ImGui::PopStyleVar();

		ImGui::Separator();
		ImGui::Text("General Settings");
		ImGui::Separator();

		ImGui::Checkbox("Debug Overlay", &show_overlay);
		ImGui::Checkbox("Show Grid", &show_grid);
		ImGui::Checkbox("Show All Layers", &show_all_layers);

		ImGui::NewLine();
		ImGui::Separator();
		ImGui::Text("Actions");
		ImGui::Separator();
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.5, 0.6f, 0.5f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.5, 0.7f, 0.6f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.5, 0.8f, 0.7f));

		if (ImGui::Button("Launch Demo")) {
			trigger_demo = true;
			map.save(filepath);
			map.save(demopath);
		};
		ImGui::PopStyleColor(3);
		if (ImGui::Button("Export Layer to .png")) {
			screencap.create(win.getSize().x, win.getSize().y);
			export_layer_texture();
		};
		if (ImGui::Button("Undo")) { map.undo(); }
		ImGui::SameLine();
		if (ImGui::Button("Redo")) { map.redo(); }

		ImGui::Text("Scene Style: ");
		int style_current = static_cast<int>(map.style);
		if (ImGui::Combo("##scenestyle", &style_current, styles, IM_ARRAYSIZE(styles))) {
			map.style = static_cast<Style>(style_current);
			setTilesetTexture(tileset_textures.at(style_current));
		}
		ImGui::Text("Scene Background: ");
		int bg_current = static_cast<int>(map.bg);
		if (ImGui::Combo("##scenebg", &bg_current, bgs, IM_ARRAYSIZE(bgs))) { map.bg = static_cast<Backdrop>(bg_current); }
		if (ImGui::Button("Clear Layer")) {
			map.map_states.push_back(map.map_states.back());
			map.map_states.back().layers.at(active_layer).clear();
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
		ImGui::Separator();
		prev_window_size = ImGui::GetWindowSize();
		prev_window_pos = ImGui::GetWindowPos();
		ImGui::End();
	}
	console.write_console(prev_window_size, prev_window_pos);
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
	sf::Vector2u mapdim{map.real_dimensions};
	screencap.create(mapdim.x, mapdim.y);
	for (int i = 0; i <= active_layer; ++i) {
		for (auto& cell : map.map_states.back().layers.at(i).grid.cells) {
			if (cell.value > 0) {
				auto x_coord = (cell.value % 16) * TILE_WIDTH;
				auto y_coord = std::floor(cell.value / 16) * TILE_WIDTH;
				tile_sprite.setTexture(tileset_textures.at(static_cast<int>(map.style)));
				tile_sprite.setTextureRect(sf::IntRect({static_cast<int>(x_coord), static_cast<int>(y_coord)}, {32, 32}));
				tile_sprite.setPosition(cell.position);
				screencap.draw(tile_sprite);
			}
		}
	}
	std::time_t const now = std::time(nullptr);
	std::string filedate = std::asctime(std::localtime(&now));
	std::erase_if(filedate, [](auto const& c) { return c == ':' || isspace(c); });
	std::string filename = "screenshot_" + filedate + ".png";
	if (screencap.getTexture().copyToImage().saveToFile(filename)) {
		std::string log = "Screenshot saved to " + filename + "\n";
		console.add_log(log.data());
	}
}

sf::Vector2<int> Editor::get_tile_coord(int lookup) {
	sf::Vector2<int> ret{};
	ret.x = static_cast<int>(lookup % 16);
	ret.y = static_cast<int>(std::floor(lookup / 16));
	return ret * 32;
}

void Editor::launch_demo(char** argv, int room_id, std::filesystem::path path, sf::Vector2<float> player_position) {
	trigger_demo = false;
	ImGui::SFML::Shutdown();
	fornani::Application demo{argv};
	std::cout << "Editor path: " << path.string() << "\n";
	demo.launch(argv, true, room_id, path, player_position);
}

} // namespace pi
