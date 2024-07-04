#include "PlayerAnimation.hpp"
#include "Player.hpp"
#include "../../service/ServiceProvider.hpp"

namespace player {

PlayerAnimation::PlayerAnimation(Player& plr) : m_player(&plr) {
	state_function = state_function();
	animation.set_params(idle);
	animation.start();
	state = AnimState::idle;
}

void PlayerAnimation::update() {
	animation.update();
	state_function = state_function();
	if (m_player->is_dead()) { state = AnimState::die; }
}

void PlayerAnimation::start() { animation.start(); }

int PlayerAnimation::get_frame() const { return animation.get_frame(); }

bool PlayerAnimation::not_jumping() { return !(state == AnimState::jumpsquat || state == AnimState::rise); }

fsm::StateFunction PlayerAnimation::update_idle() {
	animation.label = "idle";
	if (change_state(AnimState::die, die, true)) { return PA_BIND(update_die); }
	if (animation.just_started()) { idle_timer.start(); }
	idle_timer.update();
	if (idle_timer.get_count() > timers.sit) { state = AnimState::sit; }
	if (change_state(AnimState::sit, sit)) {
		idle_timer.cancel();
		return PA_BIND(update_sit);
	}
	if (change_state(AnimState::sharp_turn, sharp_turn)) { return PA_BIND(update_sharp_turn); }
	if (change_state(AnimState::jumpsquat, jumpsquat)) { return PA_BIND(update_jumpsquat); }
	if (change_state(AnimState::rise, rise)) { return PA_BIND(update_rise); }
	if (change_state(AnimState::wallslide, wallslide)) { return PA_BIND(update_wallslide); }
	if (change_state(AnimState::run, run)) { return PA_BIND(update_run); }
	if (change_state(AnimState::dash, dash)) { return PA_BIND(update_dash); }
	if (change_state(AnimState::sprint, sprint)) { return PA_BIND(update_sprint); }
	if (change_state(AnimState::suspend, suspend)) { return PA_BIND(update_suspend); }
	if (change_state(AnimState::fall, fall)) { return PA_BIND(update_fall); }
	if (change_state(AnimState::inspect, inspect)) { return PA_BIND(update_inspect); }
	if (change_state(AnimState::shield, shield)) { return PA_BIND(update_shield); }
	if (change_state(AnimState::hurt, hurt)) { return PA_BIND(update_hurt); }
	if (change_state(AnimState::turn, turn)) { return PA_BIND(update_turn); }

	state = AnimState::idle;
	return std::move(state_function);
}

fsm::StateFunction PlayerAnimation::update_sprint() {
	animation.label = "sprint";
	if (change_state(AnimState::die, die, true)) { return PA_BIND(update_die); }
	if (change_state(AnimState::rise, rise)) { return PA_BIND(update_rise); }
	if (change_state(AnimState::inspect, inspect)) { return PA_BIND(update_inspect); }
	if (change_state(AnimState::stop, stop)) { return PA_BIND(update_stop); }
	if (change_state(AnimState::jumpsquat, jumpsquat)) { return PA_BIND(update_jumpsquat); }
	if (change_state(AnimState::wallslide, wallslide)) { return PA_BIND(update_wallslide); }
	if (change_state(AnimState::run, run)) { return PA_BIND(update_run); }
	if (change_state(AnimState::dash, dash)) { return PA_BIND(update_dash); }
	if (change_state(AnimState::suspend, suspend)) { return PA_BIND(update_suspend); }
	if (change_state(AnimState::fall, fall)) { return PA_BIND(update_fall); }
	if (change_state(AnimState::idle, idle)) { return PA_BIND(update_idle); }
	if (change_state(AnimState::shield, shield)) { return PA_BIND(update_shield); }
	if (change_state(AnimState::hurt, hurt)) { return PA_BIND(update_hurt); }
	if (change_state(AnimState::sharp_turn, sharp_turn)) { return PA_BIND(update_sharp_turn); }
	if (change_state(AnimState::turn, turn)) { return PA_BIND(update_turn); }

	state = AnimState::sprint;
	return std::move(state_function);
}

fsm::StateFunction PlayerAnimation::update_shield() {
	animation.label = "shield";
	if (change_state(AnimState::die, die, true)) { return PA_BIND(update_die); }
	if (change_state(AnimState::sprint, sprint)) { return PA_BIND(update_sprint); }
	if (change_state(AnimState::run, run)) { return PA_BIND(update_run); }
	if (change_state(AnimState::idle, idle)) { return PA_BIND(update_idle); }
	if (change_state(AnimState::hurt, hurt)) { return PA_BIND(update_hurt); }
	if (change_state(AnimState::jumpsquat, jumpsquat)) { return PA_BIND(update_jumpsquat); }
	if (change_state(AnimState::dash, dash)) { return PA_BIND(update_dash); }

	state = AnimState::shield;
	return PA_BIND(update_shield);
}

fsm::StateFunction PlayerAnimation::update_run() {
	animation.label = "run";
	if (change_state(AnimState::die, die, true)) { return PA_BIND(update_die); }
	if (change_state(AnimState::sprint, sprint)) { return PA_BIND(update_sprint); }
	if (change_state(AnimState::dash, dash)) { return PA_BIND(update_dash); }
	if (change_state(AnimState::rise, rise)) { return PA_BIND(update_rise); }
	if (change_state(AnimState::stop, stop)) { return PA_BIND(update_stop); }
	if (change_state(AnimState::jumpsquat, jumpsquat)) { return PA_BIND(update_jumpsquat); }
	if (change_state(AnimState::wallslide, wallslide)) { return PA_BIND(update_wallslide); }
	if (change_state(AnimState::suspend, suspend)) { return PA_BIND(update_suspend); }
	if (change_state(AnimState::fall, fall)) { return PA_BIND(update_fall); }
	if (change_state(AnimState::inspect, inspect)) { return PA_BIND(update_inspect); }
	if (change_state(AnimState::shield, shield)) { return PA_BIND(update_shield); }
	if (change_state(AnimState::hurt, hurt)) { return PA_BIND(update_hurt); }
	if (change_state(AnimState::idle, idle)) { return PA_BIND(update_idle); }
	if (change_state(AnimState::sharp_turn, sharp_turn)) { return PA_BIND(update_sharp_turn); }
	if (change_state(AnimState::turn, turn)) { return PA_BIND(update_turn); }

	state = AnimState::run;
	return std::move(state_function);
}

fsm::StateFunction PlayerAnimation::update_jumpsquat() {
	//no jumpsquat for now
	if (change_state(AnimState::die, die, true)) { return PA_BIND(update_die); }
	state = AnimState::rise;
	animation.set_params(rise);
	return PA_BIND(update_rise);
	animation.label = "jumpsquat";
	if (animation.complete()) {
		if (change_state(AnimState::dash, dash)) { return PA_BIND(update_dash); }

	}

	state = AnimState::jumpsquat;
	return PA_BIND(update_jumpsquat);
}

fsm::StateFunction PlayerAnimation::update_turn() {
	animation.label = "turn";
	if (change_state(AnimState::die, die, true)) { return PA_BIND(update_die); }
	if (animation.complete()) {
		triggers.set(AnimTriggers::flip);
		if (change_state(AnimState::sprint, sprint)) { return PA_BIND(update_sprint); }
		if (change_state(AnimState::run, run)) { return PA_BIND(update_run); }
		if (change_state(AnimState::dash, dash)) { return PA_BIND(update_dash); }
		if (change_state(AnimState::jumpsquat, jumpsquat)) { return PA_BIND(update_jumpsquat); }
		if (change_state(AnimState::suspend, suspend)) { return PA_BIND(update_suspend); }
		if (change_state(AnimState::inspect, inspect)) { return PA_BIND(update_inspect); }
		if (change_state(AnimState::rise, rise)) { return PA_BIND(update_rise); }
		if (change_state(AnimState::shield, shield)) { return PA_BIND(update_shield); }
		if (change_state(AnimState::hurt, hurt)) { return PA_BIND(update_hurt); }

		state = AnimState::idle;
		animation.set_params(idle);
		return PA_BIND(update_idle);
	}

	state = AnimState::turn;
	return PA_BIND(update_turn);
}

fsm::StateFunction PlayerAnimation::update_sharp_turn() {
	animation.label = "sharp_turn";
	if (change_state(AnimState::die, die, true)) { return PA_BIND(update_die); }
	if (animation.complete()) {
		triggers.set(AnimTriggers::flip);
		if (change_state(AnimState::sprint, sprint)) { return PA_BIND(update_sprint); }
		if (change_state(AnimState::run, run)) { return PA_BIND(update_run); }
		if (change_state(AnimState::jumpsquat, jumpsquat)) { return PA_BIND(update_jumpsquat); }
		if (change_state(AnimState::suspend, suspend)) { return PA_BIND(update_suspend); }
		if (change_state(AnimState::inspect, inspect)) { return PA_BIND(update_inspect); }
		if (change_state(AnimState::rise, rise)) { return PA_BIND(update_rise); }
		if (change_state(AnimState::shield, shield)) { return PA_BIND(update_shield); }
		if (change_state(AnimState::hurt, hurt)) { return PA_BIND(update_hurt); }

		state = AnimState::idle;
		animation.set_params(idle);
		return PA_BIND(update_idle);
	}

	state = AnimState::sharp_turn;
	return PA_BIND(update_sharp_turn);
}

fsm::StateFunction PlayerAnimation::update_rise() {
	animation.label = "rise";
	if (change_state(AnimState::die, die, true)) { return PA_BIND(update_die); }
	if (change_state(AnimState::suspend, suspend)) { return PA_BIND(update_suspend); }
	if (change_state(AnimState::idle, idle)) { return PA_BIND(update_idle); }
	if (change_state(AnimState::dash, dash)) { return PA_BIND(update_dash); }
	if (change_state(AnimState::hurt, hurt)) { return PA_BIND(update_hurt); }

	state = AnimState::rise;
	return std::move(state_function);
}

fsm::StateFunction PlayerAnimation::update_suspend() {
	animation.label = "suspend";
	if (change_state(AnimState::die, die, true)) { return PA_BIND(update_die); }
	if (change_state(AnimState::wallslide, wallslide)) { return PA_BIND(update_wallslide); }
	if (change_state(AnimState::hurt, hurt)) { return PA_BIND(update_hurt); }
	if (change_state(AnimState::fall, fall)) { return PA_BIND(update_fall); }
	if (change_state(AnimState::land, land)) { return PA_BIND(update_land); }
	if (change_state(AnimState::run, run)) { return PA_BIND(update_run); }
	if (change_state(AnimState::sprint, sprint)) { return PA_BIND(update_sprint); }
	if (change_state(AnimState::idle, idle)) { return PA_BIND(update_idle); }
	if (change_state(AnimState::dash, dash)) { return PA_BIND(update_dash); }

	state = AnimState::suspend;
	return std::move(state_function);
}

fsm::StateFunction PlayerAnimation::update_fall() {
	animation.label = "fall";
	if (change_state(AnimState::die, die, true)) { return PA_BIND(update_die); }
	if (change_state(AnimState::hurt, hurt)) { return PA_BIND(update_hurt); }
	if (change_state(AnimState::land, land)) { return PA_BIND(update_land); }
	if (change_state(AnimState::jumpsquat, jumpsquat)) { return PA_BIND(update_jumpsquat); }
	if (change_state(AnimState::wallslide, wallslide)) { return PA_BIND(update_wallslide); }
	if (change_state(AnimState::rise, rise)) { return PA_BIND(update_rise); }
	if (change_state(AnimState::run, run)) { return PA_BIND(update_run); }
	if (change_state(AnimState::sprint, sprint)) { return PA_BIND(update_sprint); }
	if (change_state(AnimState::inspect, inspect)) { return PA_BIND(update_inspect); }
	if (change_state(AnimState::dash, dash)) { return PA_BIND(update_dash); }
	if (change_state(AnimState::idle, idle)) { return PA_BIND(update_idle); }

	state = AnimState::fall;
	return std::move(state_function);
}

fsm::StateFunction PlayerAnimation::update_stop() {
	animation.label = "stop";
	if (change_state(AnimState::die, die, true)) { return PA_BIND(update_die); }

	if (animation.complete()) {

		state = AnimState::idle;
		animation.set_params(idle);
		return PA_BIND(update_idle);
	}
	if (change_state(AnimState::sharp_turn, sharp_turn)) { return PA_BIND(update_sharp_turn); }
	if (change_state(AnimState::sprint, sprint)) { return PA_BIND(update_sprint); }
	if (change_state(AnimState::run, run)) { return PA_BIND(update_run); }
	if (change_state(AnimState::dash, dash)) { return PA_BIND(update_dash); }
	if (change_state(AnimState::jumpsquat, jumpsquat)) { return PA_BIND(update_jumpsquat); }
	if (change_state(AnimState::suspend, suspend)) { return PA_BIND(update_suspend); }
	if (change_state(AnimState::inspect, inspect)) { return PA_BIND(update_inspect); }
	if (change_state(AnimState::turn, turn)) { return PA_BIND(update_turn); }
	if (change_state(AnimState::shield, shield)) { return PA_BIND(update_shield); }

	state = AnimState::stop;
	return PA_BIND(update_stop);
}

fsm::StateFunction PlayerAnimation::update_inspect() {
	animation.label = "inspect";
	if (change_state(AnimState::die, die, true)) { return PA_BIND(update_die); }
	if (animation.complete()) {
		if (change_state(AnimState::sprint, sprint)) { return PA_BIND(update_sprint); }
		if (change_state(AnimState::run, run)) { return PA_BIND(update_run); }
		if (change_state(AnimState::idle, idle)) { return PA_BIND(update_idle); }
		if (change_state(AnimState::shield, shield)) { return PA_BIND(update_shield); }
		if (change_state(AnimState::hurt, hurt)) { return PA_BIND(update_hurt); }
		if (change_state(AnimState::turn, turn)) { return PA_BIND(update_turn); }
	}
	if (change_state(AnimState::jumpsquat, jumpsquat)) { return PA_BIND(update_jumpsquat); }
	if (change_state(AnimState::dash, dash)) { return PA_BIND(update_dash); }

	state = AnimState::inspect;
	return PA_BIND(update_inspect);
}

fsm::StateFunction PlayerAnimation::update_sit() {
	animation.label = "sit";
	if (change_state(AnimState::die, die, true)) { return PA_BIND(update_die); }
	if (animation.complete()) {
		if (change_state(AnimState::sprint, sprint)) { return PA_BIND(update_sprint); }
		if (change_state(AnimState::run, run)) { return PA_BIND(update_run); }
		if (change_state(AnimState::shield, shield)) { return PA_BIND(update_shield); }
		if (change_state(AnimState::hurt, hurt)) { return PA_BIND(update_hurt); }
		if (change_state(AnimState::idle, idle)) { return PA_BIND(update_idle); }
		if (change_state(AnimState::turn, turn)) { return PA_BIND(update_turn); }
	}
	if (change_state(AnimState::jumpsquat, jumpsquat)) {
		animation.end();
		return PA_BIND(update_jumpsquat);
	}
	if (change_state(AnimState::dash, dash)) {
		animation.end();
		return PA_BIND(update_dash);
	}

	state = AnimState::sit;
	return PA_BIND(update_sit);
}

fsm::StateFunction PlayerAnimation::update_land() {
	animation.label = "land";
	if (change_state(AnimState::die, die, true)) { return PA_BIND(update_die); }
	if (change_state(AnimState::inspect, inspect)) { return PA_BIND(update_inspect); }
	if (change_state(AnimState::rise, rise)) { return PA_BIND(update_rise); }
	if (animation.complete()) {
		if (change_state(AnimState::sprint, sprint)) { return PA_BIND(update_sprint); }
		if (change_state(AnimState::run, run)) { return PA_BIND(update_run); }
		if (change_state(AnimState::jumpsquat, jumpsquat)) { return PA_BIND(update_jumpsquat); }
		if (change_state(AnimState::shield, shield)) { return PA_BIND(update_shield); }
		if (change_state(AnimState::hurt, hurt)) { return PA_BIND(update_hurt); }
		if (change_state(AnimState::turn, turn)) { return PA_BIND(update_turn); }

		state = AnimState::idle;
		animation.set_params(idle);
		return PA_BIND(update_idle);
	}
	state = AnimState::land;
	return PA_BIND(update_land);
}

fsm::StateFunction PlayerAnimation::update_hurt() {
	animation.label = "hurt";
	if (change_state(AnimState::die, die, true)) { return PA_BIND(update_die); }
	if (animation.complete()) {
		if (change_state(AnimState::sharp_turn, sharp_turn)) { return PA_BIND(update_sharp_turn); }
		if (change_state(AnimState::jumpsquat, jumpsquat)) { return PA_BIND(update_jumpsquat); }
		if (change_state(AnimState::wallslide, wallslide)) { return PA_BIND(update_wallslide); }
		if (change_state(AnimState::run, run)) { return PA_BIND(update_run); }
		if (change_state(AnimState::sprint, sprint)) { return PA_BIND(update_sprint); }
		if (change_state(AnimState::suspend, suspend)) { return PA_BIND(update_suspend); }
		if (change_state(AnimState::fall, fall)) { return PA_BIND(update_fall); }
		if (change_state(AnimState::inspect, inspect)) { return PA_BIND(update_inspect); }
		if (change_state(AnimState::land, land)) { return PA_BIND(update_land); }
		if (change_state(AnimState::shield, shield)) { return PA_BIND(update_shield); }
		if (change_state(AnimState::turn, turn)) { return PA_BIND(update_turn); }

		state = AnimState::idle;
		animation.set_params(idle);
		return PA_BIND(update_idle);
	}
	if (change_state(AnimState::dash, dash)) { return PA_BIND(update_dash); }

	state = AnimState::hurt;
	return std::move(state_function);
}

fsm::StateFunction PlayerAnimation::update_dash() {
	animation.label = "dash";
	if (change_state(AnimState::die, die, true)) { return PA_BIND(update_die); }
	if (animation.complete()) {
		if (change_state(AnimState::sharp_turn, sharp_turn)) { return PA_BIND(update_sharp_turn); }
		if (change_state(AnimState::sprint, sprint)) { return PA_BIND(update_sprint); }
		if (change_state(AnimState::run, run)) { return PA_BIND(update_run); }
		if (change_state(AnimState::wallslide, wallslide)) { return PA_BIND(update_wallslide); }
		if (change_state(AnimState::jumpsquat, jumpsquat)) { return PA_BIND(update_jumpsquat); }
		if (change_state(AnimState::suspend, suspend)) { return PA_BIND(update_suspend); }
		if (change_state(AnimState::fall, fall)) { return PA_BIND(update_fall); }
		if (change_state(AnimState::land, land)) { return PA_BIND(update_land); }
		if (change_state(AnimState::shield, shield)) { return PA_BIND(update_shield); }
		if (change_state(AnimState::hurt, hurt)) { return PA_BIND(update_hurt); }

		state = AnimState::idle;
		animation.set_params(idle);
		return PA_BIND(update_idle);
	}

	state = AnimState::dash;
	return std::move(state_function);
}

fsm::StateFunction PlayerAnimation::update_wallslide() {
	animation.label = "wallslide";
	if (change_state(AnimState::die, die, true)) { return PA_BIND(update_die); }
	if (change_state(AnimState::run, run)) { return PA_BIND(update_run); }
	if (change_state(AnimState::suspend, suspend)) { return PA_BIND(update_suspend); }
	if (change_state(AnimState::fall, fall)) { return PA_BIND(update_fall); }
	if (change_state(AnimState::land, land)) { return PA_BIND(update_land); }
	if (change_state(AnimState::hurt, hurt)) { return PA_BIND(update_hurt); }

	state = AnimState::wallslide;
	return PA_BIND(update_wallslide);
}

fsm::StateFunction PlayerAnimation::update_die() {
	animation.label = "die";
	if (animation.just_started()) {
		m_player->m_services->music.stop();
		post_death.start();
		triggers.reset(AnimTriggers::end_death);
		m_player->m_services->state_controller.actions.set(automa::Actions::death_mode); //set here, reset on map load
		std::cout << "Death animation started.\n";
	}
	m_player->controller.restrict_movement();
	m_player->controller.prevent_movement();
	post_death.update();
	if (!m_player->m_services->death_mode()) {
		if (change_state(AnimState::idle, idle, true)) { return PA_BIND(update_idle); }
	}
	if (post_death.is_complete()) {
		if (change_state(AnimState::idle, idle, true)) { return PA_BIND(update_idle); }
		if (change_state(AnimState::run, run, true)) { return PA_BIND(update_run); }
		triggers.set(AnimTriggers::end_death);
	}
	state = AnimState::die;
	return PA_BIND(update_die);
}

bool PlayerAnimation::change_state(AnimState next, anim::Parameters params, bool hard) {
	if (state == next) {
		animation.set_params(params, hard);
		return true;
	}
	return false;
}

} // namespace player
