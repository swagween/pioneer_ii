//
//  Lookup.hpp
//  Pioneer-Lab
//
//  Created by Alex Frasca on 9/28/20.
//

#pragma once

#include <stdio.h>
#include <string>
#include <unordered_map>
#include "../canvas/Tile.hpp"
#include "../automa/GameState.hpp"
#include "../util/ServiceLocator.hpp"

namespace pi {

    namespace lookup {

        inline std::unordered_map<automa::STATE, std::string> get_state_string{
            {automa::STATE::NONE,       "None"      },
            {automa::STATE::METAGRID,   "Metagrid"  },
            {automa::STATE::EDITOR,     "Editor"    }
        };

        inline std::unordered_map<tool::TOOL_TYPE, std::string> get_tool_string{
            {tool::TOOL_TYPE::BRUSH,      "Brush" },
            {tool::TOOL_TYPE::HAND,       "Hand"  },
            {tool::TOOL_TYPE::FILL,       "Fill"  },
            {tool::TOOL_TYPE::ERASE,      "Erase" },
            {tool::TOOL_TYPE::SELECT,     "Select"},
            {tool::TOOL_TYPE::ENTITY_PLACER, "Entity Placer" }
        };

        inline std::unordered_map<canvas::STYLE, const char*> get_style_string{
            {canvas::STYLE::FIRSTWIND,  "firstwind"     },
            {canvas::STYLE::OVERTURNED, "overturned"    },
            {canvas::STYLE::GRUB,       "grub"          },
            {canvas::STYLE::TOXIC,      "toxic"         },
            {canvas::STYLE::BASE,       "base"          },
            {canvas::STYLE::FROZEN,     "frozen"        },
            {canvas::STYLE::NIGHT,      "night"         },
            {canvas::STYLE::WORM,       "worm"          },
            {canvas::STYLE::SKY,        "sky"           },
            {canvas::STYLE::ASH,        "ash"           },
            {canvas::STYLE::GREATWING,  "greatwing"     },
            {canvas::STYLE::ICE,        "ice"           },
            {canvas::STYLE::SNOW,       "snow"          },
            {canvas::STYLE::STONE,      "stone"         },
            {canvas::STYLE::ABANDONED,  "abandoned"     },
            {canvas::STYLE::ANCIENT,    "ancient"       },
            {canvas::STYLE::FACTORY,    "factory"       },
            {canvas::STYLE::SHADOW,     "shadow"        },
            {canvas::STYLE::HOARDER,    "hoarder"       },
            {canvas::STYLE::MANSION,    "mansion"       }
        };

        inline std::unordered_map<canvas::BACKDROP, const char*> get_backdrop_string{
            {canvas::BACKDROP::BG_DUSK, "DUSK"          },
            {canvas::BACKDROP::BG_SUNRISE, "SUNRISE"    },
            {canvas::BACKDROP::BG_OPEN_SKY, "OPEN_SKY"  },
            {canvas::BACKDROP::BG_ROSY_HAZE, "ROSY_HAZE"},
            {canvas::BACKDROP::BG_DAWN, "DAWN"          },
            {canvas::BACKDROP::BG_NIGHT, "NIGHT"        },
            {canvas::BACKDROP::BG_OVERCAST, "OVERCAST"  },
            {canvas::BACKDROP::BG_SLIME, "SLIME"        },
            {canvas::BACKDROP::BG_BLACK, "BLACK"        },
            {canvas::BACKDROP::BG_NAVY, "NAVY"          },
            {canvas::BACKDROP::BG_DIRT, "DIRT"          },
            {canvas::BACKDROP::BG_GEAR, "GEAR"          },
            {canvas::BACKDROP::BG_LIBRARY, "LIBRARY"    },
            {canvas::BACKDROP::BG_GRANITE, "GRANITE"    },
            {canvas::BACKDROP::BG_RUINS, "RUINS"        },
            {canvas::BACKDROP::BG_CREVASSE, "CREVASSE"  },
            {canvas::BACKDROP::BG_DEEP, "DEEP"          },
            {canvas::BACKDROP::BG_GROVE, "GROVE"        }
        };

        inline std::unordered_map<canvas::CRITTER_TYPE, const char*> get_critter_string{
            {canvas::CRITTER_TYPE::hulmet, "HULMET"},
            {canvas::CRITTER_TYPE::tank, "TANK"},
            {canvas::CRITTER_TYPE::bunker, "BUNKER"},
            {canvas::CRITTER_TYPE::thug, "THUG"},
            {canvas::CRITTER_TYPE::worker, "WORKER"},
            {canvas::CRITTER_TYPE::frdog, "FRDOG"},
            {canvas::CRITTER_TYPE::jackdaw, "JACKDAW"},
            {canvas::CRITTER_TYPE::frcat, "FRCAT"},
            {canvas::CRITTER_TYPE::biter, "BITER"},
            {canvas::CRITTER_TYPE::bee, "BEE"},
            {canvas::CRITTER_TYPE::roller, "ROLLER"},
            {canvas::CRITTER_TYPE::snek, "SNEK"},
            {canvas::CRITTER_TYPE::behemoth, "BEHEMOTH"},
            {canvas::CRITTER_TYPE::stinger, "STINGER"},
            {canvas::CRITTER_TYPE::watchman, "WATCHMAN"},
            {canvas::CRITTER_TYPE::gnat, "GNAT"},
            {canvas::CRITTER_TYPE::moth, "MOTH"}
        };

        inline std::unordered_map<int, const char*> layer_name{
            {0, "Background 0"},
            {1, "Background 1"},
            {2, "Background 2"},
            {3, "Background 3"},
            {4, "Middleground / Collidable"},
            {5, "Foreground 1"},
            {6, "Foreground 2"},
            {7, "Foreground 3"},
        };

    }

}

