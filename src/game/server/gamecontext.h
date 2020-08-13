/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SERVER_GAMECONTEXT_H
#define GAME_SERVER_GAMECONTEXT_H

#include <engine/server.h>
#include <engine/console.h>
#include <engine/shared/memheap.h>

#include <game/layers.h>
#include <game/voting.h>

#include "eventhandler.h"
#include "gamecontroller.h"
#include "gameworld.h"
#include "player.h"
#include "game/server/city/filesys.h"
#include "game/server/city/gameevent.h"
#include "game/server/city/moneycollector.h"
#include "game/server/city/items/portal.h"

/*
	Tick
		Game Context (CGameContext::tick)
			Game World (GAMEWORLD::tick)
				Reset world if requested (GAMEWORLD::reset)
				All entities in the world (ENTITY::tick)
				All entities in the world (ENTITY::tick_defered)
				Remove entities marked for deletion (GAMEWORLD::remove_entities)
			Game Controller (GAMECONTROLLER::tick)
			All players (CPlayer::tick)


	Snap
		Game Context (CGameContext::snap)
			Game World (GAMEWORLD::snap)
				All entities in the world (ENTITY::snap)
			Game Controller (GAMECONTROLLER::snap)
			Events handler (EVENT_HANDLER::snap)
			All players (CPlayer::snap)

*/
class CGameContext : public IGameServer
{
	IServer *m_pServer;
	class IConsole *m_pConsole;
	class CFileSys *m_pFilesys;
	class CMoneyCollector *m_pMoneyCollector;
	class CGameEvent *m_pGameEvent;
	CLayers m_Layers;
	CCollision m_Collision;
	CNetObjHandler m_NetObjHandler;
	CTuningParams m_Tuning;

	static void ConTuneParam(IConsole::IResult *pResult, void *pUserData);
	static void ConTuneReset(IConsole::IResult *pResult, void *pUserData);
	static void ConTuneDump(IConsole::IResult *pResult, void *pUserData);
	static void ConChangeMap(IConsole::IResult *pResult, void *pUserData);
	static void ConRestart(IConsole::IResult *pResult, void *pUserData);
	static void ConBroadcast(IConsole::IResult *pResult, void *pUserData);
	static void ConSay(IConsole::IResult *pResult, void *pUserData);
	static void ConSetTeam(IConsole::IResult *pResult, void *pUserData);
	static void ConSetTeamAll(IConsole::IResult *pResult, void *pUserData);
	static void ConAddVote(IConsole::IResult *pResult, void *pUserData);
	static void ConRemoveVote(IConsole::IResult *pResult, void *pUserData);
	static void ConForceVote(IConsole::IResult *pResult, void *pUserData);
	static void ConClearVotes(IConsole::IResult *pResult, void *pUserData);
	static void ConVote(IConsole::IResult *pResult, void *pUserData);
	static void ConchainSpecialMotdupdate(IConsole::IResult *pResult, void *pUserData, IConsole::FCommandCallback pfnCallback, void *pCallbackUserData);

	CGameContext(int Resetting);
	void Construct(int Resetting);

	bool m_Resetting;
public:
	IServer *Server() const { return m_pServer; }
	class IConsole *Console() { return m_pConsole; }
	CFileSys *Filesystem() { return m_pFilesys; }
	CGameEvent *GameEvent() { return m_pGameEvent; }
	CMoneyCollector *MoneyCollector() { return m_pMoneyCollector; }
	CCollision *Collision() { return &m_Collision; }
	CTuningParams *Tuning() { return &m_Tuning; }

	CGameContext();
	~CGameContext();

	void Clear();

	CEventHandler m_Events;
	CPlayer *m_apPlayers[MAX_CLIENTS];

	IGameController *m_pController;
	CGameWorld m_World;

	// helper functions
	class CCharacter *GetPlayerChar(int ClientID);

	// voting
	void StartVote(const char *pDesc, const char *pCommand, const char *pReason);
	void EndVote();
	void SendVoteSet(int ClientID);
	void SendVoteStatus(int ClientID, int Total, int Yes, int No);
	void AbortVoteKickOnDisconnect(int ClientID);

	int m_VoteCreator;
	int64 m_VoteCloseTime;
	bool m_VoteUpdate;
	int m_VotePos;
	char m_aVoteDescription[VOTE_DESC_LENGTH];
	char m_aVoteCommand[VOTE_CMD_LENGTH];
	char m_aVoteReason[VOTE_REASON_LENGTH];
	int m_NumVoteOptions;
	int m_VoteEnforce;

	enum
	{
		VOTE_ENFORCE_UNKNOWN=0,
		VOTE_ENFORCE_NO,
		VOTE_ENFORCE_YES,
	};
	CHeap *m_pVoteOptionHeap;
	CVoteOptionServer *m_pVoteOptionFirst;
	CVoteOptionServer *m_pVoteOptionLast;

	// helper functions
	void CreateDamageInd(vec2 Pos, float AngleMod, int Amount);
	void CreateExplosion(vec2 Pos, int Owner, int Weapon, bool NoDamage);
	void CreateHammerHit(vec2 Pos);
	void CreatePlayerSpawn(vec2 Pos);
	void CreateDeath(vec2 Pos, int Who);
	void CreateSound(vec2 Pos, int Sound, int Mask=-1);
	void CreateSoundGlobal(int Sound, int Target=-1);


	enum
	{
		CHAT_ALL=-2,
		CHAT_SPEC=-1,
		CHAT_RED=0,
		CHAT_BLUE=1
	};

	// network
	void SendChatTarget(int To, const char *pText);
	void SendPrivate(int ChatterClientID, int To, const char *pText, int SpamProtectionClientID = -1);
	void SendChat(int ClientID, int Team, const char *pText, int SpamProtectionClientID = -1);
	void SendEmoticon(int ClientID, int Emoticon);
	void SendWeaponPickup(int ClientID, int Weapon);
	void SendBroadcast(const char *pText, int ClientID);


	//
	void CheckPureTuning();
	void SendTuningParams(int ClientID);

	// engine events
	virtual void OnInit();
	virtual void OnConsoleInit();
	virtual void OnShutdown();

	virtual void OnTick();
	virtual void OnPreSnap();
	virtual void OnSnap(int ClientID);
	virtual void OnPostSnap();

	virtual void OnMessage(int MsgID, CUnpacker *pUnpacker, int ClientID);
	virtual void ProcessPrivateMsg(const char* Msg, int ClientID);

	virtual void OnClientConnected(int ClientID);
	virtual void OnClientEnter(int ClientID);
	virtual void OnClientDrop(int ClientID, const char *pReason);
	virtual void OnClientDirectInput(int ClientID, void *pInput);
	virtual void OnClientPredictedInput(int ClientID, void *pInput);

	virtual bool IsClientReady(int ClientID);
	virtual bool IsClientPlayer(int ClientID);

	virtual const char *GameType();
	virtual const char *Version();
	virtual const char *NetVersion();

	private: //KlickFoots Rconcmdsachen^^
		static void ConTeleport(IConsole::IResult *pResult, void *pUserData);
		static void ConAuth(IConsole::IResult *pResult, void *pUserData);
		static void ConVip(IConsole::IResult *pResult, void *pUserData);
		static void ConDonor(IConsole::IResult *pResult, void *pUserData);
		static void ConJail(IConsole::IResult *pResult, void *pUserData);
		static void ConGiveMoney(IConsole::IResult *pResult, void *pUserData);
		static void ConSetMoney(IConsole::IResult* pResult, void* pUserData);
		static void ConUnjail(IConsole::IResult *pResult, void *pUserData);
		static void ConSetLvl(IConsole::IResult *pResult, void *pUserData);
		static void ConSetLvlWeapon(IConsole::IResult *pResult, void *pUserData);
		static void ConSetLife(IConsole::IResult* pResult, void* pUserData);
		static void ConSetArmor(IConsole::IResult* pResult, void* pUserData);
		static void ConSetClientName(IConsole::IResult* pResult, void* pUserData);
		static void ConSetClientGravityY(IConsole::IResult* pResult, void* pUserData);
		static void ConSetClientGravityX(IConsole::IResult* pResult, void* pUserData);
		static void ConKill(IConsole::IResult* pResult, void* pUserData);
		static void ConFreeze(IConsole::IResult* pResult, void* pUserData);
		static void ConUnFreeze(IConsole::IResult* pResult, void* pUserData);
		static void ConSameIP(IConsole::IResult* pResult, void* pUserData);
		static void ConLookUp(IConsole::IResult* pResult, void* pUserData);
		static void ConSendAfk(IConsole::IResult* pResult, void* pUserData);
		static void ConStartEvent(IConsole::IResult* pResult, void* pUserData);
		static void ConAbortEvent(IConsole::IResult* pResult, void* pUserData);
		static void ConEventTimer(IConsole::IResult* pResult, void* pUserData);
		static void ConSetBounty(IConsole::IResult* pResult, void* pUserData);
		static void ConMute(IConsole::IResult *pResult, void *pUserData);
		static void ConMuteIP(IConsole::IResult *pResult, void *pUserData);
		static void ConUnmute(IConsole::IResult *pResult, void *pUserData);
		static void ConMutes(IConsole::IResult *pResult, void *pUserData);
		static void ConSendFakeParams(IConsole::IResult *pResult, void *pUserData);

		static void ConFsBackupAccounts(IConsole::IResult* pResult, void* pUserData);

		// chat
		static void ConChatLogin(IConsole::IResult* pResult, void* pUserData);
		static void ConChatRegister(IConsole::IResult* pResult, void* pUserData);
		static void ConChatLogout(IConsole::IResult* pResult, void* pUserData);
		static void ConChatChangePw(IConsole::IResult* pResult, void* pUserData);

		static void ConChatSave(IConsole::IResult* pResult, void* pUserData);
		static void ConChatLoad(IConsole::IResult* pResult, void* pUserData);
		static void ConChatTele(IConsole::IResult* pResult, void* pUserData);
		static void ConChatHome(IConsole::IResult* pResult, void* pUserData);
		static void ConChatRainbow(IConsole::IResult* pResult, void* pUserData);
		static void ConChatCrown(IConsole::IResult* pResult, void* pUserData);
		static void ConChatUp(IConsole::IResult* pResult, void* pUserData);
		static void ConChatDown(IConsole::IResult* pResult, void* pUserData);
		static void ConChatLeft(IConsole::IResult* pResult, void* pUserData);
		static void ConChatRight(IConsole::IResult* pResult, void* pUserData);
		static void ConChatJailrifle(IConsole::IResult* pResult, void* pUserData);
		static void ConChatGod(IConsole::IResult* pResult, void* pUserData);
		static void ConChatInstagib(IConsole::IResult* pResult, void* pUserData);
		static void ConChatTransfer(IConsole::IResult* pResult, void* pUserData);
		static void ConChatDisabledmg(IConsole::IResult* pResult, void* pUserData);
		static void ConChatEnabledmg(IConsole::IResult* pResult, void* pUserData);
		static void ConChatPM(IConsole::IResult* pResult, void* pUserData);
		static void ConChatSetPM(IConsole::IResult* pResult, void* pUserData);
		static void ConChatTrain(IConsole::IResult* pResult, void* pUserData);
		static void ConChatBountylist(IConsole::IResult* pResult, void* pUserData);
		static void ConChatCheckbounty(IConsole::IResult* pResult, void* pUserData);
		static void ConChatSetbounty(IConsole::IResult* pResult, void* pUserData);

		static void ConChatMCBuy(IConsole::IResult* pResult, void* pUserData);
		static void ConChatMCCollect(IConsole::IResult* pResult, void* pUserData);
		static void ConChatMCHelp(IConsole::IResult* pResult, void* pUserData);
		static void ConChatMCCmds(IConsole::IResult* pResult, void* pUserData);


		static void ConChatMe(IConsole::IResult* pResult, void* pUserData);
		static void ConChatCmdlist(IConsole::IResult* pResult, void* pUserData);
		static void ConChatHelp(IConsole::IResult* pResult, void* pUserData);
		static void ConChatDonor(IConsole::IResult* pResult, void* pUserData);
		static void ConChatDonorCmds(IConsole::IResult* pResult, void* pUserData);
		static void ConChatVip(IConsole::IResult* pResult, void* pUserData);
		static void ConChatUpgrCmds(IConsole::IResult* pResult, void* pUserData);
		static void ConChatRules(IConsole::IResult* pResult, void* pUserData);
		static void ConChatShop(IConsole::IResult* pResult, void* pUserData);
		static void ConChatCoach(IConsole::IResult* pResult, void* pUserData);
		static void ConChatWriteStats(IConsole::IResult* pResult, void* pUserData);
		static void ConChatIDs(IConsole::IResult* pResult, void* pUserData);
		static void ConChatEvent(IConsole::IResult* pResult, void* pUserData);


		static void ConChatWalls(IConsole::IResult* pResult, void* pUserData);
		static void ConChatHammerkill(IConsole::IResult* pResult, void* pUserData);
		static void ConChatPortal(IConsole::IResult* pResult, void* pUserData);
		static void ConChatPlasma(IConsole::IResult* pResult, void* pUserData);
		static void ConChatGunfreeze(IConsole::IResult* pResult, void* pUserData);
		static void ConChatRifleplasma(IConsole::IResult* pResult, void* pUserData);
		static void ConChatSwap(IConsole::IResult* pResult, void* pUserData);
		static void ConChatFly(IConsole::IResult* pResult, void* pUserData);
		static void ConChatHealHook(IConsole::IResult* pResult, void* pUserData);
		static void ConChatBoostHook(IConsole::IResult* pResult, void* pUserData);
		static void ConChatPushAura(IConsole::IResult* pResult, void* pUserData);
		static void ConChatPullAura(IConsole::IResult* pResult, void* pUserData);

	public: //Ende :D

	// City
	struct CMute
	{
		NETADDR m_Addr;
		int m_Expire;
		char m_aReason[128];
	};

	void RefreshIDs();
	void SendMotd(int ClientID, const char *pText);
	void DisableDmg(int Owner, int target);
	void EnableDmg(int Owner, int target);
	void FormatInt(long long n, char* out);
	void Mute(const NETADDR *pAddr, int Secs, const char *pDisplayName, const char *pReason);
	int ProcessSpamProtection(int ClientID);
	bool HasDmgDisabled(int Owner, int target);
	bool TryMute(const NETADDR *pAddr, int Secs, const char *pReason);

	void AddToBountyList(int ID);
	void RemoveFromBountyList(int ID);
	const int* BountyList() { return m_BountyList; }
	const int BountyList(int i) { return m_BountyList[i]; }

	bool ValidID(int ID) { return ID >= 0 && ID < MAX_CLIENTS; }
	int GetWIDByStr(const char *weapon);
	void GetStrByWID(int ID, char *out, int size) ;

	void strrev(char* str);
	int string_length(char* pointer);

	NETADDR addr;
	NETSOCKET Socket;
	int m_aaExtIDs[2][MAX_CLIENTS];
	int m_TeleNR[MAX_CLIENTS];
	int m_TeleNum;
	int m_NoDmgIDs[MAX_CLIENTS][MAX_CLIENTS];
	CMute m_aMutes[MAX_CLIENTS];
	int m_NumMutes;

	CPortal* m_aPortals[MAX_CLIENTS];
	
	//int m_TeleID[MAX_CLIENTS];
private:
	int m_BountyList[MAX_CLIENTS];

	void ResetDisabledDmg(int ID);
};

inline int CmaskAll() { return -1; }
inline int CmaskOne(int ClientID) { return 1<<ClientID; }
inline int CmaskAllExceptOne(int ClientID) { return 0x7fffffff^CmaskOne(ClientID); }
inline bool CmaskIsSet(int Mask, int ClientID) { return (Mask&CmaskOne(ClientID)) != 0; }
#endif
