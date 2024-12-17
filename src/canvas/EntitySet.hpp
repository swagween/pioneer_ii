
#pragma once

#include <SFML/Graphics.hpp>
#include <string_view>
#include <djson/json.hpp>
#include <filesystem>

namespace pi {

class ResourceFinder;

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

struct Scenery {
	int style{};
	int layer{};
	int variant{};
	sf::Vector2<uint32_t> position{};
};

struct InteractiveScenery {
	int length{};
	int size{};
	bool foreground{};
	int type{};
	bool has_platform{};
	std::vector<int> link_indeces{};
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
	int id{};
	bool placed{};
	sf::Vector2<uint32_t> position{};
};

class EntitySet {
  public:
	EntitySet() = default;
	EntitySet(ResourceFinder& finder, dj::Json& metadata, std::string const& room_name);
	void render(sf::RenderWindow& win, sf::Vector2<float> cam);
	void load(ResourceFinder& finder, dj::Json& metadata, std::string const& room_name);
	bool save(ResourceFinder& finder, dj::Json& metadata, std::string const& room_name);
	void clear();
	bool has_switch_block_at(sf::Vector2<uint32_t> pos) const;

	struct {
		sf::Sprite large_animator{};
		sf::Sprite small_animator{};
		sf::Sprite large_animator_thumb{};
		sf::Sprite small_animator_thumb{};
		sf::Sprite enemy_thumb{};
		sf::Sprite current_enemy{};
	} sprites{};

	struct {
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
		std::vector<InteractiveScenery> interactive_scenery{};
		std::vector<Scenery> scenery{};
		std::vector<Destroyer> destroyers{};
		SavePoint save_point{};
		std::string music{};
		sf::Vector2<uint32_t> player_start{};
	} variables{};

  private:
	sf::Texture large_animator_textures{};
	sf::Texture large_animator_thumbs{};
	sf::Texture small_animator_textures{};
	sf::Texture small_animator_thumbs{};
	sf::Texture enemy_thumbnails{};

	sf::RectangleShape player_box{};
	sf::RectangleShape portalbox{};
	sf::RectangleShape inspbox{};
	sf::RectangleShape platbox{};
	sf::RectangleShape chestbox{};
	sf::RectangleShape savebox{};
	sf::RectangleShape platextent{};
	sf::RectangleShape vinebox{};
	sf::RectangleShape scenerybox{};

	// read and write
	struct {
		dj::Json inspectables{};
	} data{};
};
} // namespace pi