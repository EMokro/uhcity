#ifndef GAME_SERVER_CITY_ZZRCONCMDS_H
#define GAME_SERVER_CITY_ZZRCONCMDS_H
#undef GAME_SERVER_CITY_ZZRCONCMDS_H // this file can be included several times
#ifndef CONSOLE_COMMAND
#define CONSOLE_COMMAND(name, params, flags, callback, userdata, help)
#endif

//so benutzen CONSOLE_COMMAND("name des cmds bei f2", "parameter", "CFGFLAG_SERVER, Befehl, this, "beschreibung")
//Teleport
CONSOLE_COMMAND("tele", "vi", CFGFLAG_SERVER, ConTeleport, this, "")

//Ränge
CONSOLE_COMMAND("auth", "vi", CFGFLAG_SERVER, ConAuth, this, "Set auth lvl | 4 - admin, 3 - mod, 2 - mapper, 1 - police, 0 - player")
CONSOLE_COMMAND("vip", "vi", CFGFLAG_SERVER, ConVip, this, "Enable disable vip on v")
CONSOLE_COMMAND("donor", "vi", CFGFLAG_SERVER, ConDonor, this, "Enable disable donor on v")

//Admincmds
CONSOLE_COMMAND("add_money", "vi", CFGFLAG_SERVER, ConGiveMoney, this, "add_money id money(1-5mio)")
CONSOLE_COMMAND("set_money", "vi", CFGFLAG_SERVER, ConSetMoney, this, "set_money id money")
CONSOLE_COMMAND("set_lvl", "vi", CFGFLAG_SERVER, ConSetLvl, this, "")
CONSOLE_COMMAND("set_lvl_weapon", "vii", CFGFLAG_SERVER, ConSetLvlWeapon, this, "set_lvl_weapon <id> <weaponid> <level>(max 800)")

CONSOLE_COMMAND("set_life", "vi", CFGFLAG_SERVER, ConSetLife, this, "set_life id amount")
CONSOLE_COMMAND("set_armor", "vi", CFGFLAG_SERVER, ConSetArmor, this, "set_armor id amount")
CONSOLE_COMMAND("set_client_name", "vi", CFGFLAG_SERVER, ConSetClientName, this, "set_client_name id value")
CONSOLE_COMMAND("set_client_gravity_y", "vi", CFGFLAG_SERVER, ConSetClientGravityY, this, "set_client_gravity_y id value")
CONSOLE_COMMAND("set_client_gravity_x", "vi", CFGFLAG_SERVER, ConSetClientGravityX, this, "set_client_gravity_x id value")
CONSOLE_COMMAND("event_start", "vi", CFGFLAG_SERVER, ConStartEvent, this, "event_start <id> <time> | 0 - Bounty | 1 - Money&Exp | 2 - RisingMC")
CONSOLE_COMMAND("event_abort", "", CFGFLAG_SERVER, ConAbortEvent, this, "aborts the current event")
CONSOLE_COMMAND("event_timer", "", CFGFLAG_SERVER, ConEventTimer, this, "time till next event or till event end in seconds")
CONSOLE_COMMAND("set_bounty", "vi", CFGFLAG_SERVER, ConSetBounty, this, "set_bounty <id> <amount>")

// police
CONSOLE_COMMAND("mute", "vi?r", CFGFLAG_SERVER, ConMute, this, "")
CONSOLE_COMMAND("muteip", "si?r", CFGFLAG_SERVER, ConMuteIP, this, "")
CONSOLE_COMMAND("unmute", "v", CFGFLAG_SERVER, ConUnmute, this, "")
CONSOLE_COMMAND("mutes", "", CFGFLAG_SERVER, ConMutes, this, "")
CONSOLE_COMMAND("sameip", "", CFGFLAG_SERVER, ConSameIP, this, "displays all players with the same address")
CONSOLE_COMMAND("jail", "vi", CFGFLAG_SERVER, ConJail, this, "jail v for i secounds")
CONSOLE_COMMAND("unjail", "v", CFGFLAG_SERVER, ConUnjail, this, "release v from jail")
CONSOLE_COMMAND("sendafk", "v", CFGFLAG_SERVER, ConSendAfk, this, "sendafk id")
CONSOLE_COMMAND("lookup", "v", CFGFLAG_SERVER, ConLookUp, this, "lookup id")
CONSOLE_COMMAND("kill", "i", CFGFLAG_SERVER, ConKill, this, "kill id")
CONSOLE_COMMAND("unfreeze", "v", CFGFLAG_SERVER, ConUnFreeze, this, "unfreeze id")
CONSOLE_COMMAND("freeze", "vi", CFGFLAG_SERVER, ConFreeze, this, "freeze id seconds")
CONSOLE_COMMAND("unjail", "v", CFGFLAG_SERVER, ConUnjail, this, "release v from jail")

CONSOLE_COMMAND("sendfake", "v", CFGFLAG_SERVER, ConSendFakeParams, this, "")


// filesystem
CONSOLE_COMMAND("fs_backup_accounts", "", CFGFLAG_SERVER, ConFsBackupAccounts, this, "Makes a backup")

#undef CONSOLE_COMMAND
#endif