#include <algorithm>
#include "../setup/ResourceFinder.hpp"
#include "../../demo/src/setup/Application.hpp"
#include "../util/Lookup.hpp"
#include "Editor.hpp"
#include "../gui/Console.hpp"
#include <filesystem>

namespace pi {

Editor::Editor(char** argv, WindowManager& window, ResourceFinder& finder) : window(&window), finder(&finder) {
	args = argv;
	finder.paths.levels = finder.find_directory(argv[0], "demo/resources/level");
	finder.paths.resources = finder.find_directory(argv[0], "demo/resources");
	finder.paths.out = finder.find_directory(argv[0], "export/levels");
	std::cout << "Level path: " << finder.paths.levels << "\n";
}

void Editor::run() {

	// load textures
	int const TILE_WIDTH = 32;

	// load the tilesets!
	sf::Texture t_tiles_provisional{};
	t_tiles_provisional.loadFromFile((finder->paths.resources / "image" / "tile" / "provisional_tiles.png").string());
	for (int i = 0; i < static_cast<int>(Style::END); ++i) {
		tileset_textures.push_back(sf::Texture());
		std::string style = get_style_string.at(static_cast<Style>(i));
		std::string filename = style + "_tiles.png";
		tileset_textures.back().loadFromFile((finder->paths.resources / "image" / "tile" / filename).string());
	}

	init("new_file");
	
	bool debug_mode = false;

	sf::RectangleShape background{};
	background.setSize(static_cast<sf::Vector2<float>>(screen_dimensions));
	colors.backdrop = sf::Color{40, 60, 80};
	sf::Clock delta_clock{};

	// editor loop
	while (window->get().isOpen()) {

		if (flags.test(GlobalFlags::shutdown)) { return; }

		if (trigger_demo) {
			auto ppos = static_cast<sf::Vector2<float>>(map.entities.variables.player_start) * 32.f;
			launch_demo(args, map.room_id, finder->paths.room_name, ppos);
			ImGui::SFML::Init(window->get());
			init(finder->paths.room_name);
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
				break;
			default: break;
			}
			handle_events(event, window->get());
		}

		// game logic and rendering
		logic();

		// ImGui update

		ImGui::SetMouseCursor(ImGuiMouseCursor_None);
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDrawCursor = window_hovered && !palette.hovered();
		window->get().setMouseCursorVisible(io.MouseDrawCursor);
		ImGui::SFML::Update(window->get(), delta_clock.getElapsedTime());
		delta_clock.restart();

		window->get().clear();
		background.setFillColor(colors.backdrop);
		window->get().draw(background);

		render(window->get());

		ImGui::SFML::Render(window->get());
		window->get().display();
	}
}

void Editor::init(std::string const& load_path) {

	tool_texture.loadFromFile((finder->paths.local / "gui" / "tools.png").string());

	sprites.tool.setTexture(tool_texture);

	for (int i = 0; i < static_cast<int>(Style::END); ++i) {
		char const* next = get_style_string.at(static_cast<Style>(i));
		styles[i] = next;
	}
	for (int i = 0; i < static_cast<int>(Backdrop::END); ++i) {
		char const* next = get_backdrop_string.at(static_cast<Backdrop>(i));
		bgs[i] = next;
	}

	finder->paths.room_name = load_path;
	std::string msg = "Loading room: " + finder->paths.room_name;
	console.add_log(msg.data());
	load();
	map.get_layers().layers.at(MIDDLEGROUND).active = true;
	std::cout << static_cast<int>(map.styles.tile) << "\n";
	sprites.tileset.setTexture(tileset_textures.at(static_cast<int>(map.styles.tile)));

	colors.backdrop = sf::Color{60, 40, 60, 40};
	backdrop.setOutlineColor(sf::Color{240, 230, 255, 40});
	backdrop.setOutlineThickness(4);
	backdrop.setSize(map.real_dimensions);

	target.setFillColor(sf::Color{110, 90, 200, 80});
	target.setOutlineColor(sf::Color{240, 230, 255, 100});
	target.setOutlineThickness(-2);
	target.setSize({CELL_SIZE, CELL_SIZE});

	map.center(window->f_center_screen());
}

void Editor::handle_events(sf::Event& event, sf::RenderWindow& win) {

	auto& target = palette_mode() ? palette : map;

	current_tool.get()->ready = !window_hovered || palette.hovered();
	secondary_tool.get()->ready = !window_hovered;
	if (event.type == sf::Event::EventType::KeyPressed) {
		current_tool.get()->ready = false;
		secondary_tool.get()->ready = false;
		if (control_pressed()) {
			if (event.key.code == sf::Keyboard::X) { current_tool.get()->handle_keyboard_events(target, event.key.code); }
			if (event.key.code == sf::Keyboard::C) { current_tool.get()->handle_keyboard_events(target, event.key.code); }
			if (event.key.code == sf::Keyboard::V) { current_tool.get()->handle_keyboard_events(target, event.key.code); }
		}
		if (event.key.code == sf::Keyboard::Q) { current_tool.get()->handle_keyboard_events(target, event.key.code); }
		if (event.key.code == sf::Keyboard::LShift || event.key.code == sf::Keyboard::RShift) { pressed_keys.set(PressedKeys::shift); }
		if (event.key.code == sf::Keyboard::LControl || event.key.code == sf::Keyboard::RControl) {
			current_tool.get()->handle_keyboard_events(target, event.key.code);
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
		pressed_keys.set(PressedKeys::mouse);
		if (palette.hovered()) { current_tool->clear(); }
		current_tool->has_palette_selection = false;
		target.save_state(*current_tool);
		current_tool.get()->handle_events(target, event);
		current_tool.get()->active = true;
		if (current_tool->type == ToolType::eyedropper) { selected_block = current_tool->tile; }
		if (palette_mode() && current_tool->type != ToolType::select) {
			auto pos = current_tool->get_window_position() - palette.get_position();
			auto idx = palette.tile_val_at_scaled(pos.x, pos.y, 4);
			current_tool.get()->store_tile(idx);
			selected_block = idx;
			if (!current_tool.get()->is_paintable()) { current_tool = std::move(std::make_unique<Brush>()); }
		}
	} else {
		pressed_keys.reset(PressedKeys::mouse);
		current_tool.get()->active = false;
		current_tool.get()->just_clicked = true;
		current_tool.get()->clicked_position = {0.0f, 0.0f};
		current_tool.get()->scaled_clicked_position = {0, 0};
		current_tool.get()->relative_position = {0.0f, 0.0f};
	}
	if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
		secondary_tool.get()->handle_events(target, event);
		secondary_tool.get()->active = true;
	} else {
		secondary_tool.get()->active = false;
		secondary_tool.get()->just_clicked = true;
		secondary_tool.get()->clicked_position = {0.0f, 0.0f};
		secondary_tool.get()->scaled_clicked_position = {0, 0};
		secondary_tool.get()->relative_position = {0.0f, 0.0f};
	}
	// select tool gets special treatment, because it can be used without the mouse being pressed (copy/paste)
	if (current_tool.get()->type == ToolType::select) { current_tool.get()->handle_events(target, event); }
}

void Editor::logic() {

	auto& target = palette_mode() ? palette : map;

	window_hovered = ImGui::IsAnyItemHovered() || ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) || ImGui::IsAnyItemActive() || palette.hovered();
	palette.active_layer = 4;
	map.active_layer = active_layer;
	if (current_tool->trigger_switch) { current_tool = std::move(std::make_unique<Hand>()); }
	current_tool->update();
	secondary_tool->update();
	current_tool->tile = selected_block;
	if (current_tool.get()->type == ToolType::hand && current_tool.get()->active) {
		target.move(current_tool.get()->relative_position);
		current_tool.get()->relative_position = {0.0f, 0.0f};
	} else if (secondary_tool.get()->type == ToolType::hand && secondary_tool.get()->active) {
		target.move(secondary_tool.get()->relative_position);
		secondary_tool.get()->relative_position = {0.0f, 0.0f};
	}
	map.update(*current_tool);
	palette.update(*current_tool);
	palette.set_position({12.f, 32.f});
	if (palette.hovered()) { map.unhover(); }

	show_palette && !menu_hovered && (palette.hovered() || current_tool->has_palette_selection) && (!pressed_keys.test(PressedKeys::mouse) || current_tool->type == ToolType::select) && !current_tool->clipboard() ? flags.set(GlobalFlags::palette_mode)
																																												   : flags.reset(GlobalFlags::palette_mode);
}

void Editor::load() {
	map.load(*finder, finder->paths.room_name);
	palette.load(*finder, "palette", true);
}

bool Editor::save() { return map.save(*finder, finder->paths.room_name); }

void Editor::render(sf::RenderWindow& win) {
	backdrop.setFillColor(colors.backdrop);
	backdrop.setPosition(map.get_position());
	map.hovered() ? backdrop.setOutlineColor({240, 230, 255, 80}) : backdrop.setOutlineColor({240, 230, 255, 40});
	backdrop.setSize(map.real_dimensions);
	win.draw(backdrop);

	map.render(win, sprites.tileset);

	if (current_tool.get()->ready && current_tool.get()->in_bounds(map.dimensions) && !menu_hovered && !palette_mode() &&
		(current_tool.get()->type == ToolType::brush || current_tool.get()->type == ToolType::fill || current_tool.get()->type == ToolType::erase || current_tool.get()->type == ToolType::entity_placer)) {
		for (int i = 0; i < current_tool.get()->size; i++) {
			for (int j = 0; j < current_tool.get()->size; j++) {
				target.setPosition((current_tool.get()->scaled_position.x - i) * CELL_SIZE + map.get_position().x, (current_tool.get()->scaled_position.y - j) * CELL_SIZE + map.get_position().y);
				win.draw(target);
			}
		}
	}

	if (show_palette) {
		auto palette_bg = sf::RectangleShape({512, 512});
		palette_bg.setFillColor({20, 20, 20, 160});
		palette_bg.setPosition(palette.get_position());
		flags.test(GlobalFlags::palette_mode) ? palette_bg.setOutlineColor({255, 255, 255, 180}) : palette.hovered() && !menu_hovered ? palette_bg.setOutlineColor({60, 255, 90, 180}) : palette_bg.setOutlineColor({255, 255, 255, 60});
		palette_bg.setOutlineThickness(2.f);
		win.draw(palette_bg);
		palette.render(win, sprites.tileset);

		if (palette_mode()) {
			selector.setSize({32.f, 32.f});
			selector.setOutlineColor({255, 255, 255, 80});
			selector.setOutlineThickness(-2.f);
			selector.setFillColor({250, 180, 250, 20});
			selector.setPosition(palette.get_tile_position_at(current_tool->get_window_position().x - palette.get_position().x, current_tool->get_window_position().y - palette.get_position().y) + palette.get_position());
			win.draw(selector);
		}
	}

	auto& target = palette_mode() ? palette : map;
	current_tool.get()->render(win, target.get_position(), !palette_mode());
	float tool_x = current_tool.get()->position.x + target.get_position().x;
	float tool_y = current_tool.get()->position.y + target.get_position().y;
	if (!window_hovered || palette.hovered()) {
		sprites.tool.setTextureRect({{static_cast<int>(current_tool.get()->type) * 32, 0}, {32, 32}});
		sprites.tool.setPosition(tool_x, tool_y);
		win.draw(sprites.tool);
	}

	// ImGui stuff
	gui_render(win);
}

void Editor::gui_render(sf::RenderWindow& win) {
	bool* debug{};
	float const PAD = 10.0f;
	ImGuiIO& io = ImGui::GetIO();
	io.FontGlobalScale = 1.0;
	ImGuiViewport const* viewport = ImGui::GetMainViewport();
	ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar
	ImVec2 work_size = viewport->WorkSize;
	auto f_work_size = sf::Vector2<float>{static_cast<float>(work_size.x), static_cast<float>(work_size.y)};

	auto& target = palette_mode() ? palette : map;
	current_tool.get()->position = sf::Vector2<float>{io.MousePos.x, io.MousePos.y} - target.get_position();
	secondary_tool.get()->position = sf::Vector2<float>{io.MousePos.x, io.MousePos.y} - target.get_position();
	current_tool->set_window_position(sf::Vector2<float>{io.MousePos.x, io.MousePos.y});
	secondary_tool->set_window_position(sf::Vector2<float>{io.MousePos.x, io.MousePos.y});

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

	//ImGui::ShowDemoWindow();
	bool insp{};

	// Main Menu
	menu_hovered = false;
	if (ImGui::BeginMainMenuBar()) {
		bool new_popup{};
		bool save_popup{};
		bool save_as_popup{};
		if (ImGui::BeginMenu("File")) {
			menu_hovered = true;
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
					auto open_path = std::filesystem::path{filename};
					finder->paths.room_name = open_path.parent_path().filename().string();
					std::cout << "\n\nfilename: " << finder->paths.room_name << "\n\n";
					load();
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
			ImGui::Separator();
			if (ImGui::MenuItem("Close", NULL)) { flags.set(GlobalFlags::shutdown); }
			ImGui::EndMenu();
		}
		if (save_popup) {
			if (save()) {
				console.add_log("File saved successfully.");
			} else {
				console.add_log("Encountered an error saving file!");
			}
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

			static int width{1};
			static int height{1};
			static int room_id{0};
			static int metagrid_x{};
			static int metagrid_y{};

			width = std::clamp(width, 1, std::numeric_limits<int>::max());
			height = std::clamp(height, 1, std::numeric_limits<int>::max());

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

				static int style_current = static_cast<int>(map.styles.tile);
				static int bg_current = static_cast<int>(map.bg);

				map = Canvas({static_cast<uint32_t>(width * CHUNK_SIZE), static_cast<uint32_t>(height * CHUNK_SIZE)});
				map.styles.tile = static_cast<Style>(style_current);
				sprites.tileset.setTexture(tileset_textures.at(style_current));
				map.bg = static_cast<Backdrop>(bg_current);
				map.metagrid_coordinates = {metagrid_x, metagrid_y};
				finder->paths.room_name = buffer;
				map.room_id = room_id;
				save();
				load();
				map.center(window->f_center_screen());
				std::string message = "Created new room with id " + std::to_string(room_id) + " and name " + finder->paths.room_name;
				console.add_log(message.data());
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
				finder->paths.room_name = buffer;
				save();
				ImGui::CloseCurrentPopup();
			}

			ImGui::TextUnformatted(buffer);

			ImGui::EndPopup();
		}
		if (ImGui::BeginMenu("Edit")) {
			menu_hovered = true;
			if (ImGui::MenuItem("Undo", "CTRL+Z")) { map.undo(); }
			if (ImGui::MenuItem("Redo", "CTRL+SHIFT+Z")) { map.redo(); }
			ImGui::EndMenu();
		}
		bool flag{};
		if (ImGui::BeginMenu("Insert")) {
			menu_hovered = true;
			if (ImGui::MenuItem("Inspectable", NULL, &insp)) {}
			if (ImGui::MenuItem("Save Point")) {
				current_tool = std::move(std::make_unique<EntityPlacer>());
				current_tool.get()->ent_type = ENTITY_TYPE::SAVE_POINT;
			}
			if (ImGui::MenuItem("Player Placer")) {
				current_tool = std::move(std::make_unique<EntityPlacer>());
				current_tool.get()->ent_type = ENTITY_TYPE::PLAYER_PLACER;
			}
			ImGui::EndMenu();
		}
		

		if (ImGui::BeginMenu("Help")) {
			menu_hovered = true;
			ImGui::Text("Eyedropper Tool: Alt");
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	if (insp) { ImGui::OpenPopup("Inspectable Message"); }
	popup.launch("Inspectable Message", current_tool);
	

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

	ImVec2 window_pos;
	ImVec2 prev_window_pos{};
	ImVec2 prev_window_size{};
	window_pos.x = PAD;
	window_pos.y = work_pos.y + palette.dimensions.y * CELL_SIZE + 2 * PAD;
	window_flags |= ImGuiWindowFlags_NoMove;

	ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
	if (show_overlay) {
		ImGui::ShowDemoWindow();
		ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always);
		if (ImGui::Begin("Debug Mode", debug, window_flags)) {
			prev_window_size = ImGui::GetWindowSize();
			prev_window_pos = ImGui::GetWindowPos();
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
					ImGui::Text("Any Window Hovered: %s", window_hovered ? "Yes" : "No");
					ImGui::Text("Palette Mode: %s", palette_mode() ? "Yes" : "No");
					ImGui::Text("Room ID: %u", map.room_id);
					ImGui::Text("Camera Position: (%.1f,%.1f)", map.get_position().x, map.get_position().y);
					ImGui::Text("Active Layer: %i", active_layer);
					ImGui::Text("Num Layers: %lu", map.get_layers().layers.size());
					ImGui::Text("Stored Tile Value: %u", current_tool.get()->tile);
					if (current_tool.get()->in_bounds(map.dimensions)) {
						ImGui::Text("Tile Value at Mouse Pos: %u", map.tile_val_at(current_tool.get()->scaled_position.x, current_tool.get()->scaled_position.y, active_layer));
					} else {
						ImGui::Text("Tile Value at Mouse Pos: <invalid>");
					}
					ImGui::Separator();
					ImGui::Text("Current Style: %s", get_style_string.at(map.styles.tile));
					//ImGui::Text("Current Backdrop: %s", get_backdrop_string.at(map.bg));
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Tool")) {
					ImGui::Text("Tool Position: (%.1f,%.1f)", current_tool.get()->position.x, current_tool.get()->position.y);
					ImGui::Text("Clicked Position: (%.1f,%.1f)", current_tool.get()->clicked_position.x, current_tool.get()->clicked_position.y);
					ImGui::Text("Relative Position: (%.1f,%.1f)", current_tool.get()->relative_position.x, current_tool.get()->relative_position.y);
					ImGui::Text("Tool Position (scaled): (%i,%i)", current_tool.get()->scaled_position.x, current_tool.get()->scaled_position.y);
					ImGui::Text("Tool Clicked Position (scaled): (%i,%i)", current_tool.get()->scaled_clicked_position.x, current_tool.get()->scaled_clicked_position.y);
					ImGui::Text("Tool in Bounds: %s", current_tool.get()->in_bounds(map.dimensions) ? "Yes" : "No");
					ImGui::Text("Tool Ready: %s", current_tool.get()->ready ? "Yes" : "No");
					ImGui::Text("Tool Active: %s", current_tool.get()->active ? "Yes" : "No");
					ImGui::Text("Current Tool: %s", get_tool_string.at(current_tool->type).c_str());
					ImGui::Text("Secondary Tool: %s", get_tool_string.at(secondary_tool->type).c_str());
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Resources")) {
					ImGui::Text("Size of Canvas (Bytes): %lu", sizeof(map));
					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
			}

			prev_window_size = ImGui::GetWindowSize();
			ImGui::End();
		}
	}
	if (current_tool->type == ToolType::entity_placer) {
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
					map.entities.sprites.enemy_thumb.setTextureRect(sf::IntRect({{j * 16, i * 16}, {16, 16}}));
					map.entities.sprites.enemy_thumb.setScale({2.0f, 2.0f});
					if (ImGui::ImageButton(map.entities.sprites.enemy_thumb, 2)) {
						current_tool = std::move(std::make_unique<EntityPlacer>());
						current_tool.get()->ent_type = ENTITY_TYPE::CRITTER;
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
		window_pos = {window_pos.x, window_pos.y + prev_window_size.y + PAD};
		window_flags = ImGuiWindowFlags_NoCollapse;
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
						map.entities.sprites.large_animator.setTextureRect(sf::IntRect({{idx * 64, 0}, {64, 64}}));
						map.entities.sprites.large_animator.setScale({0.5f, 0.5f});
						if (ImGui::ImageButton(map.entities.sprites.large_animator, 2)) {
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
						map.entities.sprites.small_animator.setTextureRect(sf::IntRect({{idx * 32, 0}, {32, 32}}));
						if (ImGui::ImageButton(map.entities.sprites.small_animator, 2)) {
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

			ImGui::End();
		}
	}

	ImGui::SetNextWindowBgAlpha(0.65f); // Transparent background
	work_pos = viewport->WorkPos;		// Use work area to avoid menu-bar/task-bar, if any!
	work_size = viewport->WorkSize;
	window_pos.x = work_pos.x + work_size.x - PAD;
	window_pos.y = work_pos.y + PAD;
	window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, {1, 0});
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
		sprites.tileset.setTextureRect(sf::IntRect({palette.get_tile_coord(selected_block), {32, 32}}));
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
		ImGui::Text("Canvas Settings");
		ImGui::Separator();

		if(ImGui::BeginTabBar("##gensettings")) {
			if (ImGui::BeginTabItem("General")) {
				ImGui::Checkbox("Debug Overlay", &show_overlay);
				ImGui::Checkbox("Show Entities", &map.flags.show_entities);
				ImGui::Checkbox("Show Background", &map.flags.show_background);
				ImGui::Checkbox("Show Grid", &map.flags.show_grid);
				ImGui::Checkbox("Show Palette", &show_palette);
				ImGui::EndTabItem();
			}
			if(ImGui::BeginTabItem("Layer")) {
				ImGui::Checkbox("Show All Layers", &map.flags.show_all_layers);
				ImGui::Checkbox("Show Obscured Layer", &map.flags.show_obscured_layer);
				ImGui::Checkbox("Show Indicated Layers", &map.flags.show_indicated_layers);
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Backdrop")) {
				static ImVec4 color{};
				ImGui::ColorEdit3("MyColor##1", (float*)&color);
				colors.backdrop.r = static_cast<uint8_t>(color.x * 255);
				colors.backdrop.g = static_cast<uint8_t>(color.y * 255);
				colors.backdrop.b = static_cast<uint8_t>(color.z * 255);
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}

		ImGui::Separator();
		ImGui::Text("Actions");
		ImGui::Separator();
		if (ImGui::Button("Center Canvas")) { map.center(window->f_center_screen()); }
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.5, 0.6f, 0.5f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.5, 0.7f, 0.6f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.5, 0.8f, 0.7f));
		if (ImGui::Button("Launch Demo")) {
			trigger_demo = true;
			save();
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
		int style_current = static_cast<int>(map.styles.tile);
		if (ImGui::Combo("##scenestyle", &style_current, styles, IM_ARRAYSIZE(styles))) {
			map.styles.tile = static_cast<Style>(style_current);
			sprites.tileset.setTexture(tileset_textures.at(style_current));
		}
		ImGui::Text("Scene Background: ");
		int bg_current = static_cast<int>(map.bg);
		if (ImGui::Combo("##scenebg", &bg_current, bgs, IM_ARRAYSIZE(bgs))) { map.bg = static_cast<Backdrop>(bg_current); }
		if (ImGui::Button("Clear Layer")) {
			map.save_state(*current_tool, true);
			map.get_layers().layers.at(active_layer).clear();
		}
		if (ImGui::Button("Clear All Layers")) {
			map.save_state(*current_tool, true);
			for (auto& layer : map.get_layers().layers) { layer.clear(); }
		}
		if (ImGui::Button("Clear Entire Canvas")) {
			map.save_state(*current_tool, true);
			for (auto& layer : map.get_layers().layers) { layer.clear(); }
			map.entities.clear();
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
		for (auto& cell : map.get_layers().layers.at(i).grid.cells) {
			if (cell.value > 0) {
				auto x_coord = (cell.value % 16) * TILE_WIDTH;
				auto y_coord = std::floor(cell.value / 16) * TILE_WIDTH;
				tile_sprite.setTexture(tileset_textures.at(static_cast<int>(map.styles.tile)));
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

void Editor::launch_demo(char** argv, int room_id, std::filesystem::path path, sf::Vector2<float> player_position) {
	trigger_demo = false;
	ImGui::SFML::Shutdown();
	fornani::Application demo{argv};
	std::cout << "Editor path: " << path.string() << "\n";
	std::cout << "AAA room id: " << room_id << std::endl;
	std::cout << "\n\n" << finder->paths.room_name << "\n\n";
	demo.launch(argv, true, room_id, finder->paths.room_name, player_position);
}

} // namespace pi
