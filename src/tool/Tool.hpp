//
//  Tool.hpp
//  Pioneer-Lab
//
//  Created by Alex Frasca on 10/3/20.
//

#pragma once

#include <stdio.h>
#include "../canvas/Canvas.hpp"
#include "../canvas/Clipboard.hpp"

namespace tool {

enum class TOOL_TYPE {
    BRUSH,
    FILL,
    SELECT,
    ERASE,
    HAND,
    ENTITY_PLACER
};

enum class ENTITY_TYPE {
	PORTAL,
	INSPECTABLE,
	CRITTER,
	CHEST,
    ANIMATOR,
    ENTITY_EDITOR,
    PLAYER_PLACER,
    PLATFORM,
    SAVE_POINT,
    SWITCH_BUTTON,
    SWITCH_BLOCK,
    BED
};


class Tool {
public:
    
    Tool& operator=(const Tool&) = delete;
    virtual void handle_events(canvas::Canvas& canvas, sf::Event& e) = 0;
    virtual void handle_keyboard_events(canvas::Canvas& canvas, sf::Keyboard::Key& key) = 0;
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
    
    int size = 1;
    
    bool primary{};
    bool trigger_switch{ false };
    uint8_t tile{};
    canvas::Portal current_portal{}; //ideally I wouldn't do this here, but I can't think of a better way
    canvas::Inspectable current_inspectable{};
    canvas::Animator current_animator{};
    canvas::Critter current_critter{};
	canvas::Platform current_platform{};
	canvas::Chest current_chest{};
	canvas::SwitchBlock current_switch_block{};
	canvas::SwitchButton current_switch{};
    
    TOOL_TYPE type{};
    ENTITY_TYPE ent_type{};
    
};

class Hand : public Tool {
public:
    Hand() { type = TOOL_TYPE::HAND; }
    void handle_events(canvas::Canvas& canvas, sf::Event& e);
    void handle_keyboard_events(canvas::Canvas& canvas, sf::Keyboard::Key& key);
    void update();
    void render(sf::RenderWindow& win, sf::Vector2<float> offset);
    void store_tile(int index);
};

class Brush : public Tool {
public:
    Brush() { type = TOOL_TYPE::BRUSH; }
    void handle_events(canvas::Canvas& canvas, sf::Event& e);
    void handle_keyboard_events(canvas::Canvas& canvas, sf::Keyboard::Key& key);
    void update();
    void render(sf::RenderWindow& win, sf::Vector2<float> offset);
    void store_tile(int index);
    
private:
};

class Erase : public Tool {
public:
    Erase() { type = TOOL_TYPE::ERASE; }
    void handle_events(canvas::Canvas& canvas, sf::Event& e);
    void handle_keyboard_events(canvas::Canvas& canvas, sf::Keyboard::Key& key);
    void update();
    void render(sf::RenderWindow& win, sf::Vector2<float> offset);
    void store_tile(int index);
    
private:
};

class Fill : public Tool {
public:
    Fill() { type = TOOL_TYPE::FILL; };
    void handle_events(canvas::Canvas& canvas, sf::Event& e);
    void handle_keyboard_events(canvas::Canvas& canvas, sf::Keyboard::Key& key);
    void update();
    void render(sf::RenderWindow& win, sf::Vector2<float> offset);
    void store_tile(int index);
    
    void fill_section(const uint8_t prev_val, const uint8_t new_val, uint32_t i, uint32_t j, canvas::Canvas& canvas);
};

class EntityPlacer : public Tool {
public:
    EntityPlacer() { type = TOOL_TYPE::ENTITY_PLACER; };
    void handle_events(canvas::Canvas& canvas, sf::Event& e);
    void handle_keyboard_events(canvas::Canvas& canvas, sf::Keyboard::Key& key);
    void update();
    void render(sf::RenderWindow& win, sf::Vector2<float> offset);
    void store_portal(sf::Vector2<uint32_t> dim, bool activate_on_contact, int src_id, int dest_id);
    void store_inspectable(sf::Vector2<uint32_t> dim, bool activate_on_contact, std::string);
    void store_critter(canvas::CRITTER_TYPE type);
    void store_animator(sf::Vector2<uint32_t> dim, int id, bool automatic, bool foreground);
    void store_tile(int index);
};

struct SelectBox {
    SelectBox() = default;
    SelectBox(sf::Vector2<uint32_t> pos, sf::Vector2<uint32_t> dim) : position(pos), dimensions(dim) {}
    void clear() { position = {0, 0}; dimensions = {0, 0}; }
    void adjust(sf::Vector2<uint32_t> adjustment) { dimensions = adjustment; }
    sf::Vector2<uint32_t> position{};
    sf::Vector2<uint32_t> dimensions{};
};

class SelectionRectangular : public Tool {
public:
    
    SelectionRectangular() { type = TOOL_TYPE::SELECT; }
    void handle_events(canvas::Canvas& canvas, sf::Event& e);
    void handle_keyboard_events(canvas::Canvas& canvas, sf::Keyboard::Key& key);
    void update();
    void render(sf::RenderWindow& win, sf::Vector2<float> offset);
    void store_tile(int index);
    void copy(canvas::Canvas& canvas);
    void paste(canvas::Canvas& canvas);
    
private:
    
    SelectBox selection{};
    canvas::Clipboard clipboard{};
    
};

}
