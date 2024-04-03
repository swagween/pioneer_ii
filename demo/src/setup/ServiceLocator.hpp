//
//  AssetManager.hpp
//
//  Created by Alex Frasca on 12/26/22.
//

#pragma once


#include <SFML/Graphics.hpp>
#include <stdio.h>
#include <string>

#include "AssetManager.hpp"
#include "../entities/player/Player.hpp"
#include "../utils/Camera.hpp"
#include "../service/Service.hpp"
#include "../utils/Clock.hpp"
#include "../automa/StateController.hpp"
#include "../gui/Console.hpp"
#include "../utils/Random.hpp"

namespace svc {

	enum class global_flags {greyblock_state, greyblock_trigger};
	enum counters {draw_calls = 0};
	const int num_counters = 1;

	inline auto assetLocator = forloop::Service<AssetManager>::Instance{};
	inline auto playerLocator = forloop::Service<Player>::Instance{};
	inline auto cameraLocator = forloop::Service<cam::Camera>::Instance{};
	inline auto clockLocator = forloop::Service<util::Clock>::Instance{};
	inline auto stateControllerLocator = forloop::Service<automa::StateController>::Instance{};
	inline auto consoleLocator = forloop::Service<gui::Console>::Instance{};
	inline auto randomLocator = forloop::Service<util::Random>::Instance{};
	inline auto globalBitFlagsLocator = forloop::Service<util::BitFlags<global_flags> >::Instance{};
	inline auto counterLocator = forloop::Service< std::array<uint16_t, num_counters> >::Instance{};
	inline auto floatReadoutLocator = forloop::Service<float>::Instance{};

}

/* ServiceLocator_hpp */
