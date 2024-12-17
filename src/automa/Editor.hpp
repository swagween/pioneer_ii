
#pragma once

#include <SFML/Graphics.hpp>
#include <cstdio>
#include <memory>
#include <chrono>
#include <imgui.h>
#include "../canvas/Canvas.hpp"
#include "../util/BitFlags.hpp"
#include "../tool/Tool.hpp"
#include "../setup/WindowManager.hpp"
#include "../gui/Console.hpp"
#include <SFML/OpenGL.hpp>
#include <imgui-SFML.h>
#include <sstream>
#include <windows.h>
#include <string_view>
#include <filesystem>
#include "PopupHandler.hpp"

namespace pi {

class ResourceFinder;

enum class GlobalFlags { shutdown, palette_mode };
enum class PressedKeys { control, shift, mouse };

inline char const* styles[static_cast<size_t>(Style::END)];
inline char const* bgs[static_cast<size_t>(Backdrop::END)];
inline std::string_view const& style_list{};
inline std::string styles_str{};

class Editor {
  public:
	int const TILE_WIDTH{32};
	int const NUM_TOOLS{6};
	Editor(char** argv, WindowManager& window, ResourceFinder& finder);
	void run();
	void init(std::string const& load_path);
	void handle_events(sf::Event& event, sf::RenderWindow& win);
	void logic();
	void load();
	bool save();
	void render(sf::RenderWindow& win);
	void gui_render(sf::RenderWindow& win);
	void help_marker(char const* desc);
	void export_layer_texture();
	void launch_demo(char** argv, int room_id, std::filesystem::path path, sf::Vector2<float> player_position);
	[[nodiscard]] auto control_pressed() const -> bool { return pressed_keys.test(PressedKeys::control); }
	[[nodiscard]] auto shift_pressed() const -> bool { return pressed_keys.test(PressedKeys::shift); }
	[[nodiscard]] auto palette_mode() const -> bool { return flags.test(GlobalFlags::palette_mode); }

	Canvas map{};
	Canvas palette{false};

	std::vector<sf::Texture> tileset_textures{};
	sf::Texture tool_texture{};
	sf::Sprite tool_sprites{};
	sf::RectangleShape backdrop{};
	sf::RectangleShape target{};
	sf::RectangleShape selector{};

	struct {
		sf::Sprite tool{};
		sf::Sprite tileset{};
	} sprites{};

	sf::Texture t_chest{};
	sf::Sprite s_chest{};
	sf::Texture t_npc{};
	sf::Sprite s_npc{};

	// for loading out layer pngs
	sf::RenderTexture screencap{};
	sf::Sprite tile_sprite{};

	sf::Vector2<float> mouse_clicked_position{};

	bool mouse_held{};
	bool show_palette{true};
	bool show_overlay{};
	bool demo_mode{};

	int large_index_multiplier{100};
	int small_index_multiplier{200};

	bool trigger_demo{false};
	bool window_hovered{};
	bool menu_hovered{};
	int active_layer{4};
	int selected_block{};

  private:
	WindowManager* window;
	ResourceFinder* finder;
	PopupHandler popup{};
	std::unique_ptr<Tool> current_tool = std::make_unique<Hand>();
	std::unique_ptr<Tool> secondary_tool = std::make_unique<Hand>();
	util::BitFlags<PressedKeys> pressed_keys{};
	util::BitFlags<GlobalFlags> flags{};
	char** args{};
	Console console{};
	struct {
		sf::Color backdrop{};
	} colors{};
};

} // namespace pi
