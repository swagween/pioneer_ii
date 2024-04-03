//
//  Bestiary.hpp
//  critter
//
//

#pragma once

#include <vector>
#include <cmath>
#include <random>
#include <list>
#include <optional>
#include "Critter.hpp"
#include "../../components/BehaviorComponent.hpp"
#include "critters/Frdog.hpp"
#include "critters/Hulmet.hpp"

namespace critter {

    inline std::unordered_map<int, CRITTER_TYPE> get_critter_type{
    {0, CRITTER_TYPE::hulmet},
    {1, CRITTER_TYPE::tank},
    {2, CRITTER_TYPE::bunker},
    {3, CRITTER_TYPE::thug},
    {4, CRITTER_TYPE::worker},
    {5, CRITTER_TYPE::frdog},
    {6, CRITTER_TYPE::jackdaw},
    {7, CRITTER_TYPE::frcat},
    {8, CRITTER_TYPE::biter},
    {9, CRITTER_TYPE::bee},
    {10, CRITTER_TYPE::roller},
    {11, CRITTER_TYPE::snek},
    {12, CRITTER_TYPE::behemoth},
    {13, CRITTER_TYPE::stinger},
    {14, CRITTER_TYPE::watchman},
    {15, CRITTER_TYPE::gnat},
    {16, CRITTER_TYPE::moth},
    };

    const int pool_size_singular{ 1 };
    const int pool_size_small{ 16 };
    const int pool_size_large{ 32 };
    const int num_critters{ 17 };

    /*
    CritterMetadata { id, variant, hostile, hurt_on_contact, gravity }
    CritterStats    { base_damage, base_hp, speed, loot_multiplier, energy, vision }
    */

inline Critter frdog = Critter(CritterMetadata{0, VARIANT::BEAST, true, false, true}, CritterStats{3, 35, 0.3f, 2.0f, 80, 12 * 32}, sf::Vector2<int>{72, 48}, sf::Vector2<int>{4, 5}, sf::Vector2<float>{32.0f, 32.0f});
inline Critter hulmet = Critter(CritterMetadata{1, VARIANT::SOLDIER, true, false, true }, CritterStats{ 2, 14, 0.5f, 1.0f, 16 * 32 }, sf::Vector2<int>{56, 42}, sf::Vector2<int>{1, 14}, sf::Vector2<float>{16.0f, 16.0f});

inline std::array<Frdog, pool_size_small> frdog_pool{};
inline std::array<Hulmet, pool_size_small> hulmet_pool{};

inline std::array<int, num_critters> pool_counter{};

class Bestiary {
public:
    Bestiary() = default;
    ~Bestiary() { }
    Bestiary& operator=(Bestiary&&) = delete;
    std::optional<Critter*> fetch_critter_of_type(CRITTER_TYPE type, int next) {
        switch (type) {
        case CRITTER_TYPE::hulmet:
            if (next > pool_size_small) { return std::nullopt; }
            return &hulmet_pool.at(next);
        case CRITTER_TYPE::frdog:
            if (next > pool_size_small) { return std::nullopt; }
            return &frdog_pool.at(next);
        }
        return std::nullopt;
    }
};


} // end critter

/* Bestiary_hpp */
