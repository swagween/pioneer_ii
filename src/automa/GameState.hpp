//
//  GameState.hpp
//  automa
//
//  Created by Alex Frasca on 12/26/22.
//

#pragma once

#include <SFML/Graphics.hpp>
#include <cstdio>
#include <memory>
#include <chrono>
#include <imgui.h>
#include "../canvas/Canvas.hpp"
#include "../util/ServiceLocator.hpp"
#include "../tool/Tool.hpp"
#include <SFML/OpenGL.hpp>
#include <imgui-SFML.h>
#include <sstream>
#include <windows.h>
#include <string_view>

namespace pi {

    namespace automa {

        inline const char* styles[canvas::NUM_STYLES];
        inline const char* bgs[canvas::NUM_BGS];
        inline std::string_view const& style_list{};
        inline std::string styles_str{};

        //globals

        enum class STATE {
            NONE,
            METAGRID,
            EDITOR,
			DOJO
        };

        class GameState {

        public:

            using Time = std::chrono::duration<float>;

            GameState() = default;
            GameState(int id) {

            }
            GameState& operator=(GameState&&) = delete;

            template<typename T> class StateID;

            virtual void init(const std::string& load_path) {}
            virtual void setTilesetTexture(sf::Texture tile_texture) {}
            virtual void handle_events(sf::Event& event, sf::RenderWindow& win) {}
            virtual void logic() {}
            virtual void render(sf::RenderWindow& win) {}
			virtual void gui_render(sf::RenderWindow& win) {}
			virtual void launch_demo(char** argv, int room_id, std::filesystem::path path, sf::Vector2<float> player_position);

            STATE state = STATE::NONE;
			char** args{};

            std::string filepath{};
			std::string demopath{};
			std::string folderpath{};
			std::string room{};
			int room_id{};

            bool trigger_demo{ false };
        };

        // =======================================================================
        //
        //    Metagrid
        //
        // =======================================================================

        class Metagrid : public GameState {
        public:


            Metagrid() {
                state = automa::STATE::METAGRID;
            };
            void init(const std::string& load_path) {
            }
            void setTilesetTexture(sf::Texture tile_texture) {
            }
            void handle_events(sf::Event& event, sf::RenderWindow& win) {

                if (event.type == sf::Event::EventType::KeyPressed) {
                }

            }

            void logic() {}
            void render(sf::RenderWindow& win) {}
            void gui_render(sf::RenderWindow& win);
        };

        // =======================================================================
        //
        //    Editor
        //
        // =======================================================================

        class Editor : public GameState {
        public:

            const int TILE_WIDTH{ 32 };
            const int NUM_TOOLS{ 6 };

            Editor() {
                state = STATE::EDITOR;
            }

            void init(const std::string& load_path);
            void setTilesetTexture(sf::Texture& new_tex);
            void handle_events(sf::Event& event, sf::RenderWindow& win);
            void logic();
            void render(sf::RenderWindow& win);
            void gui_render(sf::RenderWindow& win);
            void help_marker(const char* desc);
			void export_layer_texture();
			sf::Vector2<int> get_tile_coord(int lookup);

            canvas::Canvas map{};
            std::vector<sf::Texture> tileset_textures{};
            sf::Texture tool_texture{};
            sf::Sprite tool_sprites{};
            sf::Texture large_animator_textures{};
            sf::Texture large_animator_thumbs{};
            sf::Texture small_animator_textures{};
            sf::Texture small_animator_thumbs{};
            sf::Texture enemy_thumbnails{};

            struct {
				sf::Sprite tool{};
				sf::Sprite tileset{};
				sf::Sprite large_animator{};
				sf::Sprite small_animator{};
				sf::Sprite large_animator_thumb{};
				sf::Sprite small_animator_thumb{};
				sf::Sprite enemy_thumb{};
				sf::Sprite current_enemy{};

			} sprites{};

            sf::Texture t_chest{};
            sf::Sprite s_chest{};
            sf::Texture t_npc{};
            sf::Sprite s_npc{};

            //for loading out layer pngs
            sf::RenderTexture screencap{};
            sf::Sprite tile_sprite{};

            sf::Vector2<float> mouse_clicked_position{};

            bool mouse_held{};
            bool show_grid{ true };
            bool show_all_layers{ true };
            bool show_overlay{ false };
			bool demo_mode{};

            sf::RectangleShape box{};
			sf::RectangleShape player_box{};
            sf::RectangleShape gridbox{};
            sf::RectangleShape portalbox{};
            sf::RectangleShape inspbox{};
			sf::RectangleShape platbox{};
			sf::RectangleShape chestbox{};
			sf::RectangleShape savebox{};
			sf::RectangleShape platextent{};
            sf::RectangleShape target{};
            sf::RectangleShape backdrop{};

            int large_index_multiplier{ 100 };
            int small_index_multiplier{ 200 };
        };

		class Dojo : public GameState {
		public:

            Dojo(){};

            void init(const std::string& load_path);
            void setTilesetTexture(sf::Texture& new_tex);
            void handle_events(sf::Event& event, sf::RenderWindow& win);
            void logic();
            void render(sf::RenderWindow& win);
            void gui_render(sf::RenderWindow& win);

            sf::Texture tileset{};
            std::vector<sf::Sprite> tileset_sprites{};
            bool show_colliders{ false };
            int x{ 0 };

		};

    }

}

/* GameState_hpp */
