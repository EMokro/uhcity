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
		}
	}
}

void CGameContext::ConUp(IConsole::IResult *pResult, void *pUserData)
{
CGameContext *pSelf = (CGameContext *) pUserData;
	int Move = pResult->GetVictim();


	if (pSelf->m_apPlayers[Move])
	{
		CCharacter* pChr = pSelf->GetPlayerChar(Move);
		if (pChr && pSelf->GetPlayerChar(Move))
		{
			pChr->m_Core.m_Pos.y += -32;
		}
	}
}

void CGameContext::ConDown(IConsole::IResult *pResult, void *pUserData)
{
CGameContext *pSelf = (CGameContext *) pUserData;
	int Move = pResult->GetVictim();


	if (pSelf->m_apPlayers[Move])
	{
		CCharacter* pChr = pSelf->GetPlayerChar(Move);
		if (pChr && pSelf->GetPlayerChar(Move))
		{
			pChr->m_Core.m_Pos.y += 32;
		}
	}
}

void CGameContext::ConLeft(IConsole::IResult *pResult, void *pUserData)
{
CGameContext *pSelf = (CGameContext *) pUserData;
	int Move = pResult->GetVictim();


	if (pSelf->m_apPlayers[Move])
	{
		CCharacter* pChr = pSelf->GetPlayerChar(Move);
		if (pChr && pSelf->GetPlayerChar(Move))
		{
			pChr->m_Core.m_Pos.x += -32;
		}
	}
}

void CGameContext::ConRight(IConsole::IResult *pResult, void *pUserData)
{
CGameContext *pSelf = (CGameContext *) pUserData;
	int Move = pResult->GetVictim();


	if (pSelf->m_apPlayers[Move])
	{
		CCharacter* pChr = pSelf->GetPlayerChar(Move);
		if (pChr && pSelf->GetPlayerChar(Move))
		{
			pChr->m_Core.m_Pos.x += 32;
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
	int VipID = pResult->GetVictim();
	char aBuf[200];

	CCharacter* pChr = pSelf->GetPlayerChar(VipID);
	if (pChr)
	{
		pChr->GetPlayer()->m_AccData.m_VIP = Switch;
		if(Switch)
			str_format(aBuf, sizeof aBuf, "'%s' is Vip now.", pSelf->Server()->ClientName(VipID));
		else
			str_format(aBuf, sizeof aBuf, "'%s' is no longer Vip.", pSelf->Server()->ClientName(VipID));
		pSelf->SendChat(-1, CHAT_ALL, aBuf);
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
	int Time = pResult->GetInteger(0);
	int JailID = pResult->GetVictim();
	char aBuf[200];

	if(Time > 5000000 || Time < 1)
	{
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "Debug", "Set a value between 1 and 5Mio");
		return;
	}

	CCharacter* pChr = pSelf->GetPlayerChar(JailID);
	if(pChr)
	{
		if(pChr->IsAlive())
		{
			pChr->GetPlayer()->m_AccData.m_Money += Time;
	
			if(Time)
			{
				str_format(aBuf, sizeof aBuf, "'%s' got %d TC", pSelf->Server()->ClientName(JailID), Time);
				pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "Money", aBuf);
			
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
			pChr->GetPlayer()->m_AccData.m_Money = Amount;

			if (Amount)
			{
				str_format(aBuf, sizeof aBuf, "'%s' got %d TC", pSelf->Server()->ClientName(ID), Amount);
				pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "Money", aBuf);

			}
		}
	}
}

void CGameContext::ConLogout(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
	int Victim = pResult->m_ClientID;

	if(Victim)
	pSelf->Server()->Logout(Victim);
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
	int Time = pResult->GetInteger(0);
	int JailID = pResult->GetVictim();
	char aBuf[200];

	if(Time > 800 || Time < 1)
	{
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "Debug", "Set a value between 1 and 800");
		return;
	}

	CCharacter* pChr = pSelf->GetPlayerChar(JailID);
	if(pChr)
	{
		if(pChr->IsAlive())
		{
			pChr->GetPlayer()->m_AccData.m_Level = Time;
			pChr->GetPlayer()->m_Score = pChr->GetPlayer()->m_AccData.m_Level;
	
			if(Time)
			{
				str_format(aBuf, sizeof aBuf, "'%s' is now level %d", pSelf->Server()->ClientName(JailID), Time);
				pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "Level", aBuf);
			
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
			pChr->GetPlayer()->m_AccData.m_Health = Amount;

			if (Amount)
			{
				str_format(aBuf, sizeof aBuf, "'%s' got %d life", pSelf->Server()->ClientName(ID), Amount);
				pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "Money", aBuf);

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
			pChr->GetPlayer()->m_AccData.m_Armor = Amount;

			if (Amount)
			{	
				str_format(aBuf, sizeof aBuf, "'%s' got %d armor", pSelf->Server()->ClientName(ID), Amount);
				pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "Money", aBuf);
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
		pSelf->Server()->SetClientName(ID, Value);
		str_format(aBuf, sizeof aBuf, "changed %s name to %s", pSelf->Server()->ClientName(ID), Value);
	}
}
