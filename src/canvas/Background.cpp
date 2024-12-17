
#include "Background.hpp"
#include <imgui.h>
#include <algorithm>
#include "../setup/ResourceFinder.hpp"

namespace pi {

Background::Background(ResourceFinder& finder, int bg_id) : labels{{0, "dusk"}, {5, "night"}, {3, "rosy_haze"}, {18, "woods"}} {
	std::string type = labels.contains(bg_id) ? labels.at(bg_id) : "black";
	std::string doc = type + ".png";
	auto bg = dj::Json::from_file((finder.paths.resources / "data/level/background_behaviors.json").string().c_str());
	assert(!bg.is_null());
	texture.loadFromFile((finder.paths.resources / "image/background" / doc).string());

	auto const& in_data = bg[type];
	dimensions.x = in_data["dimensions"][0].as<int>();
	dimensions.y = in_data["dimensions"][1].as<int>();
	scroll_pane = dimensions;
	auto index{0};
	for (auto& layer : in_data["layers"].array_view()) {
		layers.push_back({index, layer["scroll_speed"].as<float>(), layer["parallax"].as<float>()});
		layers.back().sprite.setTexture(texture);
		layers.back().sprite.setTextureRect(sf::IntRect{{0, dimensions.y * index}, dimensions});
		++index;
	}
}

void Background::update() {
	for (auto& layer : layers) {
		// backtrack sprites for infinite scroll effect
		if (layer.position.x < -scroll_pane.x) { layer.position.x = 0.f; }
		if (layer.position.x > 0.f) { layer.position.x = -scroll_pane.x; }
		if (layer.position.y < -scroll_pane.y) { layer.position.y = layer.position.y + scroll_pane.y; }
		if (layer.position.y > 0.f) { layer.position.y = -scroll_pane.y + layer.position.y; }
		layer.velocity.x = layer.scroll_speed * 0.1f;
		layer.position += layer.velocity;
	}
}

void Background::render(sf::RenderWindow& win, sf::Vector2<float>& campos) {
	auto epsilon = 0.999f;
	for (auto& layer : layers) {
		auto final_position = layer.position - campos * layer.parallax;
		for (auto i{0}; i < 2; ++i) {

			for (auto j{0}; j < 2; ++j) {
				layer.sprite.setPosition(final_position + sf::Vector2<float>{static_cast<float>(dimensions.x * epsilon) * static_cast<float>(i), static_cast<float>(dimensions.y * epsilon) * static_cast<float>(j)});
				win.draw(layer.sprite);
			}
		}
	}
}

void Background::debug() {
	ImGuiIO& io = ImGui::GetIO();
	io.FontGlobalScale = 1.0;
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
	ImGuiViewport const* viewport = ImGui::GetMainViewport();
	ImVec2 work_pos = viewport->WorkPos;
	ImVec2 work_size = viewport->WorkSize;
	ImVec2 window_pos, window_pos_pivot;
	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
	window_flags |= ImGuiWindowFlags_NoMove;
	ImGui::SetNextWindowBgAlpha(0.65f);
	if (ImGui::Begin("Background Parameters", b_debug, window_flags)) {
		auto index{0};
		for (auto& layer : layers) {
			ImGui::Separator();
			ImGui::Text("Layer %i", index);
			std::string plabel = "parallax" + std::to_string(index);
			std::string slabel = "speed" + std::to_string(index);
			ImGui::SliderFloat(plabel.data(), &layer.parallax, 0.f, 1.f);
			ImGui::SliderFloat(slabel.data(), &layer.scroll_speed, -8.f, 8.f);
			++index;
		}
	}
	ImGui::End();
}

} // namespace bg
