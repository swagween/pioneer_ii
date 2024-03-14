//
//  main.cpp
//  pioneer
//

#include <SFML/Graphics.hpp>
#include "src/automa/StateManager.hpp"
#include "src/util/Lookup.hpp"
#include "src/util/ServiceLocator.hpp"
#include <iostream>
#include <chrono>

#include <imgui-SFML.h>
#include <imgui.h>
#include <random>

#include <filesystem>

namespace fs = std::filesystem;

fs::path find_resources(fs::path exe) {
    auto check = [](fs::path const& prefix) {
        auto path = prefix / "resources";
        if (fs::is_directory(path)) { return path; }
        return fs::path{};
    };
    while (!exe.empty()) {
        if (auto ret = check(exe); !ret.empty()) { return ret; }
        auto parent = exe.parent_path();
        if (exe == parent) { break; }
        exe = std::move(parent);
    }
    return {};
}

namespace {

auto SM = pi::automa::StateManager{};
auto window = sf::RenderWindow();
auto demo_view = sf::View();

sf::Color PIONEER_BLUE = sf::Color(85, 173, 232);

std::string state{};

static void show_overlay(bool* debug) {
    SM.get_current_state().gui_render(window);
}

const int TIME_STEP_MILLI = 100;
float G = 0.8f;

void run(char** argv) {

    //fornani demo stuff
    lookup::populate_lookup();
    //load all assets
    //images
    svc::assetLocator.get().setResourcePath(argv);
    svc::assetLocator.get().importTextures();
    svc::assetLocator.get().assignSprites();
    //sounds
    svc::assetLocator.get().load_audio();
    
    //load textures
    std::string resource_path = find_resources(argv[0]).string();
    const int TILE_WIDTH = 32;
    
    //load the tilesets!
    sf::Texture t_tiles_provisional{};
    t_tiles_provisional.loadFromFile(resource_path + "/tile/provisional_tiles.png");
    
    SM.set_current_state(std::make_unique<pi::automa::Editor>());
    SM.get_current_state().init(resource_path + "/level/TEST_ROOM_01");
    SM.get_current_state().setTilesetTexture(t_tiles_provisional);
    
    bool debug_mode = false;
    //init clock
    using Clock = std::chrono::steady_clock;
    using Time = std::chrono::duration<float>;
    auto elapsed_time = Time{};
    auto time_step = Time{std::chrono::milliseconds(TIME_STEP_MILLI)}; //FPS
    //some SFML variables for drawing a basic window + background
    window.create(sf::VideoMode(pi::screen_dimensions.x, pi::screen_dimensions.y), "Pioneer (beta v1.0)");
    ImGui::SFML::Init(window);
    
    window.setVerticalSyncEnabled(true);
    window.setKeyRepeatEnabled(false);

    sf::Vector2<float> demo_view_dimensions{ 960.f, 540.f };
    demo_view.reset(sf::FloatRect(0, 0, demo_view_dimensions.x, demo_view_dimensions.y));
    demo_view.setViewport(sf::FloatRect(0.25f, 0.25f, 0.5f, 0.5f));
    //demo_view.move(sf::Vector2<float>(pi::screen_dimensions.x / 2 - demo_view_dimensions.x / 2, pi::screen_dimensions.y / 2 - demo_view_dimensions.y / 2));
    //demo_view.setCenter((float)pi::screen_dimensions.x * 0.5, (float)pi::screen_dimensions.y * 0.5);

    sf::RectangleShape background{};
    background.setSize(static_cast<sf::Vector2<float> >(pi::screen_dimensions));
    background.setPosition(0, 0);
    background.setFillColor(sf::Color(40, 60, 80));
    
    
    //game loop
    sf::Clock deltaClock{};
    auto start = Clock::now();
    int frame{};
    while (window.isOpen()) {

        svc::clockLocator.get().tick();

        frame++;
        auto now = Clock::now();
        auto dt = Time{now - start};
        start = now;
        elapsed_time += dt;
        
        auto event = sf::Event{};
        
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);
            switch(event.type) {
                case sf::Event::Closed:
                    pi::lookup::get_state_string.clear();
                    return;
                case sf::Event::KeyPressed:
                    if(event.key.code == sf::Keyboard::D) {
                        debug_mode = !debug_mode;
                    }
                    if (event.key.code == sf::Keyboard::R) {
                        
                    }
                    if(event.key.code == sf::Keyboard::Num1) {
                        pi::svc::current_tool = std::move(std::make_unique<tool::Hand>());
                    }
                    if(event.key.code == sf::Keyboard::Num2) {
                        pi::svc::current_tool = std::move(std::make_unique<tool::Brush>());
                    }
                    if(event.key.code == sf::Keyboard::Num3) {
                        pi::svc::current_tool = std::move(std::make_unique<tool::Fill>());
                    }
                    if(event.key.code == sf::Keyboard::Num4) {
                        pi::svc::current_tool = std::move(std::make_unique<tool::SelectionRectangular>());
                    }
                    if(event.key.code == sf::Keyboard::Enter) {
                        /*SM.set_current_state(std::make_unique<pi::automa::Editor>());
                        SM.get_current_state().init(resource_path + "/level/WOODSHINE_SHACK_01");
                        SM.get_current_state().setTilesetTexture(t_tiles_provisional);*/
                    }
                    if(event.key.code == sf::Keyboard::Equal) {
                        SM.set_current_state(std::make_unique<pi::automa::Metagrid>());
                    }
                    if(event.key.code == sf::Keyboard::Escape) {
                    if(SM.get_current_state().state == pi::automa::STATE::DOJO) {
                        std::string loaddir = SM.get_current_state().filepath;
                        SM.set_current_state(std::make_unique<pi::automa::Editor>());
                        SM.get_current_state().init(loaddir);
                        window.setView(window.getDefaultView());
                    }
                    }
                    break;
                default:
                    break;
            }
            SM.get_current_state().handle_events(event, window);
        }
        
        //game logic and rendering
        SM.get_current_state().logic();
        if (SM.get_current_state().trigger_demo) {
            if (SM.get_current_state().state == pi::automa::STATE::EDITOR) {
                std::string loaddir = SM.get_current_state().filepath;
                SM.set_current_state(std::make_unique<pi::automa::Dojo>());
                SM.get_current_state().init(loaddir);
                sf::Vector2<float> player_pos = { (float)pi::svc::playerStartLocator.get().x * CELL_SIZE, (float)pi::svc::playerStartLocator.get().y * CELL_SIZE };
                svc::playerLocator.get().set_position(player_pos);
                window.setView(demo_view);
            }
        }
        
        //ImGui update
        ImGui::SFML::Update(window, deltaClock.restart());
        
        //ImGui stuff
        show_overlay(&debug_mode);
        //ImGui::ShowDemoWindow();
        
        //my renders
        window.clear();
        window.draw(background);
        
        SM.get_current_state().render(window);
        
        //draw canvas here
        
        
        ImGui::SFML::Render(window);
        window.display();

    }
    
}

} //end namespace

int main(int argc, char** argv) {
    assert(argc > 0);
    run(argv);
    window.close();
    ImGui::SFML::Shutdown();
    return 0;
}


