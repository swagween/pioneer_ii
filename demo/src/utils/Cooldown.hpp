
#pragma once
#include <algorithm>
#include <limits>

namespace util {

class Cooldown {
  public:
	Cooldown() = default;
	Cooldown(int time) : native_time(time) {}
	constexpr void start() { decrementor = native_time; }
	constexpr void start(int time) { decrementor = time; }
	constexpr void update() { decrementor = std::clamp(decrementor - 1, 0, std::numeric_limits<int>::max()); }
	constexpr void cancel() { decrementor = 0; }
	constexpr void nullify() { decrementor = -1; }
	[[nodiscard]] auto is_complete() const -> bool { return decrementor == 0; }
	[[nodiscard]] auto running() const -> bool { return decrementor != 0; }
	[[nodiscard]] auto get_cooldown() const -> int { return decrementor; }

  private:
	int decrementor{};
	int native_time{};
};

} // namespace util