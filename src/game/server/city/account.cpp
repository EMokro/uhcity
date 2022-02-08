/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */

#include <string.h>
#include <fstream>
#include <engine/config.h>
#include "account.h"
#include "filesys.h"
#include "base/rapidjson/document.h"
#include "base/rapidjson/reader.h"
#include "base/rapidjson/writer.h"
#include "base/rapidjson/filereadstream.h"
#include "base/rapidjson/filewritestream.h"
#include "base/rapidjson/error/en.h"
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
		GameServer()->SendChatTarget_Localization(m_pPlayer->GetCID(), CHATCATEGORY_INFO, _("Already logged in"));
		return;
	}
	else if(strlen(Username) > 15 || !strlen(Username))
	{
		GameServer()->SendChatTarget_Localization(m_pPlayer->GetCID(), CHATCATEGORY_INFO, _("Username too {str:ls}"), strlen(Username)?"long":"short");
		return;
    }
	else if(strlen(Password) > 15 || !strlen(Password))
	{
		GameServer()->SendChatTarget_Localization(m_pPlayer->GetCID(), CHATCATEGORY_INFO, _("Password too {str:ls}"), strlen(Password)?"long":"short");
		return;
    }
	else if(!Exists(Username))
	{
		if (!OldLogin(Username, Password)) {
			dbg_msg("account", "Account login failed ('%s' - Missing)", Username);
			GameServer()->SendChatTarget_Localization(m_pPlayer->GetCID(), CHATCATEGORY_INFO, _("This account does not exist."));
			GameServer()->SendChatTarget_Localization(m_pPlayer->GetCID(), CHATCATEGORY_INFO, _("Please register first. (/register <user> <pass>)"));
		}

		return;
	}

	char AccText[4096];
	str_format(aBuf, sizeof(aBuf), "accounts/%s.acc", Username);
	
	FILE *Accfile;
	Accfile = fopen(aBuf, "r");
	fscanf(Accfile, "%s\n", AccText);
	fclose(Accfile);

	Document AccD;
	ParseResult res = AccD.Parse(AccText);
	AccD.Parse(aBuf);

	if (res.IsError()) {
		dbg_msg("account", "parse error");
		GameServer()->SendChatTarget_Localization(m_pPlayer->GetCID(), CHATCATEGORY_INFO, _("Sorry, your account data can't be read, We were deleted your account"));
		Delete();
		return;
	}

	tl_assert(AccD.IsObject());
	Value::ConstMemberIterator itr;

	if (AccD["user"].HasMember("accdata")) {
		itr = AccD["user"].FindMember("accdata");
	}
	else 
		return;

	Value& user = AccD["user"];

	str_format(aBuf, sizeof(aBuf), "is %s", user["accdata"]["username"].GetString());
	
	for(int i = 0; i < MAX_SERVER; i++)
	{
		for(int j = 0; j < MAX_CLIENTS; j++)
		{
			if(GameServer()->m_apPlayers[j] && GameServer()->m_apPlayers[j]->m_AccData.m_UserID == user["accdata"]["accid"].GetInt())
			{
				dbg_msg("account", "Account login failed ('%s' - already in use (local))", Username);
				GameServer()->SendChatTarget_Localization(m_pPlayer->GetCID(), CHATCATEGORY_INFO, _("Account already in use"));
				return;
			}

			if(!GameServer()->m_aaExtIDs[i][j])
				continue;

			if(user["accdata"]["accid"].GetInt() == GameServer()->m_aaExtIDs[i][j])
			{
				dbg_msg("account", "Account login failed ('%s' - already in use (extern))", Username);
				GameServer()->SendChatTarget_Localization(m_pPlayer->GetCID(), CHATCATEGORY_INFO, _("Account already in use"));
				return;
			}
		}
	}

	if(strcmp(Username, user["accdata"]["username"].GetString()))
	{
		dbg_msg("account", "Account login failed ('%s' - Wrong username)", Username);
		GameServer()->SendChatTarget_Localization(m_pPlayer->GetCID(), CHATCATEGORY_INFO, _("Wrong username or password"));
		return;
	}

	if(strcmp(Password, user["accdata"]["password"].GetString()))
	{
		dbg_msg("account", "Account login failed ('%s' - Wrong password)", Username);
		GameServer()->SendChatTarget_Localization(m_pPlayer->GetCID(), CHATCATEGORY_INFO, _("Wrong username or password"));
		return;
	}

	m_pPlayer->m_AccData.m_UserID = user["accdata"]["accid"].GetInt();
	str_copy(m_pPlayer->m_AccData.m_Username, user["accdata"]["username"].GetString(), sizeof(m_pPlayer->m_AccData.m_Username));
	str_copy(m_pPlayer->m_AccData.m_Password, user["accdata"]["password"].GetString(), sizeof(m_pPlayer->m_AccData.m_Password));

	if (user["general"].HasMember("level"))
		m_pPlayer->m_AccData.m_Level = user["general"]["level"].GetInt();
	if (user["general"].HasMember("exp"))
		m_pPlayer->m_AccData.m_ExpPoints = user["general"]["exp"].GetInt64();
	m_pPlayer->m_AccData.m_Money = user["general"]["money"].GetInt64();
	m_pPlayer->m_AccData.m_Health = user["general"]["health"].GetInt();
	m_pPlayer->m_AccData.m_Armor = user["general"]["armor"].GetInt();
	m_pPlayer->m_AccData.m_HouseID = user["general"]["houseid"].GetInt();

	if (user.HasMember("level")) {
		if (user["level"].HasMember("normal"))
			m_pPlayer->m_AccData.m_Level = user["level"]["normal"].GetInt();
		if (user["level"].HasMember("hammer"))
			m_pPlayer->m_AccData.m_LvlWeapon[WEAPON_HAMMER] = user["level"]["hammer"].GetInt() > 500 ? 500 : m_pPlayer->m_AccData.m_LvlWeapon[WEAPON_HAMMER] = user["level"]["hammer"].GetInt();
		if (user["level"].HasMember("gun"))
			m_pPlayer->m_AccData.m_LvlWeapon[WEAPON_GUN] = user["level"]["gun"].GetInt() > 500 ? 500 : m_pPlayer->m_AccData.m_LvlWeapon[WEAPON_GUN] = user["level"]["gun"].GetInt();
		if (user["level"].HasMember("shotgun"))
			m_pPlayer->m_AccData.m_LvlWeapon[WEAPON_SHOTGUN] = user["level"]["shotgun"].GetInt() > 500 ? 500 : m_pPlayer->m_AccData.m_LvlWeapon[WEAPON_SHOTGUN] = user["level"]["shotgun"].GetInt();
		if (user["level"].HasMember("grenade"))
			m_pPlayer->m_AccData.m_LvlWeapon[WEAPON_GRENADE] = user["level"]["grenade"].GetInt() > 500 ? 500 : m_pPlayer->m_AccData.m_LvlWeapon[WEAPON_GRENADE] = user["level"]["grenade"].GetInt();
		if (user["level"].HasMember("rifle"))
			m_pPlayer->m_AccData.m_LvlWeapon[WEAPON_RIFLE] = user["level"]["rifle"].GetInt() > 500 ? 500 : m_pPlayer->m_AccData.m_LvlWeapon[WEAPON_RIFLE] = user["level"]["rifle"].GetInt();
	}

	if (user.HasMember("exp")) {
		if (user["exp"].HasMember("normal"))
			m_pPlayer->m_AccData.m_ExpPoints = user["exp"]["normal"].GetInt64();
		if (user["exp"].HasMember("hammer"))
			m_pPlayer->m_AccData.m_ExpWeapon[WEAPON_HAMMER] = user["exp"]["hammer"].GetInt();
		if (user["exp"].HasMember("gun"))
			m_pPlayer->m_AccData.m_ExpWeapon[WEAPON_GUN] = user["exp"]["gun"].GetInt();
		if (user["exp"].HasMember("shotgun"))
			m_pPlayer->m_AccData.m_ExpWeapon[WEAPON_SHOTGUN] = user["exp"]["shotgun"].GetInt();
		if (user["exp"].HasMember("grenade"))
			m_pPlayer->m_AccData.m_ExpWeapon[WEAPON_GRENADE] = user["exp"]["grenade"].GetInt();
		if (user["exp"].HasMember("rifle"))
			m_pPlayer->m_AccData.m_ExpWeapon[WEAPON_RIFLE] = user["exp"]["rifle"].GetInt();
	}

	

	if (user.HasMember("auth")) {
		if (user["auth"].HasMember("authlvl"))
			if(user["auth"]["authlvl"].GetInt()) GameServer()->Server()->SetRconlvl(m_pPlayer->GetCID(), user["auth"]["authlvl"].GetInt());
		if (user["auth"].HasMember("vip"))
			m_pPlayer->m_AccData.m_VIP = user["auth"]["vip"].GetInt();
	}
	if (user.HasMember("donor"))
		m_pPlayer->m_AccData.m_Donor = user["donor"].GetInt();

	if (user.HasMember("language"))
		m_pPlayer->SetLanguage(user["language"].GetString());
	
	if (user.HasMember("event")) {
		if (user["event"].HasMember("bounty"))
			m_pPlayer->m_AccData.m_Bounty = user["event"]["bounty"].GetInt64();
	}

	m_pPlayer->m_AccData.m_AllWeapons = user["items"]["basic"]["allweapons"].GetInt();
	m_pPlayer->m_AccData.m_HealthRegen = user["items"]["basic"]["healthregen"].GetInt();
	m_pPlayer->m_AccData.m_InfinityAmmo = user["items"]["basic"]["infinityammo"].GetInt();
	m_pPlayer->m_AccData.m_InfinityJumps = user["items"]["basic"]["infinityjumps"].GetInt();
	m_pPlayer->m_AccData.m_FastReload = user["items"]["basic"]["fastreload"].GetInt() >= 1 ? 1 : 0;
	m_pPlayer->m_AccData.m_NoSelfDMG = user["items"]["basic"]["noselfdmg"].GetInt();

	m_pPlayer->m_AccData.m_GunSpread = user["items"]["gun"]["gunspread"].GetInt();
	m_pPlayer->m_AccData.m_GunExplode = user["items"]["gun"]["gunexplode"].GetInt();
	m_pPlayer->m_AccData.m_GunFreeze = user["items"]["gun"]["freezegun"].GetInt();

	m_pPlayer->m_AccData.m_ShotgunSpread = user["items"]["shotgun"]["shotgunspread"].GetInt();
	m_pPlayer->m_AccData.m_ShotgunExplode = user["items"]["shotgun"]["shotgunexplode"].GetInt();
	m_pPlayer->m_AccData.m_ShotgunStars = user["items"]["shotgun"]["shotgunstars"].GetInt();

	m_pPlayer->m_AccData.m_GrenadeSpread = user["items"]["grenade"]["grenadespread"].GetInt();
	m_pPlayer->m_AccData.m_GrenadeBounce = user["items"]["grenade"]["grenadebounce"].GetInt();
	m_pPlayer->m_AccData.m_GrenadeMine = user["items"]["grenade"]["grenademine"].GetInt();

	m_pPlayer->m_AccData.m_RifleSpread = user["items"]["rifle"]["riflespread"].GetInt();
	m_pPlayer->m_AccData.m_RifleSwap = user["items"]["rifle"]["rifleswap"].GetInt();
	m_pPlayer->m_AccData.m_RiflePlasma = user["items"]["rifle"]["rifleplasma"].GetInt();

	m_pPlayer->m_AccData.m_HammerWalls = user["items"]["hammer"]["hammerwalls"].GetInt();
	m_pPlayer->m_AccData.m_HammerShot = user["items"]["hammer"]["hammershot"].GetInt();
	m_pPlayer->m_AccData.m_HammerKill = user["items"]["hammer"]["hammerkill"].GetInt();
	if (user["items"]["hammer"].HasMember("portal"))
		m_pPlayer->m_AccData.m_Portal = user["items"]["hammer"]["portal"].GetInt();

	m_pPlayer->m_AccData.m_NinjaPermanent = user["items"]["ninja"]["ninjapermanent"].GetInt();
	m_pPlayer->m_AccData.m_NinjaStart = user["items"]["ninja"]["ninjastart"].GetInt();
	m_pPlayer->m_AccData.m_NinjaSwitch = user["items"]["ninja"]["ninjaswitch"].GetInt();
	if (user["items"]["ninja"].HasMember("fly"))
		m_pPlayer->m_AccData.m_NinjaFly = user["items"]["ninja"]["fly"].GetInt();
	if (user["items"]["ninja"].HasMember("bomber"))
		m_pPlayer->m_AccData.m_NinjaBomber = user["items"]["ninja"]["bomber"].GetInt();

	if (user["items"].HasMember("hook")) {
		if (user["items"]["hook"].HasMember("endless"))
			m_pPlayer->m_AccData.m_EndlessHook = user["items"]["hook"]["endless"].GetInt();
		if ((user["items"]["hook"].HasMember("heal")))
			m_pPlayer->m_AccData.m_HealHook = user["items"]["hook"]["heal"].GetInt();
		if ((user["items"]["hook"].HasMember("boost")))
			m_pPlayer->m_AccData.m_BoostHook = user["items"]["hook"]["boost"].GetInt();
	}

	if (user["items"].HasMember("special")) {
		if (user["items"]["special"].HasMember("pushaura"))
			m_pPlayer->m_AccData.m_PushAura = user["items"]["special"]["pushaura"].GetInt();
		if (user["items"]["special"].HasMember("pullaura"))
			m_pPlayer->m_AccData.m_PullAura = user["items"]["special"]["pullaura"].GetInt();
	}

	CCharacter *pOwner = GameServer()->GetPlayerChar(m_pPlayer->GetCID());
	m_pPlayer->m_Score = m_pPlayer->m_AccData.m_Level;

	if(pOwner)
	{
		if(pOwner->IsAlive())
			pOwner->Die(m_pPlayer->GetCID(), WEAPON_GAME);
	}
	 
	if(m_pPlayer->GetTeam() == TEAM_SPECTATORS)
		m_pPlayer->SetTeam(TEAM_RED);

	if (m_pPlayer->m_AccData.m_Bounty) {
		char numBuf[32];
		GameServer()->FormatInt(m_pPlayer->m_AccData.m_Bounty, numBuf);
		GameServer()->SendChatTarget_Localization(-1, CHATCATEGORY_JOIN, _("'{str:ClientName}' joined with a bounty of {str:Bounty}$"), "ClientName", GameServer()->Server()->ClientName(m_pPlayer->GetCID()), "Bounty", numBuf);
		GameServer()->AddToBountyList(m_pPlayer->GetCID());
	}
  	
	dbg_msg("account", "Account login sucessful ('%s')", Username);
	GameServer()->SendChatTarget_Localization(m_pPlayer->GetCID(), CHATCATEGORY_INFO, _("Login succesful"));
 
	if (m_pPlayer->m_AccData.m_GunFreeze > 3) // Remove on acc reset
		m_pPlayer->m_AccData.m_GunFreeze = 3;
}

void CAccount::Register(char *Username, char *Password)
{
	char aBuf[125];
	if(m_pPlayer->m_AccData.m_UserID)
	{
		dbg_msg("account", "Account registration failed ('%s' - Logged in)", Username);
		GameServer()->SendChatTarget_Localization(m_pPlayer->GetCID(), CHATCATEGORY_INFO, _("Already logged in"));
		return;
	}
	if(strlen(Username) > 15 || !strlen(Username))
	{
		GameServer()->SendChatTarget_Localization(m_pPlayer->GetCID(), CHATCATEGORY_INFO, _("Username too {str:ls}"), strlen(Password)?"long":"short");
		return;
    }
	else if(strlen(Password) > 15 || !strlen(Password))
	{
		GameServer()->SendChatTarget_Localization(m_pPlayer->GetCID(), CHATCATEGORY_INFO, _("Password too {str:ls}"), strlen(Password)?"long":"short");
		return;
    }

	#if defined(CONF_FAMILY_UNIX)
	char Filter[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz.-_";
	// "äöü<>|!§$%&/()=?`´*'#+~«»¢“”æßðđŋħjĸł˝;,·^°@ł€¶ŧ←↓→øþ\\";
	char *p = strpbrk(Username, Filter);
	if(!p)
	{
		GameServer()->SendChatTarget_Localization(m_pPlayer->GetCID(), CHATCATEGORY_INFO, _("Don't use invalid chars for username!"));
		GameServer()->SendChatTarget_Localization(m_pPlayer->GetCID(), CHATCATEGORY_INFO, _("A - Z, a - z, 0 - 9, . - _"));
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

    writer.Key("user");
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
    writer.Key("money");
    writer.Int64(m_pPlayer->m_AccData.m_Money);
	writer.Key("health");
    writer.Int(m_pPlayer->m_AccData.m_Health<10?10:m_pPlayer->m_AccData.m_Health);
	writer.Key("armor");
    writer.Int(m_pPlayer->m_AccData.m_Armor<10?10:m_pPlayer->m_AccData.m_Armor);
	writer.Key("houseid");
    writer.Int(m_pPlayer->m_AccData.m_HouseID);
    writer.EndObject();

	writer.Key("level");
	writer.StartObject();
	writer.Key("normal");
    writer.Int64(m_pPlayer->m_AccData.m_Level);
	writer.Key("hammer");
    writer.Int(m_pPlayer->m_AccData.m_LvlWeapon[WEAPON_HAMMER]);
    writer.Key("gun");
    writer.Int(m_pPlayer->m_AccData.m_LvlWeapon[WEAPON_GUN]);
	writer.Key("shotung");
    writer.Int(m_pPlayer->m_AccData.m_LvlWeapon[WEAPON_SHOTGUN]);
	writer.Key("grenade");
    writer.Int(m_pPlayer->m_AccData.m_LvlWeapon[WEAPON_GRENADE]);
	writer.Key("rifle");
    writer.Int(m_pPlayer->m_AccData.m_LvlWeapon[WEAPON_RIFLE]);
    writer.EndObject();

	writer.Key("exp");
	writer.StartObject();
	writer.Key("normal");
	writer.Int64(m_pPlayer->m_AccData.m_ExpPoints);
	writer.Key("hammer");
    writer.Int(m_pPlayer->m_AccData.m_ExpWeapon[WEAPON_HAMMER]);
    writer.Key("gun");
    writer.Int(m_pPlayer->m_AccData.m_ExpWeapon[WEAPON_GUN]);
	writer.Key("shotung");
    writer.Int(m_pPlayer->m_AccData.m_ExpWeapon[WEAPON_SHOTGUN]);
	writer.Key("grenade");
    writer.Int(m_pPlayer->m_AccData.m_ExpWeapon[WEAPON_GRENADE]);
	writer.Key("rifle");
    writer.Int(m_pPlayer->m_AccData.m_ExpWeapon[WEAPON_RIFLE]);
    writer.EndObject();

	writer.Key("auth");
	writer.StartObject();
	writer.Key("authlvl");
	writer.Int(0);
	writer.Key("vip");
	writer.Int(m_pPlayer->m_AccData.m_VIP);
	writer.EndObject();

	writer.Key("donor");
	writer.Int(m_pPlayer->m_AccData.m_Donor);

	writer.Key("language");
	writer.String(m_pPlayer->GetLanguage());

	writer.Key("event");
	writer.StartObject();
	writer.Key("bounty");
	writer.Int64(m_pPlayer->m_AccData.m_Bounty);
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
	writer.Key("portal");
	writer.Int(m_pPlayer->m_AccData.m_Portal);
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
	writer.Key("fly");
    writer.Int(m_pPlayer->m_AccData.m_NinjaFly);
	writer.Key("bomber");
    writer.Int(m_pPlayer->m_AccData.m_NinjaBomber);
    writer.EndObject();

	writer.Key("hook");
	writer.StartObject();
	writer.Key("endless");
    writer.Int(m_pPlayer->m_AccData.m_EndlessHook);
	writer.Key("heal");
    writer.Int(m_pPlayer->m_AccData.m_HealHook);
	writer.Key("boost");
    writer.Int(m_pPlayer->m_AccData.m_BoostHook);
	writer.EndObject();

    writer.EndObject();
    writer.EndObject();
    writer.EndObject();

	Accfile = fopen(aBuf, "a+");
	fputs(strBuf.GetString(), Accfile);
	fclose(Accfile);

	dbg_msg("account", "Registration succesful ('%s')", Username);
	str_format(aBuf, sizeof(aBuf), "Registration succesful - ('/login %s %s'): ", Username, Password);
	GameServer()->SendChatTarget_Localization(m_pPlayer->GetCID(), CHATCATEGORY_INFO, _("Registration succesful - ('/login {str:Username} {str:Password}')"), "Username", Username, "Password", Password, NULL);
	Login(Username, Password);
}

bool CAccount::Exists(const char *Username)
{
	char aBuf[128];
	str_format(aBuf, sizeof(aBuf), "accounts/%s.acc", Username);

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
	str_format(aBuf, sizeof(aBuf), "accounts/%s.acc", m_pPlayer->m_AccData.m_Username);
	std::remove(aBuf);
	StringBuffer strBuf;
    Writer<StringBuffer> writer(strBuf);
	FILE *Accfile;
	
	writer.StartObject();

    writer.Key("user");
    writer.StartObject();

    writer.Key("accdata");
    writer.StartObject();
    writer.Key("accid");
    writer.Int(m_pPlayer->m_AccData.m_UserID);
	writer.Key("username");
	writer.String(m_pPlayer->m_AccData.m_Username);
	writer.Key("password");
	writer.String(m_pPlayer->m_AccData.m_Password);
	writer.EndObject();

	writer.Key("general");
	writer.StartObject();
    writer.Key("money");
    writer.Int64(m_pPlayer->m_AccData.m_Money);
	writer.Key("health");
    writer.Int(m_pPlayer->m_AccData.m_Health<10?10:m_pPlayer->m_AccData.m_Health);
	writer.Key("armor");
    writer.Int(m_pPlayer->m_AccData.m_Armor<10?10:m_pPlayer->m_AccData.m_Armor);
	writer.Key("houseid");
    writer.Int(m_pPlayer->m_AccData.m_HouseID);
    writer.EndObject();

	writer.Key("level");
	writer.StartObject();
	writer.Key("normal");
    writer.Int(m_pPlayer->m_AccData.m_Level);
	writer.Key("hammer");
    writer.Int(m_pPlayer->m_AccData.m_LvlWeapon[WEAPON_HAMMER]);
    writer.Key("gun");
    writer.Int(m_pPlayer->m_AccData.m_LvlWeapon[WEAPON_GUN]);
	writer.Key("shotgun");
    writer.Int(m_pPlayer->m_AccData.m_LvlWeapon[WEAPON_SHOTGUN]);
	writer.Key("grenade");
    writer.Int(m_pPlayer->m_AccData.m_LvlWeapon[WEAPON_GRENADE]);
	writer.Key("rifle");
    writer.Int(m_pPlayer->m_AccData.m_LvlWeapon[WEAPON_RIFLE]);
    writer.EndObject();

	writer.Key("exp");
	writer.StartObject();
	writer.Key("normal");
	writer.Int64(m_pPlayer->m_AccData.m_ExpPoints);
	writer.Key("hammer");
    writer.Int(m_pPlayer->m_AccData.m_ExpWeapon[WEAPON_HAMMER]);
    writer.Key("gun");
    writer.Int(m_pPlayer->m_AccData.m_ExpWeapon[WEAPON_GUN]);
	writer.Key("shotung");
    writer.Int(m_pPlayer->m_AccData.m_ExpWeapon[WEAPON_SHOTGUN]);
	writer.Key("grenade");
    writer.Int(m_pPlayer->m_AccData.m_ExpWeapon[WEAPON_GRENADE]);
	writer.Key("rifle");
    writer.Int(m_pPlayer->m_AccData.m_ExpWeapon[WEAPON_RIFLE]);
    writer.EndObject();

	writer.Key("auth");
	writer.StartObject();
	writer.Key("authlvl");
	writer.Int(GameServer()->Server()->AuthLvl(m_pPlayer->GetCID()));
	writer.Key("vip");
	writer.Int(m_pPlayer->m_AccData.m_VIP);
	writer.EndObject();

	writer.Key("donor");
	writer.Int(m_pPlayer->m_AccData.m_Donor);

	writer.Key("language");
	writer.String(m_pPlayer->GetLanguage());

	writer.Key("event");
	writer.StartObject();
	writer.Key("bounty");
	writer.Int64(m_pPlayer->m_AccData.m_Bounty);
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
	writer.Key("portal");
    writer.Int(m_pPlayer->m_AccData.m_Portal);
    writer.EndObject();

	writer.Key("ninja");
    writer.StartObject();
	writer.Key("ninjapermanent");
    writer.Int(m_pPlayer->m_AccData.m_NinjaPermanent);
	writer.Key("ninjastart");
    writer.Int(m_pPlayer->m_AccData.m_NinjaStart);
    writer.Key("ninjaswitch");
    writer.Int(m_pPlayer->m_AccData.m_NinjaSwitch);
	writer.Key("fly");
    writer.Int(m_pPlayer->m_AccData.m_NinjaFly);
	writer.Key("bomber");
    writer.Int(m_pPlayer->m_AccData.m_NinjaBomber);
    writer.EndObject();

	writer.Key("hook");
	writer.StartObject();
	writer.Key("endless");
    writer.Int(m_pPlayer->m_AccData.m_EndlessHook);
	writer.Key("heal");
    writer.Int(m_pPlayer->m_AccData.m_HealHook);
	writer.Key("boost");
    writer.Int(m_pPlayer->m_AccData.m_BoostHook);
	writer.EndObject();

	writer.Key("special");
	writer.StartObject();
	writer.Key("pushaura");
    writer.Int(m_pPlayer->m_AccData.m_PushAura);
	writer.Key("pullaura");
    writer.Int(m_pPlayer->m_AccData.m_PullAura);
	writer.EndObject();

    writer.EndObject();
    writer.EndObject();
    writer.EndObject();

	Accfile = fopen(aBuf,"a+");
	fputs(strBuf.GetString(), Accfile);
	fclose(Accfile);
}

void CAccount::Reset()
{
	if (m_pPlayer->m_AccData.m_Bounty)
		GameServer()->RemoveFromBountyList(m_pPlayer->GetCID());

	str_copy(m_pPlayer->m_AccData.m_Username, "", 32);
	str_copy(m_pPlayer->m_AccData.m_Password, "", 32);
	m_pPlayer->m_AccData.m_UserID = 0;
	
	m_pPlayer->m_AccData.m_HouseID = 0;
	m_pPlayer->m_AccData.m_Money = 0;
	m_pPlayer->m_AccData.m_Health = 10;
	m_pPlayer->m_AccData.m_Armor = 10;
	m_pPlayer->SetLanguage("en");

	m_pPlayer->m_AccData.m_Donor = 0;
	m_pPlayer->m_AccData.m_VIP = 0;

	m_pPlayer->m_AccData.m_Level = 1;
	m_pPlayer->m_AccData.m_ExpPoints = 0;

	for (int i = 0; i < 5; i++) {
		m_pPlayer->m_AccData.m_LvlWeapon[i] = 0;
		m_pPlayer->m_AccData.m_ExpWeapon[i] = 0;
	}
		
	m_pPlayer->m_AccData.m_Bounty = 0;

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
	m_pPlayer->m_AccData.m_NinjaFly = 0;
	m_pPlayer->m_AccData.m_NinjaBomber = 0;

	m_pPlayer->m_AccData.m_HealHook = 0;
	m_pPlayer->m_AccData.m_BoostHook = 0;
	m_pPlayer->m_AccData.m_EndlessHook = 0;

	m_pPlayer->m_AccData.m_Portal = 0;
	m_pPlayer->m_AccData.m_PushAura = 0;
	m_pPlayer->m_AccData.m_PullAura = 0;
	

	if (GameServer()->m_aPortals[m_pPlayer->GetCID()])
		GameServer()->m_aPortals[m_pPlayer->GetCID()]->Reset();

	GameServer()->Server()->Logout(m_pPlayer->GetCID());
}

void CAccount::Delete()
{
	char aBuf[128];
	if(m_pPlayer->m_AccData.m_UserID)
	{
		Reset();
		str_format(aBuf, sizeof(aBuf), "accounts/%s.acc", m_pPlayer->m_AccData.m_Username);
		std::remove(aBuf);
		dbg_msg("account", "Account deleted ('%s')", m_pPlayer->m_AccData.m_Username);
		GameServer()->SendChatTarget_Localization(m_pPlayer->GetCID(), CHATCATEGORY_INFO, _("Account deleted!"));
	}
	else
		GameServer()->SendChatTarget_Localization(m_pPlayer->GetCID(), CHATCATEGORY_INFO, _("Please, login to delete your account"));
}

void CAccount::NewPassword(char *NewPassword)
{
	char aBuf[128];
	if(!m_pPlayer->m_AccData.m_UserID)
	{
		GameServer()->SendChatTarget_Localization(m_pPlayer->GetCID(), CHATCATEGORY_INFO, _("Please, login to change the password"));
		return;
	}

	if(strlen(NewPassword) > 15 || !strlen(NewPassword))
	{
		str_format(aBuf, sizeof(aBuf), "Password too %s!", strlen(NewPassword)?"long":"short");
		GameServer()->SendChatTarget_Localization(m_pPlayer->GetCID(), CHATCATEGORY_DEFAULT, _("Password too {str:ls}!"), "ls", strlen(NewPassword)?"long":"short", NULL);
		return;
    }

	str_copy(m_pPlayer->m_AccData.m_Password, NewPassword, 32);
	Apply();

	
	dbg_msg("account", "Password changed - ('%s')", m_pPlayer->m_AccData.m_Username);
	GameServer()->SendChatTarget_Localization(m_pPlayer->GetCID(), CHATCATEGORY_INFO, _("Password successfully changed!"));
}

int CAccount::NextID()
{
	FILE *Accfile;
	int UserID = 1;
	char aBuf[32];
	char AccUserID[32];

	str_copy(AccUserID, "accounts/++UserIDs++.acc", sizeof(AccUserID));

	if(Exists("++UserIDs++"))
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

bool CAccount::OldLogin(char *Username, char *Password)
{
	char aBuf[128];

	str_format(aBuf, sizeof aBuf, "+%s", Username);

	if(!Exists(aBuf))
	{
		return false;
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
				return true;
			}

			if(!GameServer()->m_aaExtIDs[i][j])
				continue;

			if(AccID == GameServer()->m_aaExtIDs[i][j])
			{
				dbg_msg("account", "Account login failed ('%s' - already in use (extern))", Username);
				GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Account already in use");
				return true;
			}
		}
	}

	if(strcmp(Username, AccUsername))
	{
		dbg_msg("account", "Account login failed ('%s' - Wrong username)", Username);
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Wrong username or password");
		return true;
	}

	if(strcmp(Password, AccPassword))
	{
		dbg_msg("account", "Account login failed ('%s' - Wrong password)", Username);
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Wrong username or password");
		return true;
	}

	Accfile = fopen(aBuf, "r"); 
	
	char tmpBuf[128];

	fscanf(Accfile, "%s\n%s\n%s\n%d\n\n\n%d\n%d\n%d\n%d\n%d\n\n%d\n%d\n%d\n\n%d\n%d\n%d\n%d\n%d\n%d\n\n%d\n%d\n%d\n\n%d\n%d\n%d\n\n%d\n%d\n%d\n\n%d\n%d\n%d\n\n%d\n%d\n%d\n\n%d\n%d\n%d\n%d\n%d", 
		m_pPlayer->m_AccData.m_Username, // Done
		m_pPlayer->m_AccData.m_Password, // Done
		tmpBuf,
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

	m_pPlayer->m_AccData.m_Bounty = 0;

	CCharacter *pOwner = GameServer()->GetPlayerChar(m_pPlayer->GetCID());

	if(pOwner)
	{
		if(pOwner->IsAlive())
			pOwner->Die(m_pPlayer->GetCID(), WEAPON_GAME);
	}
	 
	if(m_pPlayer->GetTeam() == TEAM_SPECTATORS)
		m_pPlayer->SetTeam(TEAM_RED);
  	
	dbg_msg("account", "Account login sucessful ('%s')", Username);
	GameServer()->SendChatTarget_Localization(m_pPlayer->GetCID(), CHATCATEGORY_INFO, _("Login succesful"));
 
	if (m_pPlayer->m_AccData.m_GunFreeze > 3) // Remove on acc reset
		m_pPlayer->m_AccData.m_GunFreeze = 3;

	Apply();
	std::remove(aBuf);
	
	return true;
}

void CAccount::SetAuth(char *Username, int lvl) {
	char aBuf[512];
	char AccText[65536];
	FILE *Accfile;

	if (GameServer()->Server()->AuthLvl(m_pPlayer->GetCID()) == lvl)
		return;

	if (!Exists(Username))
		return;

	str_format(aBuf, sizeof(aBuf), "accounts/%s.acc", Username);

	Accfile = fopen(aBuf, "r");
	fscanf(Accfile, "%s\n", AccText);
	fclose(Accfile);
	
	Document AccD;
	StringBuffer strBuf;
	Writer<StringBuffer> writer(strBuf);
	ParseResult res = AccD.Parse(AccText);

	if (res.IsError()) {
		GameServer()->SendChatTarget_Localization(m_pPlayer->GetCID(), CHATCATEGORY_INFO, _(" Parse Error: Please contact UrinStone to get this fixed."));
		dbg_msg("account", "parse error");
		return;
	}

	tl_assert(AccD.IsObject());

	AccD["user"]["auth"]["authlvl"].SetInt(lvl);

	AccD.Accept(writer);
	std::remove(aBuf);
	Accfile = fopen(aBuf, "a+");
	fscanf(Accfile, "%s\n", AccText);
	fclose(Accfile);
}
