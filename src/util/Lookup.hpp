
#pragma once

#include <stdio.h>
#include <string>
#include <unordered_map>
#include "../canvas/Tile.hpp"
#include "../tool/Tool.hpp"

namespace pi {

inline std::unordered_map<ToolType, std::string> get_tool_string{{ToolType::brush, "Brush"}, {ToolType::hand, "Hand"},	   {ToolType::fill, "Fill"},
																 {ToolType::erase, "Erase"}, {ToolType::select, "Select"}, {ToolType::entity_placer, "Entity Placer"}};

inline std::unordered_map<STYLE, char const*> get_style_string{{STYLE::FIRSTWIND, "firstwind"},
															   {STYLE::OVERTURNED, "overturned"},
															   {STYLE::GRUB, "grub"},
															   {STYLE::TOXIC, "toxic"},
															   {STYLE::BASE, "base"},
															   {STYLE::FROZEN, "frozen"},
															   {STYLE::NIGHT, "night"},
															   {STYLE::WORM, "worm"},
															   {STYLE::SKY, "sky"},
															   {STYLE::ASH, "ash"},
															   {STYLE::GREATWING, "greatwing"},
															   {STYLE::ICE, "ice"},
															   {STYLE::SNOW, "snow"},
															   {STYLE::STONE, "stone"},
															   {STYLE::ABANDONED, "abandoned"},
															   {STYLE::ANCIENT, "ancient"},
															   {STYLE::FACTORY, "factory"},
															   {STYLE::SHADOW, "shadow"},
															   {STYLE::HOARDER, "hoarder"},
															   {STYLE::MANSION, "mansion"}};

inline std::unordered_map<BACKDROP, char const*> get_backdrop_string{
	{BACKDROP::BG_DUSK, "DUSK"},		 {BACKDROP::BG_SUNRISE, "SUNRISE"}, {BACKDROP::BG_OPEN_SKY, "OPEN_SKY"}, {BACKDROP::BG_ROSY_HAZE, "ROSY_HAZE"}, {BACKDROP::BG_DAWN, "DAWN"}, {BACKDROP::BG_NIGHT, "NIGHT"},
	{BACKDROP::BG_OVERCAST, "OVERCAST"}, {BACKDROP::BG_SLIME, "SLIME"},		{BACKDROP::BG_BLACK, "BLACK"},		 {BACKDROP::BG_NAVY, "NAVY"},			{BACKDROP::BG_DIRT, "DIRT"}, {BACKDROP::BG_GEAR, "GEAR"},
	{BACKDROP::BG_LIBRARY, "LIBRARY"},	 {BACKDROP::BG_GRANITE, "GRANITE"}, {BACKDROP::BG_RUINS, "RUINS"},		 {BACKDROP::BG_CREVASSE, "CREVASSE"},	{BACKDROP::BG_DEEP, "DEEP"}, {BACKDROP::BG_GROVE, "GROVE"}};

inline std::unordered_map<int, char const*> layer_name{
	{0, "Background 0"}, {1, "Background 1"}, {2, "Background 2"}, {3, "Background 3"}, {4, "Middleground / Collidable"}, {5, "Foreground 1"}, {6, "Foreground 2"}, {7, "Foreground 3"},
};

}

