/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */

#include <string.h>
#include <fstream>
#include <engine/config.h>
#include "account.h"
#include "base/rapidjson/document.h"
#include "base/rapidjson/reader.h"
#include "base/rapidjson/writer.h"
//#include "game/server/gamecontext.h"

#if defined(CONF_FAMILY_WINDOWS)
	#include <tchar.h>
	#include <direct.h>
#endif
#if defined(CONF_FAMILY_UNIX)
	#include <sys/types.h>
	#include <fcntl.h>
	#include <unistd.h>
#endif

using namespace rapidjson;

CAccount::CAccount(CPlayer *pPlayer, CGameContext *pGameServer)
{
   m_pPlayer = pPlayer;
   m_pGameServer = pGameServer;
}

/*
#ifndef GAME_VERSION_H
#define GAME_VERSION_H
#ifndef NON_HASED_VERSION
#include "generated/nethash.cpp"
#define GAME_VERSION "0.6.1"
#define GAME_NETVERSION "0.6 626fce9a778df4d4" //the std game version
#endif
#endif
*/

void CAccount::Login(char *Username, char *Password)
{
	char aBuf[128];
	if(m_pPlayer->m_AccData.m_UserID)
	{
		dbg_msg("account", "Account login failed ('%s' - Already logged in)", Username);
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Already logged in");
		return;
	}
	else if(strlen(Username) > 15 || !strlen(Username))
	{
		str_format(aBuf, sizeof(aBuf), "Username too %s", strlen(Username)?"long":"short");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), aBuf);
		return;
    }
	else if(strlen(Password) > 15 || !strlen(Password))
	{
		str_format(aBuf, sizeof(aBuf), "Password too %s!", strlen(Password)?"long":"short");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), aBuf);
		return;
    }
	else if(!Exists(Username))
	{
		dbg_msg("account", "Account login failed ('%s' - Missing)", Username);
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "This account does not exist.");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Please register first. (/register <user> <pass>)");
		return;
	}

	str_format(aBuf, sizeof(aBuf), "accounts/+%s.acc", Username);

	char AccUsername[32];
	char AccPassword[32];
	char AccRcon[32];
	int AccID;


 
	FILE *Accfile;
	Accfile = fopen(aBuf, "r");
	fscanf(Accfile, "%s\n%s\n%s\n%d", AccUsername, AccPassword, AccRcon, &AccID);
	fclose(Accfile);

	for(int i = 0; i < MAX_SERVER; i++)
	{
		for(int j = 0; j < MAX_CLIENTS; j++)
		{
			if(GameServer()->m_apPlayers[j] && GameServer()->m_apPlayers[j]->m_AccData.m_UserID == AccID)
			{
				dbg_msg("account", "Account login failed ('%s' - already in use (local))", Username);
				GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Account already in use");
				return;
			}

			if(!GameServer()->m_aaExtIDs[i][j])
				continue;

			if(AccID == GameServer()->m_aaExtIDs[i][j])
			{
				dbg_msg("account", "Account login failed ('%s' - already in use (extern))", Username);
				GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Account already in use");
				return;
			}
		}
	}

	if(strcmp(Username, AccUsername))
	{
		dbg_msg("account", "Account login failed ('%s' - Wrong username)", Username);
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Wrong username or password");
		return;
	}

	if(strcmp(Password, AccPassword))
	{
		dbg_msg("account", "Account login failed ('%s' - Wrong password)", Username);
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Wrong username or password");
		return;
	}


	Accfile = fopen(aBuf, "r"); 		//

	fscanf(Accfile, "%s\n%s\n%s\n%d\n\n\n%d\n%d\n%d\n%d\n%d\n\n%d\n%d\n%d\n\n%d\n%d\n%d\n%d\n%d\n%d\n\n%d\n%d\n%d\n\n%d\n%d\n%d\n\n%d\n%d\n%d\n\n%d\n%d\n%d\n\n%d\n%d\n%d\n\n%d\n%d\n%d\n%d\n%d", 
		m_pPlayer->m_AccData.m_Username, // Done
		m_pPlayer->m_AccData.m_Password, // Done
		m_pPlayer->m_AccData.m_RconPassword, 
		&m_pPlayer->m_AccData.m_UserID, // Done

		&m_pPlayer->m_AccData.m_HouseID,
 		&m_pPlayer->m_AccData.m_Money, // Done
		&m_pPlayer->m_AccData.m_Health, // Done
		&m_pPlayer->m_AccData.m_Armor, // Done
		&m_pPlayer->m_Score, // Done

		&m_pPlayer->m_AccData.m_Donor, 
		&m_pPlayer->m_AccData.m_VIP, // Done
		&m_pPlayer->m_AccData.m_Arrested, // Done

		&m_pPlayer->m_AccData.m_AllWeapons, // Done
		&m_pPlayer->m_AccData.m_HealthRegen, // Done
		&m_pPlayer->m_AccData.m_InfinityAmmo, // Done
		&m_pPlayer->m_AccData.m_InfinityJumps, // Done
		&m_pPlayer->m_AccData.m_FastReload, // Done
		&m_pPlayer->m_AccData.m_NoSelfDMG, // Done

		&m_pPlayer->m_AccData.m_GrenadeSpread, // Done 
		&m_pPlayer->m_AccData.m_GrenadeBounce, // Done
		&m_pPlayer->m_AccData.m_GrenadeMine,

		&m_pPlayer->m_AccData.m_ShotgunSpread, // Done
		&m_pPlayer->m_AccData.m_ShotgunExplode, // Done
		&m_pPlayer->m_AccData.m_ShotgunStars,

		&m_pPlayer->m_AccData.m_RifleSpread, // Done
		&m_pPlayer->m_AccData.m_RifleSwap, // Done
		&m_pPlayer->m_AccData.m_RiflePlasma, // Done

		&m_pPlayer->m_AccData.m_GunSpread, // Done
		&m_pPlayer->m_AccData.m_GunExplode, // Done
		&m_pPlayer->m_AccData.m_GunFreeze, // Done

		&m_pPlayer->m_AccData.m_HammerWalls, // Done
		&m_pPlayer->m_AccData.m_HammerShot, // Done
		&m_pPlayer->m_AccData.m_HammerKill, // Done

		&m_pPlayer->m_AccData.m_NinjaPermanent, // Done
		&m_pPlayer->m_AccData.m_NinjaStart, // Done
		&m_pPlayer->m_AccData.m_NinjaSwitch, // Done

		&m_pPlayer->m_AccData.m_Level,
		&m_pPlayer->m_AccData.m_ExpPoints); 

	fclose(Accfile);

	CCharacter *pOwner = GameServer()->GetPlayerChar(m_pPlayer->GetCID());

	if(pOwner)
	{
		if(pOwner->IsAlive())
			pOwner->Die(m_pPlayer->GetCID(), WEAPON_GAME);
	}
	 
	if(m_pPlayer->GetTeam() == TEAM_SPECTATORS)
		m_pPlayer->SetTeam(TEAM_RED);
  	
	dbg_msg("account", "Account login sucessful ('%s')", Username);
	GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Login succesful");
 
	if (m_pPlayer->m_AccData.m_GunFreeze > 3) // Remove on acc reset
		m_pPlayer->m_AccData.m_GunFreeze = 3;

	if(str_comp(m_pPlayer->m_AccData.m_RconPassword, g_Config.m_SvRconModPassword) == 0)
		GameServer()->Server()->SetRconlvl(m_pPlayer->GetCID(),1);

	else if(str_comp(m_pPlayer->m_AccData.m_RconPassword, g_Config.m_SvRconPassword) == 0)
		GameServer()->Server()->SetRconlvl(m_pPlayer->GetCID(),2);
	

}

void CAccount::LoginJ(char *Username, char *Password)
{
	char aBuf[128];
	if(m_pPlayer->m_AccData.m_UserID)
	{
		dbg_msg("account", "Account login failed ('%s' - Already logged in)", Username);
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Already logged in");
		return;
	}
	else if(strlen(Username) > 15 || !strlen(Username))
	{
		str_format(aBuf, sizeof(aBuf), "Username too %s", strlen(Username)?"long":"short");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), aBuf);
		return;
    }
	else if(strlen(Password) > 15 || !strlen(Password))
	{
		str_format(aBuf, sizeof(aBuf), "Password too %s!", strlen(Password)?"long":"short");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), aBuf);
		return;
    }
	else if(!Exists(Username))
	{
		dbg_msg("account", "Account login failed ('%s' - Missing)", Username);
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "This account does not exist.");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Please register first. (/register <user> <pass>)");
		return;
	}

	str_format(aBuf, sizeof(aBuf), "accounts/%s.acc", Username);
	FILE *Accfile;
	char AccText[5012];
	Accfile = fopen(aBuf, "r");
	dbg_msg("debug", "hello");

	Document AccD;
	dbg_msg("debug", "hello");
	ParseResult res = AccD.Parse();
	AccD.Parse(aBuf);
	
	if (res.IsError()) {
		dbg_msg("account", "parse error");
	}
	dbg_msg("debug", "hello");
	assert(AccD.IsObject());
	dbg_msg("debug", "hello");
	Value::ConstMemberIterator itr;
	dbg_msg("debug", "hello");
	if (AccD.HasMember("accdata")) {
		dbg_msg("debug", "has member");
		itr = AccD.FindMember("accdata");
	}
	else 
		return;

	dbg_msg("debug", "hello");

	for (itr; itr != AccD.MemberEnd(); itr++) {
		str_format(aBuf, sizeof aBuf, "%s has %s", itr->name.GetString(), itr->value.GetString());
		dbg_msg("debug", aBuf);
		

	}
	dbg_msg("debug", "hello");
	char AccUsername[32];
	char AccPassword[32];
	char AccRcon[32];
	int AccID;

	fscanf(Accfile, "%s\n%s\n%s\n%d", AccUsername, AccPassword, AccRcon, &AccID);
	fclose(Accfile);

	for(int i = 0; i < MAX_SERVER; i++)
	{
		for(int j = 0; j < MAX_CLIENTS; j++)
		{
			if(GameServer()->m_apPlayers[j] && GameServer()->m_apPlayers[j]->m_AccData.m_UserID == AccID)
			{
				dbg_msg("account", "Account login failed ('%s' - already in use (local))", Username);
				GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Account already in use");
				return;
			}

			if(!GameServer()->m_aaExtIDs[i][j])
				continue;

			if(AccID == GameServer()->m_aaExtIDs[i][j])
			{
				dbg_msg("account", "Account login failed ('%s' - already in use (extern))", Username);
				GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Account already in use");
				return;
			}
		}
	}

	if(strcmp(Username, AccUsername))
	{
		dbg_msg("account", "Account login failed ('%s' - Wrong username)", Username);
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Wrong username or password");
		return;
	}

	if(strcmp(Password, AccPassword))
	{
		dbg_msg("account", "Account login failed ('%s' - Wrong password)", Username);
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Wrong username or password");
		return;
	}


	Accfile = fopen(aBuf, "r"); 


	fclose(Accfile);

	CCharacter *pOwner = GameServer()->GetPlayerChar(m_pPlayer->GetCID());

	if(pOwner)
	{
		if(pOwner->IsAlive())
			pOwner->Die(m_pPlayer->GetCID(), WEAPON_GAME);
	}
	 
	if(m_pPlayer->GetTeam() == TEAM_SPECTATORS)
		m_pPlayer->SetTeam(TEAM_RED);
  	
	dbg_msg("account", "Account login sucessful ('%s')", Username);
	GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Login succesful");
 
	if (m_pPlayer->m_AccData.m_GunFreeze > 3) // Remove on acc reset
		m_pPlayer->m_AccData.m_GunFreeze = 3;

	if(str_comp(m_pPlayer->m_AccData.m_RconPassword, g_Config.m_SvRconModPassword) == 0)
		GameServer()->Server()->SetRconlvl(m_pPlayer->GetCID(),1);

	else if(str_comp(m_pPlayer->m_AccData.m_RconPassword, g_Config.m_SvRconPassword) == 0)
		GameServer()->Server()->SetRconlvl(m_pPlayer->GetCID(),2);

}

void CAccount::Register(char *Username, char *Password)
{
	char aBuf[125];
	if(m_pPlayer->m_AccData.m_UserID)
	{
		dbg_msg("account", "Account registration failed ('%s' - Logged in)", Username);
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Already logged in");
		return;
	}
	if(strlen(Username) > 15 || !strlen(Username))
	{
		str_format(aBuf, sizeof(aBuf), "Username too %s", strlen(Username)?"long":"short");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), aBuf);
		return;
    }
	else if(strlen(Password) > 15 || !strlen(Password))
	{
		str_format(aBuf, sizeof(aBuf), "Password too %s!", strlen(Password)?"long":"short");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), aBuf);
		return;
    }
	else if(Exists(Username))
	{
		dbg_msg("account", "Account registration failed ('%s' - Already exists)", Username);
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Account already exists.");
		return;
	}

	#if defined(CONF_FAMILY_UNIX)
	char Filter[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz.-_";
	// "äöü<>|!§$%&/()=?`´*'#+~«»¢“”æßðđŋħjĸł˝;,·^°@ł€¶ŧ←↓→øþ\\";
	char *p = strpbrk(Username, Filter);
	if(!p)
	{
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Don't use invalid chars for username!");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "A - Z, a - z, 0 - 9, . - _");
		return;
	}
	
	if(fs_makedir("accounts")) // that was some useless stuff
		dbg_msg("account", "Account folder created!");
	#endif

	#if defined(CONF_FAMILY_WINDOWS)
	static TCHAR * ValidChars = _T("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz.-_");
	if (_tcsspnp(Username, ValidChars))
	{
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Don't use invalid chars for username!");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "A - Z, a - z, 0 - 9, . - _");
		return;
	}

	if(mkdir("accounts"))
		dbg_msg("account", "Account folder created!");
	#endif

	str_format(aBuf, sizeof(aBuf), "accounts/+%s.acc", Username);

	FILE *Accfile;
	Accfile = fopen(aBuf, "a+");

	str_format(aBuf, sizeof(aBuf),
		"%s\n%s\n%s\n%d\n\n\n%d\n%d\n%d\n%d\n%d\n\n%d\n%d\n%d\n\n%d\n%d\n%d\n%d\n%d\n%d\n\n%d\n%d\n%d\n\n%d\n%d\n%d\n\n%d\n%d\n%d\n\n%d\n%d\n%d\n\n%d\n%d\n%d\n\n%d\n%d\n%d\n%d\n%d", 
		Username, 
		Password, 
		"0",
		NextID(),

		m_pPlayer->m_AccData.m_HouseID,
		m_pPlayer->m_AccData.m_Money,
		m_pPlayer->m_AccData.m_Health<10?10:m_pPlayer->m_AccData.m_Health,
		m_pPlayer->m_AccData.m_Armor<10?10:m_pPlayer->m_AccData.m_Armor,
		m_pPlayer->m_Score<0?0:m_pPlayer->m_Score, 

		m_pPlayer->m_AccData.m_Donor,
		m_pPlayer->m_AccData.m_VIP,
		m_pPlayer->m_AccData.m_Arrested,

		m_pPlayer->m_AccData.m_AllWeapons, 
		m_pPlayer->m_AccData.m_HealthRegen, 
		m_pPlayer->m_AccData.m_InfinityAmmo, 
		m_pPlayer->m_AccData.m_InfinityJumps, 
		m_pPlayer->m_AccData.m_FastReload, 
		m_pPlayer->m_AccData.m_NoSelfDMG, 

		m_pPlayer->m_AccData.m_GrenadeSpread, 
		m_pPlayer->m_AccData.m_GrenadeBounce, 
		m_pPlayer->m_AccData.m_GrenadeMine,

		m_pPlayer->m_AccData.m_ShotgunSpread,
		m_pPlayer->m_AccData.m_ShotgunExplode,
		m_pPlayer->m_AccData.m_ShotgunStars,

		m_pPlayer->m_AccData.m_RifleSpread, 
		m_pPlayer->m_AccData.m_RifleSwap, 
		m_pPlayer->m_AccData.m_RiflePlasma, 

		m_pPlayer->m_AccData.m_GunSpread, 
		m_pPlayer->m_AccData.m_GunExplode, 
		m_pPlayer->m_AccData.m_GunFreeze, 

		m_pPlayer->m_AccData.m_HammerWalls, 
		m_pPlayer->m_AccData.m_HammerShot,
		m_pPlayer->m_AccData.m_HammerKill, 

		m_pPlayer->m_AccData.m_NinjaPermanent,
		m_pPlayer->m_AccData.m_NinjaStart, 
		m_pPlayer->m_AccData.m_NinjaSwitch,

		m_pPlayer->m_AccData.m_Level,
		m_pPlayer->m_AccData.m_ExpPoints);

	fputs(aBuf, Accfile);
	fclose(Accfile);

	dbg_msg("account", "Registration succesful ('%s')", Username);
	str_format(aBuf, sizeof(aBuf), "Registration succesful - ('/login %s %s'): ", Username, Password);
	GameServer()->SendChatTarget(m_pPlayer->GetCID(), aBuf);
	Login(Username, Password);
}

void CAccount::RegisterJ(char *Username, char *Password)
{
	char aBuf[125];
	if(m_pPlayer->m_AccData.m_UserID)
	{
		dbg_msg("account", "Account registration failed ('%s' - Logged in)", Username);
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Already logged in");
		return;
	}
	if(strlen(Username) > 15 || !strlen(Username))
	{
		str_format(aBuf, sizeof(aBuf), "Username too %s", strlen(Username)?"long":"short");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), aBuf);
		return;
    }
	else if(strlen(Password) > 15 || !strlen(Password))
	{
		str_format(aBuf, sizeof(aBuf), "Password too %s!", strlen(Password)?"long":"short");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), aBuf);
		return;
    }

	#if defined(CONF_FAMILY_UNIX)
	char Filter[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz.-_";
	// "äöü<>|!§$%&/()=?`´*'#+~«»¢“”æßðđŋħjĸł˝;,·^°@ł€¶ŧ←↓→øþ\\";
	char *p = strpbrk(Username, Filter);
	if(!p)
	{
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Don't use invalid chars for username!");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "A - Z, a - z, 0 - 9, . - _");
		return;
	}
	
	if(fs_makedir("accounts")) // that was some useless stuff
		dbg_msg("account", "Account folder created!");
	#endif

	#if defined(CONF_FAMILY_WINDOWS)
	static TCHAR * ValidChars = _T("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz.-_");
	if (_tcsspnp(Username, ValidChars))
	{
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Don't use invalid chars for username!");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "A - Z, a - z, 0 - 9, . - _");
		return;
	}

	if(mkdir("accounts"))
		dbg_msg("account", "Account folder created!");
	#endif

	str_format(aBuf, sizeof(aBuf), "accounts/%s.acc", Username);

	Document doc;
	StringBuffer strBuf;
    Writer<StringBuffer> writer(strBuf);
	FILE *Accfile;

	writer.StartObject();

    writer.Key("User");
    writer.StartObject();

    writer.Key("accdata");
    writer.StartObject();
    writer.Key("accid");
    writer.Int(NextID());
	writer.Key("username");
	writer.String(Username);
	writer.Key("password");
	writer.String(Password);
	writer.EndObject();

	writer.Key("general");
	writer.StartObject();
	writer.Key("level");
    writer.Int(m_pPlayer->m_AccData.m_Level);
	writer.Key("exp");
    writer.Int64(m_pPlayer->m_AccData.m_ExpPoints);
    writer.Key("money");
    writer.Int64(m_pPlayer->m_AccData.m_Money);
	writer.Key("health");
    writer.Int(m_pPlayer->m_AccData.m_Health<10?10:m_pPlayer->m_AccData.m_Health);
	writer.Key("armor");
    writer.Int(m_pPlayer->m_AccData.m_Armor<10?10:m_pPlayer->m_AccData.m_Armor);
	writer.Key("houseid");
    writer.Int(m_pPlayer->m_AccData.m_HouseID);
    writer.EndObject();

	writer.Key("Ranks");
	writer.StartObject();
	writer.Key("donor");
	writer.Int(m_pPlayer->m_AccData.m_Donor);
	writer.Key("vip");
	writer.Int(m_pPlayer->m_AccData.m_VIP);
	writer.EndObject();

    writer.Key("items");
    writer.StartObject();

	writer.Key("basic");
	writer.StartObject();
	writer.Key("allweapons");
	writer.Int(m_pPlayer->m_AccData.m_AllWeapons);
	writer.Key("healthregen");
	writer.Int(m_pPlayer->m_AccData.m_HealthRegen);
	writer.Key("infinityammo");
	writer.Int(m_pPlayer->m_AccData.m_InfinityAmmo);
	writer.Key("infinityjumps");
	writer.Int(m_pPlayer->m_AccData.m_InfinityJumps);
	writer.Key("fastreload");
	writer.Int(m_pPlayer->m_AccData.m_FastReload);
	writer.Key("noselfdmg");
	writer.Int(m_pPlayer->m_AccData.m_NoSelfDMG);
	writer.EndObject();

    writer.Key("gun");
    writer.StartObject();
	writer.Key("gunspread");
    writer.Int(m_pPlayer->m_AccData.m_GunSpread);
	writer.Key("gunexplode");
    writer.Int(m_pPlayer->m_AccData.m_GunExplode);
    writer.Key("freezegun");
    writer.Int(m_pPlayer->m_AccData.m_GunFreeze);
    writer.EndObject();

	writer.Key("shotgun");
    writer.StartObject();
	writer.Key("shotgunspread");
    writer.Int(m_pPlayer->m_AccData.m_ShotgunSpread);
	writer.Key("shotgunexplode");
    writer.Int(m_pPlayer->m_AccData.m_ShotgunExplode);
    writer.Key("shotgunstars");
    writer.Int(m_pPlayer->m_AccData.m_ShotgunStars);
    writer.EndObject();

	writer.Key("grenade");
    writer.StartObject();
	writer.Key("grenadespread");
    writer.Int(m_pPlayer->m_AccData.m_GunSpread);
	writer.Key("grenadebounce");
    writer.Int(m_pPlayer->m_AccData.m_GrenadeBounce);
    writer.Key("grenademine");
    writer.Int(m_pPlayer->m_AccData.m_GrenadeMine);
    writer.EndObject();

	writer.Key("rifle");
    writer.StartObject();
	writer.Key("riflespread");
    writer.Int(m_pPlayer->m_AccData.m_RifleSpread);
	writer.Key("rifleswap");
    writer.Int(m_pPlayer->m_AccData.m_RifleSwap);
    writer.Key("rifleplasma");
    writer.Int(m_pPlayer->m_AccData.m_RiflePlasma);
    writer.EndObject();

	writer.Key("hammer");
    writer.StartObject();
	writer.Key("hammerwalls");
    writer.Int(m_pPlayer->m_AccData.m_HammerWalls);
	writer.Key("hammershot");
    writer.Int(m_pPlayer->m_AccData.m_HammerShot);
    writer.Key("hammerkill");
    writer.Int(m_pPlayer->m_AccData.m_HammerKill);
    writer.EndObject();

	writer.Key("ninja");
    writer.StartObject();
	writer.Key("ninjapermanent");
    writer.Int(m_pPlayer->m_AccData.m_NinjaPermanent);
	writer.Key("ninjastart");
    writer.Int(m_pPlayer->m_AccData.m_NinjaStart);
    writer.Key("ninjaswitch");
    writer.Int(m_pPlayer->m_AccData.m_NinjaSwitch);
    writer.EndObject();

    writer.EndObject();
    writer.EndObject();
    writer.EndObject();

	Accfile = fopen(aBuf, "a+");

	fputs(strBuf.GetString(), Accfile);
	fclose(Accfile);

	dbg_msg("account", "Registration succesful ('%s')", Username);
	str_format(aBuf, sizeof(aBuf), "Registration succesful - ('/login %s %s'): ", Username, Password);
	GameServer()->SendChatTarget(m_pPlayer->GetCID(), aBuf);
	LoginJ(Username, Password);
}

bool CAccount::Exists(const char *Username)
{
	char aBuf[128];
	str_format(aBuf, sizeof(aBuf), "accounts/+%s.acc", Username);
    if(FILE *Accfile = fopen(aBuf, "r"))
    {
        fclose(Accfile);
        return true;
    }
    return false;
}

void CAccount::Apply()
{
	char aBuf[512];
	str_format(aBuf, sizeof(aBuf), "accounts/+%s.acc", m_pPlayer->m_AccData.m_Username);
	std::remove(aBuf);
	FILE *Accfile;
	Accfile = fopen(aBuf,"a+");
	
	str_format(aBuf, sizeof(aBuf), "%s\n%s\n%s\n%d\n\n\n%d\n%d\n%d\n%d\n%d\n\n%d\n%d\n%d\n\n%d\n%d\n%d\n%d\n%d\n%d\n\n%d\n%d\n%d\n\n%d\n%d\n%d\n\n%d\n%d\n%d\n\n%d\n%d\n%d\n\n%d\n%d\n%d\n\n%d\n%d\n%d\n%d\n%d", 
		m_pPlayer->m_AccData.m_Username,
		m_pPlayer->m_AccData.m_Password, 
		m_pPlayer->m_AccData.m_RconPassword, 
		m_pPlayer->m_AccData.m_UserID,

		m_pPlayer->m_AccData.m_HouseID,
		m_pPlayer->m_AccData.m_Money,
		m_pPlayer->m_AccData.m_Health,
		m_pPlayer->m_AccData.m_Armor,
		m_pPlayer->m_Score, 

		m_pPlayer->m_AccData.m_Donor,
		m_pPlayer->m_AccData.m_VIP,
		m_pPlayer->m_AccData.m_Arrested,

		m_pPlayer->m_AccData.m_AllWeapons, 
		m_pPlayer->m_AccData.m_HealthRegen, 
		m_pPlayer->m_AccData.m_InfinityAmmo, 
		m_pPlayer->m_AccData.m_InfinityJumps, 
		m_pPlayer->m_AccData.m_FastReload, 
		m_pPlayer->m_AccData.m_NoSelfDMG, 

		m_pPlayer->m_AccData.m_GrenadeSpread, 
		m_pPlayer->m_AccData.m_GrenadeBounce, 
		m_pPlayer->m_AccData.m_GrenadeMine,

		m_pPlayer->m_AccData.m_ShotgunSpread,
		m_pPlayer->m_AccData.m_ShotgunExplode,
		m_pPlayer->m_AccData.m_ShotgunStars,

		m_pPlayer->m_AccData.m_RifleSpread, 
		m_pPlayer->m_AccData.m_RifleSwap, 
		m_pPlayer->m_AccData.m_RiflePlasma, 

		m_pPlayer->m_AccData.m_GunSpread, 
		m_pPlayer->m_AccData.m_GunExplode, 
		m_pPlayer->m_AccData.m_GunFreeze, 

		m_pPlayer->m_AccData.m_HammerWalls, 
		m_pPlayer->m_AccData.m_HammerShot,
		m_pPlayer->m_AccData.m_HammerKill, 

		m_pPlayer->m_AccData.m_NinjaPermanent,
		m_pPlayer->m_AccData.m_NinjaStart, 
		m_pPlayer->m_AccData.m_NinjaSwitch,

		m_pPlayer->m_AccData.m_Level,
		m_pPlayer->m_AccData.m_ExpPoints);

	fputs(aBuf, Accfile);
	fclose(Accfile);
}

void CAccount::Reset()
{
	str_copy(m_pPlayer->m_AccData.m_Username, "", 32);
	str_copy(m_pPlayer->m_AccData.m_Password, "", 32);
	str_copy(m_pPlayer->m_AccData.m_RconPassword, "", 32);
	m_pPlayer->m_AccData.m_UserID = 0;
	
	m_pPlayer->m_AccData.m_HouseID = 0;
	m_pPlayer->m_AccData.m_Money = 0;
	m_pPlayer->m_AccData.m_Health = 10;
	m_pPlayer->m_AccData.m_Armor = 10;
	m_pPlayer->m_Score = 0;

	m_pPlayer->m_AccData.m_Donor = 0;
	m_pPlayer->m_AccData.m_VIP = 0;
	m_pPlayer->m_AccData.m_Arrested = 0;

	m_pPlayer->m_AccData.m_AllWeapons = 0;
	m_pPlayer->m_AccData.m_HealthRegen = 0;
	m_pPlayer->m_AccData.m_InfinityAmmo = 0;
	m_pPlayer->m_AccData.m_InfinityJumps = 0;
	m_pPlayer->m_AccData.m_FastReload = 0;
	m_pPlayer->m_AccData.m_NoSelfDMG = 0;

	m_pPlayer->m_AccData.m_GrenadeSpread = 0;
	m_pPlayer->m_AccData.m_GrenadeBounce = 0;
	m_pPlayer->m_AccData.m_GrenadeMine = 0;

	m_pPlayer->m_AccData.m_ShotgunSpread = 0;
	m_pPlayer->m_AccData.m_ShotgunExplode = 0;
	m_pPlayer->m_AccData.m_ShotgunStars = 0;

	m_pPlayer->m_AccData.m_RifleSpread = 0;
	m_pPlayer->m_AccData.m_RifleSwap = 0;
	m_pPlayer->m_AccData.m_RiflePlasma = 0;

	m_pPlayer->m_AccData.m_GunSpread = 0;
	m_pPlayer->m_AccData.m_GunExplode = 0;
	m_pPlayer->m_AccData.m_GunFreeze = 0;

	m_pPlayer->m_AccData.m_HammerWalls = 0;
	m_pPlayer->m_AccData.m_HammerShot = 0;
	m_pPlayer->m_AccData.m_HammerKill = 0;

	m_pPlayer->m_AccData.m_NinjaPermanent = 0;
	m_pPlayer->m_AccData.m_NinjaStart = 0;
	m_pPlayer->m_AccData.m_NinjaSwitch = 0;

	m_pPlayer->m_AccData.m_Level = 1;
	m_pPlayer->m_AccData.m_ExpPoints = 0;

}

void CAccount::Delete()
{
	char aBuf[128];
	if(m_pPlayer->m_AccData.m_UserID)
	{
		Reset();
		str_format(aBuf, sizeof(aBuf), "accounts/+%s.acc", m_pPlayer->m_AccData.m_Username);
		std::remove(aBuf);
		dbg_msg("account", "Account deleted ('%s')", m_pPlayer->m_AccData.m_Username);
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Account deleted!");
	}
	else
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Please, login to delete your account");
}

void CAccount::NewPassword(char *NewPassword)
{
	char aBuf[128];
	if(!m_pPlayer->m_AccData.m_UserID)
	{
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Please, login to change the password");
		return;
	}
	if(strlen(NewPassword) > 15 || !strlen(NewPassword))
	{
		str_format(aBuf, sizeof(aBuf), "Password too %s!", strlen(NewPassword)?"long":"short");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), aBuf);
		return;
    }

	str_copy(m_pPlayer->m_AccData.m_Password, NewPassword, 32);
	Apply();

	
	dbg_msg("account", "Password changed - ('%s')", m_pPlayer->m_AccData.m_Username);
	GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Password successfully changed!");
}

void CAccount::NewUsername(char *NewUsername)
{
	char aBuf[128];
	if(!m_pPlayer->m_AccData.m_UserID)
	{
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Please, login to change the username");
		return;
	}
	if(strlen(NewUsername) > 15 || !strlen(NewUsername))
	{
		str_format(aBuf, sizeof(aBuf), "Username too %s!", strlen(NewUsername)?"long":"short");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), aBuf);
		return;
    }

	str_format(aBuf, sizeof(aBuf), "accounts/+%s.acc", m_pPlayer->m_AccData.m_Username);
	std::rename(aBuf, NewUsername);

	str_copy(m_pPlayer->m_AccData.m_Username, NewUsername, 32);
	Apply();

	
	dbg_msg("account", "Username changed - ('%s')", m_pPlayer->m_AccData.m_Username);
	GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Username successfully changed!");
}

int CAccount::NextID()
{
	FILE *Accfile;
	int UserID = 1;
	char aBuf[32];
	char AccUserID[32];

	str_copy(AccUserID, "accounts/++UserIDs++.acc", sizeof(AccUserID));

	if(Exists("+UserIDs++"))
	{
		Accfile = fopen(AccUserID, "r");
		fscanf(Accfile, "%d", &UserID);
		fclose(Accfile);

		std::remove(AccUserID);

		Accfile = fopen(AccUserID, "a+");
		str_format(aBuf, sizeof(aBuf), "%d", UserID+1);
		fputs(aBuf, Accfile);
		fclose(Accfile);

		return UserID+1;
	}
	else
	{
		Accfile = fopen(AccUserID, "a+");
		str_format(aBuf, sizeof(aBuf), "%d", UserID);
		fputs(aBuf, Accfile);
		fclose(Accfile);
	}

	return 1;
}
