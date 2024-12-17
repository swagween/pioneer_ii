
#include "EntitySet.hpp"
#include <cassert>
#include "Canvas.hpp"
#include "../setup/ResourceFinder.hpp"

namespace pi {

EntitySet::EntitySet(ResourceFinder& finder, std::string const& room_name) {

	load(finder, room_name);

	large_animator_textures.loadFromFile((finder.paths.resources / "image" / "animators" / "large_animators_01.png").string());
	large_animator_thumbs.loadFromFile((finder.paths.resources / "image" / "animators" / "large_animator_thumbs.png").string());
	small_animator_textures.loadFromFile((finder.paths.resources / "image" / "animators" / "small_animators_01.png").string());
	enemy_thumbnails.loadFromFile((finder.paths.local / "enemies" / "thumbnails.png").string());

	sprites.current_enemy.setTexture(enemy_thumbnails);
	sprites.enemy_thumb.setTexture(enemy_thumbnails);
	sprites.large_animator.setTexture(large_animator_textures);
	sprites.large_animator_thumb.setTexture(large_animator_thumbs);
	sprites.small_animator.setTexture(small_animator_textures);
	sprites.small_animator_thumb.setTexture(small_animator_thumbs);

	player_box.setFillColor(sf::Color{100, 200, 100, 10});
	player_box.setOutlineColor(sf::Color{100, 200, 100, 70});
	player_box.setOutlineThickness(-2);
	player_box.setSize({CELL_SIZE, CELL_SIZE});

	portalbox.setFillColor(sf::Color{120, 220, 200, 128});
	portalbox.setOutlineColor(sf::Color{240, 230, 255, 180});
	portalbox.setOutlineThickness(-1);
	portalbox.setSize({CELL_SIZE, CELL_SIZE});

	chestbox.setFillColor(sf::Color{220, 220, 80, 128});
	chestbox.setOutlineColor(sf::Color{40, 30, 255, 180});
	chestbox.setOutlineThickness(-3);
	chestbox.setSize({CELL_SIZE, CELL_SIZE});

	savebox.setFillColor(sf::Color{220, 20, 220, 128});
	savebox.setOutlineColor(sf::Color{240, 230, 255, 180});
	savebox.setOutlineThickness(-1);
	savebox.setSize({CELL_SIZE, CELL_SIZE});

	inspbox.setFillColor(sf::Color{220, 120, 100, 128});
	inspbox.setOutlineColor(sf::Color{240, 230, 255, 180});
	inspbox.setOutlineThickness(-1);
	inspbox.setSize({CELL_SIZE, CELL_SIZE});

	vinebox.setOutlineColor(sf::Color{240, 230, 80, 80});
	vinebox.setOutlineThickness(-1);

	scenerybox.setOutlineColor(sf::Color{20, 20, 180, 30});
	scenerybox.setOutlineThickness(-1);

	platextent.setFillColor(sf::Color::Transparent);
	platextent.setOutlineColor(sf::Color{240, 230, 55, 80});
	platextent.setOutlineThickness(-2);
	platbox.setFillColor(sf::Color{220, 120, 100, 128});
	platbox.setOutlineColor(sf::Color{240, 230, 255, 180});
	platbox.setOutlineThickness(-4);
}

void EntitySet::render(sf::RenderWindow& win, sf::Vector2<float> cam) {

	for (auto& portal : variables.portals) {
		for (uint32_t i = 0; i < portal.dimensions.x; ++i) {
			for (uint32_t j = 0; j < portal.dimensions.y; ++j) {
				portalbox.setPosition((portal.position.x + i) * CELL_SIZE + cam.x, (portal.position.y + j) * CELL_SIZE + cam.y);
				win.draw(portalbox);
			}
		}
	}

	for (auto& inspectable : variables.inspectables) {
		inspbox.setPosition((inspectable.position.x) * CELL_SIZE + cam.x, (inspectable.position.y) * CELL_SIZE + cam.y);
		win.draw(inspbox);
	}

	for (auto& block : variables.switch_blocks) {
		inspbox.setPosition((block.position.x) * CELL_SIZE + cam.x, (block.position.y) * CELL_SIZE + cam.y);
		win.draw(inspbox);
	}

	for (auto& button : variables.switch_buttons) {
		inspbox.setPosition((button.position.x) * CELL_SIZE + cam.x, (button.position.y) * CELL_SIZE + cam.y);
		win.draw(inspbox);
	}

	for (auto& chest : variables.chests) {
		chestbox.setPosition((chest.position.x) * CELL_SIZE + cam.x, (chest.position.y) * CELL_SIZE + cam.y);
		win.draw(chestbox);
	}
	for (auto& scenery : variables.scenery) {
		scenerybox.setPosition((scenery.position.x) * CELL_SIZE + cam.x, (scenery.position.y) * CELL_SIZE + cam.y);
		win.draw(scenerybox);
	}

	for (auto& scenery : variables.interactive_scenery) {
		vinebox.setPosition((scenery.position.x) * CELL_SIZE + cam.x, (scenery.position.y) * CELL_SIZE + cam.y);
		scenery.type == 0 ? vinebox.setSize({static_cast<float>(scenery.size) * 8.f, static_cast<float>(scenery.length) * CELL_SIZE})
						  : vinebox.setSize({static_cast<float>(scenery.size) * 8.f, static_cast<float>(scenery.length) * CELL_SIZE});
		scenery.foreground ? vinebox.setFillColor(sf::Color{90, 120, 80, 88}) : vinebox.setFillColor(sf::Color{220, 120, 80, 88});
		scenery.type == 0 ? vinebox.setOutlineColor(sf::Color::White) : vinebox.setOutlineColor(sf::Color::Blue);
		win.draw(vinebox);
		if (scenery.has_platform) {
			for (auto& link : scenery.link_indeces) {
				vinebox.setSize({64, 16});
				vinebox.setOrigin({32, 0});
				vinebox.setPosition(vinebox.getPosition() + sf::Vector2<float>(0.f, static_cast<float>(CELL_SIZE * link)));
				win.draw(vinebox);
			}
		}
		vinebox.setOrigin({});
	}

	if (variables.save_point.placed) {
		savebox.setPosition((variables.save_point.position.x) * CELL_SIZE + cam.x, (variables.save_point.position.y) * CELL_SIZE + cam.y);
		win.draw(savebox);
	}

	for (auto& platform : variables.platforms) {
		auto f_extent = platform.extent * 32.f;
		platextent.setSize({f_extent, f_extent});
		platextent.setPosition((platform.position.x) * CELL_SIZE + cam.x + platform.dimensions.x * 16.f, (platform.position.y) * CELL_SIZE + cam.y + platform.dimensions.y * 16.f);
		platbox.setSize({platform.dimensions.x * 32.f, platform.dimensions.y * 32.f});
		platbox.setPosition((platform.position.x) * CELL_SIZE + cam.x, (platform.position.y) * CELL_SIZE + cam.y);
		win.draw(platextent);
		win.draw(platbox);
	}

	for (auto& critter : variables.critters) {
		int idx = critter.id;
		sprites.current_enemy.setTextureRect(sf::IntRect({(idx * 16) % (64), (idx / 4) * 16}, {16, 16}));
		sprites.current_enemy.setScale({2.0f, 2.0f});
		sprites.current_enemy.setPosition((critter.position.x) * CELL_SIZE + cam.x, (critter.position.y) * CELL_SIZE + cam.y);
		win.draw(sprites.current_enemy);
	}

	for (auto& animator : variables.animators) {
		sf::Vector2<float> anim_pos = {animator.position.x * CELL_SIZE + cam.x, animator.position.y * CELL_SIZE + cam.y};
		inspbox.setPosition(anim_pos.x, anim_pos.y);
		auto which = static_cast<int>(animator.id / 100);
		auto lookup = static_cast<int>(animator.id % 100);
		if (which == 1) {
			win.draw(inspbox);
			sprites.large_animator.setTextureRect(sf::IntRect{{lookup * 64, 0}, {64, 64}});
			sprites.large_animator.setPosition(anim_pos.x, anim_pos.y);
			sprites.large_animator.setScale({1.0f, 1.0f});
			win.draw(sprites.large_animator);
		} else {
			win.draw(inspbox);
			sprites.small_animator.setTextureRect(sf::IntRect{{lookup * 32, 0}, {32, 32}});
			sprites.small_animator.setPosition(anim_pos.x, anim_pos.y);
			win.draw(sprites.small_animator);
		}
	}

	// player start
	player_box.setPosition(static_cast<sf::Vector2<float>>(variables.player_start) * (float)CELL_SIZE + cam);
	win.draw(player_box);
}

void EntitySet::load(ResourceFinder& finder, std::string const& room_name) {

	std::string metapath = (finder.paths.levels / room_name / "meta.json").string();
	std::string inspectable_path = (finder.paths.levels / room_name / "inspectables.json").string();

	data.meta = dj::Json::from_file((metapath).c_str());
	assert(!data.meta.is_null());
	data.inspectables = dj::Json::from_file((inspectable_path).c_str());

	// get npc data
	if (!data.meta.is_null()) {

		variables.music = data.meta["music"].as_string();

		for (auto& entry : data.meta["chests"].array_view()) {
			Chest c{};
			c.position.x = entry["position"][0].as<int>();
			c.position.y = entry["position"][1].as<int>();
			c.id = entry["id"].as<int>();
			c.item_id = entry["item_id"].as<int>();
			c.type = entry["type"].as<int>();
			c.amount = entry["amount"].as<int>();
			c.rarity = entry["rarity"].as<float>();
			variables.chests.push_back(c);
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
			variables.portals.push_back(p);
		}

		auto const& savept = data.meta["save_point"];
		if (data.meta["save_point"].contains("id")) { variables.save_point.placed = true; }
		variables.save_point.position.x = savept["position"][0].as<int>();
		variables.save_point.position.y = savept["position"][1].as<int>();
		variables.save_point.id = savept["id"].as<int>();

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
			variables.npcs.push_back(n);
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
			variables.animators.push_back(a);
		}
		for (auto& entry : data.meta["beds"].array_view()) { variables.beds.push_back(Bed{{entry["position"][0].as<uint32_t>(), entry["position"][1].as<uint32_t>()}}); }
		for (auto& entry : data.meta["scenery"]["vines"].array_view()) {
			InteractiveScenery i{};
			i.position.x = entry["position"][0].as<int>();
			i.position.y = entry["position"][1].as<int>();
			i.length = entry["length"].as<int>();
			i.size = entry["size"].as<int>();
			i.foreground = static_cast<bool>(entry["foreground"].as_bool());
			if (entry["platform"]) {
				for (auto& index : entry["platform"]["link_indeces"].array_view()) { i.link_indeces.push_back(index.as<int>()); }
				i.has_platform = !i.link_indeces.empty();
			}
			i.type = 0;
			variables.interactive_scenery.push_back(i);
		}
		for (auto& entry : data.meta["scenery"]["basic"].array_view()) {
			Scenery s{};
			s.position.x = entry["position"][0].as<int>();
			s.position.y = entry["position"][1].as<int>();
			s.style = entry["style"].as<int>();
			s.variant = entry["variant"].as<int>();
			s.layer = entry["layer"].as<int>();
			variables.scenery.push_back(s);
		}
		for (auto& entry : data.meta["scenery"]["grass"].array_view()) {
			InteractiveScenery i{};
			i.position.x = entry["position"][0].as<int>();
			i.position.y = entry["position"][1].as<int>();
			i.length = entry["length"].as<int>();
			i.size = entry["size"].as<int>();
			i.foreground = static_cast<bool>(entry["foreground"].as_bool());
			i.type = 1;
			variables.interactive_scenery.push_back(i);
		}
		for (auto& entry : data.meta["switch_blocks"].array_view()) {
			SwitchBlock s{};
			s.position.x = entry["position"][0].as<int>();
			s.position.y = entry["position"][1].as<int>();
			s.id = entry["button_id"].as<int>();
			s.type = entry["type"].as<int>();
			variables.switch_blocks.push_back(s);
		}
		for (auto& entry : data.meta["switches"].array_view()) {
			SwitchButton s{};
			s.position.x = entry["position"][0].as<int>();
			s.position.y = entry["position"][1].as<int>();
			s.id = entry["button_id"].as<int>();
			s.type = entry["type"].as<int>();
			variables.switch_buttons.push_back(s);
		}
		for (auto& entry : data.meta["destroyers"].array_view()) {
			Destroyer d{};
			d.position.x = entry["position"][0].as<int>();
			d.position.y = entry["position"][1].as<int>();
			d.id = entry["quest_id"].as<int>();
			variables.destroyers.push_back(d);
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
			variables.platforms.push_back(p);
		}

		for (auto& entry : data.meta["inspectables"].array_view()) {
			Inspectable i{};
			i.key = entry["key"].as_string();
			i.position.x = entry["position"][0].as<int>();
			i.position.y = entry["position"][1].as<int>();
			i.dimensions.x = entry["dimensions"][0].as<int>();
			i.dimensions.y = entry["dimensions"][1].as<int>();
			i.alternates = entry["alternates"].as<int>();
			variables.inspectables.push_back(i);
			variables.inspectables.back().activate_on_contact = (bool)entry["activate_on_contact"].as_bool();
		}
		for (auto& inspectable : variables.inspectables) {
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
			variables.critters.push_back(e);
		}
	}
}

bool EntitySet::save(ResourceFinder& finder, std::string const& room_name) {

	// clean jsons
	data = {};

	// empty json array
	constexpr auto empty_array = R"([])";
	auto const wipe = dj::Json::parse(empty_array);

	int ctr{};
	for (auto& portal : variables.portals) {
		data.meta["portals"].push_back(wipe);
		data.meta["portals"][ctr]["dimensions"][0] = portal.dimensions.x;
		data.meta["portals"][ctr]["dimensions"][1] = portal.dimensions.y;
		data.meta["portals"][ctr]["position"][0] = portal.position.x;
		data.meta["portals"][ctr]["position"][1] = portal.position.y;
		data.meta["portals"][ctr]["source_id"] = portal.source_map_id;
		data.meta["portals"][ctr]["destination_id"] = portal.destination_map_id;
		data.meta["portals"][ctr]["activate_on_contact"] = (dj::Boolean)(portal.activate_on_contact);
		data.meta["portals"][ctr]["already_open"] = (dj::Boolean)(portal.already_open);
		data.meta["portals"][ctr]["locked"] = (dj::Boolean)(portal.locked);
		data.meta["portals"][ctr]["key_id"] = portal.key_id;
		++ctr;
	}
	ctr = 0;
	for (auto& inspectable : variables.inspectables) {
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
	for (auto& animator : variables.animators) {
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
	for (auto& plat : variables.platforms) {
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
	for (auto& bed : variables.beds) {
		data.meta["beds"].push_back(wipe);
		data.meta["beds"][ctr]["position"][0] = bed.position.x;
		data.meta["beds"][ctr]["position"][1] = bed.position.y;
		++ctr;
	}
	ctr = 0;
	for (auto& s : variables.scenery) {
		data.meta["scenery"]["basic"].push_back(wipe);
		data.meta["scenery"]["basic"][ctr]["position"][0] = s.position.x;
		data.meta["scenery"]["basic"][ctr]["position"][1] = s.position.y;
		data.meta["scenery"]["basic"][ctr]["style"] = s.style;
		data.meta["scenery"]["basic"][ctr]["variant"] = s.variant;
		data.meta["scenery"]["basic"][ctr]["layer"] = s.layer;
		++ctr;
	}
	ctr = 0;
	for (auto& scenery : variables.interactive_scenery) {
		std::string key = scenery.type == 0 ? "vines" : "grass";
		data.meta["scenery"][key].push_back(wipe);
		data.meta["scenery"][key][ctr]["position"][0] = scenery.position.x;
		data.meta["scenery"][key][ctr]["position"][1] = scenery.position.y;
		data.meta["scenery"][key][ctr]["length"] = scenery.length;
		data.meta["scenery"][key][ctr]["size"] = scenery.size;
		data.meta["scenery"][key][ctr]["foreground"] = dj::Boolean{scenery.foreground};
		if (scenery.has_platform) {
			for (auto& index : scenery.link_indeces) { data.meta["scenery"][key][ctr]["platform"]["link_indeces"].push_back(index); }
		}
		++ctr;
	}
	ctr = 0;
	for (auto& destroyer : variables.destroyers) {
		data.meta["destroyers"].push_back(wipe);
		data.meta["destroyers"][ctr]["quest_id"] = destroyer.id;
		data.meta["destroyers"][ctr]["position"][0] = destroyer.position.x;
		data.meta["destroyers"][ctr]["position"][1] = destroyer.position.y;
		++ctr;
	}
	ctr = 0;
	for (auto& critter : variables.critters) {
		data.meta["enemies"].push_back(wipe);
		data.meta["enemies"][ctr]["id"] = critter.id;
		data.meta["enemies"][ctr]["position"][0] = critter.position.x;
		data.meta["enemies"][ctr]["position"][1] = critter.position.y;
		++ctr;
	}
	for (auto& npc : variables.npcs) {
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
	for (auto& chest : variables.chests) {
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
	for (auto& block : variables.switch_blocks) {
		data.meta["switch_blocks"].push_back(wipe);
		data.meta["switch_blocks"][ctr]["button_id"] = block.id;
		data.meta["switch_blocks"][ctr]["type"] = block.type;
		data.meta["switch_blocks"][ctr]["position"][0] = block.position.x;
		data.meta["switch_blocks"][ctr]["position"][1] = block.position.y;
		++ctr;
	}
	ctr = 0;
	for (auto& button : variables.switch_buttons) {
		data.meta["switches"].push_back(wipe);
		data.meta["switches"][ctr]["button_id"] = button.id;
		data.meta["switches"][ctr]["type"] = button.type;
		data.meta["switches"][ctr]["position"][0] = button.position.x;
		data.meta["switches"][ctr]["position"][1] = button.position.y;
		++ctr;
	}
	if (variables.save_point.placed) {
		data.meta["save_point"]["id"] = variables.save_point.id;
		data.meta["save_point"]["position"][0] = variables.save_point.position.x;
		data.meta["save_point"]["position"][1] = variables.save_point.position.y;
	}

	auto success{true};
	if (!data.meta.to_file((finder.paths.levels / room_name / "meta.json").string().c_str())) { success = false; }
	if (!data.inspectables.to_file((finder.paths.levels / room_name / "inspectables.json").string().c_str())) { success = false; }
	if (!data.meta.to_file((finder.paths.out / room_name / "meta.json").string().c_str())) { success = false; }
	if (!data.inspectables.to_file((finder.paths.out / room_name / "inspectables.json").string().c_str())) { success = false; }

	return success;

	return true;
}

void EntitySet::clear() { variables = {}; }

bool EntitySet::has_switch_block_at(sf::Vector2<uint32_t> pos) const {
	for (auto& s : variables.switch_blocks) {
		if (s.position.x == pos.x && s.position.y == pos.y) { return true; };
	}
	return false;
}

} // namespace pi

