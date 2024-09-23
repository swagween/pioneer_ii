#include <SFML/Graphics.hpp>
#include "src/util/Lookup.hpp"
#include "src/setup/Application.hpp"
#include <iostream>
#include <chrono>
#include <imgui-SFML.h>
#include <imgui.h>
#include <random>
#include <steam/steam_api.h>

int main(int argc, char** argv) {
	assert(argc > 0);
	if (SteamAPI_RestartAppIfNecessary(FORNANI_STEAM_APP_ID)) {
		std::cout << "Re-launching through Steam." << std::endl;
		return 0;
	}
	SteamErrMsg errMsg;
	if (SteamAPI_InitEx(&errMsg) != k_ESteamAPIInitResult_OK) {
		std::cout << "Failed to init Steam: " << errMsg << std::endl;
		return 0;
	}
	pi::Application app{argv};
	app.launch(argv);
    ImGui::SFML::Shutdown();
    return 0;
}


