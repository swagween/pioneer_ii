//
//  Canvas.hpp
//  Pioneer-Lab
//
//  Created by Alex Frasca on 9/30/20.
//

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

namespace pi {

const int NUM_LAYERS{8};
const int CHUNK_SIZE{16};
const int num_critter_types{ 17 };

enum class BACKDROP {
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
    BG_GROVE
};

enum LAYER_ORDER {
    BACKGROUND = 0,
    MIDDLEGROUND = 4,
    FOREGROUND = 7,
};

enum class STYLE {
    FIRSTWIND,  // in progress
    OVERTURNED, // in progress
    GRUB,       // in progress
    TOXIC,      // done
    BASE,       // in progress
    FROZEN,     // done
    NIGHT,      // done
    WORM,       // not started
    SKY,        //
    ASH,
    GREATWING,
    ICE,
    SNOW,
    STONE,
    ABANDONED,
    ANCIENT,
    FACTORY,
    SHADOW,
    HOARDER,
    MANSION
};

enum class CRITTER_TYPE {
    hulmet,
    tank,
    bunker,
    thug,
    worker,
    frdog,
    jackdaw,
    frcat,
    biter,
    bee,
    roller,
    snek,
    behemoth,
    stinger,
    watchman,
    gnat,
    moth
};

const int NUM_STYLES{20};
const int NUM_BGS{18};
const int CELL_SIZE{32};
float const f_cell_size{32.f};


struct Portal {
    sf::Vector2<uint32_t> dimensions{};
    bool activate_on_contact{};
	bool already_open{};
    int source_map_id{};
    int destination_map_id{};
    bool locked{};
	int key_id{};
    sf::Vector2<uint32_t> position{};
};

struct Inspectable {
    sf::Vector2<uint32_t> dimensions{};
    bool activate_on_contact{};
    std::string key{};
	std::vector<std::vector<std::string>> suites{};
	std::vector<std::vector<std::string>> responses{};

    sf::Vector2<uint32_t> position{};
	int alternates{};
};

struct Critter {
    CRITTER_TYPE type{};
    int id{};
    sf::Vector2<uint32_t> position{};
};

struct Animator {
    sf::Vector2<uint32_t> dimensions{};
    int id{};
    bool automatic{};
    bool foreground{};
	int style{};
	sf::Vector2<uint32_t> position{};
};

struct Bed {
	sf::Vector2<uint32_t> position{};
};

struct Platform {
    sf::Vector2<uint32_t> position{};
    sf::Vector2<uint32_t> dimensions{};
    int extent{};
    int style{};
    std::string type{};
    float start{};
};

struct SpecialBlock {
    sf::Vector2<uint32_t> dimensions{};
    int type{};
    int id{};
    sf::Vector2<uint32_t> position{};
};

struct NPC {
    int id{};
	bool background{};
    sf::Vector2<uint32_t> position{};
	std::vector<std::vector<std::string>> suites{};
};

struct Chest {
    int id{};
    int item_id{};
	int type{};
	float rarity{};
	int amount{};
    sf::Vector2<uint32_t> position{};
};

struct SwitchBlock {
	int id{};
	int type{};
	sf::Vector2<uint32_t> position{};
};

struct Destroyer {
	int id{};
	sf::Vector2<uint32_t> position{};
};

struct SwitchButton {
	int id{};
	int type{};
	sf::Vector2<uint32_t> position{};
};

struct SavePoint {
	bool placed{};
	sf::Vector2<uint32_t> position{};
};

struct Map {
    std::vector<Layer> layers{};
};

class Tool;

class Canvas {
    
public:
    
    Canvas();
    Canvas(sf::Vector2<uint32_t> dim);
    void load(const std::string& path);
    bool save(const std::string& path);
    void clear();
	void save_state(Tool& tool);
    void undo();
    void redo();
    void clear_redo_states();
	bool has_switch_block_at(sf::Vector2<uint32_t> pos) const;

    void edit_tile_at(int i, int j, int new_val, int layer_index);
    int tile_val_at(int i, int j, int layer);
    TILE_TYPE lookup_type(int idx);
    
    //layers
    std::vector<Map> map_states{};
    std::vector<Map> redo_states{};
    sf::Vector2<float> real_dimensions{};
    sf::Vector2<uint32_t> dimensions{};
    sf::Vector2<uint32_t> chunk_dimensions{};
	sf::Vector2<int> metagrid_coordinates{};

    //entities
    std::vector<Portal> portals{};
    std::vector<Inspectable> inspectables{};
    std::vector<Animator> animators{};
    std::vector<Critter> critters{};
    std::vector<NPC> npcs{};
    std::vector<Chest> chests{};
    std::vector<SpecialBlock> special_blocks{};
	std::vector<Platform> platforms{};
	std::vector<SwitchBlock> switch_blocks{};
	std::vector<SwitchButton> switch_buttons{};
	std::vector<Bed> beds{};
	std::vector<Destroyer> destroyers{};
	SavePoint save_point{};
	std::string music{};

    // read and write
    struct {
        dj::Json meta{};
        dj::Json tiles{};
        dj::Json inspectables{};
    } data{};

    struct {
        float cell_size{ 32.f };
    } constants{};

    struct {
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
    
    STYLE style{};
    BACKDROP bg{};
    
    uint32_t room_id{}; // should be assigned to its constituent chunks
    
};

}
