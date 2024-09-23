//
//  GameState.hpp
//  automa
//
//  Created by Alex Frasca on 12/26/22.
//

#pragma once

#include <SFML/Graphics.hpp>
#include <cstdio>
#include <memory>
#include <chrono>
#include <imgui.h>
#include "../canvas/Canvas.hpp"
#include "../util/Camera.hpp"
#include "../tool/Tool.hpp"
#include "../setup/WindowManager.hpp"
#include <SFML/OpenGL.hpp>
#include <imgui-SFML.h>
#include <sstream>
#include <windows.h>
#include <string_view>

namespace pi {

class ResourceFinder;

inline char const* styles[NUM_STYLES];
inline char const* bgs[NUM_BGS];
inline std::string_view const& style_list{};
inline std::string styles_str{};

class Editor {
  public:
	int const TILE_WIDTH{32};
	int const NUM_TOOLS{6};
	Editor(char** argv, WindowManager& window, ResourceFinder& finder);
	void run();
	void init(std::string const& load_path);
	void setTilesetTexture(sf::Texture& new_tex);
	void handle_events(sf::Event& event, sf::RenderWindow& win);
	void logic();
	void render(sf::RenderWindow& win);
	void gui_render(sf::RenderWindow& win);
	void help_marker(char const* desc);
	void export_layer_texture();
	void launch_demo(char** argv, int room_id, std::filesystem::path path, sf::Vector2<float> player_position);
	sf::Vector2<int> get_tile_coord(int lookup);

	Canvas map{};
	std::vector<sf::Texture> tileset_textures{};
	sf::Texture tool_texture{};
	sf::Sprite tool_sprites{};
	sf::Texture large_animator_textures{};
	sf::Texture large_animator_thumbs{};
	sf::Texture small_animator_textures{};
	sf::Texture small_animator_thumbs{};
	sf::Texture enemy_thumbnails{};

	struct {
		sf::Sprite tool{};
		sf::Sprite tileset{};
		sf::Sprite large_animator{};
		sf::Sprite small_animator{};
		sf::Sprite large_animator_thumb{};
		sf::Sprite small_animator_thumb{};
		sf::Sprite enemy_thumb{};
		sf::Sprite current_enemy{};
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
	bool show_grid{true};
	bool show_all_layers{true};
	bool show_overlay{false};
	bool demo_mode{};

	sf::RectangleShape box{};
	sf::RectangleShape player_box{};
	sf::RectangleShape gridbox{};
	sf::RectangleShape portalbox{};
	sf::RectangleShape inspbox{};
	sf::RectangleShape platbox{};
	sf::RectangleShape chestbox{};
	sf::RectangleShape savebox{};
	sf::RectangleShape platextent{};
	sf::RectangleShape target{};
	sf::RectangleShape backdrop{};

	int large_index_multiplier{100};
	int small_index_multiplier{200};

	std::string filepath{};
	std::string demopath{};
	std::string folderpath{};
	std::string room{};
	int room_id{};

	bool trigger_demo{false};
	bool window_hovered{};
	int active_layer{4};
	int selected_block{};
	sf::Vector2<uint32_t> player_start{};

  private:
	WindowManager* window;
	ResourceFinder* finder;
	Camera camera = Camera();
	std::unique_ptr<Tool> current_tool = std::make_unique<Hand>();
	std::unique_ptr<Tool> secondary_tool = std::make_unique<Hand>();
	char** args{};
};

} // namespace pi
