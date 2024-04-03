//
//  LookupTables.hpp
//
//  Created by Alex Frasca on 12/26/22.
//

#pragma once


#include <SFML/Graphics.hpp>
#include <string>

#include "ServiceLocator.hpp"
#include "../weapon/Weapon.hpp"

namespace lookup {

	inline std::unordered_map<arms::WEAPON_TYPE, std::vector<sf::Sprite>&> weapon_sprites{
		{arms::WEAPON_TYPE::BRYNS_GUN,  svc::assetLocator.get().sp_bryns_gun},
		{arms::WEAPON_TYPE::PLASMER,    svc::assetLocator.get().sp_plasmer  },
		{arms::WEAPON_TYPE::WASP,       svc::assetLocator.get().sp_bryns_gun},
		{arms::WEAPON_TYPE::BLIZZARD,   svc::assetLocator.get().sp_bryns_gun},
		{arms::WEAPON_TYPE::BISMUTH,    svc::assetLocator.get().sp_bryns_gun},
		{arms::WEAPON_TYPE::UNDERDOG,   svc::assetLocator.get().sp_bryns_gun},
		{arms::WEAPON_TYPE::ELECTRON,   svc::assetLocator.get().sp_bryns_gun},
		{arms::WEAPON_TYPE::CLOVER,     svc::assetLocator.get().sp_clover   },
		{arms::WEAPON_TYPE::TRITON,     svc::assetLocator.get().sp_bryns_gun},
		{arms::WEAPON_TYPE::WILLET_585, svc::assetLocator.get().sp_bryns_gun},
		{arms::WEAPON_TYPE::QUASAR,     svc::assetLocator.get().sp_bryns_gun},
		{arms::WEAPON_TYPE::NOVA,       svc::assetLocator.get().sp_bryns_gun},
		{arms::WEAPON_TYPE::VENOM,      svc::assetLocator.get().sp_bryns_gun},
		{arms::WEAPON_TYPE::TWIN,       svc::assetLocator.get().sp_bryns_gun},
		{arms::WEAPON_TYPE::CARISE,     svc::assetLocator.get().sp_bryns_gun},
		{arms::WEAPON_TYPE::STINGER,    svc::assetLocator.get().sp_bryns_gun},
		{arms::WEAPON_TYPE::TUSK,       svc::assetLocator.get().sp_bryns_gun},
		{arms::WEAPON_TYPE::TOMAHAWK,   svc::assetLocator.get().sp_bryns_gun}
	};

	inline std::unordered_map<arms::WEAPON_TYPE, sf::Texture&> weapon_texture {
	{arms::WEAPON_TYPE::BRYNS_GUN,  svc::assetLocator.get().t_bryns_gun},
	{arms::WEAPON_TYPE::PLASMER,    svc::assetLocator.get().t_plasmer  },
	{arms::WEAPON_TYPE::WASP,       svc::assetLocator.get().t_bryns_gun},
	{arms::WEAPON_TYPE::BLIZZARD,   svc::assetLocator.get().t_bryns_gun},
	{arms::WEAPON_TYPE::BISMUTH,    svc::assetLocator.get().t_bryns_gun},
	{arms::WEAPON_TYPE::UNDERDOG,   svc::assetLocator.get().t_bryns_gun},
	{arms::WEAPON_TYPE::ELECTRON,   svc::assetLocator.get().t_bryns_gun},
	{arms::WEAPON_TYPE::CLOVER,     svc::assetLocator.get().t_clover   },
	{arms::WEAPON_TYPE::TRITON,     svc::assetLocator.get().t_bryns_gun},
	{arms::WEAPON_TYPE::WILLET_585, svc::assetLocator.get().t_bryns_gun},
	{arms::WEAPON_TYPE::QUASAR,     svc::assetLocator.get().t_bryns_gun},
	{arms::WEAPON_TYPE::NOVA,       svc::assetLocator.get().t_nova	},
	{arms::WEAPON_TYPE::VENOM,      svc::assetLocator.get().t_bryns_gun},
	{arms::WEAPON_TYPE::TWIN,       svc::assetLocator.get().t_bryns_gun},
	{arms::WEAPON_TYPE::CARISE,     svc::assetLocator.get().t_bryns_gun},
	{arms::WEAPON_TYPE::STINGER,    svc::assetLocator.get().t_bryns_gun},
	{arms::WEAPON_TYPE::TUSK,       svc::assetLocator.get().t_bryns_gun},
	{arms::WEAPON_TYPE::TOMAHAWK,   svc::assetLocator.get().t_bryns_gun}
	};

	inline std::unordered_map<arms::WEAPON_TYPE, std::vector<sf::Sprite>&> projectile_sprites{
		{arms::WEAPON_TYPE::BRYNS_GUN,  svc::assetLocator.get().sp_bryns_gun_projectile},
		{arms::WEAPON_TYPE::PLASMER,    svc::assetLocator.get().sp_plasmer_projectile  },
		{arms::WEAPON_TYPE::WASP,       svc::assetLocator.get().sp_bryns_gun_projectile},
		{arms::WEAPON_TYPE::BLIZZARD,   svc::assetLocator.get().sp_bryns_gun_projectile},
		{arms::WEAPON_TYPE::BISMUTH,    svc::assetLocator.get().sp_bryns_gun_projectile},
		{arms::WEAPON_TYPE::UNDERDOG,   svc::assetLocator.get().sp_bryns_gun_projectile},
		{arms::WEAPON_TYPE::ELECTRON,   svc::assetLocator.get().sp_bryns_gun_projectile},
		{arms::WEAPON_TYPE::CLOVER,     svc::assetLocator.get().sp_clover_projectile   },
		{arms::WEAPON_TYPE::TRITON,     svc::assetLocator.get().sp_bryns_gun_projectile},
		{arms::WEAPON_TYPE::WILLET_585, svc::assetLocator.get().sp_bryns_gun_projectile},
		{arms::WEAPON_TYPE::QUASAR,     svc::assetLocator.get().sp_bryns_gun_projectile},
		{arms::WEAPON_TYPE::NOVA,       svc::assetLocator.get().sp_bryns_gun_projectile},
		{arms::WEAPON_TYPE::VENOM,      svc::assetLocator.get().sp_bryns_gun_projectile},
		{arms::WEAPON_TYPE::TWIN,       svc::assetLocator.get().sp_bryns_gun_projectile},
		{arms::WEAPON_TYPE::CARISE,     svc::assetLocator.get().sp_bryns_gun_projectile},
		{arms::WEAPON_TYPE::STINGER,    svc::assetLocator.get().sp_bryns_gun_projectile},
		{arms::WEAPON_TYPE::TUSK,       svc::assetLocator.get().sp_bryns_gun_projectile},
		{arms::WEAPON_TYPE::TOMAHAWK,   svc::assetLocator.get().sp_bryns_gun_projectile}
	};

	inline std::unordered_map<arms::WEAPON_TYPE, sf::Texture&> projectile_texture {
		{arms::WEAPON_TYPE::BRYNS_GUN,  svc::assetLocator.get().t_bryns_gun_projectile},
	{arms::WEAPON_TYPE::PLASMER,    svc::assetLocator.get().t_plasmer_projectile  },
	{arms::WEAPON_TYPE::WASP,       svc::assetLocator.get().t_wasp_projectile},
	{arms::WEAPON_TYPE::BLIZZARD,   svc::assetLocator.get().t_blizzard_projectile},
	{arms::WEAPON_TYPE::BISMUTH,    svc::assetLocator.get().t_bismuth_projectile},
	{arms::WEAPON_TYPE::UNDERDOG,   svc::assetLocator.get().t_underdog_projectile},
	{arms::WEAPON_TYPE::ELECTRON,   svc::assetLocator.get().t_electron_projectile},
	{arms::WEAPON_TYPE::CLOVER,     svc::assetLocator.get().t_clover_projectile   },
	{arms::WEAPON_TYPE::TRITON,     svc::assetLocator.get().t_triton_projectile},
	{arms::WEAPON_TYPE::WILLET_585, svc::assetLocator.get().t_willet_585_projectile},
	{arms::WEAPON_TYPE::QUASAR,     svc::assetLocator.get().t_quasar_projectile},
	{arms::WEAPON_TYPE::NOVA,       svc::assetLocator.get().t_nova_projectile},
	{arms::WEAPON_TYPE::VENOM,      svc::assetLocator.get().t_venom_projectile},
	{arms::WEAPON_TYPE::TWIN,       svc::assetLocator.get().t_twin_projectile},
	{arms::WEAPON_TYPE::CARISE,     svc::assetLocator.get().t_carise_projectile},
	{arms::WEAPON_TYPE::STINGER,    svc::assetLocator.get().t_stinger_projectile},
	{arms::WEAPON_TYPE::TUSK,       svc::assetLocator.get().t_tusk_projectile},
	{arms::WEAPON_TYPE::TOMAHAWK,   svc::assetLocator.get().t_tomahawk_projectile}
	};

	inline std::unordered_map<arms::WEAPON_TYPE, arms::Weapon&> type_to_weapon{
		{arms::WEAPON_TYPE::BRYNS_GUN,  arms::bryns_gun     },
		{arms::WEAPON_TYPE::PLASMER,    arms::plasmer       },
		{arms::WEAPON_TYPE::WASP,       arms::wasp          },
		{arms::WEAPON_TYPE::BLIZZARD,   arms::blizzard      },
		{arms::WEAPON_TYPE::BISMUTH,    arms::bismuth       },
		{arms::WEAPON_TYPE::UNDERDOG,   arms::underdog      },
		{arms::WEAPON_TYPE::ELECTRON,   arms::electron      },
		{arms::WEAPON_TYPE::CLOVER,     arms::clover        },
		{arms::WEAPON_TYPE::TRITON,     arms::triton        },
		{arms::WEAPON_TYPE::WILLET_585, arms::willet_585    },
		{arms::WEAPON_TYPE::QUASAR,     arms::quasar        },
		{arms::WEAPON_TYPE::NOVA,       arms::nova          },
		{arms::WEAPON_TYPE::VENOM,      arms::venom         },
		{arms::WEAPON_TYPE::TWIN,       arms::twin          },
		{arms::WEAPON_TYPE::CARISE,     arms::carise        },
		{arms::WEAPON_TYPE::STINGER,    arms::stinger       },
		{arms::WEAPON_TYPE::TUSK,       arms::tusk          },
		{arms::WEAPON_TYPE::TOMAHAWK,   arms::tomahawk      }
	};

	inline std::unordered_map<arms::WEAPON_TYPE, int> type_to_index{
		{arms::WEAPON_TYPE::BRYNS_GUN, 0},
		{arms::WEAPON_TYPE::PLASMER, 1},
		{arms::WEAPON_TYPE::WASP, 2},
		{arms::WEAPON_TYPE::BLIZZARD, 3},
		{arms::WEAPON_TYPE::BISMUTH, 4},
		{arms::WEAPON_TYPE::UNDERDOG, 5 },
		{arms::WEAPON_TYPE::ELECTRON, 6 },
	{ arms::WEAPON_TYPE::CLOVER, 7 },
	{ arms::WEAPON_TYPE::TRITON, 8 },
	{ arms::WEAPON_TYPE::WILLET_585, 9 },
	{ arms::WEAPON_TYPE::QUASAR, 10 },
	{ arms::WEAPON_TYPE::NOVA, 11 },
	{ arms::WEAPON_TYPE::VENOM, 12 },
	{ arms::WEAPON_TYPE::TWIN, 13 },
	{ arms::WEAPON_TYPE::CARISE, 14 },
	{ arms::WEAPON_TYPE::STINGER, 15 },
	{ arms::WEAPON_TYPE::TUSK, 16 },
	{arms::WEAPON_TYPE::TOMAHAWK, 17 }
	};

}

/* LookupTables_hpp */
