//
//  Canvas.cpp
//  Pioneer-Lab
//
//  Created by Alex Frasca on 9/30/20.
//

#include "Canvas.hpp"
#include "../util/ServiceLocator.hpp"
#include "../util/Lookup.hpp"

namespace canvas {

Canvas::Canvas() {
}

Canvas::Canvas(sf::Vector2<uint32_t> dim) {
    dimensions = dim;
    real_dimensions = {(float)dim.x * CELL_SIZE, (float)dim.y * CELL_SIZE};
    chunk_dimensions.x = dim.x / CHUNK_SIZE;
    chunk_dimensions.y = dim.y / CHUNK_SIZE;
    clear();
    map_states.push_back(Map());
    for(int i = 0; i < NUM_LAYERS; ++i) {
        map_states.back().layers.push_back(Layer( i, (i == MIDDLEGROUND), dim ));
        map_states.back().layers.back().grid.initialize();
    }
}

void Canvas::load(const std::string& path) {

    map_states.clear();
    redo_states.clear();

    //init map_states
    map_states.push_back(Map());
    
    clear();
    
    std::string filepath = path + "/map_data.txt";
    
    int value{};
    int counter = 0;
    std::ifstream input{};
    input.open(filepath);
    if (!input.is_open()) {
        printf("Failed to open file.\n");
        return;
    }
    
    //dimensions and layers
    input >> value; room_id = value; input.ignore();
    input >> value; dimensions.x = value; input.ignore();
    input >> value; dimensions.y = value; input.ignore();
    input >> value; chunk_dimensions.x = value; input.ignore();
    input >> value; chunk_dimensions.y = value; input.ignore();
    if((dimensions.x / chunk_dimensions.x != CHUNK_SIZE) ||
       (dimensions.y / chunk_dimensions.y != CHUNK_SIZE)) { printf("File is corrupted: Invalid dimensions.\n"); return; }
    real_dimensions = {(float)dimensions.x * CELL_SIZE, (float)dimensions.y * CELL_SIZE};
    for(int i = 0; i < NUM_LAYERS; ++i) {
        map_states.back().layers.push_back(Layer( i, (i == MIDDLEGROUND), dimensions ));
        map_states.back().layers.back().grid.initialize();
    }
    //style
    input >> value; input.ignore();
    if(value >= pi::lookup::get_style.size()) { printf("File is corrupted: Invalid style.\n"); return; } else {
        style = pi::lookup::get_style.at(value);
    }
    //bg;
    input >> value; input.ignore();
    if(value >= pi::lookup::get_backdrop.size()) { printf("File is corrupted: Invalid backdrop.\n"); return; } else {
        bg = pi::lookup::get_backdrop.at(value);
    }
    input.close();
    
    //get map tiles from text files
    for(auto& layer : map_states.back().layers) {
        input.open(path + "/map_tiles_" + std::to_string(counter) + ".txt");
        for(auto& cell : layer.grid.cells) {
            input >> value;
            canvas::TILE_TYPE typ = lookup_type(value);
            cell.value = value;
            cell.type = typ;
            
            input.ignore(); //ignore the delimiter
        }
        layer.grid.update();
        //close the current file
        input.close();
        ++counter;
    }
    pi::svc::active_layer = MIDDLEGROUND;

    //get portal data
    input.open(path + "/map_portals.txt");
    if (input.is_open()) {
        while(!input.eof()) {
            Portal p{};
            input >> p.dimensions.x; input.ignore();
            input >> p.dimensions.y; input.ignore();
            input >> value; p.activate_on_contact = (bool)value; input.ignore();
            input >> p.source_map_id; input.ignore();
            input >> p.destination_map_id; input.ignore();
            input >> p.position.x; input.ignore();
            input >> p.position.y; input.ignore();
            if (p.dimensions.x != 0) { //only push if one was read, otherwise we reached the end of the file
                portals.push_back(p);
            }
        }
        input.close();
    }

    //get inspectable data
    input.open(path + "/map_inspectables.txt");
    if (input.is_open()) {
        while (!input.eof()) {
            Inspectable p{};
            input >> p.dimensions.x; input.ignore();
            input >> p.dimensions.y; input.ignore();
            input >> value; p.activate_on_contact = (bool)value; input.ignore(); input.ignore();
            std::getline(input, p.message, '#'); input.ignore();
            input >> p.position.x; input.ignore();
            input >> p.position.y; input.ignore();
            if (p.dimensions.x != 0) { //only push if one was read, otherwise we reached the end of the file
                inspectables.push_back(p);
            }
        }
        input.close();
    }

    //get critter data
    input.open(path + "/map_critters.txt");
    if (input.is_open()) {
        while (!input.eof()) {
            Critter c{};
            input >> value; if (value >= 0 && value < num_critter_types) { c.type = pi::lookup::get_critter_type.at(value); c.id = value; };
            input.ignore();
            input >> c.position.x; input.ignore();
            input >> c.position.y; input.ignore();
            if (c.id >= 0 && c.id < num_critter_types && c.position != sf::Vector2<unsigned int>{0, 0}) { //only push if one was read, otherwise we reached the end of the file
                critters.push_back(c);
            }
        }
        input.close();
    }

    //get animator data
    input.open(path + "/map_animators.txt");
    if (input.is_open()) {
        while (!input.eof()) {
            Animator p{};
            input >> p.dimensions.x; input.ignore();
            input >> p.dimensions.y; input.ignore();
            input >> value; p.id = value; input.ignore();
            input >> value; p.automatic = (bool)value; input.ignore();
            input >> value; p.foreground = (bool)value; input.ignore();
            input >> p.position.x; input.ignore();
            input >> p.position.y; input.ignore();
            if (p.dimensions.x != 0) { //only push if one was read, otherwise we reached the end of the file
                animators.push_back(p);
            }
        }
        input.close();
    }

    //get special block data
    input.open(path + "/map_special_blocks.txt");
    if (input.is_open()) {
        while (!input.eof()) {
            SpecialBlock p{};
            input >> p.dimensions.x; input.ignore();
            input >> p.dimensions.y; input.ignore();
            input >> value; p.type = value; input.ignore(); input.ignore();
            input >> p.id; input.ignore();
            input >> p.position.x; input.ignore();
            input >> p.position.y; input.ignore();
            if (p.dimensions.x != 0) { //only push if one was read, otherwise we reached the end of the file
                special_blocks.push_back(p);
            }
        }
        input.close();
    }
    
}

bool Canvas::save(const std::string& path) {
    
    std::string filepath = path + "/map_data.txt";
    
    int value{};
    int counter = 0;
    std::filesystem::create_directory(path);
    std::ofstream output(filepath);
    if (!output.is_open()) {
        return false;
    }
    output << room_id << ", " << dimensions.x << ", " << dimensions.y << ", "
    << chunk_dimensions.x << ", " << chunk_dimensions.y << ", "
    << pi::lookup::get_style_id.at(style) << ", " << pi::lookup::get_backdrop_id.at(bg);
    output.close();
    
    for(auto& layer : map_states.back().layers) {
        int cell_ctr{0};
        std::ofstream tile_output(path + "/map_tiles_" + std::to_string(counter) + ".txt");
        for(auto& cell : layer.grid.cells) {
            tile_output << std::to_string(layer.grid.cells.at(cell_ctr).value);
            
            tile_output << ",";
            
            ++cell_ctr;
        }
        layer.grid.update();
        //close the current file
        tile_output.close();
        ++counter;
    }
    output.open(path + "/map_portals.txt");
    if (output.is_open()) {
        for (auto& portal : portals) {
            int aoc = portal.activate_on_contact ? 1 : 0;
            output << portal.dimensions.x << "," << portal.dimensions.y << "," << aoc << "," << portal.source_map_id << "," << portal.destination_map_id << "," << portal.position.x << "," << portal.position.y << "#\n";
        }
    }
    output.close();
    output.open(path + "/map_inspectables.txt");
    if (output.is_open()) {
        for (auto& inspectable : inspectables) {
            int aoc = inspectable.activate_on_contact ? 1 : 0;
            output << inspectable.dimensions.x << "," << inspectable.dimensions.y << "," << aoc << "#\n" << inspectable.message << "#\n" << inspectable.position.x << "," << inspectable.position.y << "#\n";
        }
    }
    output.close();
    output.open(path + "/map_animators.txt");
    if (output.is_open()) {
        for (auto& animator : animators) {
            int automatic = animator.automatic ? 1 : 0;
            int foreground = animator.foreground ? 1 : 0;
            output << animator.dimensions.x << "," << animator.dimensions.y << "," << animator.id << "," << automatic << "," << foreground << "," << animator.position.x << "," << animator.position.y << "#\n";
        }
    }
    output.close();
    output.open(path + "/map_critters.txt");
    if (output.is_open()) {
        for (auto& critter : critters) {
            output << critter.id << "," << critter.position.x << "," << critter.position.y << "#\n";
        }
    }
    output.close();
    output.open(path + "/map_special_blocks.txt");
    if (output.is_open()) {
        for (auto& block : special_blocks) {
            int type = block.type;
            int id = block.id;
            output << block.dimensions.x << "," << block.dimensions.y << "," << type << "," << id << "," << block.position.x << "," << block.position.y << "#\n";
        }
    }
    output.close();
    return true;
}

void Canvas::clear() {
    if (!map_states.empty()) {
        for (auto& layer : map_states.back().layers) {
            layer.erase();
        }
        map_states.back().layers.clear();
        portals.clear();
        inspectables.clear();
        critters.clear();
        animators.clear();
    }
}

void Canvas::save_state() {
    auto undoable_tool = pi::svc::current_tool.get()->type == tool::TOOL_TYPE::BRUSH || pi::svc::current_tool.get()->type == tool::TOOL_TYPE::FILL;
    auto just_clicked = !pi::svc::current_tool.get()->active && pi::svc::current_tool.get()->ready;
    if (undoable_tool && just_clicked) {
        map_states.emplace_back(canvas::Map(map_states.back()));
        clear_redo_states();
    }
}

void Canvas::undo() {
    if (map_states.size() > 1) {
        redo_states.push_back(map_states.back());
        map_states.pop_back();
    }
}

void Canvas::redo() {
    if (redo_states.size() > 0) {
        map_states.push_back(redo_states.back());
        redo_states.pop_back();
    }
}

void Canvas::clear_redo_states() {
    redo_states.clear();
}

void Canvas::update_dimensions() {
    if(dimensions.x / chunk_dimensions.x == CHUNK_SIZE && dimensions.y / chunk_dimensions.y == CHUNK_SIZE) {
        return;
    }
    if(dimensions.x / chunk_dimensions.x != CHUNK_SIZE) {
        dimensions.x = chunk_dimensions.x * CHUNK_SIZE;
        real_dimensions.x = dimensions.x * CELL_SIZE;
        for(auto& layer : map_states.back().layers) {
            layer.erase();
            layer.grid.initialize();
            layer.grid.update();
        }
    }
    if(dimensions.y / chunk_dimensions.y != CHUNK_SIZE) {
        dimensions.y = chunk_dimensions.y * CHUNK_SIZE;
        real_dimensions.y = dimensions.y * CELL_SIZE;
        for(auto& layer : map_states.back().layers) {
            layer.erase();
            layer.grid.initialize();
            layer.grid.update();
        }
    }
}

void Canvas::edit_tile_at(int i, int j, int new_val, int layer_index) {
    if(layer_index >= map_states.back().layers.size()) { return; }
    if((i + j * dimensions.x) >= map_states.back().layers.at(layer_index).grid.cells.size()) { return; };
    map_states.back().layers.at(layer_index).grid.cells.at(i + j * dimensions.x).value = new_val;
}

int Canvas::tile_val_at(int i, int j, int layer) {
    return map_states.back().layers.at(layer).grid.cells.at(i + j * dimensions.x).value;
}

TILE_TYPE Canvas::lookup_type(int idx) {
    if(idx < 1) {
        return TILE_NULL;
    }
    if(idx < 192) {
        return TILE_BASIC;
    }
    if(idx <= 223) {
        return TILE_RAMP;
    }
    if(idx <= 227) {
        return TILE_LAVA;
    }
    if(idx <= 231) {
        return TILE_CURRENT;
    }
    if(idx <= 235) {
        return TILE_FLAMMABLE;
    }
    if(idx <= 239) {
        return TILE_PLATFORM;
    }
    if(idx <= 243) {
        return TILE_WATER;
    }
    if(idx <= 247) {
        return TILE_BREAKABLE;
    }
    if(idx <= 251) {
        return TILE_LADDER;
    }
    if(idx <= 255) {
        return TILE_SPIKES;
    }
    return TILE_NULL;
}


}
