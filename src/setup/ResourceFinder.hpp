
#pragma once

#include <SFML/Graphics.hpp>
#include <djson/json.hpp>
#include <filesystem>
#include <iostream>
#include <string>

namespace pi {

namespace fs = std::filesystem;

class ResourceFinder {

  public:

	void set_resource_path(char** argv) { resource_path = find_resources(argv[0]).string(); }

	fs::path find_resources(fs::path exe) {
		auto check = [](fs::path const& prefix) {
			auto path = prefix / "assets";
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

	std::string resource_path{};
};

} // namespace pi