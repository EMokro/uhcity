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

#endif
