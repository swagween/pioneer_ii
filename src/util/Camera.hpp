//
//  Camera.hpp
//
//
//  Created by Alex Frasca on 12/26/22.
//
#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include "PhysicsComponent.hpp"

namespace pi {

const float CAM_FRICTION = 0.9f;
const float CAM_MASS = 1.0f;
const float CAM_GRAV = 0.003f;

const int CX_OFFSET = 60;
const int CY_OFFSET = 60;

const float CAM_BOUNDARY = 2048.0f;

const sf::Vector2<uint32_t> aspect_ratio { 3840, 2160 };
const sf::Vector2<uint32_t> screen_dimensions { aspect_ratio.x / 2, aspect_ratio.y / 2 };


class Camera {
public:
    
    Camera() {
        physics = components::PhysicsComponent({CAM_FRICTION, CAM_FRICTION}, CAM_MASS);
        bounding_box = sf::Rect<float>({0.0f, 0.0f}, {(float)screen_dimensions.x, (float)screen_dimensions.y});
    }
    
    void move(sf::Vector2<float> new_position) {
        physics.apply_force(physics.position - new_position);
        update();
    }
    
    void update() {
        physics.update_dampen();
        bounding_box.left = physics.position.x;
        bounding_box.top = physics.position.y;
    }
    
    void set_position(sf::Vector2<float> new_pos) {
        physics.position = new_pos;
    }
    
    void center(sf::Vector2<float> new_position) {
        float gx = physics.position.x;
        float gy = physics.position.y;
        float mx = new_position.x;
        float my = new_position.y;
        
        float force_x = mx - gx;
        float force_y = my - gy;
        float mag = sqrt((force_x*force_x) + (force_y*force_y));
        float str = CAM_GRAV / mag*mag;
        force_x *= str;
        force_y *= str;
        physics.apply_force({force_x, force_y});
        update();
    }
    
    sf::Rect<float> bounding_box{};
    components::PhysicsComponent physics{};
    
};

} // end namespace

/* Camera_hpp */
