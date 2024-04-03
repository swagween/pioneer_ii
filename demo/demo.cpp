//
//  main.cpp
//  For Nani
//
//#include <SFML/Graphics.hpp>
//#include <cmath>
//#include <ctime>
//#include <iomanip>
//#include <iostream>
////all services and providers included first
//#include "src/setup/ServiceLocator.hpp"
//#include "src/automa/StateManager.hpp"
//#include "src/setup/EnumLookups.hpp"
//#include "src/utils/Grid.hpp"
//#include "src/utils/Shape.hpp"
//
//#include <imgui-SFML.h>
//#include <imgui.h>
//#include <random>
//
//namespace demo {
//
//auto d_SM = automa::StateManager{};
//auto d_window = sf::RenderWindow();
//auto d_minimap = sf::View();
//
//sf::Texture d_screencap{};
//
//const int NUM_TIMESTEPS = 64;
//int d_TIME_STEP_MILLI = 0;
//int d_frame{};
//using Clock = std::chrono::steady_clock;
//using Time = std::chrono::duration<float>;
//auto d_elapsed_time = Time{};
//auto d_elapsed_marker = Time{};
//float d_time_markers[NUM_TIMESTEPS]{};
//auto d_time_step = Time{std::chrono::milliseconds(d_TIME_STEP_MILLI)}; //FPS
//float d_seconds = 0.0;
//int d_FPS_counter = 0;
//float d_FPS = 0.0;
//
//int d_shake_counter = 0;
//int d_frame_draw_counter{ 0 };
//
//sf::Vector2<uint32_t> d_win_size{};
//float d_height_ratio{};
//float d_width_ratio{};
//
//
//
//void save_screenshot() {
//
//    const std::time_t now = std::time(nullptr);
//
//    std::string filedate = std::asctime(std::localtime(&now));
//    std::erase_if(filedate, [](auto const& c) { return c == ':' || isspace(c); });
//    std::string filename = "screenshot_" + filedate + ".png";
//
//    if (d_screencap.copyToImage().saveToFile(filename)) {
//        std::cout << "screenshot saved to " << filename << std::endl;
//    }
//
//}
//
//
//static void show_overlay() {
//    bool* debug{};
//    const float PAD = 10.0f;
//    static int corner = 1;
//    ImGuiIO& io = ImGui::GetIO();
//    io.FontGlobalScale = 1.0;
//    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
//    if (corner != -1) {
//        const ImGuiViewport* viewport = ImGui::GetMainViewport();
//        ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
//        ImVec2 work_size = viewport->WorkSize;
//        ImVec2 window_pos, window_pos_pivot;
//        window_pos.x = (corner & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
//        window_pos.y = (corner & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
//        window_pos_pivot.x = (corner & 1) ? 1.0f : 0.0f;
//        window_pos_pivot.y = (corner & 2) ? 1.0f : 0.0f;
//        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
//        window_flags |= ImGuiWindowFlags_NoMove;
//        ImGui::SetNextWindowBgAlpha(0.65f); // Transparent background
//        if (ImGui::Begin("Debug Mode", debug, window_flags)) {
//            ImGui::Text("Debug Window\n" "For Nani (beta version 1.0.0)");
//            ImGui::Text("Window Focused: ");
//            ImGui::SameLine();
//            if(d_window.hasFocus()) { ImGui::Text("Yes"); } else { ImGui::Text("No"); }
//            if(!d_window.hasFocus()) {
//                d_window.RenderTarget::setActive();
//            }
//            ImGui::Separator();
//            ImGui::Text("Screen Dimensions X: %u", cam::screen_dimensions.x);
//            ImGui::Text("Screen Dimensions Y: %u", cam::screen_dimensions.y);
//            if (ImGui::IsMousePosValid()) {
//                ImGui::Text("Mouse Position: (%.1f,%.1f)", io.MousePos.x, io.MousePos.y);
//            } else {
//                ImGui::Text("Mouse Position: <invalid>");
//            }
//            ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
//            if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
//            {
//                if (ImGui::BeginTabItem("Time"))
//                {
//                    ImGui::Separator();
//                    ImGui::Text("Time");
//                    ImGui::TextUnformatted(std::to_string(d_time_markers[d_frame % NUM_TIMESTEPS]).c_str());
//                    ImGui::Text("Timer Elapsed Time: %.5f", svc::clockLocator.get().elapsed_time.count());
//                    ImGui::Text("Timer Time (seconds): %.1f", svc::clockLocator.get().seconds);
//                    ImGui::Text("Time Step (milliseconds): %.1f", svc::clockLocator.get().dt.count());
//                    ImGui::Text("Accumulator: %.1f", svc::clockLocator.get().accumulator);
//                    ImGui::Text("FPS: %.1f", svc::clockLocator.get().FPS);
//
//                    ImGui::SliderFloat("Tick Rate: ", &svc::clockLocator.get().rate, 0.00050f, 0.0080f, "%.8f");
//                    if (ImGui::Button("Reset")) { svc::clockLocator.get().rate = 0.001; }
//                    ImGui::Separator();
//                    ImGui::EndTabItem();
//                    ImGui::PlotHistogram("Frame Times", d_time_markers, NUM_TIMESTEPS, 0, NULL, 0.0f, 0.02f, ImVec2(0, 80.0f));
//                }
//                if (ImGui::BeginTabItem("Audio"))
//                {
//                    ImGui::Separator();
//                    ImGui::Text("Music Volume");
//                    ImGui::SliderInt("##musvol", &svc::assetLocator.get().music_vol, 0, 100);
//                    ImGui::EndTabItem();
//                }
//                if (ImGui::BeginTabItem("Player"))
//                {
//                    ImGui::Text("Player Stats");
//                    ImGui::SliderInt("Max HP", &svc::playerLocator.get().player_stats.max_health, 3, 12);
//                    ImGui::SliderInt("HP", &svc::playerLocator.get().player_stats.health, 3, 12);
//                    ImGui::SliderInt("Max Orbs", &svc::playerLocator.get().player_stats.max_orbs, 99, 99999);
//                    ImGui::SliderInt("Orbs", &svc::playerLocator.get().player_stats.orbs, 0, 99999);
//                                 
//                ImGui::Text("Alive? %s", svc::playerLocator.get().flags.state.test(State::alive) ? "Yes" : "No");
//                    ImGui::Text("Player Behavior: %s", svc::playerLocator.get().behavior.current_state.params.behavior_id);
//                    ImGui::Text("Behavior Restricted? %s", svc::playerLocator.get().behavior.restricted() ? "Yes" : "No");
//                    ImGui::Text("Behavior Current Frame: %i", svc::playerLocator.get().behavior.current_state.params.current_frame);
//                    ImGui::Text("Behavior Complete? %s", svc::playerLocator.get().behavior.current_state.params.complete ? "Yes" : "No");
//                    ImGui::Text("Behavior No Loop? %s", svc::playerLocator.get().behavior.current_state.params.no_loop ? "Yes" : "No");
//                    ImGui::Text("Invincibility Counter: %i", svc::playerLocator.get().counters.invincibility);
//                    ImGui::Text("Spike Trigger: %s", svc::playerLocator.get().collider.spike_trigger ? "True" : "False");
//                    ImGui::Text("On Ramp: %s", svc::playerLocator.get().collider.on_ramp() ? "True" : "False");
//
//                    ImGui::Text("Inspecting? %s", svc::playerLocator.get().flags.input.test(Input::inspecting) ? "Yes" : "No");
//
//                    ImGui::Text("Player Facing: %s", svc::playerLocator.get().print_direction(false).c_str());
//                    ImGui::Text("Player Facing LR: %s", svc::playerLocator.get().print_direction(true).c_str());
//                    ImGui::Text("Colliding with Level: ");
//                    ImGui::SameLine();
//                    if(svc::playerLocator.get().collider.flags.test(shape::State::is_colliding_with_level)) { ImGui::Text("Yes"); } else { ImGui::Text("No"); }
//                    ImGui::Text("Grounded: ");
//                    ImGui::SameLine();
//                    if(svc::playerLocator.get().grounded()) { ImGui::Text("Yes"); } else { ImGui::Text("No"); }
//                    ImGui::Text("Jump Request : %i", svc::playerLocator.get().jump_request);
//                    ImGui::Text("Jump Pressed : %s", svc::playerLocator.get().flags.jump.test(Jump::is_pressed) ? "Yes" : "No");
//                    ImGui::Text("Jump Hold    : %s", svc::playerLocator.get().flags.jump.test(Jump::hold) ? "Yes" : "No");
//                    ImGui::Text("Jump Released: %s", svc::playerLocator.get().flags.jump.test(Jump::is_released) ? "Yes" : "No");
//                    ImGui::Text("Jumping      : %s", svc::playerLocator.get().flags.jump.test(Jump::jumping) ? "Yes" : "No");
//                    ImGui::Text("Sprite Lookup: %i", svc::playerLocator.get().behavior.current_state.params.lookup_value);
//                    ImGui::Text("Has Right Collision: ");
//                    ImGui::SameLine();
//                    if(svc::playerLocator.get().collider.flags.test(shape::State::has_right_collision)) { ImGui::Text("Yes"); } else { ImGui::Text("No"); }
//                    ImGui::Text("Has Left Collision: ");
//                    ImGui::SameLine();
//                    if (svc::playerLocator.get().collider.flags.test(shape::State::has_left_collision)) { ImGui::Text("Yes"); } else { ImGui::Text("No"); }
//                    if (svc::playerLocator.get().collider.flags.test(shape::State::is_any_jump_collision)) { ImGui::Text("Yes"); } else { ImGui::Text("No"); }
//
//                    ImGui::Text("Player Pos: (%.4f,%.4f)", svc::playerLocator.get().collider.physics.position.x, svc::playerLocator.get().collider.physics.position.y);
//                    ImGui::Text("Player Vel: (%.4f,%.4f)", svc::playerLocator.get().collider.physics.velocity.x, svc::playerLocator.get().collider.physics.velocity.y);
//                    ImGui::Text("Player Acc: (%.4f,%.4f)", svc::playerLocator.get().collider.physics.acceleration.x, svc::playerLocator.get().collider.physics.acceleration.y);
//                    
//                    ImGui::SliderFloat("GRAVITY",                   &svc::playerLocator.get().stats.PLAYER_GRAV, 0.000f, 0.008f);
//                    ImGui::SliderFloat("PLAYER MASS",               &svc::playerLocator.get().stats.PLAYER_MASS, 0.1f, 2.0f);
//                    ImGui::SliderFloat("AIR MANEUVERABILITY",       &svc::playerLocator.get().stats.AIR_MULTIPLIER, 0.0f, 5.0f);
//                    ImGui::SliderFloat("TERMINAL VELOCITY",         &svc::playerLocator.get().stats.TERMINAL_VELOCITY, 1.0f, 8.0f);
//
//                    ImGui::Text("Friction Multipliers");
//                    ImGui::SliderFloat("GROUND FRICTION",           &svc::playerLocator.get().stats.PLAYER_GROUND_FRIC, 0.900f, 1.000f);
//                    ImGui::SliderFloat("HORIZONTAL AIR FRICTION",   &svc::playerLocator.get().stats.PLAYER_HORIZ_AIR_FRIC, 0.900f, 1.000f);
//                    ImGui::SliderFloat("VERTICAL AIR FRICTION",     &svc::playerLocator.get().stats.PLAYER_VERT_AIR_FRIC, 0.900f, 1.000f);
//                    ImGui::NewLine();
//
//                    ImGui::SliderFloat("GROUND SPEED",              &svc::playerLocator.get().stats.X_ACC, 0.0f, 0.2f);
//                    ImGui::SliderFloat("AIR SPEED",                 &svc::playerLocator.get().stats.X_ACC_AIR, 0.0f, 0.2f);
//                    ImGui::SliderFloat("JUMP HEIGHT",               &svc::playerLocator.get().stats.JUMP_MAX, 0.15f, 0.5f);
//                    ImGui::EndTabItem();
//                }
//                if (ImGui::BeginTabItem("Weapon"))
//                {
//					if (ImGui::Button("Toggle Weapons")) {
//                        if (svc::playerLocator.get().weapons_hotbar.empty()) {
//                            svc::playerLocator.get().weapons_hotbar = {
//                                arms::WEAPON_TYPE::BRYNS_GUN,
//                                arms::WEAPON_TYPE::PLASMER,
//                                arms::WEAPON_TYPE::CLOVER,
//                                arms::WEAPON_TYPE::NOVA
//                            };
//                            svc::playerLocator.get().loadout.equipped_weapon = svc::playerLocator.get().weapons_hotbar.at(0);
//                        } else {
//                            svc::playerLocator.get().weapons_hotbar.clear();
//                        }
//					}
//                    
//                    ImGui::Separator();
//                    ImGui::Text("Equipped Weapon: ");
//                    ImGui::SameLine();
//                    ImGui::TextUnformatted(svc::playerLocator.get().loadout.get_equipped_weapon().label.c_str());
//                    ImGui::Text("Weapon Fired: ");
//                    ImGui::SameLine();
//                    if(svc::playerLocator.get().weapon_fired) { ImGui::Text("Yes"); } else { ImGui::Text("No"); }
//                    ImGui::Separator();
//                    ImGui::Text("Weapon Stats: ");
//                    ImGui::Indent();
//                    ImGui::Text("Rate: (%i)",     svc::playerLocator.get().loadout.get_equipped_weapon().attributes.rate);
//                    ImGui::Text("Cooldown: (%i)", svc::playerLocator.get().loadout.get_equipped_weapon().attributes.cooldown_time);
//                    ImGui::Text("Recoil: (%.2f)", svc::playerLocator.get().loadout.get_equipped_weapon().attributes.recoil);
//                    ImGui::Separator();
//                    ImGui::Unindent();
//                    ImGui::Text("Projectile Stats: ");
//                    ImGui::Indent();
//                    ImGui::Text("Damage: (%i)",   svc::playerLocator.get().loadout.get_equipped_weapon().projectile.stats.damage);
//                    ImGui::Text("Range: (%i)", svc::playerLocator.get().loadout.get_equipped_weapon().projectile.stats.range);
//                    ImGui::Text("Speed: (%.2f)",  svc::playerLocator.get().loadout.get_equipped_weapon().projectile.stats.speed);
//                    ImGui::Text("Velocity: (%.4f,%.4f)", svc::playerLocator.get().loadout.get_equipped_weapon().projectile.physics.velocity.x,
//                                svc::playerLocator.get().loadout.get_equipped_weapon().projectile.physics.velocity.y);
//                    ImGui::Text("Position: (%.4f,%.4f)", svc::playerLocator.get().loadout.get_equipped_weapon().projectile.physics.position.x,
//                                svc::playerLocator.get().loadout.get_equipped_weapon().projectile.physics.position.y);
//                    ImGui::EndTabItem();
//                    
//                }
//                if (ImGui::BeginTabItem("General"))
//                {
//                    ImGui::Text("Float Readout: (%.8f)", svc::floatReadoutLocator.get());
//                    ImGui::Text("Camera Position: (%.8f,%.8f)", svc::cameraLocator.get().position.x, svc::cameraLocator.get().position.y);
//                    ImGui::Text("Observed Camera Velocity: (%.8f,%.8f)", svc::cameraLocator.get().observed_velocity.x, svc::cameraLocator.get().observed_velocity.y);
//                    ImGui::Text("Console Active : %s", svc::consoleLocator.get().flags.test(gui::ConsoleFlags::active) ? "Yes" : "No");
//                    if(ImGui::Button("Save Screenshot")) {
//                        save_screenshot();
//                    }
//                    ImGui::Separator();
//                    if(ImGui::Button("Toggle Greyblock Mode")) {
//                        if (svc::globalBitFlagsLocator.get().test(svc::global_flags::greyblock_state)) {
//                            svc::globalBitFlagsLocator.get().reset(svc::global_flags::greyblock_state);
//                            svc::globalBitFlagsLocator.get().set(svc::global_flags::greyblock_trigger);
//                        } else {
//                            svc::globalBitFlagsLocator.get().set(svc::global_flags::greyblock_state);
//                            svc::globalBitFlagsLocator.get().set(svc::global_flags::greyblock_trigger);
//                        }
//                        
//                    }
//                    ImGui::Text("Greybox Mode : %s", svc::globalBitFlagsLocator.get().test(svc::global_flags::greyblock_state) ? "On" : "Off");
//                    ImGui::Separator();
//                    ImGui::Text("Draw Calls: %u", d_frame_draw_counter);
//                    d_frame_draw_counter = 0;
//
//                    ImGui::EndTabItem();
//                }
//                if (ImGui::BeginTabItem("Resources"))
//                {
//                    ImGui::Text("Size of Asset Manager (Bytes): %lu", sizeof(svc::assetLocator.get()));
//                    ImGui::Text("Size of Camera (Bytes): %lu", sizeof(svc::cameraLocator.get()));
//                    ImGui::Text("Size of Player (Bytes): %lu", sizeof(svc::playerLocator.get()));
//                    ImGui::EndTabItem();
//                }
//                if (ImGui::BeginTabItem("State"))
//                {
//                    ImGui::Separator();
//                    ImGui::Text("State");
//                    ImGui::Text("Current State: ");
//                    ImGui::SameLine();
//                    ImGui::TextUnformatted(d_SM.get_current_state_string().c_str());
//                    ImGui::EndTabItem();
//                }
//                ImGui::EndTabBar();
//            }
//            
//            
//        }
//        ImVec2 prev_size = ImGui::GetWindowSize();
//        ImGui::End();
//    }
//}
//
//
//void run(char** argv) {
//    
//    //load all assets
//    //images
//    svc::assetLocator.get().setResourcePath(argv);
//    svc::assetLocator.get().importTextures();
//    svc::assetLocator.get().assignSprites();
//    //sounds
//    svc::assetLocator.get().load_audio();
//    
//    //state manager
//    d_SM.set_current_state(std::make_unique<automa::GameState>());
//    
//    d_window.create(sf::VideoMode(cam::screen_dimensions.x, cam::screen_dimensions.y), "For Nani (beta v1.0)");
//
//    d_screencap.create(d_window.getSize().x, d_window.getSize().y);
//    
//    bool debug_mode = false;
//    
//    //some SFML variables for drawing a basic window + background
//    d_window.setVerticalSyncEnabled(true);
//    //window.setFramerateLimit(20);
//    d_window.setKeyRepeatEnabled(false);
//    
//    ImGui::SFML::Init(d_window);
//    d_window.requestFocus();
//    
//    //lookups
//    lookup::populate_lookup();
//    
//    sf::RectangleShape background{};
//    background.setSize(static_cast<sf::Vector2<float> >(cam::screen_dimensions));
//    background.setPosition(0, 0);
//    background.setFillColor(sf::Color(10, 10, 20));
//
//    d_width_ratio = (float)cam::screen_dimensions.x / (float)cam::screen_dimensions.y;
//    d_height_ratio = (float)cam::screen_dimensions.y / (float)cam::screen_dimensions.x;
//    
//    
//    //game loop
//    sf::Clock deltaClock{};
//    
//    while (d_window.isOpen()) {
//
//        svc::clockLocator.get().tick();
//        d_win_size.x = d_window.getSize().x;
//        d_win_size.y = d_window.getSize().y;
//
//        uint16_t draw_counter = 0;
//        svc::counterLocator.get().at(svc::draw_calls) = draw_counter;
//        
//        //SFML event variable
//        auto event = sf::Event{};
//        //check window events
//        while (d_window.pollEvent(event)) {
//            ImGui::SFML::ProcessEvent(event);
//            switch(event.type) {
//                case sf::Event::Closed:
//                    return;
//                case sf::Event::Resized:
//                    d_win_size.x = d_window.getSize().x;
//                    d_win_size.y = d_window.getSize().y;
//                    if(d_win_size.y * d_width_ratio <= d_win_size.x) {
//                        d_win_size.x = d_win_size.y * d_width_ratio;
//                    } else if(d_win_size.x * d_height_ratio <= d_win_size.y) {
//                        d_win_size.y = d_win_size.x * d_height_ratio;
//                    }
//                    d_window.setSize(sf::Vector2u{ d_win_size.x, d_win_size.y});
//                    d_screencap.create(d_window.getSize().x, d_window.getSize().y);
//                    break;
//                case sf::Event::KeyPressed:
//					if (event.key.code == sf::Keyboard::Escape) {
//						return;
//					}
//					if (event.key.code == sf::Keyboard::D) {
//						debug_mode = !debug_mode;
//						svc::assetLocator.get().click.play();
//					}
//                    if (event.key.code == sf::Keyboard::K) {
//                        svc::playerLocator.get().kill();
//                    }
//                    if (event.key.code == sf::Keyboard::T) {
//                        svc::consoleLocator.get().begin();
//                        svc::consoleLocator.get().flags.set(gui::ConsoleFlags::active);
//                    }
//                    if (event.key.code == sf::Keyboard::Q) {
//                        d_SM.set_current_state(std::make_unique<flstates::MainMenu>());
//                    }
//                    if (event.key.code == sf::Keyboard::W) {
//                        d_SM.set_current_state(std::make_unique<flstates::Dojo>());
//                        d_SM.get_current_state().init(svc::assetLocator.get().resource_path + "/level/FIRSTWIND_PRISON_01");
//                        svc::playerLocator.get().set_position(sf::Vector2<float>(200.f, 390.f));
//                        svc::assetLocator.get().dusken_cove.setVolume(svc::assetLocator.get().music_vol);
//                        //svc::assetLocator.get().dusken_cove.play();
//                        svc::assetLocator.get().dusken_cove.setLoop(true);
//                        svc::playerLocator.get().assign_texture(svc::assetLocator.get().t_nani);
//
//                    }
//                    if (event.key.code == sf::Keyboard::P) {
//                        save_screenshot();
//                    }
//                    break;
//                default:
//                    break;
//            }
//            d_SM.get_current_state().handle_events(event);
//        }
//        
//        //game logic and rendering
//        
//        d_SM.get_current_state().logic();
//        d_SM.get_current_state().debug_mode = debug_mode;
//
//        //switch states
//        if (svc::stateControllerLocator.get().trigger) {
//            d_SM.set_current_state(std::make_unique<flstates::Dojo>());
//            d_SM.get_current_state().init(svc::assetLocator.get().resource_path + "/level/" + svc::stateControllerLocator.get().next_state);
//            svc::stateControllerLocator.get().trigger = false;
//        }
//
//        //reset global triggers
//        svc::globalBitFlagsLocator.get().reset(svc::global_flags::greyblock_trigger);
//        
//            
//        ImGui::SFML::Update(d_window, deltaClock.restart());
//        d_screencap.update(d_window);
//        
//        //ImGui stuff
//        if(debug_mode) {
//            show_overlay();
//        }
//        
//        //my renders
//        d_window.clear();
//        d_window.draw(background);
//        
//        d_SM.get_current_state().render(d_window);
//        
//        ImGui::SFML::Render(d_window);
//        d_window.display();
//        d_frame_draw_counter = svc::counterLocator.get().at(svc::draw_calls);
//    }
//    
//}


//} //end namespace


