
#include "PopupHandler.hpp"
#include "../tool/Tool.hpp"
#include <imgui.h>
#include <string>

namespace pi {

void PopupHandler::launch(char const* label, std::unique_ptr<Tool>& tool) {
	if (ImGui::BeginPopupModal(label, NULL, ImGuiWindowFlags_AlwaysAutoResize)) {

		static bool activate_on_contact{false};
		static char keybuffer[128] = "";
		static char msgbuffer[512] = "";

		ImGui::InputTextWithHint("Key", "Title (invisible in-game; must be unique per room)", keybuffer, IM_ARRAYSIZE(keybuffer));
		ImGui::Separator();
		ImGui::NewLine();

		ImGui::InputTextWithHint("Message", "Type message here...", msgbuffer, IM_ARRAYSIZE(msgbuffer));
		ImGui::Separator();
		ImGui::NewLine();

		ImGui::Checkbox("Activate on contact?", &activate_on_contact);
		ImGui::SameLine();

		if (ImGui::Button("Create")) {
			// switch to entity tool, and store the specified portal for placement
			tool = std::move(std::make_unique<EntityPlacer>());
			tool->ent_type = ENTITY_TYPE::INSPECTABLE;
			tool->current_inspectable = Inspectable(sf::Vector2<uint32_t>{1, 1}, activate_on_contact, keybuffer);
			tool->current_inspectable.suites.push_back(std::vector<std::string>{msgbuffer});
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Close")) { ImGui::CloseCurrentPopup(); }
		ImGui::EndPopup();
	}
}

} // namespace pi