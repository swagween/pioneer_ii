
#pragma once

#include "../GameState.hpp"
#include "../../level/Camera.hpp"
#include "../../utils/Collider.hpp"

namespace automa {

class Dojo : public GameState {
  public:
	Dojo() = default;
	Dojo(ServiceProvider& svc, player::Player& player, std::string_view scene, int id = 0);
	void init(ServiceProvider& svc, int room_number = 100, std::string room_name = "");
	void handle_events(ServiceProvider& svc, sf::Event& event);
	void tick_update(ServiceProvider& svc);
	void frame_update(ServiceProvider& svc);
	void render(ServiceProvider& svc, sf::RenderWindow& win);
	void toggle_inventory(ServiceProvider& svc);
	void toggle_pause_menu(ServiceProvider& svc);
	void bake_maps(ServiceProvider& svc, std::vector<int> ids, bool current = false);

	world::Map map;
	world::Map gui_map;
	fornani::Camera camera{};
	bool show_colliders{false};
	int x{0};
	util::Cooldown enter_room{};

	//shape::Collider A{};
	//shape::Collider B{};
};

} // namespace automa