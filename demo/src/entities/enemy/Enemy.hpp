#pragma once

#include "../Entity.hpp"
#include "../../utils/Collider.hpp"
#include "../../utils/BitFlags.hpp"
#include "../animation/Animation.hpp"
#include "../../utils/StateFunction.hpp"
#include "../packages/Health.hpp"
#include "../packages/WeaponPackage.hpp"
#include "../packages/Caution.hpp"
#include "../packages/Attack.hpp"
#include "../packages/Shockwave.hpp"
#include "../player/Indicator.hpp"
#include <string_view>
#include <iostream>

namespace player {
class Player;
}

namespace arns {
class Projectile;
}

namespace enemy {

enum class GeneralFlags { mobile, gravity, player_collision, hurt_on_contact, map_collision, post_death_render, no_loot, custom_sounds };
enum class StateFlags { alive, alert, hostile, shot, vulnerable, hurt, shaking };
enum class Triggers { hostile, alert };
enum class Variant { beast, soldier, elemental, worker };
struct Attributes {
	float base_hp{};
	float base_damage{};
	float speed{};
	float loot_multiplier{};
	sf::Vector2<int> drop_range{};
};

struct Flags {
	util::BitFlags<GeneralFlags> general{};
	util::BitFlags<StateFlags> state{};
	util::BitFlags<Triggers> triggers{};
};

class Enemy : public entity::Entity {
  public:
	Enemy() = default;
	virtual ~Enemy() {}
	Enemy(automa::ServiceProvider& svc, std::string_view label);
	void update(automa::ServiceProvider& svc, world::Map& map, player::Player& player);
	void render(automa::ServiceProvider& svc, sf::RenderWindow& win, sf::Vector2<float> cam) override;
	void render_indicators(automa::ServiceProvider& svc, sf::RenderWindow& win, sf::Vector2<float> cam);
	virtual void unique_update(automa::ServiceProvider& svc, world::Map& map, player::Player& player){};
	virtual void unique_render(automa::ServiceProvider& svc, sf::RenderWindow& win, sf::Vector2<float> cam){};
	virtual void gui_render(automa::ServiceProvider& svc, sf::RenderWindow& win, sf::Vector2<float> cam){};
	void handle_player_collision(player::Player& player) const;
	void on_hit(automa::ServiceProvider& svc, world::Map& map, arms::Projectile& proj);
	[[nodiscard]] auto hostile() const -> bool { return flags.state.test(StateFlags::hostile); }
	[[nodiscard]] auto alert() const -> bool { return flags.state.test(StateFlags::alert); }
	[[nodiscard]] auto hostility_triggered() const -> bool { return flags.triggers.test(Triggers::hostile); }
	[[nodiscard]] auto alertness_triggered() const -> bool { return flags.triggers.test(Triggers::alert); }
	[[nodiscard]] auto get_attributes() const -> Attributes { return attributes; }
	[[nodiscard]] auto get_flags() const -> Flags { return flags; }
	[[nodiscard]] auto get_collider() -> shape::Collider& { return collider; }
	[[nodiscard]] auto died() const -> bool { return health.is_dead(); }
	[[nodiscard]] auto just_died() const -> bool { return health.is_dead() && post_death.get_cooldown() == afterlife; }
	[[nodiscard]] auto gone() const -> bool { return post_death.is_complete(); }
	[[nodiscard]] auto player_collision() const -> bool { return flags.general.test(GeneralFlags::player_collision); }
	[[nodiscard]] auto spawn_loot() const -> bool { return !flags.general.test(GeneralFlags::no_loot); }
	void set_position(sf::Vector2<float> pos) {
		collider.physics.position = pos;
		collider.sync_components();
		health_indicator.set_position(pos);
	}
	void hurt() { flags.state.set(StateFlags::hurt); }
	void shake() { flags.state.set(StateFlags::shaking); }
	void stop_shaking() { flags.state.reset(StateFlags::shaking); }

	entity::Health health{};
	player::Indicator health_indicator{};
	anim::Animation animation{};

  protected:
	std::string_view label{};
	shape::Collider collider{};
	shape::Collider secondary_collider{};
	std::vector<anim::Parameters> animation_parameters{};
	Flags flags{};
	Attributes attributes{};
	util::Cooldown post_death{};
	int afterlife{200};

	struct {
		int id{};
		std::string_view variant{};
	} metadata{};

	struct {
		std::vector<shape::Shape> hurtbox_atlas{};
		std::vector<shape::Shape> hurtboxes{};
		shape::Shape alert_range{};
		shape::Shape hostile_range{};
	} physical{};

	struct {
		int effect_type{};
		int effect_size{};
	} visual{};

	struct {
		sf::Sound hit{};
		sf::Sound inv_hit{};
	} sounds{};
};

} // namespace enemy