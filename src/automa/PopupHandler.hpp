
#pragma once

#include <memory>

namespace pi {
class Tool;
class PopupHandler{
  public:
	void launch(char const* label, std::unique_ptr<Tool>& tool);
};
} // namespace pi