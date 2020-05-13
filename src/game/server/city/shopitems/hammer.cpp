/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <game/generated/protocol.h>
#include <game/server/gamecontext.h>
#include "hammer.h"

CHammer::CHammer(CGameWorld *pGameWorld, int Owner, vec2 Pos, int Type)
: CEntity(pGameWorld, CGameWorld::ENTTYPE_PROJECTILE)
{
	m_Owner = Owner;
	m_Pos = Pos;
	m_Type = Type;
	m_StartTick = Server()->Tick();
	m_Visible = false;

	for(int i = 0; i < 10; i++)
		m_IDs[i] = Server()->SnapNewID();

	GameWorld()->InsertEntity(this);
}

void CHammer::Reset()
{
	GameServer()->m_World.DestroyEntity(this);

	for(int i = 0; i < 10; i++)
		Server()->SnapFreeID(m_IDs[i]);
}

void CHammer::Tick()
{
	CCharacter *pOwner = GameServer()->GetPlayerChar(m_Owner);

	if(!pOwner)
	{
		Reset();
		return;
	}

	if(!m_Visible)
		return;

	char aBuf[128];
	int Click = pOwner->MouseEvent(m_Pos);

	if(!Click)
	{
		m_LastClick = 0;
		return;
	}
	
	switch(m_Type)
	{
	case 1:
		pOwner->Buy("Hammer walls", &pOwner->GetPlayer()->m_AccData.m_HammerWalls, g_Config.m_EuHammerWalls, Click, 5);
		break;
	case 2:
		pOwner->Buy("Hammer shot", &pOwner->GetPlayer()->m_AccData.m_HammerShot, g_Config.m_EuHammerShot, Click, 3);
		break;
	case 3:
		pOwner->Buy("Hammer kill", &pOwner->GetPlayer()->m_AccData.m_HammerKill, g_Config.m_EuHammerKill, Click, 1);
		break;
	}
}


void CHammer::Snap(int SnappingClient)
{
	if(SnappingClient != m_Owner || NetworkClipped(SnappingClient, m_Pos) || !m_Visible)
		return;
	if(Server()->Tick()%5 == 0)
		m_StartTick = Server()->Tick();

	vec2 Positions[3];
	Positions[0] = vec2(m_Pos.x - 32, m_Pos.y - 16);
	Positions[1] = vec2(m_Pos.x + 32, m_Pos.y - 16);
	Positions[2] = vec2(m_Pos.x, m_Pos.y + 16);

	if(m_Type == 1)// Wall
	{
		CNetObj_Laser *pObj[2];
	
		for(int i = 0; i < 2; i++)
		{
			pObj[i] = static_cast<CNetObj_Laser *>(Server()->SnapNewItem(NETOBJTYPE_LASER, m_IDs[i], sizeof(CNetObj_Laser)));

			if(!pObj[i])
				return;

			pObj[i]->m_X = (int)i?m_Pos.x - 16:m_Pos.x + 16;
			pObj[i]->m_Y = (int)i?m_Pos.y + 32:m_Pos.y - 32;
			pObj[i]->m_FromX = (int)m_Pos.x - 16;
			pObj[i]->m_FromY = (int)m_Pos.y + 32;
			pObj[i]->m_StartTick = Server()->Tick();
		}

		if(Server()->Tick()%100 == 0)
		{
			CNetEvent_Death *pEvent = (CNetEvent_Death *)GameServer()->m_Events.Create(NETEVENTTYPE_DEATH, sizeof(CNetEvent_Death));

			if(pEvent)
			{
				pEvent->m_X = (int)m_Pos.x;
				pEvent->m_Y = (int)m_Pos.y;
				pEvent->m_ClientID = m_Owner;
			}
		}

	}
	else if(m_Type == 2)//Shot
	{
		CNetObj_Laser *pObj[3];
	
		for(int i = 0; i < 3; i++)
		{
			pObj[i] = static_cast<CNetObj_Laser *>(Server()->SnapNewItem(NETOBJTYPE_LASER, m_IDs[i], sizeof(CNetObj_Laser)));

			if(!pObj[i])
				return;

			pObj[i]->m_X = (int)Positions[i].x;
			pObj[i]->m_Y = (int)Positions[i].y;
			pObj[i]->m_FromX = (int)Positions[i].x;
			pObj[i]->m_FromY = (int)Positions[i].y;
			pObj[i]->m_StartTick = Server()->Tick();
		}
	}
	else if(m_Type == 3)// Kill
	{
		CNetObj_Pickup *pP = static_cast<CNetObj_Pickup *>(Server()->SnapNewItem(NETOBJTYPE_PICKUP, m_ID, sizeof(CNetObj_Pickup)));

		if(!pP)
			return;

		pP->m_X = (int)m_Pos.x;
		pP->m_Y = (int)m_Pos.y;
		pP->m_Type = POWERUP_WEAPON;
		pP->m_Subtype = WEAPON_HAMMER;

		if(Server()->Tick()%100 == 0)
		{
			CNetEvent_Death *pEvent = (CNetEvent_Death *)GameServer()->m_Events.Create(NETEVENTTYPE_DEATH, sizeof(CNetEvent_Death));

			if(pEvent)
			{
				pEvent->m_X = (int)m_Pos.x;
				pEvent->m_Y = (int)m_Pos.y;
				pEvent->m_ClientID = m_Owner;
			}
		}

	}

	/*
	CNetObj_Pickup *pP[2];

	for(int i = 0; i < 2; i++)
	{
		pP[i] = static_cast<CNetObj_Pickup *>(Server()->SnapNewItem(NETOBJTYPE_PICKUP, m_IDs[i], sizeof(CNetObj_Pickup)));

		if(!pP[i])
			return;

		pP[i]->m_X = (int)m_Pos.x - 25 + 5*i;
		pP[i]->m_Y = (int)m_Pos.y + 5*i;
		pP[i]->m_Type = i;
		pP[i]->m_Subtype = 0;
	
	}

	vec2 Positions[6];
	Positions[0] = vec2(m_Pos.x - 45, m_Pos.y);
	Positions[1] = vec2(m_Pos.x + 45, m_Pos.y);

	
	CCharacter *pOwner = GameServer()->GetPlayerChar(m_Owner);

	if(pOwner)
	{
		if(pOwner->GetPlayer()->m_AccData.m_AllWeapons)
		{

		}
	}
	
	*/
}

//http://hugelol.org/lol/61730
//http://hugelol.org/lol/61768
//http://hugelol.org/lol/61756
//http://hugelol.org/lol/61674
//http://hugelol.org/lol/61400
//http://hugelol.org/lol/61255
//http://hugelol.org/lol/61311
//http://hugelol.org/lol/60946
//http://hugelol.org/lol/60690
//http://hugelol.org/lol/60684
//http://hugelol.org/lol/60497
//http://hugelol.org/lol/60681
//http://hugelol.org/lol/60519
//http://hugelol.org/lol/59592
//http://hugelol.org/lol/59527
//http://hugelol.org/lol/59429
//http://hugelol.org/lol/59112
//http://hugelol.org/lol/59184
//http://hugelol.org/lol/59105
//http://hugelol.org/lol/58769
//http://hugelol.org/lol/58701
//http://hugelol.org/lol/58459
//http://hugelol.org/lol/58230
//http://hugelol.org/lol/57971
//http://hugelol.org/lol/57724
//http://hugelol.org/lol/57788
//http://hugelol.org/lol/57700
//http://hugelol.org/lol/57590
//http://hugelol.org/lol/57461
//http://hugelol.org/lol/57523
//http://hugelol.org/lol/57529
//http://hugelol.org/lol/57481