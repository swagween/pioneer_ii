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

    /* begin overhaul */
    
    std::string filepath = path + "/meta.json";

    data.meta = dj::Json::from_file((path + "/meta.json").c_str());
    assert(!data.meta.is_null());
    data.tiles = dj::Json::from_file((path + "/tile.json").c_str());
    assert(!data.tiles.is_null());
    data.inspectables = dj::Json::from_file((path + "/inspectables.json").c_str());

    // get npc data
    if (!data.meta.is_null()) {
        auto const& meta = data.meta["meta"];
        room_id = meta["id"].as<int>();
        dimensions.x = meta["dimensions"][0].as<int>();
        dimensions.y = meta["dimensions"][1].as<int>();
        chunk_dimensions.x = meta["chunk_dimensions"][0].as<int>();
        chunk_dimensions.y = meta["chunk_dimensions"][1].as<int>();
        real_dimensions = { (float)dimensions.x * constants.cell_size, (float)dimensions.y * constants.cell_size };
        for (int i = 0; i < NUM_LAYERS; ++i) { map_states.back().layers.push_back(Layer(i, (i == MIDDLEGROUND), dimensions)); }

        auto style_value = meta["style"].as<int>();
        style = pi::lookup::get_style.at(meta["style"].as<int>());
        bg = pi::lookup::get_backdrop.at(meta["background"].as<int>());

        for (auto& entry : data.meta["portals"].array_view()) {
            Portal p{};
            p.position.x = entry["position"][0].as<int>();
            p.position.y = entry["position"][1].as<int>();
            p.dimensions.x = entry["dimensions"][0].as<int>();
            p.dimensions.y = entry["dimensions"][1].as<int>();
            p.source_map_id = entry["source_id"].as<int>();
            p.destination_map_id = entry["destination_id"].as<int>();
            p.activate_on_contact = (bool)entry["activate_on_contact"].as_bool();
            portals.push_back(p);
        }

        /*auto const& savept = data.meta["save_point"];
        save_point.id = savept["id"].as<int>();
        save_point.scaled_position.x = savept["position"][0].as<int>();
        save_point.scaled_position.y = savept["position"][1].as<int>();*/

        for (auto& entry : data.meta["npcs"].array_view()) {
            NPC n{};
            n.position.x = entry["position"][0].as<int>();
            n.position.y = entry["position"][1].as<int>();
            n.id = entry["id"][0].as<int>();
            npcs.push_back(n);
        }

        for (auto& entry : data.meta["chests"].array_view()) {
            Chest c{};
            c.position.x = entry["position"][0].as<int>();
            c.position.y = entry["position"][1].as<int>();
            c.id = entry["id"][0].as<int>();
            c.item_id = entry["item_id"][0].as<int>();
            chests.push_back(c);
        }

        for (auto& entry : data.meta["animators"].array_view()) {
            Animator a{};
            a.position.x = entry["position"][0].as<int>();
            a.position.y = entry["position"][1].as<int>();
            a.dimensions.x = entry["dimensions"][0].as<int>();
            a.dimensions.y = entry["dimensions"][1].as<int>();
            a.id = entry["id"].as<int>();
            a.automatic = (bool)entry["automatic"].as_bool();
            a.foreground = (bool)entry["foreground"].as_bool();
            animators.push_back(a);
        }

        for (auto& entry : data.meta["inspectables"].array_view()) {
            Inspectable i{};
            i.message = entry["key"].as_string();
            i.position.x = entry["position"][0].as<int>();
            i.position.y = entry["position"][1].as<int>();
            i.dimensions.x = entry["dimensions"][0].as<int>();
            i.dimensions.y = entry["dimensions"][1].as<int>();
            inspectables.push_back(i);
            inspectables.back().activate_on_contact = (bool)entry["activate_on_contact"].as_bool();
        }

        for (auto& entry : data.meta["enemies"].array_view()) {
            Critter e{};
            e.id = entry["id"].as<int>();
            e.position.x = entry["position"][0].as<int>();
            e.position.y = entry["position"][1].as<int>();
            critters.push_back(e);
        }
    }

    // tiles
    int layer_counter{};
    for (auto& layer : map_states.back().layers) {
        int cell_counter{};
        for (auto& cell : data.tiles["layers"][layer_counter].array_view()) {
            layer.grid.cells.at(cell_counter).value = cell.as<int>();
            layer.grid.cells.at(cell_counter).type = lookup_type(cell.as<int>());
            ++cell_counter;
        }
        layer.grid.update();
        ++layer_counter;
    }
    
}

bool Canvas::save(const std::string& path) {
    
    int value{};
    int counter = 0;
    std::filesystem::create_directory(path);

    /*json overhaul*/

    //clean jsons
    data = {};

    //empty json array
    constexpr auto empty_array = R"([])";
    auto const wipe = dj::Json::parse(empty_array);

    //data.meta
    data.meta["meta"]["room_id"] = room_id;
    data.meta["meta"]["dimensions"][0] = dimensions.x;
    data.meta["meta"]["dimensions"][1] = dimensions.y;
    data.meta["meta"]["chunk_dimensions"][0] = chunk_dimensions.x;
    data.meta["meta"]["chunk_dimensions"][1] = chunk_dimensions.y;
    data.meta["meta"]["style"] = pi::lookup::get_style_id.at(style);
    data.meta["meta"]["background"] = pi::lookup::get_backdrop_id.at(bg);

    int ctr{};
    for (auto& portal : portals) {
        data.meta["portals"].push_back(wipe);
        data.meta["portals"][ctr]["dimensions"][0] = portal.dimensions.x;
        data.meta["portals"][ctr]["dimensions"][1] = portal.dimensions.y;
        data.meta["portals"][ctr]["position"][0] = portal.position.x;
        data.meta["portals"][ctr]["position"][1] = portal.position.y;
        data.meta["portals"][ctr]["source_id"] = portal.source_map_id;
        data.meta["portals"][ctr]["destination_id"] = portal.destination_map_id;
        data.meta["portals"][ctr]["activate_on_contact"] = (dj::Boolean)(portal.activate_on_contact);
        ++ctr;
    }
    ctr = 0;
    for (auto& inspectable : inspectables) {
        data.meta["inspectables"].push_back(wipe);
        data.meta["inspectables"][ctr]["dimensions"][0] = inspectable.dimensions.x;
        data.meta["inspectables"][ctr]["dimensions"][1] = inspectable.dimensions.y;
        data.meta["inspectables"][ctr]["position"][0] = inspectable.position.x;
        data.meta["inspectables"][ctr]["position"][1] = inspectable.position.y;
        data.meta["inspectables"][ctr]["activate_on_contact"] = (dj::Boolean)(inspectable.activate_on_contact);
        data.meta["inspectables"][ctr]["key"] = inspectable.message;
        ++ctr;
    }
    ctr = 0;
    for (auto& animator : animators) {
        data.meta["animators"].push_back(wipe);
        data.meta["animators"][ctr]["id"] = animator.id;
        data.meta["animators"][ctr]["dimensions"][0] = animator.dimensions.x;
        data.meta["animators"][ctr]["dimensions"][1] = animator.dimensions.y;
        data.meta["animators"][ctr]["position"][0] = animator.position.x;
        data.meta["animators"][ctr]["position"][1] = animator.position.y;
        data.meta["animators"][ctr]["automatic"] = (dj::Boolean)(animator.automatic);
        data.meta["animators"][ctr]["foreground"] = (dj::Boolean)(animator.foreground);
        ++ctr;
    }
    ctr = 0;
    for (auto& critter : critters) {
        data.meta["enemies"].push_back(wipe);
        data.meta["enemies"][ctr]["id"] = critter.id;
        data.meta["enemies"][ctr]["position"][0] = critter.position.x;
        data.meta["enemies"][ctr]["position"][1] = critter.position.y;
        ++ctr;
    }


    data.tiles["layers"] = wipe;
    for (int i = 0; i < NUM_LAYERS; ++i) {
        data.tiles["layers"].push_back(wipe);
    }
    //push layer data
    int current_layer{};
    for (auto& layer : map_states.back().layers) {
        int current_cell{};
        for (auto& cell : layer.grid.cells) {
            data.tiles["layers"][current_layer].push_back(layer.grid.cells.at(current_cell).value);
            ++current_cell;
        }
        ++current_layer;
    }

    data.meta.to_file((path + "/meta.json").c_str());
    data.tiles.to_file((path + "/tile.json").c_str());

    /*json overhaul*/
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
        chests.clear();
        npcs.clear();
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
