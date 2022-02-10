/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_VARIABLES_H
#define GAME_VARIABLES_H
#undef GAME_VARIABLES_H // this file will be included several times

// City
MACRO_CONFIG_INT(EuHealth, eu_health, 50000, 0, 1000000, CFGFLAG_SERVER, "Price - Health+1")
MACRO_CONFIG_INT(EuArmor, eu_armor, 50000, 0, 1000000, CFGFLAG_SERVER, "Price - Armor+1")

MACRO_CONFIG_INT(EuVIP, eu_vip, 1000000, 0, 100000000, CFGFLAG_SERVER, "Price - VIP")
//MACRO_CONFIG_INT(EuArrested, eu_arrested, 10000, 0, 1000000, CFGFLAG_SERVER, "Price - Freedom")

MACRO_CONFIG_INT(EuAllWeapons, eu_allweapons, 100000, 0, 10000000, CFGFLAG_SERVER, "Price - All weapons")
MACRO_CONFIG_INT(EuHealthRegen, eu_health_regen, 1000000, 0, 10000000, CFGFLAG_SERVER, "Price - Health Regeneration")
MACRO_CONFIG_INT(EuInfAmmo, eu_infammo, 500000, 0, 10000000, CFGFLAG_SERVER, "Price - Infinity ammo")
MACRO_CONFIG_INT(EuInfJumps, eu_infjumps, 16000000, 0, 10000000, CFGFLAG_SERVER, "Price - Infinity jumps")
MACRO_CONFIG_INT(EuFastReload, eu_fast_reload, 500000, 0, 10000000, CFGFLAG_SERVER, "Price - Fast reload")
MACRO_CONFIG_INT(EuNoSelfDMG, eu_no_selfdmg, 500000, 0, 10000000, CFGFLAG_SERVER, "Price - No self Damage")

MACRO_CONFIG_INT(EuGrenadeSpread, eu_grenade_spread, 2000000, 0, 10000000, CFGFLAG_SERVER, "Price - Grenade spread")
MACRO_CONFIG_INT(EuGrenadeBounce, eu_grenade_bounce, 5000000, 0, 10000000, CFGFLAG_SERVER, "Price - Grenade ammo")

MACRO_CONFIG_INT(EuShotgunSpread, eu_shotgun_spread, 2000000, 0, 10000000, CFGFLAG_SERVER, "Price - Shotgun spread")
MACRO_CONFIG_INT(EuShotgunExplode, eu_shotgun_explode, 5000000, 0, 10000000, CFGFLAG_SERVER, "Price - Shotgun explode")

MACRO_CONFIG_INT(EuRifleSpread, eu_rifle_spread, 2000000, 0, 10000000, CFGFLAG_SERVER, "Price - Rifle spread")
MACRO_CONFIG_INT(EuRifleSwap, eu_rifle_swap, 5000000, 0, 1000000, CFGFLAG_SERVER, "Price - Rifle swap")
MACRO_CONFIG_INT(EuRiflePlasma, eu_rifle_plasma, 100000000, 0, 100000000, CFGFLAG_SERVER, "Price - Rifle plasma")

MACRO_CONFIG_INT(EuGunSpread, eu_gun_spread, 2000000, 0, 100000000, CFGFLAG_SERVER, "Price - Gun spread")
MACRO_CONFIG_INT(EuGunExplode, eu_gun_explode, 5000000, 0, 100000000, CFGFLAG_SERVER, "Price - Gun explode")
MACRO_CONFIG_INT(EuGunFreeze, eu_gun_freeze, 800000000, 0, 1000000000, CFGFLAG_SERVER, "Price - Gun ammo")

MACRO_CONFIG_INT(EuHammerWalls, eu_hammer_walls, 50000000, 0, 1000000, CFGFLAG_SERVER, "Price - Hammer walls")
MACRO_CONFIG_INT(EuHammerShot, eu_hammer_shot, 30000000, 0, 1000000, CFGFLAG_SERVER, "Price - Hammer shot")
MACRO_CONFIG_INT(EuHammerKill, eu_hammer_kill, 100000000, 0, 500000000, CFGFLAG_SERVER, "Price - Hammer kill")
MACRO_CONFIG_LONGLONG(EuPortal, eu_portal, 500000000, 0, 1000000, CFGFLAG_SERVER, "Price - Portal")

MACRO_CONFIG_INT(EuNinjaPermanent, eu_ninja_permanent, 5000000, 0, 1000000, CFGFLAG_SERVER, "Price - Ninja permanent")
MACRO_CONFIG_INT(EuNinjaStart, eu_ninja_start, 100000, 0, 1000000, CFGFLAG_SERVER, "Price - Ninja start")
MACRO_CONFIG_INT(EuNinjaSwitch, eu_ninja_switch, 100000, 0, 1000000, CFGFLAG_SERVER, "Price - Ninja switch")
MACRO_CONFIG_LONGLONG(EuNinjaFly, eu_ninja_fly, 1000000, 0, 900000000000, CFGFLAG_SERVER, "Price - Ninja fly")
MACRO_CONFIG_LONGLONG(EuNinjaBomber, eu_ninja_bomber, 1000000, 0, 900000000000, CFGFLAG_SERVER, "Price - Ninja bomber")

MACRO_CONFIG_LONGLONG(EuHookEndless, eu_hook_endless, 500000, 0, 900000000000, CFGFLAG_SERVER, "Price - Endlesshook")
MACRO_CONFIG_LONGLONG(EuHookHeal, eu_hook_heal, 50000000, 0, 900000000000, CFGFLAG_SERVER, "Price - Healhook")
MACRO_CONFIG_LONGLONG(EuHookBoost, eu_hook_boost, 50000000, 0, 900000000000, CFGFLAG_SERVER, "Price - Boosthook")

MACRO_CONFIG_LONGLONG(EuPushAura, eu_special_pushaura, 1000000000, 0, 900000000000, CFGFLAG_SERVER, "Price - PushAura")
MACRO_CONFIG_LONGLONG(EuPullAura, eu_special_pullaura, 1000000000, 0, 900000000000, CFGFLAG_SERVER, "Price - PullAura")

MACRO_CONFIG_STR(SvBroadcast, sv_broadcast, 128, "", CFGFLAG_SERVER, "Broadcast message")

MACRO_CONFIG_INT(EnableInstagib, sv_enable_instagib, 1, 0, 1, CFGFLAG_SERVER, "Enable/Disable Instagib")
MACRO_CONFIG_INT(EnableMonster, sv_enable_monster, 1, 0, 1, CFGFLAG_SERVER, "Enable/Disable Monster")
MACRO_CONFIG_INT(SvChatDelay, sv_chat_delay, 1, 0, 9999, CFGFLAG_SERVER, "The time in seconds between chat messages")
MACRO_CONFIG_INT(SvChatPenalty, sv_chat_penalty, 250, 50, 1000, CFGFLAG_SERVER, "chat score will be increased by this on every message, and decremented by 1 on every tick.")
MACRO_CONFIG_INT(SvChatThreshold, sv_chat_threshold, 1000, 50, 10000 , CFGFLAG_SERVER, "if chats core exceeds this, the player will be muted for sv_spam_mute_duration seconds")
MACRO_CONFIG_INT(SvSpamMuteDuration, sv_spam_mute_duration, 60, 0, 3600 , CFGFLAG_SERVER, "how many seconds to mute, if player triggers mute on spam. 0 = off")

// client
MACRO_CONFIG_INT(ClPredict, cl_predict, 1, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Predict client movements")
MACRO_CONFIG_INT(ClNameplates, cl_nameplates, 1, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Show name plates")
MACRO_CONFIG_INT(ClNameplatesAlways, cl_nameplates_always, 1, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Always show name plates disregarding of distance")
MACRO_CONFIG_INT(ClNameplatesTeamcolors, cl_nameplates_teamcolors, 1, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Use team colors for name plates")
MACRO_CONFIG_INT(ClNameplatesSize, cl_nameplates_size, 50, 0, 100, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Size of the name plates from 0 to 100%")
MACRO_CONFIG_INT(ClAutoswitchWeapons, cl_autoswitch_weapons, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Auto switch weapon on pickup")

MACRO_CONFIG_INT(ClShowhud, cl_showhud, 1, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Show ingame HUD")
MACRO_CONFIG_INT(ClShowfps, cl_showfps, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Show ingame FPS counter")

MACRO_CONFIG_INT(ClAirjumpindicator, cl_airjumpindicator, 1, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "")
MACRO_CONFIG_INT(ClThreadsoundloading, cl_threadsoundloading, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Load sound files threaded")

MACRO_CONFIG_INT(ClWarningTeambalance, cl_warning_teambalance, 1, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Warn about team balance")

MACRO_CONFIG_INT(ClMouseDeadzone, cl_mouse_deadzone, 300, 0, 0, CFGFLAG_CLIENT|CFGFLAG_SAVE, "")
MACRO_CONFIG_INT(ClMouseFollowfactor, cl_mouse_followfactor, 60, 0, 200, CFGFLAG_CLIENT|CFGFLAG_SAVE, "")
MACRO_CONFIG_INT(ClMouseMaxDistance, cl_mouse_max_distance, 800, 0, 0, CFGFLAG_CLIENT|CFGFLAG_SAVE, "")

MACRO_CONFIG_INT(EdShowkeys, ed_showkeys, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "")

//MACRO_CONFIG_INT(ClFlow, cl_flow, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "")

MACRO_CONFIG_INT(ClShowWelcome, cl_show_welcome, 1, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "")
MACRO_CONFIG_INT(ClMotdTime, cl_motd_time, 10, 0, 100, CFGFLAG_CLIENT|CFGFLAG_SAVE, "How long to show the server message of the day")

MACRO_CONFIG_STR(ClVersionServer, cl_version_server, 100, "version.teeworlds.com", CFGFLAG_CLIENT|CFGFLAG_SAVE, "Server to use to check for new versions")

MACRO_CONFIG_STR(ClLanguagefile, cl_languagefile, 255, "", CFGFLAG_CLIENT|CFGFLAG_SAVE, "What language file to use")

MACRO_CONFIG_INT(PlayerUseCustomColor, player_use_custom_color, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Toggles usage of custom colors")
MACRO_CONFIG_INT(PlayerColorBody, player_color_body, 65408, 0, 0xFFFFFF, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Player body color")
MACRO_CONFIG_INT(PlayerColorFeet, player_color_feet, 65408, 0, 0xFFFFFF, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Player feet color")
MACRO_CONFIG_STR(PlayerSkin, player_skin, 24, "default", CFGFLAG_CLIENT|CFGFLAG_SAVE, "Player skin")

MACRO_CONFIG_INT(UiPage, ui_page, 5, 0, 10, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Interface page")
MACRO_CONFIG_INT(UiToolboxPage, ui_toolbox_page, 0, 0, 2, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Toolbox page")
MACRO_CONFIG_STR(UiServerAddress, ui_server_address, 64, "localhost:8303", CFGFLAG_CLIENT|CFGFLAG_SAVE, "Interface server address")
MACRO_CONFIG_INT(UiScale, ui_scale, 100, 50, 150, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Interface scale")
MACRO_CONFIG_INT(UiMousesens, ui_mousesens, 100, 5, 100000, CFGFLAG_SAVE|CFGFLAG_CLIENT, "Mouse sensitivity for menus/editor")

MACRO_CONFIG_INT(UiColorHue, ui_color_hue, 160, 0, 255, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Interface color hue")
MACRO_CONFIG_INT(UiColorSat, ui_color_sat, 70, 0, 255, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Interface color saturation")
MACRO_CONFIG_INT(UiColorLht, ui_color_lht, 175, 0, 255, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Interface color lightness")
MACRO_CONFIG_INT(UiColorAlpha, ui_color_alpha, 228, 0, 255, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Interface alpha")

MACRO_CONFIG_INT(GfxNoclip, gfx_noclip, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Disable clipping")

// server
MACRO_CONFIG_INT(SvWarmup, sv_warmup, 0, 0, 0, CFGFLAG_SERVER, "Number of seconds to do warmup before round starts")
MACRO_CONFIG_STR(SvMotd, sv_motd, 900, "", CFGFLAG_SERVER, "Message of the day to display for the clients")
MACRO_CONFIG_INT(SvTeamdamage, sv_teamdamage, 0, 0, 1, CFGFLAG_SERVER, "Team damage")
MACRO_CONFIG_STR(SvMaprotation, sv_maprotation, 768, "", CFGFLAG_SERVER, "Maps to rotate between")
MACRO_CONFIG_INT(SvRoundsPerMap, sv_rounds_per_map, 1, 1, 100, CFGFLAG_SERVER, "Number of rounds on each map before rotating")
MACRO_CONFIG_INT(SvPowerups, sv_powerups, 1, 0, 1, CFGFLAG_SERVER, "Allow powerups like ninja")
MACRO_CONFIG_INT(SvScorelimit, sv_scorelimit, 0, 0, 0, CFGFLAG_SERVER, "Score limit (0 disables)")
MACRO_CONFIG_INT(SvTimelimit, sv_timelimit, 0, 0, 1000, CFGFLAG_SERVER, "Time limit in minutes (0 disables)")
MACRO_CONFIG_STR(SvGametype, sv_gametype, 32, "dm", CFGFLAG_SERVER, "Game type (dm, tdm, ctf)")
MACRO_CONFIG_INT(SvTournamentMode, sv_tournament_mode, 0, 0, 1, CFGFLAG_SERVER, "Tournament mode. When enabled, players joins the server as spectator")
MACRO_CONFIG_INT(SvSpamprotection, sv_spamprotection, 1, 0, 1, CFGFLAG_SERVER, "Spam protection")

MACRO_CONFIG_INT(SvRespawnDelayTDM, sv_respawn_delay_tdm, 3, 0, 10, CFGFLAG_SERVER, "Time needed to respawn after death in tdm gametype")

MACRO_CONFIG_INT(SvSpectatorSlots, sv_spectator_slots, 0, 0, MAX_CLIENTS, CFGFLAG_SERVER, "Number of slots to reserve for spectators")
MACRO_CONFIG_INT(SvTeambalanceTime, sv_teambalance_time, 1, 0, 1000, CFGFLAG_SERVER, "How many minutes to wait before autobalancing teams")
MACRO_CONFIG_INT(SvInactiveKickTimeDonor, sv_inactivekick_time_donor, 60, 0, 1000, CFGFLAG_SERVER, "How many minutes to wait before taking care of inactive donors")
MACRO_CONFIG_INT(SvInactiveKickTime, sv_inactivekick_time, 30, 0, 1000, CFGFLAG_SERVER, "How many minutes to wait before taking care of inactive players")
MACRO_CONFIG_INT(SvInactiveKick, sv_inactivekick, 1, 0, 2, CFGFLAG_SERVER, "How to deal with inactive players (0=move to spectator, 1=move to free spectator slot/kick, 2=kick)")

MACRO_CONFIG_INT(SvStrictSpectateMode, sv_strict_spectate_mode, 0, 0, 1, CFGFLAG_SERVER, "Restricts information in spectator mode")
MACRO_CONFIG_INT(SvVoteSpectate, sv_vote_spectate, 1, 0, 1, CFGFLAG_SERVER, "Allow voting to move players to spectators")
MACRO_CONFIG_INT(SvVoteSpectateRejoindelay, sv_vote_spectate_rejoindelay, 3, 0, 1000, CFGFLAG_SERVER, "How many minutes to wait before a player can rejoin after being moved to spectators by vote")
MACRO_CONFIG_INT(SvVoteKick, sv_vote_kick, 1, 0, 1, CFGFLAG_SERVER, "Allow voting to kick players")
MACRO_CONFIG_INT(SvVoteKickMin, sv_vote_kick_min, 0, 0, MAX_CLIENTS, CFGFLAG_SERVER, "Minimum number of players required to start a kick vote")
MACRO_CONFIG_INT(SvVoteKickBantime, sv_vote_kick_bantime, 5, 0, 1440, CFGFLAG_SERVER, "The time to ban a player if kicked by vote. 0 makes it just use kick")

//Monster
MACRO_CONFIG_INT(HardNinja, monster_spawn_ninja, 1, 0, 1, CFGFLAG_SERVER, "")
MACRO_CONFIG_INT(SvDeathAnimation, monster_death_animation, 0, 0, 1, CFGFLAG_SERVER, "")


/* # Discord # */
MACRO_CONFIG_STR(SvDiscordToken, sv_discord_token, 128, "", CFGFLAG_SAVE|CFGFLAG_SERVER, "Discord Bot's token.")
MACRO_CONFIG_STR(SvDiscordServer, sv_discord_server, 128, "", CFGFLAG_SAVE|CFGFLAG_SERVER, "Discord Bot's needed server's ID.")
MACRO_CONFIG_STR(SvDiscordChannel, sv_discord_channel, 128, "", CFGFLAG_SAVE|CFGFLAG_SERVER, "Discord Bot's needed channel's ID.")
/* # Discord # */

// debug
#ifdef CONF_DEBUG // this one can crash the server if not used correctly
	MACRO_CONFIG_INT(DbgDummies, dbg_dummies, 0, 0, 15, CFGFLAG_SERVER, "")
#endif

MACRO_CONFIG_INT(DbgFocus, dbg_focus, 0, 0, 1, CFGFLAG_CLIENT, "")
MACRO_CONFIG_INT(DbgTuning, dbg_tuning, 0, 0, 1, CFGFLAG_CLIENT, "")
#endif
