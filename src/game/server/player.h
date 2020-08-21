/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SERVER_PLAYER_H
#define GAME_SERVER_PLAYER_H

// this include should perhaps be removed
#include "entities/character.h"
#include "gamecontext.h"
#include "city/account.h"

// player object
class CPlayer
{
	MACRO_ALLOC_POOL_ID()

public:
	CPlayer(CGameContext *pGameServer, int ClientID, int Team);
	~CPlayer();

	void Init(int CID);

	void TryRespawn();
	void Respawn();
	void SetTeam(int Team);
	int GetTeam() const { return m_Team; };
	int GetCID() const { return m_ClientID; };

	void Tick();
	void PostTick();
	void Snap(int SnappingClient);

	void OnDirectInput(CNetObj_PlayerInput *NewInput);
	void OnPredictedInput(CNetObj_PlayerInput *NewInput);
	void OnDisconnect(const char *pReason);

	void SendAfk();

	void KillCharacter(int Weapon = WEAPON_GAME);
	CCharacter *GetCharacter();

	//---------------------------------------------------------
	// this is used for snapping so we know how we can clip the view for the player
	vec2 m_ViewPos;

	// states if the client is chatting, accessing a menu etc.
	int m_PlayerFlags;

	// used for snapping to just update latency if the scoreboard is active
	int m_aActLatency[MAX_CLIENTS];

	// used for spectator mode
	int m_SpectatorID;

	bool m_IsReady;

	//
	int m_Vote;
	int m_VotePos;
	//
	int m_LastVoteCall;
	int m_LastVoteTry;
	int m_LastChat;
	int m_LastSetTeam;
	int m_LastSetSpectatorMode;
	int m_LastChangeInfo;
	int m_LastEmote;
	int m_LastKill;

	// TODO: clean this up
	struct
	{
		char m_SkinName[64];
		int m_UseCustomColor;
		int m_ColorBody;
		int m_ColorFeet;
	} m_TeeInfos;

	int m_RespawnTick;
	int m_DieTick;
	int m_Score;
	int m_ScoreStartTick;
	bool m_ForceBalanced;
	int m_LastActionTick;
	int m_TeamChangeTick;
	struct
	{
		int m_TargetX;
		int m_TargetY;
		int m_Direction;
		int m_Jump;
		int m_Fire;
		int m_Hook;
	} m_LatestActivity;

	// network latency calculations
	struct
	{
		int m_Accum;
		int m_AccumMin;
		int m_AccumMax;
		int m_Avg;
		int m_Min;
		int m_Max;
	} m_Latency;

	// City
	struct
	{
		// Main
		int m_UserID;
		char m_Username[32];
		char m_Password[32];
		char m_RconPassword[32];

		// basic
		unsigned long long m_Money;
		unsigned long long m_exp;
		int m_Health;
		int m_Armor;	
		int m_Kills;
		int m_HouseID;

		// levels
		unsigned int m_Level;
		unsigned long long int m_ExpPoints;
		unsigned int m_LvlWeapon[5];
		unsigned int m_ExpWeapon[5];

		// Rank
		int m_Donor;
		int m_VIP;

		// Event
		long long m_Bounty;

		// Punishments
		int m_Arrested;

		// Player
		int m_AllWeapons;
		int m_HealthRegen;
		int m_InfinityAmmo;
		int m_InfinityJumps;
		int m_FastReload;
		int m_NoSelfDMG;
		int m_Portal;

		// Weapons
		int m_GrenadeSpread;
		int m_GrenadeBounce;
		int m_GrenadeMine;

		int m_ShotgunSpread;
		int m_ShotgunExplode;
		int m_ShotgunStars;

		int m_RifleSpread;
		int m_RifleSwap; 
		int m_RiflePlasma;

		int m_GunSpread;
		int m_GunExplode;
		int m_GunFreeze;

		int m_HammerWalls;
		int m_HammerShot;
		int m_HammerKill;

		int m_NinjaPermanent;
		int m_NinjaStart;
		int m_NinjaSwitch;
		int m_NinjaFly;
		int m_NinjaBomber;

		int m_EndlessHook;
		int m_HealHook;
		int m_BoostHook;

		int m_PushAura;
		int m_PullAura;
	} m_AccData;

	bool m_Rainbow;
	bool m_God;
	bool m_Insta;
	bool m_Afk;
	
	int m_RainbowColor;
	int m_GravAuraCooldown;
	int m_AciveUpgrade[ITEM_AMOUNT];
	int m_Authed;
	int m_ChatScore;
	int m_PmID;
	char m_aRank[64];

	// items
	bool m_NinjaFly;
	bool m_NinjaBomber;

	//Klickfoots scheiß
	int m_ReleaseCarousel;
	int m_Crown;

	class CAccount *m_pAccount;//(CPlayer *m_Player, CGameContext *gameserver);

	void FakeSnap(int SnappingClient);

private:
	CCharacter *m_pCharacter;
	CGameContext *m_pGameServer;

	CGameContext *GameServer() const { return m_pGameServer; }
	IServer *Server() const;

	//
	bool m_Spawning;
	int m_ClientID;
	int m_Team;
};

#endif
