//
//  StateManager.hpp
//  automa
//
//  Created by Alex Frasca on 12/26/22.
//

#pragma once

#include "GameState.hpp"

namespace pi {

    namespace automa {

        class StateManager {

        public:

            StateManager();
            ~StateManager();
            StateManager& operator=(StateManager&&) = delete;

            GameState& initialize();
            GameState& get_current_state();
            GameState& set_current_state(std::unique_ptr<automa::GameState> gameState);

        }; // End StateManager

    } // End automa

}
 /* StateManager_hpp */
