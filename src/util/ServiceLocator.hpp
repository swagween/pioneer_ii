//
//  AssetManager.hpp
//
//  Created by Alex Frasca on 12/26/22.
//

#pragma once


#include <SFML/Graphics.hpp>
#include <stdio.h>
#include <string>
#include "Service.hpp"

#include "Camera.hpp"
#include "../tool/Tool.hpp"

namespace pi {

	namespace svc {

		inline auto cameraLocator = svc::Service<Camera>::Instance{};
		inline std::unique_ptr<tool::Tool> current_tool = std::make_unique<tool::Hand>();
		inline std::unique_ptr<tool::Tool> secondary_tool = std::make_unique<tool::Hand>();
		inline auto selectedBlockLocator = Service<int>::Instance{};
		inline auto playerStartLocator = Service<sf::Vector2<uint32_t> >::Instance{};
		inline auto windowHoveredLocator = Service<bool>::Instance{};

		inline auto active_layer{ 4 };

	}

}

/* ServiceLocator_hpp */
