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

	if(!pOwner || !pOwner->IsAlive())
	{
		Reset();
		return;
	}

	if(!m_Visible)
		return;

	int Click = pOwner->MouseEvent(m_Pos);

	if(!Click)
	{
		m_LastClick = 0;
		return;
	}

	const char* pLanguage = pOwner->GetPlayer()->GetLanguage();
	dynamic_string Buffer;
	
	switch(m_Type)
	{
	case 1:
		Server()->Localization()->Format_L(Buffer, pLanguage, _("Hammer walls"));
		pOwner->Buy(Buffer.buffer(), &pOwner->GetPlayer()->m_AccData.m_HammerWalls, g_Config.m_EuHammerWalls, Click, 5);
		break;
	case 2:
		Server()->Localization()->Format_L(Buffer, pLanguage, _("Hammer shot"));
		pOwner->Buy(Buffer.buffer(), &pOwner->GetPlayer()->m_AccData.m_HammerShot, g_Config.m_EuHammerShot, Click, 3);
		break;
	case 3:
		Server()->Localization()->Format_L(Buffer, pLanguage, _("Hammer kill"));
		pOwner->Buy(Buffer.buffer(), &pOwner->GetPlayer()->m_AccData.m_HammerKill, g_Config.m_EuHammerKill, Click, 1);
		break;
	case 4:
		Server()->Localization()->Format_L(Buffer, pLanguage, _("Portal"));
		pOwner->Buy(Buffer.buffer(), &pOwner->GetPlayer()->m_AccData.m_Portal, g_Config.m_EuPortal, Click, 1);
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
	else if(m_Type == 4)// portal
	{
		/*CNetEvent_Spawn* pEvent = (CNetEvent_Spawn*)GameServer()->m_Events.Create(NETEVENTTYPE_SPAWN, sizeof(CNetEvent_Spawn));

		if(!pEvent)
			return;

		pEvent->m_X = (int)m_Pos.x;
		pEvent->m_Y = (int)m_Pos.y;*/
		if(Server()->Tick()%50 == 0)
			GameServer()->CreatePlayerSpawn(m_Pos);
	}
}