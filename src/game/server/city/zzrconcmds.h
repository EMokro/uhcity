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
CONSOLE_COMMAND("Police", "vi", CFGFLAG_SERVER, ConPolice, this, "Enable disable Police on v")
CONSOLE_COMMAND("Vip", "vi", CFGFLAG_SERVER, ConVip, this, "Enable disable Police on v")
CONSOLE_COMMAND("Donor", "vi", CFGFLAG_SERVER, ConDonor, this, "Enable disable Police on v")

//Admincmds
CONSOLE_COMMAND("Jail", "vi", CFGFLAG_SERVER, ConJail, this, "Jail v for i secounds")
CONSOLE_COMMAND("up", "?v", CFGFLAG_SERVER, ConUp, this, "")
CONSOLE_COMMAND("down", "?v", CFGFLAG_SERVER, ConDown, this, "")
CONSOLE_COMMAND("left", "?v", CFGFLAG_SERVER, ConLeft, this, "")
CONSOLE_COMMAND("right", "?v", CFGFLAG_SERVER, ConRight, this, "")
CONSOLE_COMMAND("GiveMoney", "vi", CFGFLAG_SERVER, ConGiveMoney, this, "GiveMoney (Player ID) (Money [1-5Mio])") // Test cmd, need to be deleted

//Sinnvolles
//CONSOLE_COMMAND("logout", "v", CFGFLAG_SERVER, ConLogout, this, "")
CONSOLE_COMMAND("unjail", "v", CFGFLAG_SERVER, ConUnjail, this, "")


#undef CONSOLE_COMMAND

#endif