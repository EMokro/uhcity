#include <game/server/gamecontext.h>
#include <game/server/entities/character.h>
#include <engine/shared/config.h>
#include <game/version.h>
#include <game/generated/nethash.cpp>

void CGameContext::ConTeleport(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
	int TeleTo = pResult->GetInteger(0);
	int Tele = pResult->GetVictim();


	if (pSelf->m_apPlayers[TeleTo])
	{
		CCharacter* pChr = pSelf->GetPlayerChar(Tele);
		if (pChr && pSelf->GetPlayerChar(TeleTo))
		{
			pChr->m_Core.m_Pos = pSelf->m_apPlayers[TeleTo]->m_ViewPos;
			CNetEvent_Spawn* pEvent = (CNetEvent_Spawn*)pChr->GameServer()->m_Events.Create(NETEVENTTYPE_SPAWN, sizeof(CNetEvent_Spawn));

			if (pEvent)
			{
				pEvent->m_X = pChr->m_Core.m_Pos.x;
				pEvent->m_Y = pChr->m_Core.m_Pos.y;
			}
		}
	}
}

void CGameContext::ConAuth(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
	int Level = pResult->GetInteger(0);
	int ID = pResult->GetVictim();

	CCharacter* pChr = pSelf->GetPlayerChar(ID);
	if (pChr)
		pSelf->Server()->SetRconlvl(ID, Level);
}

void CGameContext::ConVip(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
	int Switch = pResult->GetInteger(0);
	int ID = pResult->GetVictim();
	char aBuf[200];

	CCharacter* pChr = pSelf->GetPlayerChar(ID);
	if (pChr)
	{
		CPlayer* pP = pSelf->m_apPlayers[ID];
		pChr->GetPlayer()->m_AccData.m_VIP = Switch;

		if(Switch)
			str_format(aBuf, sizeof aBuf, "You are now vip.", pSelf->Server()->ClientName(ID));
		else
			str_format(aBuf, sizeof aBuf, "You are no longer vip.", pSelf->Server()->ClientName(ID));

		pP->GetCharacter()->GameServer()->SendChatTarget(ID, aBuf);
	}
}
void CGameContext::ConDonor(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
	int Switch = pResult->GetInteger(0);
	int DonorID = pResult->GetVictim();
	char aBuf[200];

	CCharacter* pChr = pSelf->GetPlayerChar(DonorID);
	if (pChr)
	{
		pChr->GetPlayer()->m_AccData.m_Donor = Switch;
		if(Switch)
				str_format(aBuf, sizeof aBuf, "'%s' is a Donor now.", pSelf->Server()->ClientName(DonorID));
		else
				str_format(aBuf, sizeof aBuf, "'%s' is no longer a Donor.", pSelf->Server()->ClientName(DonorID));

		pSelf->SendChat(-1, CHAT_ALL, aBuf);
	}
}

void CGameContext::ConJail(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
	int Time = pResult->GetInteger(0);
	int JailID = pResult->GetVictim();
	char aBuf[200];

	if(Time < 20 || Time > 2419200)
	{
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "Jail", "Set a time between 20 and 2419200 (4 weeks)");
		return;
	}

	CCharacter* pChr = pSelf->GetPlayerChar(JailID);
	if(pChr)
	{
		if(pChr->IsAlive())
		{
			pChr->GetPlayer()->m_AccData.m_Arrested = Time;
	
			if(Time)
			{
				str_format(aBuf, sizeof aBuf, "'%s' is arrested for %i seconds.", pSelf->Server()->ClientName(JailID), Time);
				pSelf->SendChat(-1, CHAT_ALL, aBuf);
				pChr->Die(pChr->GetPlayer()->GetCID(), WEAPON_GAME);
			}
		}
	}
}
void CGameContext::ConGiveMoney(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
	long long Amount = pResult->GetLongLong(0);
	int ID = pResult->GetVictim();
	char aBuf[200];

	if(Amount > 5000000 || Amount < 1)
	{
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "console", "Set a value between 1 and 5Mio");
		return;
	}

	CCharacter* pChr = pSelf->GetPlayerChar(ID);
	if(pChr)
	{
		if(pChr->IsAlive())
		{
			pChr->GetPlayer()->m_AccData.m_Money += Amount;
	
			if(Amount)
			{
				str_format(aBuf, sizeof aBuf, "'%s' got %lld$", pSelf->Server()->ClientName(ID), Amount);
				pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "Money", aBuf);

				CPlayer* pP = pSelf->m_apPlayers[ID];

				str_format(aBuf, sizeof aBuf, "An Admin gave you %lld$", Amount);
				pP->GetCharacter()->GameServer()->SendChatTarget(ID, aBuf);
			}
		}
	}
}

void CGameContext::ConSetMoney(IConsole::IResult* pResult, void* pUserData)
{
	CGameContext* pSelf = (CGameContext*)pUserData;
	long long Amount = pResult->GetLongLong(0);
	int ID = pResult->GetVictim();
	char aBuf[200];

	if (Amount < 1)
	{
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "console", "Set a value of at least 1");
		return;
	}

	CCharacter* pChr = pSelf->GetPlayerChar(ID);
	if (pChr)
	{
		if (pChr->IsAlive())
		{
			if (Amount)
			{
				char numBuf[16];
				pChr->GetPlayer()->m_AccData.m_Money = Amount;

				str_format(aBuf, sizeof aBuf, "'%s' got %lld$", pSelf->Server()->ClientName(ID), Amount);
				pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "Money", aBuf);

				CPlayer* pP = pSelf->m_apPlayers[ID];

				pSelf->FormatInt(Amount, numBuf);
				str_format(aBuf, sizeof aBuf, "An Admin set your money to %s$", numBuf);
				pP->GetCharacter()->GameServer()->SendChatTarget(ID, aBuf);
			}
		}
	}
}

void CGameContext::ConUnjail(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
	int UnJailID = pResult->GetVictim();

	CCharacter* pChr = pSelf->GetPlayerChar(UnJailID);
	if(pChr)
	{
		pChr->GetPlayer()->m_AccData.m_Arrested = 1;
	}
}

void CGameContext::ConSetLvl(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
	int ID = pResult->GetVictim();
	int Amount = pResult->GetInteger(0);
	char aBuf[200];

	if(Amount > 800 || Amount < 1)
	{
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "console", "Set a value between 1 and 800");
		return;
	}

	CCharacter* pChr = pSelf->GetPlayerChar(ID);
	if(pChr)
	{
		if(pChr->IsAlive())
		{
			if(Amount)
			{
				CPlayer* pP = pChr->GetPlayer();

				pP->m_AccData.m_Level = Amount;
				pP->m_AccData.m_ExpPoints = 0;
				pP->m_Score = pChr->GetPlayer()->m_AccData.m_Level;

				str_format(aBuf, sizeof aBuf, "'%s' is now level %d", pSelf->Server()->ClientName(ID), Amount);
				pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "Level", aBuf);

				str_format(aBuf, sizeof aBuf, "An Admin set your level to %d", Amount);
				pP->GetCharacter()->GameServer()->SendChatTarget(ID, aBuf);
			}
		}
	}
}

void CGameContext::ConSetLvlWeapon(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
	int ID = pResult->GetVictim();
	int Weapon = pResult->GetInteger(0);
	int Amount = pResult->GetInteger(1);
	char aBuf[200];

	if(Amount > 800 || Amount < 1)
	{
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "console", "Set a value between 1 and 800");
		return;
	}

	if (Weapon < -1 || Weapon > WEAPON_RIFLE) {
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "console", "No such weapon");
		return;
	}

	CCharacter* pChr = pSelf->GetPlayerChar(ID);
	if(pChr)
	{
		if(pChr->IsAlive())
		{
			if(Amount)
			{
				CPlayer* pP = pChr->GetPlayer();

				pP->m_AccData.m_LvlWeapon[Weapon] = Amount;
				pP->m_AccData.m_ExpWeapon[Weapon] = 0;

				str_format(aBuf, sizeof aBuf, "'%s' Weapon level is now %d", pSelf->Server()->ClientName(ID), Amount);
				pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "console", aBuf);

				str_format(aBuf, sizeof aBuf, "An admin set your level to %d", Amount);
				pP->GetCharacter()->GameServer()->SendChatTarget(ID, aBuf);
			}
		}
	}
}

void CGameContext::ConSetLife(IConsole::IResult* pResult, void* pUserData)
{
	CGameContext* pSelf = (CGameContext*)pUserData;
	int Amount = pResult->GetInteger(0);
	int ID = pResult->GetVictim();
	char aBuf[200];

	if (Amount < 10 && Amount > 500)
	{
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "console", "Value must be between 10 and 500");
		return;
	}

	CCharacter* pChr = pSelf->GetPlayerChar(ID);
	if (pChr)
	{
		if (pChr->IsAlive())
		{
			if (Amount)
			{
				CPlayer* pP = pSelf->m_apPlayers[ID];

				pChr->GetPlayer()->m_AccData.m_Health = Amount;

				str_format(aBuf, sizeof aBuf, "'%s' got %d life", pSelf->Server()->ClientName(ID), Amount);
				pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "Money", aBuf);

				str_format(aBuf, sizeof aBuf, "An Admin set your life to %d", Amount);
				pP->GetCharacter()->GameServer()->SendChatTarget(ID, aBuf);
			}
		}
	}
}

void CGameContext::ConSetArmor(IConsole::IResult* pResult, void* pUserData)
{
	CGameContext* pSelf = (CGameContext*)pUserData;
	int Amount = pResult->GetInteger(0);
	int ID = pResult->GetVictim();
	char aBuf[200];

	if (Amount < 10 && Amount > 500)
	{
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "console", "Value must be between 10 and 500");
		return;
	}

	CCharacter* pChr = pSelf->GetPlayerChar(ID);
	if (pChr)
	{
		if (pChr->IsAlive())
		{
			if (Amount)
			{	
				CPlayer* pP = pSelf->m_apPlayers[ID];

				pChr->GetPlayer()->m_AccData.m_Armor = Amount;

				str_format(aBuf, sizeof aBuf, "'%s' got %d armor", pSelf->Server()->ClientName(ID), Amount);
				pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "Money", aBuf);

				str_format(aBuf, sizeof aBuf, "An Admin set your armor to %d", Amount);
				pP->GetCharacter()->GameServer()->SendChatTarget(ID, aBuf);
			}
		}
	}
}

void CGameContext::ConSetClientName(IConsole::IResult* pResult, void* pUserData)
{
	CGameContext* pSelf = (CGameContext*)pUserData;
	char Value[16];
	str_append(Value, pResult->GetString(0), sizeof(Value));
	int ID = pResult->GetVictim();

	if (Value) {
		char aBuf[128];
		str_format(aBuf, sizeof aBuf, "'%s' changed name to '%s'", pSelf->Server()->ClientName(ID), Value);
		pSelf->Server()->SetClientName(ID, Value);
		pSelf->SendChat(-1, CHAT_ALL, aBuf);
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "console", aBuf);
	}
}

void CGameContext::ConKill(IConsole::IResult* pResult, void* pUserData)
{
	CGameContext* pSelf = (CGameContext*)pUserData;
	int ID = pResult->GetInteger(0);
	char aBuf[128];

	if (ID < 0 || ID > MAX_CLIENTS) {
		str_format(aBuf, sizeof aBuf, "%d ist invalid", ID);
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "console", aBuf);
		return;
	}

	CPlayer* pP = pSelf->m_apPlayers[ID];

	if (pP) {
		pP->GetCharacter()->GameServer()->SendChatTarget(ID, "You got killed by console");
		pP->KillCharacter();
	}
	else
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "console", "No such player");
}

void CGameContext::ConSetClientGravityY(IConsole::IResult* pResult, void* pUserData)
{
	CGameContext* pSelf = (CGameContext*)pUserData;
	int ID = pResult->GetVictim();
	float Amount = pResult->GetFloat(0);
	char aBuf[128];

	if (ID < 0 || ID > MAX_CLIENTS) {
		str_format(aBuf, sizeof aBuf, "%d ist invalid", ID);
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "console", aBuf);
		return;
	}

	CCharacter *pChar = pSelf->m_apPlayers[ID]->GetCharacter();

	if (!pChar) {
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "console", "Player not available");
		return;
	}

	pChar->m_GravityY = Amount;
	pSelf->SendTuningParams(ID);
	pSelf->SendChatTarget(ID, "Your gravity got changed");
}

void CGameContext::ConSetClientGravityX(IConsole::IResult* pResult, void* pUserData)
{
	CGameContext* pSelf = (CGameContext*)pUserData;
	int ID = pResult->GetVictim();
	float Amount = pResult->GetFloat(0);
	char aBuf[128];

	if (ID < 0 || ID > MAX_CLIENTS) {
		str_format(aBuf, sizeof aBuf, "%d ist invalid", ID);
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "console", aBuf);
		return;
	}

	CCharacter *pChar = pSelf->m_apPlayers[ID]->GetCharacter();

	if (!pChar) {
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "console", "Player not available");
		return;
	}

	pChar->m_GravityX = Amount;
	pSelf->SendChatTarget(ID, "Your gravity got changed");
}

void CGameContext::ConFreeze(IConsole::IResult* pResult, void* pUserData)
{
	CGameContext* pSelf = (CGameContext*)pUserData;
	int Amount = pResult->GetInteger(0);
	int ID = pResult->GetVictim();
	char aBuf[128];

	if (ID < 0 || ID > MAX_CLIENTS) {
		str_format(aBuf, sizeof aBuf, "%d ist invalid", ID);
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "console", aBuf);
		return;
	}

	CPlayer* pP = pSelf->m_apPlayers[ID];

	if (pP) {
		pP->GetCharacter()->Freeze(Amount * 50);

		str_format(aBuf, sizeof aBuf, "You got freezed for %d seconds by console", Amount);
		pP->GetCharacter()->GameServer()->SendChatTarget(ID, aBuf);
	}
	else
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "console", "No such player");
}

void CGameContext::ConUnFreeze(IConsole::IResult* pResult, void* pUserData)
{
	CGameContext* pSelf = (CGameContext*)pUserData;
	int ID = pResult->GetVictim();
	char aBuf[128];

	if (ID < 0 || ID > MAX_CLIENTS) {
		str_format(aBuf, sizeof aBuf, "%d ist invalid", ID);
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "console", aBuf);
		return;
	}

	CPlayer* pP = pSelf->m_apPlayers[ID];

	if (pP) {
		pP->GetCharacter()->Unfreeze();
		pP->GetCharacter()->GameServer()->SendChatTarget(ID, "You were thawed by console");
	}
	else
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "console", "No such player");
}

void CGameContext::ConSameIP(IConsole::IResult* pResult, void* pUserData)
{
	char aBuf[256];
	char checkAddr[NETADDR_MAXSTRSIZE];
	char resAddr[NETADDR_MAXSTRSIZE];
	CGameContext* pSelf = (CGameContext*)pUserData;
	int counter = 0;

	for (int i = 0; i < MAX_CLIENTS; i++) {
		if (pSelf->Server()->ClientIngame(i)) {

			pSelf->Server()->GetClientAddr(i, checkAddr, NETADDR_MAXSTRSIZE);

			for (int j = i + 1; j < MAX_CLIENTS; j++) {
				if (pSelf->Server()->ClientIngame(j)) {
					pSelf->Server()->GetClientAddr(j, resAddr, NETADDR_MAXSTRSIZE);

					if (!str_comp_nocase(checkAddr, resAddr)) {
						str_format(aBuf, sizeof aBuf, "IP [%s]: ID [%d]%s [%d]%s", checkAddr,
						i, !pSelf->m_apPlayers[i]->m_AccData.m_UserID ? "D" : "",
						j, !pSelf->m_apPlayers[j]->m_AccData.m_UserID ? "D" : "");
						pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "server", aBuf);
						counter++;
					}
				}
			}
		}
	}

	str_format(aBuf, sizeof aBuf, "%d results found", counter);
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "server", aBuf);
}

// mute through client id
void CGameContext::ConMute(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
	int Victim = pResult->GetVictim();
	int Amount = clamp(pResult->GetInteger(0), 1, 86400);
	const char *pReason = pResult->NumArguments() > 2 ? pResult->GetString(1) : "";

	if (Victim < 0 || Victim > MAX_CLIENTS || !pSelf->m_apPlayers[Victim])
	{
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "muteid", "Client id not found.");
		return;
	}

	NETADDR Addr;
	pSelf->Server()->GetClientAddr(Victim, &Addr);

	pSelf->Mute(&Addr, Amount, pSelf->Server()->ClientName(Victim), pReason);
}

// mute through ip, arguments reversed to workaround parsing
void CGameContext::ConMuteIP(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
	NETADDR Addr;
	const char *pReason = pResult->NumArguments() > 2 ? pResult->GetString(1) : "";

	if (net_addr_from_str(&Addr, pResult->GetString(0)))
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "mutes", "Invalid network address to mute");

	
	pSelf->Mute(&Addr, clamp(pResult->GetInteger(0), 1, 86400), NULL, pReason);
}

// unmute by mute list index
void CGameContext::ConUnmute(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
	char aIpBuf[64];
	char aBuf[64];
	int Victim = pResult->GetVictim();

	if (Victim < 0 || Victim >= pSelf->m_NumMutes)
		return;

	pSelf->m_NumMutes--;
	pSelf->m_aMutes[Victim] = pSelf->m_aMutes[pSelf->m_NumMutes];

	net_addr_str(&pSelf->m_aMutes[Victim].m_Addr, aIpBuf, sizeof(aIpBuf));
	str_format(aBuf, sizeof(aBuf), "Unmuted %s", aIpBuf);
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "mutes", aBuf);

	pSelf->SendChatTarget(Victim, "You got unmuted by console");
}

// list mutes
void CGameContext::ConMutes(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;

	if (pSelf->m_NumMutes <= 0)
	{
		// Just to make sure.
		pSelf->m_NumMutes = 0;
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "mutes",
			"There are no active mutes.");
		return;
	}

	char aIpBuf[64];
	char aBuf[128];
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "mutes", "Active mutes:");
	for (int i = 0; i < pSelf->m_NumMutes; i++)
	{
		net_addr_str(&pSelf->m_aMutes[i].m_Addr, aIpBuf, sizeof(aIpBuf));
		str_format(aBuf, sizeof aBuf, "%d: '%s', %d seconds left (%s)", i, aIpBuf,
				(pSelf->m_aMutes[i].m_Expire - pSelf->Server()->Tick()) / pSelf->Server()->TickSpeed(), pSelf->m_aMutes[i].m_aReason);
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "mutes", aBuf);
	}
}

void CGameContext::ConLookUp(IConsole::IResult* pResult, void* pUserData)
{
	CGameContext* pSelf = (CGameContext*)pUserData;
	int ID = pResult->GetVictim();
	char aBuf[128], numBuf[32];
	char addrBuf[NETADDR_MAXSTRSIZE];

	if (ID < 0 || ID > MAX_CLIENTS) {
		str_format(aBuf, sizeof aBuf, "%d ist invalid", ID);
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "console", aBuf);
		return;
	}

	CPlayer* pP = pSelf->m_apPlayers[ID];

	if (pP) {
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "Server", "** Look Up **");

		str_format(aBuf, sizeof aBuf, "ID: %d", ID);
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "Server", aBuf);

		str_format(aBuf, sizeof aBuf, "AccID: %d", pP->m_AccData.m_UserID);
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "Server", aBuf);
		
		pSelf->Server()->GetClientAddr(pP->GetCID(), addrBuf, NETADDR_MAXSTRSIZE);
		str_format(aBuf, sizeof aBuf, "IP: %s", addrBuf);
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "Server", aBuf);

		str_format(aBuf, sizeof aBuf, "Username: %s", pP->m_AccData.m_Username);
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "Server", aBuf);

		pSelf->FormatInt(pP->m_AccData.m_Money, numBuf);
		str_format(aBuf, sizeof aBuf, "Money: %s$", numBuf);
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "Server", aBuf);
	}
	else
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "console", "No such player");
}

void CGameContext::ConSendAfk(IConsole::IResult* pResult, void* pUserData)
{
	CGameContext* pSelf = (CGameContext*)pUserData;
	int ID = pResult->GetVictim();
	char aBuf[128];

	if (ID < 0 || ID > MAX_CLIENTS) {
		str_format(aBuf, sizeof aBuf, "%d ist invalid", ID);
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "console", aBuf);
		return;
	}

	CPlayer* pP = pSelf->m_apPlayers[ID];

	if (pP) {
		pP->SendAfk();

		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "console", "Player is afk now");
	} else 
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "console", "No such player");
}

void CGameContext::ConSetBounty(IConsole::IResult* pResult, void* pUserData) {
	CGameContext* pSelf = (CGameContext*)pUserData;
	int ID = pResult->GetVictim();
	int Amount = pResult->GetLongLong(0);
	char aBuf[128], numBuf[32];

	CPlayer *pP = pSelf->m_apPlayers[ID];

	if (!pP) {
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "console", "Player does not exist");
		return;
	}

	if (Amount < 0) {
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "console", "The bounty must be higher than 0");
		return;
	}

	pSelf->FormatInt(Amount, numBuf);
	str_format(aBuf, sizeof aBuf, "An admin put a bounty of %s$ on you.", numBuf);
	pSelf->SendChatTarget(ID, aBuf);

	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "console", aBuf);
}


void CGameContext::ConStartEvent(IConsole::IResult* pResult, void* pUserData) {
	CGameContext* pSelf = (CGameContext*)pUserData;
	int ID = pResult->GetVictim();
	int Duration = pResult->GetInteger(0);

	if (Duration > g_Config.m_SvEventDurationMax || Duration < 0) {
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "console", "Incorrect duration. Check the config");
		return;
	}

	pSelf->GameEvent()->Create(ID, Duration);
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "console", "event started");
}

void CGameContext::ConAbortEvent(IConsole::IResult* pResult, void* pUserData) {
	CGameContext* pSelf = (CGameContext*)pUserData;

	pSelf->GameEvent()->Abort();
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "console", "event aborted");
}

void CGameContext::ConEventTimer(IConsole::IResult* pResult, void* pUserData) {
	CGameContext* pSelf = (CGameContext*)pUserData;
	char aBuf[16];

	str_format(aBuf, sizeof aBuf, "%d", pSelf->GameEvent()->m_Timer);
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "console", aBuf);
}

void CGameContext::ConSendFakeParams(IConsole::IResult* pResult, void* pUserData)
{
	CGameContext* pSelf = (CGameContext*)pUserData;
	int Victim = pResult->GetVictim();

	pSelf->SendTuningParams(Victim);
}

// Filesystem

void CGameContext::ConFsBackupAccounts(IConsole::IResult* pResult, void* pUserData)
{
	CGameContext* pSelf = (CGameContext*)pUserData;

	pSelf->Filesystem()->BackupAccounts();
}
