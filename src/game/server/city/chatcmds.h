#ifndef GAME_SERVER_CITY_CHATCMDS_H
#define GAME_SERVER_CITY_CHATCMDS_H
#undef GAME_SERVER_CITY_CHATCMDS_H // this file can be included several times
#ifndef CONSOLE_COMMAND
#define CONSOLE_COMMAND(name, params, flags, callback, userdata, help)
#endif

// account
CONSOLE_COMMAND("login", "?s?s", CFGFLAG_CHAT, ConChatLogin, this, "")
CONSOLE_COMMAND("register", "?s?s", CFGFLAG_CHAT, ConChatRegister, this, "")
CONSOLE_COMMAND("logout", "", CFGFLAG_CHAT, ConChatLogout, this, "")
CONSOLE_COMMAND("changepw", "?s", CFGFLAG_CHAT, ConChatChangePw, this, "")

// functions
CONSOLE_COMMAND("save", "", CFGFLAG_CHAT, ConChatSave, this, "")
CONSOLE_COMMAND("load", "", CFGFLAG_CHAT, ConChatLoad, this, "")
CONSOLE_COMMAND("tele", "", CFGFLAG_CHAT, ConChatTele, this, "")
CONSOLE_COMMAND("home", "", CFGFLAG_CHAT, ConChatHome, this, "")
CONSOLE_COMMAND("rainbow", "", CFGFLAG_CHAT, ConChatRainbow, this, "")
CONSOLE_COMMAND("crown", "", CFGFLAG_CHAT, ConChatRainbow, this, "")
CONSOLE_COMMAND("up", "", CFGFLAG_CHAT, ConChatUp, this, "")
CONSOLE_COMMAND("down", "", CFGFLAG_CHAT, ConChatDown, this, "")
CONSOLE_COMMAND("left", "", CFGFLAG_CHAT, ConChatLeft, this, "")
CONSOLE_COMMAND("right", "", CFGFLAG_CHAT, ConChatRight, this, "")
CONSOLE_COMMAND("jailrifle", "", CFGFLAG_CHAT, ConChatJailrifle, this, "")
CONSOLE_COMMAND("god", "", CFGFLAG_CHAT, ConChatGod, this, "")
CONSOLE_COMMAND("instagib", "", CFGFLAG_CHAT, ConChatInstagib, this, "")
CONSOLE_COMMAND("transfer", "?i", CFGFLAG_CHAT, ConChatTransfer, this, "")
CONSOLE_COMMAND("disabledmg", "?v", CFGFLAG_CHAT, ConChatDisabledmg, this, "")
CONSOLE_COMMAND("enabledmg", "?v", CFGFLAG_CHAT, ConChatEnabledmg, this, "")
CONSOLE_COMMAND("train", "?s?i", CFGFLAG_CHAT, ConChatTrain, this, "")
CONSOLE_COMMAND("bountylist", "?i", CFGFLAG_CHAT, ConChatBountylist, this, "")
CONSOLE_COMMAND("checkbounty", "?v", CFGFLAG_CHAT, ConChatCheckbounty, this, "")
CONSOLE_COMMAND("setbounty", "?v?i", CFGFLAG_CHAT, ConChatSetbounty, this, "")

// info
CONSOLE_COMMAND("me", "", CFGFLAG_CHAT, ConChatMe, this, "")
CONSOLE_COMMAND("cmdlist", "?i", CFGFLAG_CHAT, ConChatCmdlist, this, "")
CONSOLE_COMMAND("help", "", CFGFLAG_CHAT, ConChatHelp, this, "")
CONSOLE_COMMAND("donor", "", CFGFLAG_CHAT, ConChatDonor, this, "")
CONSOLE_COMMAND("vip", "", CFGFLAG_CHAT, ConChatVip, this, "")
CONSOLE_COMMAND("upgrcmds", "?s", CFGFLAG_CHAT, ConChatUpgrCmds, this, "")
CONSOLE_COMMAND("rules", "?s", CFGFLAG_CHAT, ConChatRules, this, "")
CONSOLE_COMMAND("shop", "?s", CFGFLAG_CHAT, ConChatShop, this, "")
CONSOLE_COMMAND("coach", "", CFGFLAG_CHAT, ConChatCoach, this, "")
CONSOLE_COMMAND("writestats", "", CFGFLAG_CHAT, ConChatWriteStats, this, "")


// items
CONSOLE_COMMAND("walls", "", CFGFLAG_CHAT, ConChatWalls, this, "")
CONSOLE_COMMAND("hammerkill", "", CFGFLAG_CHAT, ConChatHammerkill, this, "")
CONSOLE_COMMAND("plasma", "", CFGFLAG_CHAT, ConChatPlasma, this, "")
CONSOLE_COMMAND("fgun", "", CFGFLAG_CHAT, ConChatGunfreeze, this, "")
CONSOLE_COMMAND("rplasma", "", CFGFLAG_CHAT, ConChatRifleplasma, this, "")
CONSOLE_COMMAND("swap", "", CFGFLAG_CHAT, ConChatSwap, this, "")
CONSOLE_COMMAND("fly", "", CFGFLAG_CHAT, ConChatFly, this, "")


#undef CONSOLE_COMMAND

#endif