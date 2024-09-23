
#pragma once

#include <stdio.h>
#include "../canvas/Canvas.hpp"
#include "../canvas/Clipboard.hpp"

namespace pi {

enum class ToolType { brush, fill, select, erase, hand, entity_placer };

enum class ENTITY_TYPE { PORTAL, INSPECTABLE, CRITTER, CHEST, ANIMATOR, ENTITY_EDITOR, PLAYER_PLACER, PLATFORM, SAVE_POINT, SWITCH_BUTTON, SWITCH_BLOCK, BED };

class Tool {
  public:
	Tool& operator=(Tool const&) = delete;
	virtual void handle_events(Canvas& canvas, sf::Event& e) = 0;
	virtual void handle_keyboard_events(Canvas& canvas, sf::Keyboard::Key& key) = 0;
	virtual void update() = 0;
	virtual void render(sf::RenderWindow& win, sf::Vector2<float> offset) = 0;
	virtual void store_tile(int index) = 0;

	bool in_bounds(sf::Vector2<uint32_t>& bounds) const;
	bool is_paintable() const;

	bool active{};
	bool ready{};
	bool just_clicked = true;

	sf::Vector2<float> position{};
	sf::Vector2<float> clicked_position{};
	sf::Vector2<float> relative_position{};
	sf::Vector2<uint32_t> scaled_position{};
	sf::Vector2<uint32_t> scaled_clicked_position{};

	int xorigin{};
	int yorigin{};
	int tempx{};
	int tempy{};

	int size{1};

	bool primary{};
	bool trigger_switch{false};
	uint8_t tile{};
	Portal current_portal{}; // ideally I wouldn't do this here, but I can't think of a better way
	Inspectable current_inspectable{};
	Animator current_animator{};
	Critter current_critter{};
	Platform current_platform{};
	Chest current_chest{};
	SwitchBlock current_switch_block{};
	SwitchButton current_switch{};

	ToolType type{};
	ENTITY_TYPE ent_type{};
};

class Hand : public Tool {
  public:
	Hand() { type = ToolType::hand; }
	void handle_events(Canvas& canvas, sf::Event& e);
	void handle_keyboard_events(Canvas& canvas, sf::Keyboard::Key& key);
	void update();
	void render(sf::RenderWindow& win, sf::Vector2<float> offset);
	void store_tile(int index);
};

class Brush : public Tool {
  public:
	Brush() { type = ToolType::brush; }
	void handle_events(Canvas& canvas, sf::Event& e);
	void handle_keyboard_events(Canvas& canvas, sf::Keyboard::Key& key);
	void update();
	void render(sf::RenderWindow& win, sf::Vector2<float> offset);
	void store_tile(int index);

  private:
};

class Erase : public Tool {
  public:
	Erase() { type = ToolType::erase; }
	void handle_events(Canvas& canvas, sf::Event& e);
	void handle_keyboard_events(Canvas& canvas, sf::Keyboard::Key& key);
	void update();
	void render(sf::RenderWindow& win, sf::Vector2<float> offset);
	void store_tile(int index);

  private:
};

class Fill : public Tool {
  public:
	Fill() { type = ToolType::fill; };
	void handle_events(Canvas& canvas, sf::Event& e);
	void handle_keyboard_events(Canvas& canvas, sf::Keyboard::Key& key);
	void update();
	void render(sf::RenderWindow& win, sf::Vector2<float> offset);
	void store_tile(int index);

	void fill_section(uint8_t const prev_val, uint8_t const new_val, uint32_t i, uint32_t j, Canvas& canvas);
};

class EntityPlacer : public Tool {
  public:
	EntityPlacer() { type = ToolType::entity_placer; };
	void handle_events(Canvas& canvas, sf::Event& e);
	void handle_keyboard_events(Canvas& canvas, sf::Keyboard::Key& key);
	void update();
	void render(sf::RenderWindow& win, sf::Vector2<float> offset);
	void store_portal(sf::Vector2<uint32_t> dim, bool activate_on_contact, int src_id, int dest_id);
	void store_inspectable(sf::Vector2<uint32_t> dim, bool activate_on_contact, std::string);
	void store_critter(CRITTER_TYPE type);
	void store_animator(sf::Vector2<uint32_t> dim, int id, bool automatic, bool foreground);
	void store_tile(int index);
};

struct SelectBox {
	SelectBox() = default;
	SelectBox(sf::Vector2<uint32_t> pos, sf::Vector2<uint32_t> dim) : position(pos), dimensions(dim) {}
	void clear() {
		position = {0, 0};
		dimensions = {0, 0};
	}
	void adjust(sf::Vector2<uint32_t> adjustment) { dimensions = adjustment; }
	sf::Vector2<uint32_t> position{};
	sf::Vector2<uint32_t> dimensions{};
};

class SelectionRectangular : public Tool {
  public:
	SelectionRectangular() { type = ToolType::select; }
	void handle_events(Canvas& canvas, sf::Event& e);
	void handle_keyboard_events(Canvas& canvas, sf::Keyboard::Key& key);
	void update();
	void render(sf::RenderWindow& win, sf::Vector2<float> offset);
	void store_tile(int index);
	void copy(Canvas& canvas);
	void paste(Canvas& canvas);

  private:
	SelectBox selection{};
	Clipboard clipboard{};
};

} // namespace pi
