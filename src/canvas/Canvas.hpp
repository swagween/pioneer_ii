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

namespace canvas {

const int NUM_LAYERS{8};
const int CHUNK_SIZE{16};
const int num_critter_types{ 17 };

enum class BACKDROP {
    BG_NULL,
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


struct Portal {
    sf::Vector2<uint32_t> dimensions{};
    bool activate_on_contact{};
    int source_map_id{};
    int destination_map_id{};

    sf::Vector2<uint32_t> position{};
};

struct Inspectable {
    sf::Vector2<uint32_t> dimensions{};
    bool activate_on_contact{};
    std::string message{};

    sf::Vector2<uint32_t> position{};
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
    sf::Vector2<uint32_t> position{};
};

struct SpecialBlock {
    sf::Vector2<uint32_t> dimensions{};
    int type{};
    int id{};
    sf::Vector2<uint32_t> position{};
};

struct Map {
    std::vector<Layer> layers{};
};

class Canvas {
    
public:
    
    Canvas();
    Canvas(sf::Vector2<uint32_t> dim);
    void load(const std::string& path);
    bool save(const std::string& path);
    void clear();
    void save_state();
    void undo();
    void redo();
    void clear_redo_states();

    void update_dimensions();
    void edit_tile_at(int i, int j, int new_val, int layer_index);
    int tile_val_at(int i, int j, int layer);
    void render(sf::RenderWindow& win, std::vector<sf::Sprite>& tileset, sf::Vector2<float> cam);
    canvas::Tile& tile_at(const uint8_t i, const uint8_t j);
    TILE_TYPE lookup_type(int idx);
    
    //layers
    std::vector<Map> map_states{};
    std::vector<Map> redo_states{};
    sf::Vector2<float> real_dimensions{};
    sf::Vector2<uint32_t> dimensions{};
    sf::Vector2<uint32_t> chunk_dimensions{};

    //entities
    std::vector<Portal> portals{};
    std::vector<Inspectable> inspectables{};
    std::vector<Animator> animators{};
    std::vector<Critter> critters{};
    std::vector<SpecialBlock> special_blocks{};

    dj::Json metadata{};
    dj::Json tiles{};
    
    STYLE style{};
    BACKDROP bg{};
    
    uint32_t room_id{}; // should be assigned to its constituent chunks
    
};

}
