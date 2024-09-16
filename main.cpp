#include <SFML/Graphics.hpp>
#include "src/util/Lookup.hpp"
#include "src/setup/Application.hpp"
#include <iostream>
#include <chrono>
#include <imgui-SFML.h>
#include <imgui.h>
#include <random>

int main(int argc, char** argv) {
    assert(argc > 0);
	pi::Application app{argv};
	app.launch(argv);
    ImGui::SFML::Shutdown();
    return 0;
}


