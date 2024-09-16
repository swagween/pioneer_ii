//
//  Hand.cpp
//  Pioneer-Lab
//
//  Created by Alex Frasca on 10/3/20.
//

#include "Tool.hpp"

namespace pi {

void Hand::handle_events(Canvas& canvas, sf::Event& e) {
    if(ready) {
        if(just_clicked) {
            clicked_position = position;
            just_clicked = false;
        }
        relative_position = position - clicked_position;
        active = true;
    } else {
        clicked_position = {0.0f, 0.0f};
        relative_position = {0.0f, 0.0f};
    }
    update();
}

void Hand::handle_keyboard_events(Canvas& canvas, sf::Keyboard::Key& key) {
    
}

void Hand::update() {
    Tool::update();
}

void Hand::render(sf::RenderWindow& win, sf::Vector2<float> offset) {
    
}

void Hand::store_tile(int index) {

}

}
