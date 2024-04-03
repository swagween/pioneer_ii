//
//  MapLookups.hpp
//
//  Created by Alex Frasca on 12/26/22.
//

#pragma once


#include <SFML/Graphics.hpp>
#include <string>
#include <unordered_map>

namespace lookup {

    //this should not be inline, but rather loaded from a .txt file generated from existing maps (or the metagrid?)
inline std::unordered_map<int, std::string> get_map_label {

    {101,     "FIRSTWIND_PRISON_01"     },
    {108,     "FIRSTWIND_CLOSET_01"     },
    {104,     "FIRSTWIND_ATRIUM_01"     },
    {103,     "FIRSTWIND_LAB_01"     },
    {102,     "FIRSTWIND_CORRIDOR_01"   },
    {105,     "FIRSTWIND_CORRIDOR_02"   },
    {106,     "FIRSTWIND_CARGO_01"   },
    {107,     "FIRSTWIND_SHAFT_01"   },
    {109,     "FIRSTWIND_SHAFT_02"   },
    {110,     "FIRSTWIND_HOLD_01"   },
    {111,     "FIRSTWIND_ATRIUM_02"   },
    {112,     "FIRSTWIND_HANGAR_01"   },
    {113,     "FIRSTWIND_CORRIDOR_03"   },
    {114,     "FIRSTWIND_CLOSET_02"   },
    {115,     "FIRSTWIND_LAB_01"   },
    {116,     "FIRSTWIND_SHAFT_03"   },
    {117,     "FIRSTWIND_BRIDGE_01"   },
    {118,     "FIRSTWIND_BRIDGE_02"   },
    {119,     "FIRSTWIND_OUTSIDE_01"   },
    {120,     "FIRSTWIND_CLOSET_03"   },
    {121,     "FIRSTWIND_INFIRMARY_01"   },
    {122,     "FIRSTWIND_DECK_01"   },
    {198,     "OVERTURNED_DOJO_02"   },
    {199,     "OVERTURNED_DOJO_01"   },

    {305,     "WOODSHINE_VILLAGE_01"   },
    {306,     "WOODSHINE_GAIA_01"   },
    {307,     "WOODSHINE_SHACK_01"   },
    {308,     "WOODSHINE_INN_01"   },
    {309,     "WOODSHINE_BRODY_01"   },
    {310,     "WOODSHINE_HALL_01"   },
    {311,     "WOODSHINE_WAREHOUSE_01"   },
    {312,     "WOODSHINE_BRODY_02"   },

    {800,     "UNDER_LEDGE_01"       },
    {801,     "UNDER_HOME_01"       },
    {802,     "SHADOW_CORRIDOR_01"      },
    {803,     "SHADOW_CORRIDOR_02"      },
    {804,     "SHADOW_LEDGE_01"         },

    {1101,     "STONE_CORRIDOR_01"      },
    {1102,     "STONE_CHAMBER_01"      },
    {1103,     "STONE_HALL_01"      },

    {1701,     "SKY_CHAMBER_01"      },
    {1702,     "SKY_CHAMBER_02"      },

    {1703,     "SKY_CHAMBER_03"      },
    {1704,     "SKY_CHAMBER_04"      },
    {1705,     "SKY_CHAMBER_05"      }
};

}

/* LookupTables_hpp */
