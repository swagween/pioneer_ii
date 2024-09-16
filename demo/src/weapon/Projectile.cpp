
#include "Projectile.hpp"
#include "Weapon.hpp"
#include "../entities/player/Player.hpp"
#include "../service/ServiceProvider.hpp"

namespace arms {

Projectile::Projectile() {
	physics = components::PhysicsComponent({1.0f, 1.0f}, 1.0f);
	physics.velocity.x = stats.speed;
};

Projectile::Projectile(automa::ServiceProvider& svc, std::string_view label, int id, Weapon& weapon) : label(label), id(id), sparkler(svc), m_weapon(&weapon) {

	auto const& in_data = svc.data.weapon["weapons"][id]["projectile"];

	type = static_cast<WEAPON_TYPE>(id);

	stats.base_damage = in_data["attributes"]["base_damage"].as<float>();
	stats.range = in_data["attributes"]["range"].as<int>();
	stats.speed = in_data["attributes"]["speed"].as<float>();
	stats.variance = in_data["attributes"]["variance"].as<float>();
	stats.stun_time = in_data["attributes"]["stun_time"].as<float>();
	stats.knockback = in_data["attributes"]["knockback"].as<float>();
	stats.boomerang = (bool)in_data["attributes"]["boomerang"].as_bool();
	stats.persistent = (bool)in_data["attributes"]["persistent"].as_bool();
	stats.transcendent = (bool)in_data["attributes"]["transcendent"].as_bool();
	stats.constrained = (bool)in_data["attributes"]["constrained"].as_bool();
	stats.spring = (bool)in_data["attributes"]["spring"].as_bool();
	stats.range_variance = in_data["attributes"]["range_variance"].as<int>();
	stats.acceleration_factor = in_data["attributes"]["acceleration_factor"].as<float>();
	stats.dampen_factor = in_data["attributes"]["dampen_factor"].as<float>();
	stats.gravitator_force = in_data["attributes"]["gravitator_force"].as<float>();
	stats.gravitator_max_speed = in_data["attributes"]["gravitator_max_speed"].as<float>();
	stats.gravitator_friction = in_data["attributes"]["gravitator_friction"].as<float>();
	stats.spring_dampen = in_data["attributes"]["spring_dampen"].as<float>();
	stats.spring_constant = in_data["attributes"]["spring_constant"].as<float>();
	stats.spring_rest_length = in_data["attributes"]["spring_rest_length"].as<float>();
	stats.spring_slack = in_data["attributes"]["spring_slack"].as<float>();

	anim.num_sprites = in_data["animation"]["num_sprites"].as<int>();
	anim.num_frames = in_data["animation"]["num_frames"].as<int>();
	anim.framerate = in_data["animation"]["framerate"].as<int>();

	visual.effect_type = in_data["visual"]["effect_type"].as<int>();

	sf::Vector2<float> dim{};
	dim.x = in_data["dimensions"]["x"].as<float>();
	dim.y = in_data["dimensions"]["y"].as<float>();

	if (svc.styles.spray_colors.contains(label)) { sparkler = vfx::Sparkler(svc, dim, svc.styles.spray_colors.at(label), in_data["sparkler_type"].as_string()); }

	render_type = anim.num_sprites > 1 ? RENDER_TYPE::MULTI_SPRITE : RENDER_TYPE::SINGLE_SPRITE;

	physics = components::PhysicsComponent({1.0f, 1.0f}, 1.0f);
	physics.velocity.x = stats.speed;
	if (stats.dampen_factor != 0.f) { physics.set_global_friction(stats.dampen_factor); }

	// Gravitator
	gravitator = vfx::Gravitator(physics.position, svc.styles.colors.goldenrod, stats.gravitator_force);
	gravitator.collider.physics = components::PhysicsComponent(sf::Vector2<float>{stats.gravitator_friction, stats.gravitator_friction}, 1.0f);
	gravitator.collider.physics.maximum_velocity = {stats.gravitator_max_speed, stats.gravitator_max_speed};

	// spring
	hook = GrapplingHook(svc);
	hook.spring = vfx::Spring({stats.spring_dampen, stats.spring_constant, stats.spring_rest_length});

	anim::Parameters params = {0, anim.num_frames, anim.framerate, -1};
	animation.set_params(params);

	bounding_box.dimensions = dim;
	max_dimensions = dim;

	state.set(ProjectileState::initialized);
	cooldown.start(40);
	seed(svc);
	set_sprite(svc);
}

void Projectile::update(automa::ServiceProvider& svc, player::Player& player) {

	// animation
	animation.update();
	sparkler.update(svc);
	cooldown.update();

	if (stats.spring) {
		hook.update(svc, player);
		if (hook.grapple_flags.test(arms::GrappleState::probing)) {
			hook.spring.set_anchor(physics.position);
			hook.spring.set_bob(player.apparent_position);
			if (!player.controller.hook_held()) {
				hook.grapple_flags.set(arms::GrappleState::snaking);
				hook.grapple_flags.reset(arms::GrappleState::probing);
			}
		}
		if (hook.grapple_flags.test(arms::GrappleState::anchored)) { lock_to_anchor(); }
		if (hook.grapple_flags.test(arms::GrappleState::snaking)) {
			physics.position = hook.spring.get_bob();
			bounding_box.position = physics.position;
			if (bounding_box.overlaps(player.collider.predictive_combined) && cooldown.is_complete()) {
				destroy(true);
				hook.grapple_flags = {};
				hook.grapple_triggers = {};
				hook.spring.reverse_anchor_and_bob();
				hook.spring.set_rest_length(stats.spring_rest_length);
				svc.soundboard.flags.weapon.set(audio::Weapon::tomahawk_catch);
			} // destroy when player catches it
		}
	}

	// tomahawk-specific stuff
	if (stats.boomerang) {
		gravitator.set_target_position(player.apparent_position);
		gravitator.update(svc);
		physics.position = gravitator.collider.physics.position;
		bounding_box.set_position(physics.position);

		svc.soundboard.flags.weapon.set(svc.soundboard.gun_sounds.at(label)); // repeat sound
		// use predictive bounding box so player can "meet up" with the boomerang
		if (bounding_box.overlaps(player.collider.predictive_combined) && cooldown.is_complete()) {
			destroy(true);
			svc.soundboard.flags.weapon.set(audio::Weapon::tomahawk_catch);
		} // destroy when player catches it
	}

	constrain_hitbox_at_barrel();
	if (state.test(ProjectileState::destruction_initiated)) { constrain_hitbox_at_destruction_point(); }
	if (state.test(ProjectileState::destruction_initiated) && !stats.constrained) { destroy(true); }
	
	physics.update_euler(svc);

	if (direction.lr == dir::LR::left) {
		bounding_box.set_position(shape::Shape::Vec{physics.position.x, physics.position.y - bounding_box.dimensions.y / 2});
	} else if (direction.lr == dir::LR::right) {
		bounding_box.set_position(shape::Shape::Vec{physics.position.x - bounding_box.dimensions.x, physics.position.y - bounding_box.dimensions.y / 2});
	} else if (direction.und == dir::UND::up) {
		bounding_box.set_position(shape::Shape::Vec{physics.position.x - bounding_box.dimensions.x / 2, physics.position.y});
	} else if (direction.und == dir::UND::down) {
		bounding_box.set_position(shape::Shape::Vec{physics.position.x - bounding_box.dimensions.x / 2, physics.position.y - bounding_box.dimensions.y});
	}

	sparkler.set_position(bounding_box.position);
	sparkler.set_dimensions(bounding_box.dimensions);

	position_history.push_back(physics.position);
	if (position_history.size() > history_limit) { position_history.pop_front(); }

	if (direction.lr == dir::LR::left || direction.lr == dir::LR::right) {
		if (abs(physics.position.x - fired_point.x) >= stats.range) {
			state.set(ProjectileState::whiffed);
			destroy(false, true);
		}
	} else {
		if (abs(physics.position.y - fired_point.y) >= stats.range) {
			state.set(ProjectileState::whiffed);
			destroy(false, true);
		}
	}

	if (state.test(arms::ProjectileState::destroyed)) { --m_weapon->active_projectiles; }
}

void Projectile::render(automa::ServiceProvider& svc, player::Player& player, sf::RenderWindow& win, sf::Vector2<float>& campos) {

	// this is the right idea but needs to be refactored and generalized
	if (render_type == RENDER_TYPE::MULTI_SPRITE) {
		int u = sprite_index * (int)max_dimensions.x;
		int v = (int)(animation.get_frame() * max_dimensions.y);
		sprite.setTextureRect(sf::IntRect({u, v}, {(int)max_dimensions.x, (int)max_dimensions.y}));
		constrain_sprite_at_barrel(sprite, campos);
		if (state.test(ProjectileState::destruction_initiated)) { constrain_sprite_at_destruction_point(sprite, campos); }
		win.draw(sprite);

		return;
	}
		// get UV coords (only one column of sprites is supported)
		int u = 0;
		int v = (int)(animation.get_frame() * max_dimensions.y);
		sprite.setTextureRect(sf::IntRect({u, v}, {(int)max_dimensions.x, (int)max_dimensions.y}));

		// unconstrained projectiles have to get sprites set here
		if (stats.boomerang) { sprite.setPosition(gravitator.collider.physics.position - campos); }
		if (stats.spring) { hook.render(svc, player, win, campos); }
		if (stats.spring && hook.grapple_flags.test(GrappleState::snaking)) {
			sprite.setPosition(hook.spring.get_bob() - campos);
		} else if (stats.spring) {
			sprite.setPosition(hook.spring.get_anchor() - campos);
		}

		constrain_sprite_at_barrel(sprite, campos);
		if (state.test(ProjectileState::destruction_initiated)) { constrain_sprite_at_destruction_point(sprite, campos); }

		// proj bounding box for debug
		box.setSize(bounding_box.dimensions);
		if (state.test(ProjectileState::destruction_initiated)) {
			box.setFillColor(sf::Color{255, 255, 60, 160});
		} else {
			box.setFillColor(sf::Color{255, 255, 255, 160});
		}
		box.setPosition(bounding_box.position.x - campos.x, bounding_box.position.y - campos.y);

		if (svc.greyblock_mode()) {
			gravitator.render(svc, win, campos);
			win.draw(box);
		} else {
			win.draw(sprite);
		}
	
}

void Projectile::destroy(bool completely, bool whiffed) {

	if (!whiffed) { state.set(ProjectileState::contact); }

	if (completely) {
		state.set(ProjectileState::destroyed);
		return;
	}

	if (!state.test(ProjectileState::destruction_initiated)) {
		if (direction.lr == dir::LR::left || direction.und == dir::UND::up) {
			destruction_point = bounding_box.position;
		} else {
			destruction_point = bounding_box.position + bounding_box.dimensions;
		}
		state.set(ProjectileState::destruction_initiated);
	}

	stats.base_damage = 0;
}

void Projectile::seed(automa::ServiceProvider& svc) {

	float var = svc.random.random_range_float(-stats.variance, stats.variance);
	if (stats.spring) {
		physics.velocity = hook.probe_velocity(stats.speed);
		return;
	}
	switch (direction.lr) {
	case dir::LR::left: physics.velocity = {-stats.speed + (var / 2), var}; break;
	case dir::LR::right: physics.velocity = {stats.speed + (var / 2), var}; break;
	}
	switch (direction.und) {
	case dir::UND::up: physics.velocity = {var, -stats.speed + (var / 2)}; break;
	case dir::UND::down: physics.velocity = {var, stats.speed + (var / 2)}; break;
	}
}

void Projectile::set_sprite(automa::ServiceProvider& svc) {
	set_orientation(sprite);
	if (!svc.assets.projectile_textures.contains(label)) {
		//std::cout << label.data() << " missing from AssetManager tables.\n";
		return;
	}
	sprite.setTexture(svc.assets.projectile_textures.at(label));
	sprite_index = svc.random.random_range(0, anim.num_sprites - 1);
}

void Projectile::set_orientation(sf::Sprite& sprite) {
	// assume right
	sprite.setScale({1.0f, 1.0f});
	sprite.setRotation(0.0f);
	sprite.setOrigin(0, 0);

	switch (direction.lr) {
	case dir::LR::left: sprite.scale({-1.0f, 1.0f}); break;
	case dir::LR::right: break;
	}
	switch (direction.und) {
	case dir::UND::up: sprite.rotate(-90); break;
	case dir::UND::down: sprite.rotate(90); break;
	}
}

void Projectile::set_position(sf::Vector2<float> pos) {
	physics.position = pos;
	bounding_box.position = pos;
	gravitator.set_position(pos);
	fired_point = pos;
}

void Projectile::set_boomerang_speed() {
	gravitator.collider.physics.velocity.x = direction.lr == dir::LR::left ? -stats.speed : (direction.lr == dir::LR::right ? stats.speed : 0.f);
	gravitator.collider.physics.velocity.y = direction.und == dir::UND::up ? -stats.speed : (direction.und == dir::UND::down ? stats.speed : 0.f);
}

void Projectile::set_hook_speed() {
	//hook.spring.variables.physics.velocity.x = direction.lr == dir::LR::left ? -stats.speed : (direction.lr == dir::LR::right ? stats.speed : 0.f);
	//hook.spring.variables.physics.velocity.y = direction.und == dir::UND::up ? -stats.speed : (direction.und == dir::UND::down ? stats.speed : 0.f);
}

void Projectile::sync_position() { gravitator.collider.physics.position = fired_point; }

void Projectile::constrain_sprite_at_barrel(sf::Sprite& sprite, sf::Vector2<float> campos) {
	if (!stats.constrained) { return; }
	int u = (int)(sprite_index * max_dimensions.x);
	int v = (int)(animation.get_frame() * max_dimensions.y);
	if (direction.lr != dir::LR::neutral) {
		if (abs(physics.position.x - fired_point.x) < max_dimensions.x) {
			auto fwidth = abs(physics.position.x - fired_point.x);
			auto iwidth = static_cast<int>(fwidth);
			sprite.setTextureRect(sf::IntRect({(int)(max_dimensions.x - iwidth) + u, v}, {iwidth, (int)max_dimensions.y}));
			bounding_box.dimensions.x = fwidth;
		} else {
			sprite.setTextureRect(sf::IntRect({u, v}, {(int)(bounding_box.dimensions.x), (int)(bounding_box.dimensions.y)}));
			bounding_box.dimensions.x = max_dimensions.x;
		}
		if (direction.lr == dir::LR::right) {
			sprite.setPosition(bounding_box.position.x - campos.x, bounding_box.position.y - campos.y);
		} else if (direction.lr == dir::LR::left) {
			sprite.setPosition(bounding_box.position.x + bounding_box.dimensions.x - campos.x, bounding_box.position.y - campos.y);
		}

	} else {
		bounding_box.dimensions.x = max_dimensions.y;
		if (abs(physics.position.y - fired_point.y) < max_dimensions.x) {
			auto fheight = abs(physics.position.y - fired_point.y);
			auto iheight = static_cast<int>(fheight);
			sprite.setTextureRect(sf::IntRect({(int)(max_dimensions.x - iheight) + u, v}, {iheight, (int)max_dimensions.y}));
			bounding_box.dimensions.y = fheight;
		} else {
			sprite.setTextureRect(sf::IntRect({u, v}, {(int)(max_dimensions.x), (int)(max_dimensions.y)}));
			bounding_box.dimensions.y = max_dimensions.x;
		}
		if (direction.und == dir::UND::up) {
			sprite.setPosition(bounding_box.position.x - campos.x, bounding_box.position.y + bounding_box.dimensions.y - campos.y);
		} else if (direction.und == dir::UND::down) {
			sprite.setPosition(bounding_box.position.x + bounding_box.dimensions.x - campos.x, bounding_box.position.y - campos.y);
		}
	}
}

void Projectile::constrain_sprite_at_destruction_point(sf::Sprite& sprite, sf::Vector2<float> campos) {
	if (!stats.constrained) { return; }
	auto u = static_cast<int>(sprite_index * max_dimensions.x);
	auto v = static_cast<int>(animation.get_frame() * max_dimensions.y);
	if (direction.lr != dir::LR::neutral) {
		if (direction.lr == dir::LR::left) {
			auto rear = bounding_box.dimensions.x + physics.position.x;
			auto fwidth = abs(rear - destruction_point.x);
			auto iwidth = static_cast<int>(fwidth);
			sprite.setTextureRect(sf::IntRect({u, v}, {iwidth, (int)max_dimensions.y}));
			bounding_box.dimensions.x = fwidth;
			bounding_box.position.x = destruction_point.x;
			sprite.setPosition(bounding_box.position.x + bounding_box.dimensions.x - campos.x, bounding_box.position.y - campos.y);
			if (rear <= destruction_point.x) { destroy(true); }
		} else {
			auto rear = physics.position.x - bounding_box.dimensions.x;
			auto fwidth = abs(rear - destruction_point.x);
			auto iwidth = static_cast<int>(fwidth);
			sprite.setTextureRect(sf::IntRect({u, v}, {iwidth, (int)max_dimensions.y}));
			bounding_box.dimensions.x = fwidth;
			bounding_box.position.x = destruction_point.x - fwidth;
			sprite.setPosition(bounding_box.position.x - campos.x, bounding_box.position.y - campos.y);
			if (rear >= destruction_point.x) { destroy(true); }
		}

	} else {
		if (direction.und == dir::UND::up) {
			auto rear = bounding_box.dimensions.y + physics.position.y;
			auto fheight = abs(rear - destruction_point.y);
			auto iheight = static_cast<int>(fheight);
			sprite.setTextureRect(sf::IntRect({u, v}, {iheight, (int)max_dimensions.y}));
			bounding_box.dimensions.y = fheight;
			bounding_box.position.y = destruction_point.y;
			sprite.setPosition(bounding_box.position.x - campos.x, bounding_box.position.y + bounding_box.dimensions.y - campos.y);
			if (rear <= destruction_point.y) { destroy(true); }
		} else {
			auto rear = physics.position.y - bounding_box.dimensions.y;
			auto fheight = abs(rear - destruction_point.y);
			auto iheight = static_cast<int>(fheight);
			sprite.setTextureRect(sf::IntRect({u, v}, {iheight, (int)max_dimensions.y}));
			bounding_box.dimensions.y = fheight;
			bounding_box.position.y = destruction_point.y - fheight;
			sprite.setPosition(bounding_box.position.x + bounding_box.dimensions.x - campos.x, bounding_box.position.y - campos.y);
			if (rear >= destruction_point.y) { destroy(true); }
		}
	}
}

void Projectile::constrain_hitbox_at_barrel() {
	if (!stats.constrained) { return; }
	if (direction.lr != dir::LR::neutral) {
		if (abs(physics.position.x - fired_point.x) < max_dimensions.x) {
			auto width = abs(physics.position.x - fired_point.x);
			bounding_box.dimensions.x = width;
		} else {
			bounding_box.dimensions.x = max_dimensions.x;
		}
		if (direction.lr == dir::LR::right) {
		} else if (direction.lr == dir::LR::left) {
		}

	} else {
		bounding_box.dimensions.x = max_dimensions.y;
		if (abs(physics.position.y - fired_point.y) < max_dimensions.x) {
			auto height = abs(physics.position.y - fired_point.y);
			bounding_box.dimensions.y = height;
		} else {
			bounding_box.dimensions.y = max_dimensions.x;
		}
	}
}

void Projectile::constrain_hitbox_at_destruction_point() {
	if (!stats.constrained) { return; }
	if (direction.lr != dir::LR::neutral) {
		if (direction.lr == dir::LR::left) {
			auto rear = bounding_box.dimensions.x + physics.position.x;
			auto fwidth = abs(rear - destruction_point.x);
			bounding_box.dimensions.x = fwidth;
			bounding_box.position.x = destruction_point.x;
			if (rear <= destruction_point.x) { destroy(true); }
		} else {
			auto rear = physics.position.x - bounding_box.dimensions.x;
			auto fwidth = abs(rear - destruction_point.x);
			bounding_box.dimensions.x = fwidth;
			bounding_box.position.x = destruction_point.x - fwidth;
			if (rear >= destruction_point.x) { destroy(true); }
		}

	} else {
		if (direction.und == dir::UND::up) {
			auto rear = bounding_box.dimensions.y + physics.position.y;
			auto fheight = abs(rear - destruction_point.y);
			bounding_box.dimensions.y = fheight;
			bounding_box.position.y = destruction_point.y;
			if (rear <= destruction_point.y) { destroy(true); }
		} else {
			auto rear = physics.position.y - bounding_box.dimensions.y;
			auto fheight = abs(rear - destruction_point.y);
			bounding_box.dimensions.y = fheight;
			bounding_box.position.y = destruction_point.y - fheight;
			if (rear >= destruction_point.y) { destroy(true); }
		}
	}
}

void Projectile::lock_to_anchor() {
	physics.position = hook.spring.get_anchor() - bounding_box.dimensions * 0.5f;
	bounding_box.position = physics.position;
}

} // namespace arms
