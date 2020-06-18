#include <game/server/gamecontext.h>
#include <game/server/entities/character.h>
#include <engine/shared/config.h>
#include <game/version.h>
#include <game/generated/nethash.cpp>
#if defined(CONF_SQL)
#include <game/server/score/sql_score.h>
#include <engine/server/server.h>
#endif

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

void CGameContext::ConPolice(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
	int Switch = pResult->GetInteger(0);
	int ID = pResult->GetVictim();
	char aBuf[200];

	CCharacter* pChr = pSelf->GetPlayerChar(ID);
	if (pChr)
	{
		pSelf->Server()->Police(ID, Switch ? 1 : 0);
		if(Switch)
			str_format(aBuf, sizeof aBuf, "'%s' is a Police now.", pSelf->Server()->ClientName(ID));
		else
			str_format(aBuf, sizeof aBuf, "'%s' is no longer a Police.", pSelf->Server()->ClientName(ID));
		pSelf->SendChat(-1, CHAT_ALL, aBuf);
	}
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
	int Amount = pResult->GetInteger(0);
	int ID = pResult->GetVictim();
	char aBuf[200];

	if(Amount > 5000000 || Amount < 1)
	{
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "Debug", "Set a value between 1 and 5Mio");
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
				str_format(aBuf, sizeof aBuf, "'%s' got %d$", pSelf->Server()->ClientName(ID), Amount);
				pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "Money", aBuf);

				CPlayer* pP = pSelf->m_apPlayers[ID];

				str_format(aBuf, sizeof aBuf, "An Admin gave you %d$", Amount);
				pP->GetCharacter()->GameServer()->SendChatTarget(ID, aBuf);
			}
		}
	}
}

void CGameContext::ConSetMoney(IConsole::IResult* pResult, void* pUserData)
{
	CGameContext* pSelf = (CGameContext*)pUserData;
	int Amount = pResult->GetInteger(0);
	int ID = pResult->GetVictim();
	char aBuf[200];

	if (Amount < 1)
	{
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "Debug", "Set a value of at least 1");
		return;
	}

	CCharacter* pChr = pSelf->GetPlayerChar(ID);
	if (pChr)
	{
		if (pChr->IsAlive())
		{
			if (Amount)
			{
				pChr->GetPlayer()->m_AccData.m_Money = Amount;

				str_format(aBuf, sizeof aBuf, "'%s' got %d$", pSelf->Server()->ClientName(ID), Amount);
				pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "Money", aBuf);

				CPlayer* pP = pSelf->m_apPlayers[ID];

				str_format(aBuf, sizeof aBuf, "An Admin set your money to %d$", Amount);
				pP->GetCharacter()->GameServer()->SendChatTarget(ID, aBuf);
			}
		}
	}
}

void CGameContext::ConUnjail(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
	int UnJailID = pResult->GetVictim();
	char aBuf[200];


	CCharacter* pChr = pSelf->GetPlayerChar(UnJailID);
	if(pChr)
	{
		pChr->GetPlayer()->m_AccData.m_Arrested = 1;
	}
}

void CGameContext::ConSetLvl(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
	int Amount = pResult->GetInteger(0);
	int ID = pResult->GetVictim();
	char aBuf[200];

	if(Amount > 800 || Amount < 1)
	{
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "Debug", "Set a value between 1 and 800");
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

void CGameContext::ConSetLife(IConsole::IResult* pResult, void* pUserData)
{
	CGameContext* pSelf = (CGameContext*)pUserData;
	int Amount = pResult->GetInteger(0);
	int ID = pResult->GetVictim();
	char aBuf[200];

	if (Amount < 10 && Amount > 500)
	{
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "Debug", "Value must be between 10 and 500");
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
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "Debug", "Value must be between 10 and 500");
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
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "Debug", aBuf);
	}
}

void CGameContext::ConKill(IConsole::IResult* pResult, void* pUserData)
{
	CGameContext* pSelf = (CGameContext*)pUserData;
	int ID = pResult->GetInteger(0);
	char aBuf[128];

	if (ID < 0 || ID > MAX_CLIENTS) {
		str_format(aBuf, sizeof aBuf, "%d ist invalid", ID);
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "Debug", aBuf);
		return;
	}
	CPlayer* pP = pSelf->m_apPlayers[ID];

	if (pP) {
		pP->GetCharacter()->GameServer()->SendChatTarget(ID, "You got killed by console");
		pP->KillCharacter();
	}
	else
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "Debug", "No such player");
}

void CGameContext::ConFreeze(IConsole::IResult* pResult, void* pUserData)
{
	CGameContext* pSelf = (CGameContext*)pUserData;
	int Amount = pResult->GetInteger(0);
	int ID = pResult->GetVictim();
	char aBuf[128];

	if (ID < 0 || ID > MAX_CLIENTS) {
		str_format(aBuf, sizeof aBuf, "%d ist invalid", ID);
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "Debug", aBuf);
		return;
	}

	CPlayer* pP = pSelf->m_apPlayers[ID];

	if (pP) {
		pP->GetCharacter()->Freeze(Amount * 50);

		str_format(aBuf, sizeof aBuf, "You got freezed for %d seconds by console", Amount);
		pP->GetCharacter()->GameServer()->SendChatTarget(ID, aBuf);
	}
	else
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "Debug", "No such player");
}

void CGameContext::ConUnFreeze(IConsole::IResult* pResult, void* pUserData)
{
	CGameContext* pSelf = (CGameContext*)pUserData;
	int ID = pResult->GetVictim();
	char aBuf[128];

	if (ID < 0 || ID > MAX_CLIENTS) {
		str_format(aBuf, sizeof aBuf, "%d ist invalid", ID);
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "Debug", aBuf);
		return;
	}

	CPlayer* pP = pSelf->m_apPlayers[ID];

	if (pP) {
		pP->GetCharacter()->Unfreeze();
		pP->GetCharacter()->GameServer()->SendChatTarget(ID, "You were thawed by console");
	}
	else
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "Debug", "No such player");
}

void CGameContext::ConSameIP(IConsole::IResult* pResult, void* pUserData)
{
	NETADDR Addr;
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
					pSelf->Server()->GetClientAddr(i, resAddr, NETADDR_MAXSTRSIZE);

					if (str_comp(checkAddr, resAddr) == 0) {

						str_format(aBuf, sizeof aBuf, "ID %d, %d: IP %s", i, j, checkAddr);
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

void CGameContext::ConLookUp(IConsole::IResult* pResult, void* pUserData)
{
	CGameContext* pSelf = (CGameContext*)pUserData;
	int ID = pResult->GetVictim();
	char aBuf[128];
	char addrBuf[NETADDR_MAXSTRSIZE];

	if (ID < 0 || ID > MAX_CLIENTS) {
		str_format(aBuf, sizeof aBuf, "%d ist invalid", ID);
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "Debug", aBuf);
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

		str_format(aBuf, sizeof aBuf, "Money: %d$", pP->m_AccData.m_Money);
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "Server", aBuf);
	}
	else
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "Debug", "No such player");
}
