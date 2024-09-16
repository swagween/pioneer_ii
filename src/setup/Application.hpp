#pragma once
#include "ResourceFinder.hpp"
#include "../../demo/src/setup/Version.hpp"
#include "WindowManager.hpp"

namespace pi {

class Application {
  public:
	Application() = default;
	Application(char** argv);
	~Application() {}
	void launch(char** argv);

  private:
	sf::Texture t_loading{};
	sf::Sprite loading{};
	dj::Json game_info{};
	dj::Json app_settings{};
	ResourceFinder finder{};
	fornani::Version metadata{};
	WindowManager window{};
};

} // namespace pi
