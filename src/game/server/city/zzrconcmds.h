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
CONSOLE_COMMAND("police", "vi", CFGFLAG_SERVER, ConPolice, this, "Enable disable Police on v")
CONSOLE_COMMAND("vip", "vi", CFGFLAG_SERVER, ConVip, this, "Enable disable Police on v")
CONSOLE_COMMAND("donor", "vi", CFGFLAG_SERVER, ConDonor, this, "Enable disable Police on v")

//Admincmds
CONSOLE_COMMAND("jail", "vi", CFGFLAG_SERVER, ConJail, this, "jail v for i secounds")
CONSOLE_COMMAND("up", "?v", CFGFLAG_SERVER, ConUp, this, "")
CONSOLE_COMMAND("down", "?v", CFGFLAG_SERVER, ConDown, this, "")
CONSOLE_COMMAND("left", "?v", CFGFLAG_SERVER, ConLeft, this, "")
CONSOLE_COMMAND("right", "?v", CFGFLAG_SERVER, ConRight, this, "")
CONSOLE_COMMAND("add_money", "vi", CFGFLAG_SERVER, ConGiveMoney, this, "add_money id money(1-5mio)")
CONSOLE_COMMAND("set_money", "vi", CFGFLAG_SERVER, ConSetMoney, this, "set_money id money")
CONSOLE_COMMAND("set_lvl", "vi", CFGFLAG_SERVER, ConSetLvl, this, "set_lvl id level(max 800)")
CONSOLE_COMMAND("set_life", "vi", CFGFLAG_SERVER, ConSetLife, this, "set_life id amount")
CONSOLE_COMMAND("set_armor", "vi", CFGFLAG_SERVER, ConSetArmor, this, "set_armor id amount")
CONSOLE_COMMAND("set_client_name", "vi", CFGFLAG_SERVER, ConSetClientName, this, "set_client_name id value")
CONSOLE_COMMAND("kill", "i", CFGFLAG_SERVER, ConKill, this, "kill id")

//Sinnvolles
//CONSOLE_COMMAND("logout", "v", CFGFLAG_SERVER, ConLogout, this, "")
CONSOLE_COMMAND("unjail", "v", CFGFLAG_SERVER, ConUnjail, this, "release v from jail")


#undef CONSOLE_COMMAND

#endif