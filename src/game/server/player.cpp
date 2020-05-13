/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <new>
#include <engine/shared/config.h>
#include "player.h"
#include "city/cmds.h"


MACRO_ALLOC_POOL_ID_IMPL(CPlayer, MAX_CLIENTS)

IServer *CPlayer::Server() const { return m_pGameServer->Server(); }

CPlayer::CPlayer(CGameContext *pGameServer, int ClientID, int Team)
{
	m_pGameServer = pGameServer;
	m_RespawnTick = Server()->Tick();
	m_DieTick = Server()->Tick();
	m_ScoreStartTick = Server()->Tick();
	m_pCharacter = 0;
	m_ClientID = ClientID;
	m_Team = GameServer()->m_pController->ClampTeam(Team);
	m_SpectatorID = SPEC_FREEVIEW;
	m_LastActionTick = Server()->Tick();
	m_TeamChangeTick = Server()->Tick();

	// City
	m_Rainbow = false;
	m_Insta = false;
	m_pAccount = new CAccount(this, m_pGameServer);
	m_pChatCmd = new CCmd(this, m_pGameServer);

	if(m_AccData.m_Health < 10)
		m_AccData.m_Health = 10;

	if(m_AccData.m_UserID)
		m_pAccount->Apply();
}

CPlayer::~CPlayer()
{
	// City
	delete m_pChatCmd;
	m_pChatCmd = 0;

	delete m_pCharacter;
	m_pCharacter = 0;
}

void CPlayer::Tick()
{
#ifdef CONF_DEBUG
	if(!g_Config.m_DbgDummies || m_ClientID < MAX_CLIENTS-g_Config.m_DbgDummies)
#endif
	if(!Server()->ClientIngame(m_ClientID))
		return;

	Server()->SetClientScore(m_ClientID, m_Score);
	Server()->SetClientAccID(m_ClientID, m_AccData.m_UserID);


	if(Server()->Tick()%50 == 0)
	{
	if(m_ReleaseCarousel)
		m_ReleaseCarousel -= 1;
	}


	// do latency stuff
	{
		IServer::CClientInfo Info;
		if(Server()->GetClientInfo(m_ClientID, &Info))
		{
			m_Latency.m_Accum += Info.m_Latency;
			m_Latency.m_AccumMax = max(m_Latency.m_AccumMax, Info.m_Latency);
			m_Latency.m_AccumMin = min(m_Latency.m_AccumMin, Info.m_Latency);
		}
		// each second
		if(Server()->Tick()%Server()->TickSpeed() == 0)
		{
			m_Latency.m_Avg = m_Latency.m_Accum/Server()->TickSpeed();
			m_Latency.m_Max = m_Latency.m_AccumMax;
			m_Latency.m_Min = m_Latency.m_AccumMin;
			m_Latency.m_Accum = 0;
			m_Latency.m_AccumMin = 1000;
			m_Latency.m_AccumMax = 0;
		}
	}

	if(!m_pCharacter && m_Team == TEAM_SPECTATORS && m_SpectatorID == SPEC_FREEVIEW)
		m_ViewPos -= vec2(clamp(m_ViewPos.x-m_LatestActivity.m_TargetX, -500.0f, 500.0f), clamp(m_ViewPos.y-m_LatestActivity.m_TargetY, -400.0f, 400.0f));

	if(!m_pCharacter && m_DieTick+Server()->TickSpeed()*3 <= Server()->Tick())
		m_Spawning = true;

	if(m_pCharacter)
	{
		if(m_pCharacter->IsAlive())
		{
			m_ViewPos = m_pCharacter->m_Pos;
		}
		else
		{
			delete m_pCharacter;
			m_pCharacter = 0;
		}
	}
	else if(m_Spawning && m_RespawnTick <= Server()->Tick())
		TryRespawn();

	if(m_AccData.m_Arrested)
	{
		if(m_AccData.m_Arrested > 1)
		{
			char aBuf[40];
			str_format(aBuf, sizeof(aBuf), "You are arrested for %i secound%s", m_AccData.m_Arrested-2, m_AccData.m_Arrested==3?" ":"s");
			GameServer()->SendBroadcast(aBuf, GetCID());
			if(m_Insta)
				m_Insta = false;
			if(m_AccData.m_Arrested && Server()->Tick()%50 == 0)
				m_AccData.m_Arrested--;
		}
		else if(m_AccData.m_Arrested == 1)
		{
			char zBuf[50];
			CCharacter *pTarget = GameServer()->GetPlayerChar(GetCID());
			m_AccData.m_Arrested = 0;
			if(pTarget && pTarget->IsAlive())
				pTarget->Die(pTarget->GetPlayer()->GetCID(), WEAPON_GAME);
			str_format(zBuf, sizeof(zBuf), "'%s' no longer in jail", Server()->ClientName(GetCID()));
			GameServer()->SendChat(-1, CGameContext::CHAT_ALL, zBuf);
		}
	}

	// City
	static int RainbowColor = 0;
	RainbowColor = (RainbowColor + 1) % 256;
	m_RainbowColor = RainbowColor * 0x010000 + 0xff00;

	if(g_Config.m_SvTournamentMode && !m_AccData.m_UserID && m_Team != TEAM_SPECTATORS)
		SetTeam(TEAM_SPECTATORS);

	
	str_copy(m_aRank, "", sizeof(m_aRank));
	
	if(GameServer()->Server()->IsAdmin(GetCID()))
	{
		//Server()->SetClientClan(GetCID(), "[*Admin*]");
		//str_format(m_aRank, sizeof(m_aRank), "%s", );
		str_copy(m_aRank, "[*Admin*]", sizeof(m_aRank));
		str_format(m_AccData.m_RconPassword, sizeof(m_AccData.m_RconPassword), g_Config.m_SvRconPassword);
	}
	else if(GameServer()->Server()->IsMod(GetCID()))
	{
		//Server()->SetClientClan(GetCID(), "[*Police*]");
		str_copy(m_aRank, "[*Police*]", sizeof(m_aRank));
		str_format(m_AccData.m_RconPassword, sizeof(m_AccData.m_RconPassword), g_Config.m_SvRconModPassword);
	}
	else if(m_AccData.m_Donor)
		//Server()->SetClientClan(GetCID(), "[*Donor*]");
		str_copy(m_aRank, "[*Donor*]", sizeof(m_aRank));
	else if(m_AccData.m_VIP)
		//Server()->SetClientClan(GetCID(), "[*Vip*]");
		str_copy(m_aRank, "[*Vip*]", sizeof(m_aRank));

	const char *pMatchAdmin = str_find_nocase(Server()->ClientClan(GetCID()), "Admin");
	const char *pMatchVip = str_find_nocase(Server()->ClientClan(GetCID()), "Vip");
	const char *pMatchDonor = str_find_nocase(Server()->ClientClan(GetCID()), "Donor");
	const char *pMatchPolice = str_find_nocase(Server()->ClientClan(GetCID()), "Police");

	if(pMatchAdmin || pMatchVip || pMatchDonor || pMatchPolice)
		Server()->SetClientClan(GetCID(), "");

		/*else if(!str_find_nocase(Server()->ClientClan(GetCID()), "Admin") || !str_find_nocase(Server()->ClientClan(GetCID()), "Vip") || !str_find_nocase(Server()->ClientClan(GetCID()), "Donor") || !str_find_nocase(Server()->ClientClan(GetCID()), "Police"))
		Server()->SetClientClan(GetCID(), " ");*/
	
	
}

void CPlayer::PostTick()
{
	// update latency value
	if(m_PlayerFlags&PLAYERFLAG_SCOREBOARD)
	{
		for(int i = 0; i < MAX_CLIENTS; ++i)
		{
			if(GameServer()->m_apPlayers[i] && GameServer()->m_apPlayers[i]->GetTeam() != TEAM_SPECTATORS)
				m_aActLatency[i] = GameServer()->m_apPlayers[i]->m_Latency.m_Min;
		}
	}

	// update view pos for spectators
	if(m_Team == TEAM_SPECTATORS && m_SpectatorID != SPEC_FREEVIEW && GameServer()->m_apPlayers[m_SpectatorID])
		m_ViewPos = GameServer()->m_apPlayers[m_SpectatorID]->m_ViewPos;
}

void CPlayer::Snap(int SnappingClient)
{
#ifdef CONF_DEBUG
	if(!g_Config.m_DbgDummies || m_ClientID < MAX_CLIENTS-g_Config.m_DbgDummies)
#endif
	if(!Server()->ClientIngame(m_ClientID))
		return;

	CNetObj_ClientInfo *pClientInfo = static_cast<CNetObj_ClientInfo *>(Server()->SnapNewItem(NETOBJTYPE_CLIENTINFO, m_ClientID, sizeof(CNetObj_ClientInfo)));
	if(!pClientInfo)
		return;

	StrToInts(&pClientInfo->m_Name0, 4, Server()->ClientName(m_ClientID));
	
	if(str_comp(m_aRank, "") && Server()->Tick() % 100 < 50)
		StrToInts(&pClientInfo->m_Clan0, 3, m_aRank);
	else
		StrToInts(&pClientInfo->m_Clan0, 3, Server()->ClientClan(m_ClientID));
	

	pClientInfo->m_Country = Server()->ClientCountry(m_ClientID);
	StrToInts(&pClientInfo->m_Skin0, 6, m_TeeInfos.m_SkinName);
	pClientInfo->m_UseCustomColor = m_Rainbow?true:m_TeeInfos.m_UseCustomColor;

	pClientInfo->m_ColorBody = m_Rainbow?m_RainbowColor:m_TeeInfos.m_ColorBody;
	pClientInfo->m_ColorFeet = m_Rainbow?m_RainbowColor:m_TeeInfos.m_ColorFeet;

	CNetObj_PlayerInfo *pPlayerInfo = static_cast<CNetObj_PlayerInfo *>(Server()->SnapNewItem(NETOBJTYPE_PLAYERINFO, m_ClientID, sizeof(CNetObj_PlayerInfo)));
	if(!pPlayerInfo)
		return;

	pPlayerInfo->m_Latency = SnappingClient == -1 ? m_Latency.m_Min : GameServer()->m_apPlayers[SnappingClient]->m_aActLatency[m_ClientID];
	pPlayerInfo->m_Local = 0;
	pPlayerInfo->m_ClientID = m_ClientID;
	pPlayerInfo->m_Score = m_Score;
	pPlayerInfo->m_Team = m_Team;

	if(m_ClientID == SnappingClient)
		pPlayerInfo->m_Local = 1;

	if(m_ClientID == SnappingClient && m_Team == TEAM_SPECTATORS)
	{
		CNetObj_SpectatorInfo *pSpectatorInfo = static_cast<CNetObj_SpectatorInfo *>(Server()->SnapNewItem(NETOBJTYPE_SPECTATORINFO, m_ClientID, sizeof(CNetObj_SpectatorInfo)));
		if(!pSpectatorInfo)
			return;

		pSpectatorInfo->m_SpectatorID = m_SpectatorID;
		pSpectatorInfo->m_X = m_ViewPos.x;
		pSpectatorInfo->m_Y = m_ViewPos.y;
	}
}

void CPlayer::OnDisconnect(const char *pReason)
{
	// City
	if(m_AccData.m_UserID)
		m_pAccount->Reset();

	KillCharacter();

	if(Server()->ClientIngame(m_ClientID))
	{
		char aBuf[512];
		if(pReason && *pReason)
			str_format(aBuf, sizeof(aBuf), "'%s' has left the game (%s)", Server()->ClientName(m_ClientID), pReason);
		else
			str_format(aBuf, sizeof(aBuf), "'%s' has left the game", Server()->ClientName(m_ClientID));
		GameServer()->SendChat(-1, CGameContext::CHAT_ALL, aBuf);

		str_format(aBuf, sizeof(aBuf), "leave player='%d:%s'", m_ClientID, Server()->ClientName(m_ClientID));
		GameServer()->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "game", aBuf);
	}
}

void CPlayer::OnPredictedInput(CNetObj_PlayerInput *NewInput)
{
	// skip the input if chat is active
	if((m_PlayerFlags&PLAYERFLAG_CHATTING) && (NewInput->m_PlayerFlags&PLAYERFLAG_CHATTING))
		return;

	if(m_pCharacter)
		m_pCharacter->OnPredictedInput(NewInput);
}

void CPlayer::OnDirectInput(CNetObj_PlayerInput *NewInput)
{
	if(NewInput->m_PlayerFlags&PLAYERFLAG_CHATTING)
	{
		// skip the input if chat is active
		if(m_PlayerFlags&PLAYERFLAG_CHATTING)
			return;

		// reset input
		if(m_pCharacter)
			m_pCharacter->ResetInput();

		m_PlayerFlags = NewInput->m_PlayerFlags;
 		return;
	}

	m_PlayerFlags = NewInput->m_PlayerFlags;

	if(m_pCharacter)
		m_pCharacter->OnDirectInput(NewInput);

	if(!m_pCharacter && m_Team != TEAM_SPECTATORS && (NewInput->m_Fire&1))
		m_Spawning = true;

	// check for activity
	if(NewInput->m_Direction || m_LatestActivity.m_TargetX != NewInput->m_TargetX ||
		m_LatestActivity.m_TargetY != NewInput->m_TargetY || NewInput->m_Jump ||
		NewInput->m_Fire&1 || NewInput->m_Hook)
	{
		m_LatestActivity.m_TargetX = NewInput->m_TargetX;
		m_LatestActivity.m_TargetY = NewInput->m_TargetY;
		m_LastActionTick = Server()->Tick();
	}
}

CCharacter *CPlayer::GetCharacter()
{
	if(m_pCharacter && m_pCharacter->IsAlive())
		return m_pCharacter;
	return 0;
}

void CPlayer::KillCharacter(int Weapon)
{
	if(m_pCharacter)
	{
		m_pCharacter->Die(m_ClientID, Weapon);
		delete m_pCharacter;
		m_pCharacter = 0;
	}
}

void CPlayer::Respawn()
{
	if(m_Team != TEAM_SPECTATORS)
		m_Spawning = true;
}

void CPlayer::SetTeam(int Team)
{
	// clamp the team
	Team = GameServer()->m_pController->ClampTeam(Team);
	if(m_Team == Team)
		return;

	char aBuf[512];
	str_format(aBuf, sizeof(aBuf), "'%s' joined the %s", Server()->ClientName(m_ClientID), GameServer()->m_pController->GetTeamName(Team));
	GameServer()->SendChat(-1, CGameContext::CHAT_ALL, aBuf);

	KillCharacter();

	m_Team = Team;
	m_LastActionTick = Server()->Tick();
	// we got to wait 0.5 secs before respawning
	m_RespawnTick = Server()->Tick()+Server()->TickSpeed()/2;
	str_format(aBuf, sizeof(aBuf), "team_join player='%d:%s' m_Team=%d", m_ClientID, Server()->ClientName(m_ClientID), m_Team);
	GameServer()->Console()->Print(IConsole::OUTPUT_LEVEL_DEBUG, "game", aBuf);

	GameServer()->m_pController->OnPlayerInfoChange(GameServer()->m_apPlayers[m_ClientID]);

	if(Team == TEAM_SPECTATORS)
	{
		// update spectator modes
		for(int i = 0; i < MAX_CLIENTS; ++i)
		{
			if(GameServer()->m_apPlayers[i] && GameServer()->m_apPlayers[i]->m_SpectatorID == m_ClientID)
				GameServer()->m_apPlayers[i]->m_SpectatorID = SPEC_FREEVIEW;
		}
	}
}

void CPlayer::TryRespawn()
{
	vec2 SpawnPos;

	if(m_Insta)
	{
	if(!GameServer()->m_pController->CanSpawn(m_Team, &SpawnPos, m_Insta?2:0))
		return;
	}
	else
	{
		if(!GameServer()->m_pController->CanSpawn(m_Team, &SpawnPos, m_AccData.m_Arrested?1:0))
		return;
	}

	m_Spawning = false;
	m_pCharacter = new(m_ClientID) CCharacter(&GameServer()->m_World);
	m_pCharacter->Spawn(this, SpawnPos);
	GameServer()->CreatePlayerSpawn(SpawnPos);
}
