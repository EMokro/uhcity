/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SERVER_CITY_CONFIG_H
#define GAME_SERVER_CITY_CONFIG_H
#undef GAME_SERVER_CITY_CONFIG_H // this file will be included several times

// - fast map download
MACRO_CONFIG_INT(SvMapWindow, sv_map_window, 15, 0, 100, CFGFLAG_SERVER, "Map downloading send-ahead window")
MACRO_CONFIG_INT(SvFastDownload, sv_fast_download, 1, 0, 1, CFGFLAG_SERVER, "Enables fast download of maps")

MACRO_CONFIG_INT(SvDoorType, sv_door_type, 0, 0, 2, CFGFLAG_SERVER, "Type of laser doors. 0 = collision, 1 = unhookable, 2 = death")

// events
MACRO_CONFIG_INT(SvEventTimerMin, sv_event_timer_min, 600, 0, 0, CFGFLAG_SERVER, "The minimum time between random events in seconds")
MACRO_CONFIG_INT(SvEventTimerMax, sv_event_timer_max, 1200, 0, 0, CFGFLAG_SERVER, "The maximum time between random events in seconds")
MACRO_CONFIG_INT(SvEventDurationMin, sv_event_duration_min, 60, 0, 0, CFGFLAG_SERVER, "The minimum event duration in seconds")
MACRO_CONFIG_INT(SvEventDurationMax, sv_event_duration_min, 240, 0, 0, CFGFLAG_SERVER, "The maximum event duration in seconds")

MACRO_CONFIG_INT(SvEventBountyMin, sv_event_bounty_min, 100000, 1000, 0, CFGFLAG_SERVER, "The minimum bounty amount")
MACRO_CONFIG_INT(SvEventBountyMax, sv_event_bounty_max, 500000, 1000, 100000000, CFGFLAG_SERVER, "The maximum bounty amount")

// coach
MACRO_CONFIG_INT(SvExpPrice, sv_exp_price, 1000000, 0, 50000000, CFGFLAG_SERVER, "The price one exp costs")

// items
MACRO_CONFIG_INT(SvBoostHookStr, sv_boost_hook_str, 50, 1, 1000000, CFGFLAG_SERVER, "Strength of boosthook")

MACRO_CONFIG_INT(SvGravAuraRadius, sv_gravaura_radius, 160, 1, 1000000, CFGFLAG_SERVER, "Gravity aura radius")
MACRO_CONFIG_INT(SvGravAuraForce, sv_gravaura_force, 2, 1, 1000000, CFGFLAG_SERVER, "Gravity aura force strength")
MACRO_CONFIG_INT(SvGravAuraCooldown, sv_gravaura_cooldown, 120, 0, 1000000, CFGFLAG_SERVER, "Gravity aura Cooldown in s")

// - 64 player
MACRO_CONFIG_INT(SvMapUpdateRate, sv_mapupdaterate, 5, 1, 100, CFGFLAG_SERVER, "How often the ClientID's are swapped for a player")

#endif
