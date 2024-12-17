
#include "Canvas.hpp"
#include "../util/Lookup.hpp"
#include "../tool/Tool.hpp"
#include "../setup/ResourceFinder.hpp"
#include <cassert>

namespace pi {

Canvas::Canvas(bool editable) {
	editable ? properties.set(CanvasProperties::editable) : properties.reset(CanvasProperties::editable);
	box.setOutlineColor(sf::Color{200, 200, 200, 20});
	box.setOutlineThickness(-2);
	box.setSize({CELL_SIZE, CELL_SIZE});

	gridbox.setFillColor(sf::Color::Transparent);
	gridbox.setOutlineColor(sf::Color{240, 230, 255, 20});
	gridbox.setOutlineThickness(-1);
	gridbox.setSize({CELL_SIZE, CELL_SIZE});
}

Canvas::Canvas(sf::Vector2<uint32_t> dim) {
    dimensions = dim;
    real_dimensions = {(float)dim.x * CELL_SIZE, (float)dim.y * CELL_SIZE};
    chunk_dimensions.x = dim.x / CHUNK_SIZE;
    chunk_dimensions.y = dim.y / CHUNK_SIZE;
    clear();
    map_states.push_back(Map());
	for (int i = 0; i < NUM_LAYERS; ++i) { map_states.back().layers.push_back(Layer(i, (i == MIDDLEGROUND), dim)); }

	box.setOutlineColor(sf::Color{200, 200, 200, 20});
	box.setOutlineThickness(-2);
	box.setSize({CELL_SIZE, CELL_SIZE});

	gridbox.setFillColor(sf::Color::Transparent);
	gridbox.setOutlineColor(sf::Color{240, 230, 255, 20});
	gridbox.setOutlineThickness(-1);
	gridbox.setSize({CELL_SIZE, CELL_SIZE});
}

void Canvas::update(Tool& tool, bool transformed) {
	if (transformed) {
		within_bounds(tool.position) ? state.set(CanvasState::hovered) : state.reset(CanvasState::hovered);
	} else {
		within_bounds(tool.get_window_position()) ? state.set(CanvasState::hovered) : state.reset(CanvasState::hovered);
    }
	camera.update();
	background->update();
}

void Canvas::render(sf::RenderWindow& win, sf::Sprite& tileset) {
	if (flags.show_background) { background->render(win, camera.position); }
	if (!states_empty()) {
		for (auto& layer : get_layers().layers) {
			box.setFillColor(sf::Color{40, 240, 80, 40});
			for (auto& cell : layer.grid.cells) {
				if (cell.value == 0) { continue; }
				if (layer.render_order == active_layer || flags.show_all_layers) {
					tileset.setTextureRect(sf::IntRect{get_tile_coord(cell.value), {32, 32}});
					tileset.setPosition(cell.position + camera.position);
					if (layer.render_order != 7 || flags.show_obscured_layer) { win.draw(tileset); }
				} else if (flags.show_indicated_layers) {
					box.setPosition(cell.position.x + camera.position.x, cell.position.y + camera.position.y);
					win.draw(box);
				}
			}
		}
	}
	if (flags.show_entities) { entities.render(win, get_position()); }
	if (flags.show_grid && !states_empty()) {
		if (get_layers().layers.empty()) { return; }
		for (auto& cell : get_layers().layers.back().grid.cells) {
			if (cell.position.x + camera.bounding_box.left < 0) { continue; }
			if (cell.position.x + camera.bounding_box.left > screen_dimensions.x) { continue; }
			if (cell.position.y + camera.bounding_box.top < 0) { continue; }
			if (cell.position.y + camera.bounding_box.top > screen_dimensions.y) { continue; }
			gridbox.setPosition(cell.position.x + camera.position.x, cell.position.y + camera.position.y);
			win.draw(gridbox);
		}
	}
}

void Canvas::load(ResourceFinder& finder, std::string const& room_name, bool local) {

	map_states.clear();
	redo_states.clear();

	// init map_states
	map_states.push_back(Map());
	clear();

	auto const& source = local ? finder.paths.local : finder.paths.levels;

	std::string metapath = (source / room_name / "meta.json").string();
	std::string tilepath = (source / room_name / "tile.json").string();

	data.meta = dj::Json::from_file((metapath).c_str());
	assert(!data.meta.is_null());
	data.tiles = dj::Json::from_file((tilepath).c_str());
	assert(!data.tiles.is_null());

	if (!local) { entities = EntitySet{finder, data.meta, room_name}; }

	auto const& meta = data.meta["meta"];
	room_id = meta["room_id"].as<int>();
	metagrid_coordinates.x = meta["metagrid"][0].as<int>();
	metagrid_coordinates.y = meta["metagrid"][1].as<int>();
	dimensions.x = meta["dimensions"][0].as<int>();
	dimensions.y = meta["dimensions"][1].as<int>();
	chunk_dimensions.x = meta["chunk_dimensions"][0].as<int>();
	chunk_dimensions.y = meta["chunk_dimensions"][1].as<int>();
	real_dimensions = {(float)dimensions.x * constants.cell_size, (float)dimensions.y * constants.cell_size};
	for (int i = 0; i < NUM_LAYERS; ++i) { map_states.back().layers.push_back(Layer(i, (i == MIDDLEGROUND), dimensions)); }

	auto style_value = meta["style"].as<int>();
	styles.tile = static_cast<Style>(meta["style"].as<int>());
	bg = static_cast<Backdrop>(meta["background"].as<int>());
	entities.variables.music = meta["music"].as_string();
	styles.breakable = meta["styles"]["breakables"].as<int>();
	cutscene.flag = static_cast<bool>(meta["cutscene_on_entry"]["flag"].as_bool());
	cutscene.type = meta["cutscene_on_entry"]["type"].as<int>();
	cutscene.id = meta["cutscene_on_entry"]["id"].as<int>();
	cutscene.source = meta["cutscene_on_entry"]["source"].as<int>();

	background = std::make_unique<Background>(finder, meta["background"].as<int>());

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

bool Canvas::save(ResourceFinder& finder, std::string const& room_name) {

	std::filesystem::create_directory(finder.paths.levels / room_name);
	std::filesystem::create_directory(finder.paths.out / room_name);

	// clean jsons
	data = {};

	// empty json array
	constexpr auto empty_array = R"([])";
	auto const wipe = dj::Json::parse(empty_array);

	// data.meta
	data.meta["meta"]["room_id"] = room_id;
	data.meta["meta"]["metagrid"][0] = metagrid_coordinates.x;
	data.meta["meta"]["metagrid"][1] = metagrid_coordinates.y;
	data.meta["meta"]["dimensions"][0] = dimensions.x;
	data.meta["meta"]["dimensions"][1] = dimensions.y;
	data.meta["meta"]["chunk_dimensions"][0] = chunk_dimensions.x;
	data.meta["meta"]["chunk_dimensions"][1] = chunk_dimensions.y;
	data.meta["meta"]["style"] = static_cast<int>(styles.tile);
	data.meta["meta"]["background"] = static_cast<int>(bg);
	data.meta["meta"]["music"] = entities.variables.music;
	data.meta["meta"]["styles"]["breakables"] = styles.breakable;
	data.meta["meta"]["cutscene_on_entry"]["flag"] = dj::Boolean{cutscene.flag};
	data.meta["meta"]["cutscene_on_entry"]["type"] = cutscene.type;
	data.meta["meta"]["cutscene_on_entry"]["id"] = cutscene.id;
	data.meta["meta"]["cutscene_on_entry"]["source"] = cutscene.source;

	int value{};
	int counter = 0;
	entities.save(finder, data.meta, room_name);

	data.tiles["layers"] = wipe;
	for (int i = 0; i < NUM_LAYERS; ++i) { data.tiles["layers"].push_back(wipe); }
	// push layer data
	int current_layer{};
	for (auto& layer : map_states.back().layers) {
		int current_cell{};
		for (auto& cell : layer.grid.cells) {
			data.tiles["layers"][current_layer].push_back(layer.grid.cells.at(current_cell).value);
			++current_cell;
		}
		++current_layer;
	}

	auto success{true};
	if (!data.meta.to_file((finder.paths.levels / room_name / "meta.json").string().c_str())) { success = false; }
	if (!data.tiles.to_file((finder.paths.levels / room_name / "tile.json").string().c_str())) { success = false; }
	if (!data.meta.to_file((finder.paths.out / room_name / "meta.json").string().c_str())) { success = false; }
	if (!data.tiles.to_file((finder.paths.out / room_name / "tile.json").string().c_str())) { success = false; }

    return success;
}

void Canvas::clear() {
    if (!map_states.empty()) {
		for (auto& layer : map_states.back().layers) { layer.erase(); }
        map_states.back().layers.clear();
    }
}

void Canvas::save_state(Tool& tool, bool force) {
    auto const& type = tool.type;
    auto undoable_tool = type == ToolType::brush || type == ToolType::fill || type == ToolType::select || type == ToolType::erase;
    auto just_clicked = !tool.active && tool.ready;
	if ((undoable_tool && just_clicked) || force) {
		map_states.emplace_back(Map{map_states.back()});
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

void Canvas::clear_redo_states() { redo_states.clear(); }

void Canvas::unhover() { state.reset(CanvasState::hovered); }

void Canvas::move(sf::Vector2<float> distance) { camera.move(distance); }

void Canvas::set_position(sf::Vector2<float> to_position) { camera.set_position(to_position); }

void Canvas::center(sf::Vector2<float> point) { set_position(point - real_dimensions * 0.5f); }

Map& Canvas::get_layers() { return map_states.back(); }

sf::Vector2<int> Canvas::get_tile_coord(int lookup) {
	sf::Vector2<int> ret{};
	ret.x = static_cast<int>(lookup % 16);
	ret.y = static_cast<int>(std::floor(lookup / 16));
	return ret * 32;
}

void Canvas::edit_tile_at(int i, int j, int new_val, int layer_index) {
    if(layer_index >= map_states.back().layers.size()) { return; }
    if((i + j * dimensions.x) >= map_states.back().layers.at(layer_index).grid.cells.size()) { return; };
    map_states.back().layers.at(layer_index).grid.cells.at(i + j * dimensions.x).value = new_val;
}

void Canvas::erase_at(int i, int j, int layer_index) { edit_tile_at(i, j, 0, layer_index); }

int Canvas::tile_val_at(int i, int j, int layer) { return map_states.back().layers.at(layer).grid.cells.at(i + j * dimensions.x).value; }

int Canvas::tile_val_at_scaled(int i, int j, int layer) {
	auto u = std::floor(i / 32);
	auto v = std::floor(j / 32);
	auto idx = static_cast<std::size_t>(u + v * dimensions.x);
	if (idx < 0 || idx >= map_states.back().layers.at(layer).grid.cells.size()) { return 0; }
	return map_states.back().layers.at(layer).grid.cells.at(idx).value;
}

sf::Vector2<float> Canvas::get_tile_position_at(int i, int j, int layer) {
	auto u = std::floor(i / 32);
	auto v = std::floor(j / 32);
	auto idx = static_cast<std::size_t>(u + v * dimensions.x);
	if (idx < 0 || idx >= map_states.back().layers.at(layer).grid.cells.size()) { return {}; }
	return map_states.back().layers.at(layer).grid.cells.at(idx).position;
}

Tile& Canvas::get_tile_at(int i, int j, int layer) {
	auto u = std::floor(i / 32);
	auto v = std::floor(j / 32);
	auto idx = static_cast<std::size_t>(u + v * dimensions.x);
	if (idx < 0 || idx >= map_states.back().layers.at(layer).grid.cells.size()) { return map_states.back().layers.at(layer).grid.cells.at(0); }
	return map_states.back().layers.at(layer).grid.cells.at(idx);
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
