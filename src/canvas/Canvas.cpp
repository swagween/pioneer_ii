
#include "Canvas.hpp"
#include "../util/Lookup.hpp"
#include "../tool/Tool.hpp"
#include <cassert>

namespace pi {

Canvas::Canvas() {
}

Canvas::Canvas(sf::Vector2<uint32_t> dim) {
    dimensions = dim;
    real_dimensions = {(float)dim.x * CELL_SIZE, (float)dim.y * CELL_SIZE};
    chunk_dimensions.x = dim.x / CHUNK_SIZE;
    chunk_dimensions.y = dim.y / CHUNK_SIZE;
    clear();
    map_states.push_back(Map());
	for (int i = 0; i < NUM_LAYERS; ++i) { map_states.back().layers.push_back(Layer(i, (i == MIDDLEGROUND), dim)); }
}

void Canvas::load(const std::string& path) {

    map_states.clear();
    redo_states.clear();

    //init map_states
    map_states.push_back(Map());
    clear();
    
    std::string filepath = path + "/meta.json";

    data.meta = dj::Json::from_file((path + "/meta.json").c_str());
    assert(!data.meta.is_null());
    data.tiles = dj::Json::from_file((path + "/tile.json").c_str());
    assert(!data.tiles.is_null());
    data.inspectables = dj::Json::from_file((path + "/inspectables.json").c_str());

    // get npc data
    if (!data.meta.is_null()) {
        auto const& meta = data.meta["meta"];
        room_id = meta["room_id"].as<int>();
		metagrid_coordinates.x = meta["metagrid"][0].as<int>();
		metagrid_coordinates.y = meta["metagrid"][1].as<int>();
        dimensions.x = meta["dimensions"][0].as<int>();
        dimensions.y = meta["dimensions"][1].as<int>();
        chunk_dimensions.x = meta["chunk_dimensions"][0].as<int>();
        chunk_dimensions.y = meta["chunk_dimensions"][1].as<int>();
        real_dimensions = { (float)dimensions.x * constants.cell_size, (float)dimensions.y * constants.cell_size };
        for (int i = 0; i < NUM_LAYERS; ++i) { map_states.back().layers.push_back(Layer(i, (i == MIDDLEGROUND), dimensions)); }

        auto style_value = meta["style"].as<int>();
        style = static_cast<STYLE>(meta["style"].as<int>());
		bg = static_cast<BACKDROP>(meta["background"].as<int>());
		music = meta["music"].as_string();
		styles.breakable = meta["styles"]["breakables"].as<int>();
		cutscene.flag = static_cast<bool>(meta["cutscene_on_entry"]["flag"].as_bool());
		cutscene.type = meta["cutscene_on_entry"]["type"].as<int>();
		cutscene.id = meta["cutscene_on_entry"]["id"].as<int>();
		cutscene.source = meta["cutscene_on_entry"]["source"].as<int>();

		for (auto& entry : data.meta["chests"].array_view()) {
			Chest c{};
			c.position.x = entry["position"][0].as<int>();
			c.position.y = entry["position"][1].as<int>();
			c.id = entry["id"].as<int>();
			c.item_id = entry["item_id"].as<int>();
			c.type = entry["type"].as<int>();
			c.amount = entry["amount"].as<int>();
			c.rarity = entry["rarity"].as<float>();
			chests.push_back(c);
		}

        for (auto& entry : data.meta["portals"].array_view()) {
            Portal p{};
            p.position.x = entry["position"][0].as<int>();
            p.position.y = entry["position"][1].as<int>();
            p.dimensions.x = entry["dimensions"][0].as<int>();
            p.dimensions.y = entry["dimensions"][1].as<int>();
            p.source_map_id = entry["source_id"].as<int>();
            p.destination_map_id = entry["destination_id"].as<int>();
			p.activate_on_contact = static_cast<bool>(entry["activate_on_contact"].as_bool());
			p.already_open = static_cast<bool>(entry["already_open"].as_bool());
			p.locked = static_cast<bool>(entry["locked"].as_bool());
			p.key_id = entry["key_id"].as<int>();
            portals.push_back(p);
        }

        auto const& savept = data.meta["save_point"];
		if (data.meta["save_point"].contains("id")) { save_point.placed = true; }
        save_point.position.x = savept["position"][0].as<int>();
		save_point.position.y = savept["position"][1].as<int>();

        for (auto& entry : data.meta["npcs"].array_view()) {
            NPC n{};
            n.position.x = entry["position"][0].as<int>();
            n.position.y = entry["position"][1].as<int>();
            n.id = entry["id"].as<int>();
			n.background = static_cast<bool>(entry["background"].as_bool());
			for (auto& suite : entry["suites"].array_view()) {
				std::vector<std::string> in_set{};
				for (auto& set : suite.array_view()) {
					in_set.push_back(set.as_string().data());
					std::cout << "In set: " << in_set.back() << "\n";
				}
				n.suites.push_back(in_set);
			}
            npcs.push_back(n);
        }
        for (auto& entry : data.meta["animators"].array_view()) {
            Animator a{};
            a.position.x = entry["position"][0].as<int>();
            a.position.y = entry["position"][1].as<int>();
            a.dimensions.x = entry["dimensions"][0].as<int>();
            a.dimensions.y = entry["dimensions"][1].as<int>();
			a.id = entry["id"].as<int>();
			a.style = entry["style"].as<int>();
			a.automatic = static_cast<bool>(entry["automatic"].as_bool());
            a.foreground = static_cast<bool>(entry["foreground"].as_bool());
            animators.push_back(a);
		}
		for (auto& entry : data.meta["beds"].array_view()) { beds.push_back(Bed{{entry["position"][0].as<uint32_t>(), entry["position"][1].as<uint32_t>()}}); }
		for (auto& entry : data.meta["switch_blocks"].array_view()) {
			SwitchBlock s{};
			s.position.x = entry["position"][0].as<int>();
			s.position.y = entry["position"][1].as<int>();
			s.id = entry["button_id"].as<int>();
			s.type = entry["type"].as<int>();
			switch_blocks.push_back(s);
		}
		for (auto& entry : data.meta["switches"].array_view()) {
			SwitchButton s{};
			s.position.x = entry["position"][0].as<int>();
			s.position.y = entry["position"][1].as<int>();
			s.id = entry["button_id"].as<int>();
			s.type = entry["type"].as<int>();
			switch_buttons.push_back(s);
		}
		for (auto& entry : data.meta["destroyers"].array_view()) {
			Destroyer d{};
			d.position.x = entry["position"][0].as<int>();
			d.position.y = entry["position"][1].as<int>();
			d.id = entry["quest_id"].as<int>();
			destroyers.push_back(d);
		}
        for (auto& entry : data.meta["platforms"].array_view()) {
            Platform p{};
            p.position.x = entry["position"][0].as<int>();
            p.position.y = entry["position"][1].as<int>();
            p.dimensions.x = entry["dimensions"][0].as<int>();
            p.dimensions.y = entry["dimensions"][1].as<int>();
            p.extent = entry["extent"].as<int>();
            p.style = entry["style"].as<int>();
            p.start = entry["start"].as<float>();
            p.type = entry["type"].as_string();
            platforms.push_back(p);
        }

        for (auto& entry : data.meta["inspectables"].array_view()) {
            Inspectable i{};
            i.key = entry["key"].as_string();
            i.position.x = entry["position"][0].as<int>();
            i.position.y = entry["position"][1].as<int>();
            i.dimensions.x = entry["dimensions"][0].as<int>();
			i.dimensions.y = entry["dimensions"][1].as<int>();
			i.alternates = entry["alternates"].as<int>();
            inspectables.push_back(i);
            inspectables.back().activate_on_contact = (bool)entry["activate_on_contact"].as_bool();
        }
        for (auto& inspectable : inspectables) {
			auto entry = data.inspectables[inspectable.key];
			for (auto& suite : entry["suite"].array_view()) {
				std::vector<std::string> in_set{};
				for (auto& set : suite.array_view()) { in_set.push_back(set.as_string().data()); }
				inspectable.suites.push_back(in_set);
			}
			for (auto& response : entry["responses"].array_view()) {
				std::vector<std::string> in_set{};
				for (auto& set : response.array_view()) { in_set.push_back(set.as_string().data()); }
				inspectable.suites.push_back(in_set);
			}
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
	data.meta["meta"]["metagrid"][0] = metagrid_coordinates.x;
	data.meta["meta"]["metagrid"][1] = metagrid_coordinates.y; 
    data.meta["meta"]["dimensions"][0] = dimensions.x;
    data.meta["meta"]["dimensions"][1] = dimensions.y;
    data.meta["meta"]["chunk_dimensions"][0] = chunk_dimensions.x;
    data.meta["meta"]["chunk_dimensions"][1] = chunk_dimensions.y;
    data.meta["meta"]["style"] = static_cast<int>(style);
    data.meta["meta"]["background"] = static_cast<int>(bg);
	data.meta["meta"]["music"] = music;
	data.meta["meta"]["styles"]["breakables"] = styles.breakable;
	data.meta["meta"]["cutscene_on_entry"]["flag"] = dj::Boolean{cutscene.flag};
	data.meta["meta"]["cutscene_on_entry"]["type"] = cutscene.type;
	data.meta["meta"]["cutscene_on_entry"]["id"] = cutscene.id;
	data.meta["meta"]["cutscene_on_entry"]["source"] = cutscene.source;

    int ctr{};
    for (auto& portal : portals) {
        data.meta["portals"].push_back(wipe);
        data.meta["portals"][ctr]["dimensions"][0] = portal.dimensions.x;
        data.meta["portals"][ctr]["dimensions"][1] = portal.dimensions.y;
        data.meta["portals"][ctr]["position"][0] = portal.position.x;
        data.meta["portals"][ctr]["position"][1] = portal.position.y;
        data.meta["portals"][ctr]["source_id"] = room_id;
        data.meta["portals"][ctr]["destination_id"] = portal.destination_map_id;
		data.meta["portals"][ctr]["activate_on_contact"] = (dj::Boolean)(portal.activate_on_contact);
		data.meta["portals"][ctr]["already_open"] = (dj::Boolean)(portal.already_open);
		data.meta["portals"][ctr]["locked"] = (dj::Boolean)(portal.locked);
		data.meta["portals"][ctr]["key_id"] = portal.key_id;
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
		data.meta["inspectables"][ctr]["key"] = inspectable.key;
		data.meta["inspectables"][ctr]["alternates"] = inspectable.alternates;
		for (auto& suite : inspectable.suites) {
			auto out_set = wipe;
			for (auto& message : suite) { out_set.push_back(message); }
			data.inspectables[inspectable.key]["suite"].push_back(out_set);
		}
		for (auto& response : inspectable.responses) {
			auto out_set = wipe;
			for (auto& message : response) { out_set.push_back(message); }
			data.inspectables[inspectable.key]["responses"].push_back(out_set);
		}
        ++ctr;
    }
    ctr = 0;
    for (auto& animator : animators) {
        data.meta["animators"].push_back(wipe);
		data.meta["animators"][ctr]["id"] = animator.id;
		data.meta["animators"][ctr]["style"] = animator.style;
        data.meta["animators"][ctr]["dimensions"][0] = animator.dimensions.x;
        data.meta["animators"][ctr]["dimensions"][1] = animator.dimensions.y;
        data.meta["animators"][ctr]["position"][0] = animator.position.x;
        data.meta["animators"][ctr]["position"][1] = animator.position.y;
        data.meta["animators"][ctr]["automatic"] = (dj::Boolean)(animator.automatic);
        data.meta["animators"][ctr]["foreground"] = (dj::Boolean)(animator.foreground);
        ++ctr;
    }
    ctr = 0;
    for (auto& plat : platforms) {
        data.meta["platforms"].push_back(wipe);
        data.meta["platforms"][ctr]["position"][0] = plat.position.x;
        data.meta["platforms"][ctr]["position"][1] = plat.position.y;
        data.meta["platforms"][ctr]["dimensions"][0] = plat.dimensions.x;
        data.meta["platforms"][ctr]["dimensions"][1] = plat.dimensions.y;
        data.meta["platforms"][ctr]["extent"] = plat.extent;
        data.meta["platforms"][ctr]["style"] = plat.style;
        data.meta["platforms"][ctr]["type"] = plat.type;
        data.meta["platforms"][ctr]["start"] = plat.start;
        ++ctr;
	}
	ctr = 0;
	for (auto& bed : beds) {
		data.meta["beds"].push_back(wipe);
		data.meta["beds"][ctr]["position"][0] = bed.position.x;
		data.meta["beds"][ctr]["position"][1] = bed.position.y;
		++ctr;
	}
	ctr = 0;
	for (auto& destroyer : destroyers) {
		data.meta["destroyers"].push_back(wipe);
		data.meta["destroyers"][ctr]["quest_id"] = destroyer.id;
		data.meta["destroyers"][ctr]["position"][0] = destroyer.position.x;
		data.meta["destroyers"][ctr]["position"][1] = destroyer.position.y;
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
	for (auto& npc : npcs) {
		data.meta["npcs"].push_back(wipe);
		data.meta["npcs"][ctr]["id"] = npc.id;
		data.meta["npcs"][ctr]["position"][0] = npc.position.x;
		data.meta["npcs"][ctr]["position"][1] = npc.position.y;
		data.meta["npcs"][ctr]["background"] = (dj::Boolean)npc.background;
		for (auto& suite : npc.suites) {
			auto out_set = wipe;
			for (auto& number : suite) { out_set.push_back(number); }
			data.meta["npcs"][ctr]["suites"].push_back(out_set);
			std::cout << "Out set: " << out_set.as_string() << "\n";
		}
		++ctr;
	}
	ctr = 0;
	for (auto& chest : chests) {
		data.meta["chests"].push_back(wipe);
		data.meta["chests"][ctr]["id"] = chest.id;
		data.meta["chests"][ctr]["item_id"] = chest.item_id;
		data.meta["chests"][ctr]["type"] = chest.type;
		data.meta["chests"][ctr]["amount"] = chest.amount;
		data.meta["chests"][ctr]["rarity"] = chest.rarity;
		data.meta["chests"][ctr]["position"][0] = chest.position.x;
		data.meta["chests"][ctr]["position"][1] = chest.position.y;
		++ctr;
	}
	ctr = 0;
	for (auto& block : switch_blocks) {
		data.meta["switch_blocks"].push_back(wipe);
		data.meta["switch_blocks"][ctr]["button_id"] = block.id;
		data.meta["switch_blocks"][ctr]["type"] = block.type;
		data.meta["switch_blocks"][ctr]["position"][0] = block.position.x;
		data.meta["switch_blocks"][ctr]["position"][1] = block.position.y;
		++ctr;
	}
	ctr = 0;
	for (auto& button : switch_buttons) {
		data.meta["switches"].push_back(wipe);
		data.meta["switches"][ctr]["button_id"] = button.id;
		data.meta["switches"][ctr]["type"] = button.type;
		data.meta["switches"][ctr]["position"][0] = button.position.x;
		data.meta["switches"][ctr]["position"][1] = button.position.y;
		++ctr;
	}
	if (save_point.placed) {
		data.meta["save_point"]["id"] = room_id;
		data.meta["save_point"]["position"][0] = save_point.position.x;
		data.meta["save_point"]["position"][1] = save_point.position.y;
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
    data.inspectables.to_file((path + "/inspectables.json").c_str());
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
		platforms.clear();
        npcs.clear();
        switch_blocks.clear();
		switch_buttons.clear();
		beds.clear();
		destroyers.clear();
		save_point.placed = false;
    }
}

void Canvas::save_state(Tool& tool) {
    auto const& type = tool.type;
    auto undoable_tool = type == ToolType::brush || type == ToolType::fill || type == ToolType::select || type == ToolType::erase;
    auto just_clicked = !tool.active && tool.ready;
    if (undoable_tool && just_clicked) {
        map_states.emplace_back(Map(map_states.back()));
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
    redo_states.clear(); }

bool Canvas::has_switch_block_at(sf::Vector2<uint32_t> pos) const {
	for (auto& s : switch_blocks) {
		if (s.position.x == pos.x && s.position.y == pos.y) { return true; };
	}
	return false;
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
