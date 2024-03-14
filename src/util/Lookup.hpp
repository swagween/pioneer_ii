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

        inline std::unordered_map<int, canvas::STYLE> get_style{
            {0,     canvas::STYLE::FIRSTWIND    },
            {1,     canvas::STYLE::OVERTURNED   },
            {2,     canvas::STYLE::GRUB         },
            {3,     canvas::STYLE::TOXIC        },
            {4,     canvas::STYLE::BASE         },
            {5,     canvas::STYLE::FROZEN       },
            {6,     canvas::STYLE::NIGHT        },
            {7,     canvas::STYLE::WORM         },
            {8,     canvas::STYLE::SKY          },
            {9,     canvas::STYLE::ASH          },
            {10,    canvas::STYLE::GREATWING    },
            {11,    canvas::STYLE::ICE          },
            {12,    canvas::STYLE::SNOW         },
            {13,    canvas::STYLE::STONE        },
            {14,    canvas::STYLE::ABANDONED    },
            {15,    canvas::STYLE::ANCIENT      },
            {16,    canvas::STYLE::FACTORY      },
            {17,    canvas::STYLE::SHADOW       },
            {18,    canvas::STYLE::HOARDER      },
            {19,    canvas::STYLE::MANSION      }
        };

        inline std::unordered_map<int, canvas::BACKDROP> get_backdrop{
            {-1,    canvas::BACKDROP::BG_NULL       },
            {0,     canvas::BACKDROP::BG_DUSK       },
            {1,     canvas::BACKDROP::BG_SUNRISE    },
            {2,     canvas::BACKDROP::BG_OPEN_SKY   },
            {3,     canvas::BACKDROP::BG_ROSY_HAZE  },
            {4,     canvas::BACKDROP::BG_DAWN       },
            {5,     canvas::BACKDROP::BG_NIGHT      },
            {6,     canvas::BACKDROP::BG_OVERCAST   },

            {7,     canvas::BACKDROP::BG_SLIME      },
            {8,     canvas::BACKDROP::BG_BLACK      },
            {9,     canvas::BACKDROP::BG_NAVY       },
            {10,    canvas::BACKDROP::BG_DIRT       },
            {11,    canvas::BACKDROP::BG_GEAR       },
            {12,    canvas::BACKDROP::BG_LIBRARY    },
            {13,    canvas::BACKDROP::BG_GRANITE    },
            {14,    canvas::BACKDROP::BG_RUINS      },
            {15,    canvas::BACKDROP::BG_CREVASSE   },
            {16,    canvas::BACKDROP::BG_DEEP       },
            {17,    canvas::BACKDROP::BG_GROVE      }
        };

        inline std::unordered_map<canvas::STYLE, int> get_style_id{
            {canvas::STYLE::FIRSTWIND, 0    },
            {canvas::STYLE::OVERTURNED, 1   },
            {canvas::STYLE::GRUB, 2         },
            {canvas::STYLE::TOXIC, 3        },
            {canvas::STYLE::BASE, 4         },
            {canvas::STYLE::FROZEN, 5       },
            {canvas::STYLE::NIGHT, 6        },
            {canvas::STYLE::WORM, 7         },
            {canvas::STYLE::SKY, 8          },
            {canvas::STYLE::ASH, 9          },
            {canvas::STYLE::GREATWING, 10   },
            {canvas::STYLE::ICE, 11         },
            {canvas::STYLE::SNOW, 12        },
            {canvas::STYLE::STONE, 13       },
            {canvas::STYLE::ABANDONED, 14   },
            {canvas::STYLE::ANCIENT, 15     },
            {canvas::STYLE::FACTORY, 16     },
            {canvas::STYLE::SHADOW, 17      },
            {canvas::STYLE::HOARDER, 18     },
            {canvas::STYLE::MANSION, 19     }
        };

        inline std::unordered_map<canvas::BACKDROP, int> get_backdrop_id{
            {canvas::BACKDROP::BG_NULL, -1      },
            {canvas::BACKDROP::BG_DUSK, 0       },
            {canvas::BACKDROP::BG_SUNRISE, 1    },
            {canvas::BACKDROP::BG_OPEN_SKY, 2   },
            {canvas::BACKDROP::BG_ROSY_HAZE, 3  },
            {canvas::BACKDROP::BG_DAWN, 4       },
            {canvas::BACKDROP::BG_NIGHT, 5      },
            {canvas::BACKDROP::BG_OVERCAST, 6   },
            {canvas::BACKDROP::BG_SLIME, 7      },
            {canvas::BACKDROP::BG_BLACK, 8      },
            {canvas::BACKDROP::BG_NAVY, 9       },
            {canvas::BACKDROP::BG_DIRT, 10      },
            {canvas::BACKDROP::BG_GEAR, 11      },
            {canvas::BACKDROP::BG_LIBRARY, 12   },
            {canvas::BACKDROP::BG_GRANITE, 13   },
            {canvas::BACKDROP::BG_RUINS, 14     },
            {canvas::BACKDROP::BG_CREVASSE, 15  },
            {canvas::BACKDROP::BG_DEEP, 16      },
            {canvas::BACKDROP::BG_GROVE, 17     }
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
            {canvas::BACKDROP::BG_NULL, "NULL"          },
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

        inline std::unordered_map<int, canvas::CRITTER_TYPE> get_critter_type{
            {0, canvas::CRITTER_TYPE::hulmet},
            {1, canvas::CRITTER_TYPE::tank},
            {2, canvas::CRITTER_TYPE::bunker},
            {3, canvas::CRITTER_TYPE::thug},
            {4, canvas::CRITTER_TYPE::worker},
            {5, canvas::CRITTER_TYPE::frdog},
            {6, canvas::CRITTER_TYPE::jackdaw},
            {7, canvas::CRITTER_TYPE::frcat},
            {8, canvas::CRITTER_TYPE::biter},
            {9, canvas::CRITTER_TYPE::bee},
            {10, canvas::CRITTER_TYPE::roller},
            {11, canvas::CRITTER_TYPE::snek},
            {12, canvas::CRITTER_TYPE::behemoth},
            {13, canvas::CRITTER_TYPE::stinger},
            {14, canvas::CRITTER_TYPE::watchman},
            {15, canvas::CRITTER_TYPE::gnat},
            {16, canvas::CRITTER_TYPE::moth},
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

