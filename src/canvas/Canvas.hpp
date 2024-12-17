
#pragma once

#include <SFML/Graphics.hpp>
#include <stdio.h>
#include <string>
#include "Layer.hpp"
#include <fstream>
#include <vector>
#include <cstdio>
#include <filesystem>
#include <system_error>
#include <iostream>
#include <djson/json.hpp>
#include "../util/BitFlags.hpp"
#include "../util/Camera.hpp"
#include "EntitySet.hpp"
#include "Background.hpp"

namespace pi {

const int NUM_LAYERS{8};
const int CHUNK_SIZE{16};

enum class Backdrop {
    BG_DUSK,
    BG_SUNRISE,
    BG_OPEN_SKY,
    BG_ROSY_HAZE,
    BG_NIGHT,
    BG_DAWN,
    BG_OVERCAST,
    
    BG_SLIME,
    BG_BLACK,
    BG_NAVY,
    BG_DIRT,
    BG_GEAR,
    BG_LIBRARY,
    BG_GRANITE,
    BG_RUINS,
    BG_CREVASSE,
    BG_DEEP,
    BG_GROVE,

    END
};

enum LAYER_ORDER {
    BACKGROUND = 0,
    MIDDLEGROUND = 4,
    FOREGROUND = 7,
};

enum class Style { firstwind, overturned, base, factory, greatwing, provisional, END };

const int CELL_SIZE{32};
float const f_cell_size{32.f};
enum class CanvasProperties { editable };
enum class CanvasState { hovered };

struct Map {
    std::vector<Layer> layers{};
};

class Tool;
class ResourceFinder;

class Canvas {
    
public:
    
    Canvas(bool editable = true);
    Canvas(sf::Vector2<uint32_t> dim);
	void update(Tool& tool, bool transformed = false);
	void render(sf::RenderWindow& win, sf::Sprite& tileset);
	void load(ResourceFinder& finder, std::string const& room_name, bool local = false);
	bool save(ResourceFinder& finder, std::string const& room_name);
    void clear();
	void save_state(Tool& tool, bool force = false);
    void undo();
    void redo();
    void clear_redo_states();
	void unhover();
	void move(sf::Vector2<float> distance);
	void set_position(sf::Vector2<float> to_position);
	void center(sf::Vector2<float> point);
	Map& get_layers();
	sf::Vector2<int> get_tile_coord(int lookup);
	[[nodiscard]] auto states_empty() const -> bool { return map_states.empty(); }
	[[nodiscard]] auto hovered() const -> bool { return state.test(CanvasState::hovered); }
	[[nodiscard]] auto editable() const -> bool { return properties.test(CanvasProperties::editable); }
	[[nodiscard]] auto get_position() const -> sf::Vector2<float> { return camera.position; }
	[[nodiscard]] auto within_bounds(sf::Vector2<float> const& point) const -> bool { return point.x > camera.position.x && point.x < real_dimensions.x + camera.position.x && point.y > camera.position.y && point.y < real_dimensions.y + camera.position.y; }

    void edit_tile_at(int i, int j, int new_val, int layer_index);
	void erase_at(int i, int j, int layer_index);
	int tile_val_at(int i, int j, int layer);
	int tile_val_at_scaled(int i, int j, int layer);
	sf::Vector2<float> get_tile_position_at(int i, int j, int layer = 4);
	Tile& get_tile_at(int i, int j, int layer = 4);
    TILE_TYPE lookup_type(int idx);
    
    //layers
    sf::Vector2<float> real_dimensions{};
    sf::Vector2<uint32_t> dimensions{};
    sf::Vector2<uint32_t> chunk_dimensions{};
	sf::Vector2<int> metagrid_coordinates{};

    struct {
		bool show_grid{true};
		bool show_all_layers{true};
		bool show_obscured_layer{true};
		bool show_indicated_layers{true};
		bool show_entities{true};
		bool show_background{};
	} flags{};

	EntitySet entities;

    // read and write
    struct {
        dj::Json meta{};
        dj::Json tiles{};
    } data{};

    struct {
        float cell_size{ 32.f };
    } constants{};

    struct {
		Style tile{};
		int breakable{};
	} styles{};

    struct {
		bool flag{};
		int type{};
		int id{};
		int source{};
    } cutscene{};

	sf::Vector2u player_start{};
    int active_layer{};
    
    Backdrop bg{};
    
    uint32_t room_id{}; // should be assigned to its constituent chunks
  private:
	std::vector<Map> map_states{};
	std::vector<Map> redo_states{};
	util::BitFlags<CanvasState> state{};
	util::BitFlags<CanvasProperties> properties{};
	Camera camera{};
	sf::RectangleShape box{};
	sf::RectangleShape gridbox{};
	std::unique_ptr<Background> background{};
};

}
