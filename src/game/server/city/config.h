/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SERVER_CITY_CONFIG_H
#define GAME_SERVER_CITY_CONFIG_H
#undef GAME_SERVER_CITY_CONFIG_H // this file will be included several times

// - fast map download
MACRO_CONFIG_INT(SvMapWindow, sv_map_window, 15, 0, 100, CFGFLAG_SERVER, "Map downloading send-ahead window")
MACRO_CONFIG_INT(SvFastDownload, sv_fast_download, 1, 0, 1, CFGFLAG_SERVER, "Enables fast download of maps")

MACRO_CONFIG_INT(SvDoorType, sv_door_type, 0, 0, 2, CFGFLAG_SERVER, "Type of laser doors. 0 = collision, 1 = unhookable, 2 = death")

#endif
