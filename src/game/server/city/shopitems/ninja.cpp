/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <game/generated/protocol.h>
#include <game/server/gamecontext.h>
#include "ninja.h"

CNinja::CNinja(CGameWorld *pGameWorld, int Owner, vec2 Pos, int Type)
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

void CNinja::Reset()
{
	GameServer()->m_World.DestroyEntity(this);

	for(int i = 0; i < 10; i++)
		Server()->SnapFreeID(m_IDs[i]);
}

void CNinja::Tick()
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
		pOwner->Buy("Ninja permanent", &pOwner->GetPlayer()->m_AccData.m_NinjaPermanent, g_Config.m_EuNinjaPermanent, Click, 1);
		break;
	case 2:
		pOwner->Buy("Ninja at start", &pOwner->GetPlayer()->m_AccData.m_NinjaStart, g_Config.m_EuNinjaStart, Click, 1);
		break;
	case 3:
		pOwner->Buy("Ninja switch", &pOwner->GetPlayer()->m_AccData.m_NinjaSwitch, g_Config.m_EuNinjaSwitch, Click, 1);
		break;
	}
}


void CNinja::Snap(int SnappingClient)
{
	if(SnappingClient != m_Owner || NetworkClipped(SnappingClient, m_Pos) || !m_Visible)
		return;
	if(Server()->Tick()%5 == 0)
		m_StartTick = Server()->Tick();

	vec2 Positions[3];
	Positions[0] = vec2(m_Pos.x - 32, m_Pos.y - 16);
	Positions[1] = vec2(m_Pos.x + 32, m_Pos.y - 16);
	Positions[2] = vec2(m_Pos.x, m_Pos.y + 16);

	if(m_Type == 1)//Permanent
	{
		CNetObj_Pickup *pP = static_cast<CNetObj_Pickup *>(Server()->SnapNewItem(NETOBJTYPE_PICKUP, m_ID, sizeof(CNetObj_Pickup)));

		if(!pP)
			return;

		pP->m_X = (int)m_Pos.x;
		pP->m_Y = (int)m_Pos.y;
		pP->m_Type = POWERUP_NINJA;
		pP->m_Subtype = WEAPON_NINJA;
			
	}
	else if(m_Type == 2)// Start
	{
		CNetObj_Pickup *pP = static_cast<CNetObj_Pickup *>(Server()->SnapNewItem(NETOBJTYPE_PICKUP, m_ID, sizeof(CNetObj_Pickup)));

		if(!pP)
			return;

		pP->m_X = (int)m_Pos.x;
		pP->m_Y = (int)m_Pos.y;
		pP->m_Type = POWERUP_WEAPON;
		pP->m_Subtype = WEAPON_NINJA;
			
		if(Server()->Tick()%100 == 0)
		{
			CNetEvent_Spawn *ev = (CNetEvent_Spawn *)GameServer()->m_Events.Create(NETEVENTTYPE_SPAWN, sizeof(CNetEvent_Spawn));
			if(ev)
			{
				ev->m_X = (int)m_Pos.x;
				ev->m_Y = (int)m_Pos.y;
			}
		}
		
	}
	else if(m_Type == 3)// Switch
	{
		CNetObj_Pickup *pP[6];

		for(int i = 0; i < 5; i++)
		{
			pP[i] = static_cast<CNetObj_Pickup *>(Server()->SnapNewItem(NETOBJTYPE_PICKUP, m_IDs[i], sizeof(CNetObj_Pickup)));

			if(!pP[i])
				return;

			if(i != 4)
			{
				pP[i]->m_X = (int)m_Pos.x;
				pP[i]->m_Y = (int)m_Pos.y + 10 + 20 - i * 20;
				pP[i]->m_Type = POWERUP_WEAPON;
				pP[i]->m_Subtype = i+1;
			}
			else
			{
				pP[i]->m_X = (int)m_Pos.x;
				pP[i]->m_Y = (int)m_Pos.y;
				pP[i]->m_Type = POWERUP_NINJA;
				pP[i]->m_Subtype = 0;
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