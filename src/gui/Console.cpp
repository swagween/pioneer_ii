#include <imgui.h>
#include <iostream>
#include "Console.hpp"

namespace pi {

	void Console::add_log(char const* message) { log.add_log(message); }

void Console::write_console(ImVec2 prev_size, ImVec2 prev_pos) {
	auto pad{10.f};
	ImGuiViewport const* viewport = ImGui::GetMainViewport();
	auto port_size = viewport->WorkSize;
	ImGui::SetNextWindowBgAlpha(0.65f);
	ImVec2 work_pos = prev_pos;
	ImVec2 work_size = prev_size;
	work_pos.y += prev_size.y + pad;
	work_size.y = port_size.y - prev_size.y - 3.f * pad;
	ImGui::SetNextWindowPos(work_pos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(work_size);
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

	if (ImGui::Begin("Console", NULL, window_flags)) { log.draw("Console", NULL); }
	ImGui::End();
}

} // namespace pi