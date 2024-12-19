
#pragma once

#include <stdio.h>
#include <optional>
#include "../canvas/Canvas.hpp"
#include "../canvas/Clipboard.hpp"
#include "../util/BitFlags.hpp"

namespace pi {

enum class ToolType { brush, fill, select, erase, hand, entity_placer, eyedropper };
enum class ENTITY_TYPE { PORTAL, INSPECTABLE, CRITTER, CHEST, ANIMATOR, ENTITY_EDITOR, PLAYER_PLACER, PLATFORM, SAVE_POINT, SWITCH_BUTTON, SWITCH_BLOCK, BED, INTERACTIVE_SCENERY, SCENERY };

enum class SelectMode { none, select, clipboard };

class Tool {
  public:
	Tool& operator=(Tool const&) = delete;
	virtual void handle_events(Canvas& canvas, sf::Event& e) = 0;
	virtual void handle_keyboard_events(Canvas& canvas, sf::Keyboard::Key& key) = 0;
	virtual void update() = 0;
	virtual void render(Canvas& canvas, sf::RenderWindow& win, sf::Vector2<float> offset, bool transformed = true) = 0;
	virtual void store_tile(int index) = 0;
	virtual void clear() = 0;

	void set_position(sf::Vector2<float> to_position); 
	void set_window_position(sf::Vector2<float> to_position);
	void reset();
	void activate();
	
	[[nodiscard]] auto f_position() const -> sf::Vector2<float> { return position; }
	[[nodiscard]] auto scaled_position() const -> sf::Vector2<uint32_t> { return {static_cast<uint32_t>(position.x / 32.f), static_cast<uint32_t>(position.y / 32.f)}; }
	[[nodiscard]] auto f_scaled_position() const -> sf::Vector2<float> { return {static_cast<float>(scaled_position().x), static_cast<float>(scaled_position().y)}; }
	[[nodiscard]] auto scaled_clicked_position() const -> sf::Vector2<uint32_t> { return {static_cast<uint32_t>(clicked_position.x / 32.f), static_cast<uint32_t>(clicked_position.y / 32.f)}; }
	[[nodiscard]] auto get_window_position() const -> sf::Vector2<float> { return window_position; }
	[[nodiscard]] auto get_window_position_scaled() const -> sf::Vector2<float> { return window_position / 32.f; }
	[[nodiscard]] auto palette_interactable() const -> bool { return type == ToolType::select || type == ToolType::eyedropper; }
	[[nodiscard]] auto is_active() const -> bool { return active; }
	[[nodiscard]] auto clipboard() const -> bool { return mode == SelectMode::clipboard; }

	bool in_bounds(sf::Vector2<uint32_t>& bounds) const;
	bool is_paintable() const;

	bool pervasive{};
	bool ready{};
	bool just_clicked = true;
	bool has_palette_selection{};

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
	InteractiveScenery current_interactive_scenery{};
	Scenery current_scenery{};

	ToolType type{};
	ENTITY_TYPE ent_type{};

  protected:
	bool active{};
	SelectMode mode{};
	float scale{};
	sf::Vector2<float> position{};
	sf::Vector2<float> clicked_position{};
	sf::Vector2<float> relative_position{};
	sf::Vector2<float> window_position{};
};

class Hand : public Tool {
  public:
	Hand() { type = ToolType::hand; }
	void handle_events(Canvas& canvas, sf::Event& e);
	void handle_keyboard_events(Canvas& canvas, sf::Keyboard::Key& key);
	void update();
	void render(Canvas& canvas, sf::RenderWindow& win, sf::Vector2<float> offset, bool transformed = true);
	void store_tile(int index);
	void clear();
};

class Brush : public Tool {
  public:
	Brush() { type = ToolType::brush; }
	void handle_events(Canvas& canvas, sf::Event& e);
	void handle_keyboard_events(Canvas& canvas, sf::Keyboard::Key& key);
	void update();
	void render(Canvas& canvas, sf::RenderWindow& win, sf::Vector2<float> offset, bool transformed = true);
	void store_tile(int index);
	void clear();

  private:
};

class Erase : public Tool {
  public:
	Erase() { type = ToolType::erase; }
	void handle_events(Canvas& canvas, sf::Event& e);
	void handle_keyboard_events(Canvas& canvas, sf::Keyboard::Key& key);
	void update();
	void render(Canvas& canvas, sf::RenderWindow& win, sf::Vector2<float> offset, bool transformed = true);
	void store_tile(int index);
	void clear();

  private:
};

class Fill : public Tool {
  public:
	Fill() { type = ToolType::fill; };
	void handle_events(Canvas& canvas, sf::Event& e);
	void handle_keyboard_events(Canvas& canvas, sf::Keyboard::Key& key);
	void update();
	void render(Canvas& canvas, sf::RenderWindow& win, sf::Vector2<float> offset, bool transformed = true);
	void store_tile(int index);
	void clear();

	void fill_section(uint8_t const prev_val, uint8_t const new_val, uint32_t i, uint32_t j, Canvas& canvas);
};

class EntityPlacer : public Tool {
  public:
	EntityPlacer() { type = ToolType::entity_placer; };
	void handle_events(Canvas& canvas, sf::Event& e);
	void handle_keyboard_events(Canvas& canvas, sf::Keyboard::Key& key);
	void update();
	void render(Canvas& canvas, sf::RenderWindow& win, sf::Vector2<float> offset, bool transformed = true);
	void store_portal(sf::Vector2<uint32_t> dim, bool activate_on_contact, int src_id, int dest_id);
	void store_inspectable(sf::Vector2<uint32_t> dim, bool activate_on_contact, std::string);
	void store_critter(int type);
	void store_animator(sf::Vector2<uint32_t> dim, int id, bool automatic, bool foreground);
	void store_tile(int index);
	void clear();
};

struct SelectBox {
	SelectBox() = default;
	SelectBox(sf::Vector2<uint32_t> pos, sf::Vector2<uint32_t> dim) : position(pos), dimensions(dim) {}
	void clear() {
		position = {0, 0};
		dimensions = {0, 0};
	}
	void adjust(sf::Vector2<uint32_t> adjustment) { dimensions = adjustment; }
	[[nodiscard]] auto f_position() const -> sf::Vector2<float> { return {static_cast<float>(position.x), static_cast<float>(position.y)}; }
	[[nodiscard]] auto empty() const -> bool { return dimensions.x * dimensions.y == 0; }
	sf::Vector2<uint32_t> position{};
	sf::Vector2<uint32_t> dimensions{};
};


class SelectionRectangular : public Tool {
  public:
	SelectionRectangular() { type = ToolType::select; }
	void handle_events(Canvas& canvas, sf::Event& e);
	void handle_keyboard_events(Canvas& canvas, sf::Keyboard::Key& key);
	void update();
	void render(Canvas& canvas, sf::RenderWindow& win, sf::Vector2<float> offset, bool transformed = true);
	void store_tile(int index);
	void cut(Canvas& canvas);
	void copy(Canvas& canvas);
	void paste(Canvas& canvas);
	void clear();

  private:
	SelectBox selection{};
	std::optional<Clipboard> clipboard{};
};

class Eyedropper : public Tool {
  public:
	Eyedropper() { type = ToolType::eyedropper; }
	void handle_events(Canvas& canvas, sf::Event& e);
	void handle_keyboard_events(Canvas& canvas, sf::Keyboard::Key& key);
	void update();
	void render(Canvas& canvas, sf::RenderWindow& win, sf::Vector2<float> offset, bool transformed = true);
	void store_tile(int index);
	void clear();
};

} // namespace pi
