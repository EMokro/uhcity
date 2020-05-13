/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */

#include <string.h>
#include <fstream>
#include <engine/config.h>
#include "account.h"
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
	char aBuf[125];
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


	Accfile = fopen(aBuf, "r"); 		

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
 

	if(str_comp(m_pPlayer->m_AccData.m_RconPassword,g_Config.m_SvRconModPassword) == 0)
		GameServer()->Server()->SetRconlvl(m_pPlayer->GetCID(),1);

	else if(str_comp(m_pPlayer->m_AccData.m_RconPassword,g_Config.m_SvRconPassword) == 0)
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
	
	if(mkdir("accounts", mode_t S_IRWXU || S_IRWXG | S_IROTH | S_IXOTH))
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

	str_format(aBuf, sizeof(aBuf), "%s\n%s\n%s\n%d\n\n\n%d\n%d\n%d\n%d\n%d\n\n%d\n%d\n%d\n\n%d\n%d\n%d\n%d\n%d\n%d\n\n%d\n%d\n%d\n\n%d\n%d\n%d\n\n%d\n%d\n%d\n\n%d\n%d\n%d\n\n%d\n%d\n%d\n\n%d\n%d\n%d\n%d\n%d", 
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


