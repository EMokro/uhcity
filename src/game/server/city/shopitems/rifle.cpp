/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <game/generated/protocol.h>
#include <game/server/gamecontext.h>
#include "rifle.h"

CRifle::CRifle(CGameWorld *pGameWorld, int Owner, vec2 Pos, int Type)
: CEntity(pGameWorld, CGameWorld::ENTTYPE_PROJECTILE)
{
	m_Owner = Owner;
	m_Pos = Pos;
	m_Type = Type;
	m_Visible = false;

	for(int i = 0; i < 10; i++)
		m_IDs[i] = Server()->SnapNewID();

	GameWorld()->InsertEntity(this);
}

void CRifle::Reset()
{
	GameServer()->m_World.DestroyEntity(this);
	for(int i = 0; i < 10; i++)
		Server()->SnapFreeID(m_IDs[i]);
}

void CRifle::Tick()
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
		pOwner->Buy("Rifle spread", &pOwner->GetPlayer()->m_AccData.m_RifleSpread, g_Config.m_EuRifleSpread, Click, 10);
		break;
	case 2:
		pOwner->Buy("Rifle swap", &pOwner->GetPlayer()->m_AccData.m_RifleSwap, g_Config.m_EuRifleSwap, Click, 1);
		break;
	case 3:
		pOwner->Buy("Rifle plasma", &pOwner->GetPlayer()->m_AccData.m_RiflePlasma, g_Config.m_EuRiflePlasma, Click, 10);
		break;
	}
}


void CRifle::Snap(int SnappingClient)
{
	if(SnappingClient != m_Owner || NetworkClipped(SnappingClient, m_Pos) || !m_Visible)
		return;

	if(m_Type == 1)
	{
		CNetObj_Laser *pObj[4];
	
		for(int i = 0; i < 3; i++)
		{
			pObj[i] = static_cast<CNetObj_Laser *>(Server()->SnapNewItem(NETOBJTYPE_LASER, m_IDs[i], sizeof(CNetObj_Laser)));

			if(!pObj[i])
				return;

			pObj[i]->m_X = (int)m_Pos.x - 25 + 25*i;
			pObj[i]->m_Y = (int)m_Pos.y + 32;
			pObj[i]->m_FromX = (int)m_Pos.x - 10 + 10*i;
			pObj[i]->m_FromY = (int)m_Pos.y - 32;
			pObj[i]->m_StartTick = Server()->Tick();

		}
	}
	else if(m_Type == 2)
	{
		vec2 Positions[6];
		
		Positions[0] = vec2(m_Pos.x, m_Pos.y - 32); 
		Positions[2] = vec2(m_Pos.x - 32, m_Pos.y - 16);
		Positions[1] = vec2(m_Pos.x + 32, m_Pos.y - 16);

		Positions[3] = vec2(m_Pos.x, m_Pos.y + 32);
		Positions[4] = vec2(m_Pos.x - 32, m_Pos.y + 16);
		Positions[5] = vec2(m_Pos.x + 32, m_Pos.y + 16);

		CNetObj_Laser *pObj[4];
	
		for(int i = 0; i < 6; i++)
		{
			pObj[i] = static_cast<CNetObj_Laser *>(Server()->SnapNewItem(NETOBJTYPE_LASER, m_IDs[i], sizeof(CNetObj_Laser)));

			if(!pObj[i])
				return;

			pObj[i]->m_X = (int)Positions[i].x;
			pObj[i]->m_Y = (int)Positions[i].y;
			pObj[i]->m_FromX = (int)(i==2 || i == 5)?Positions[i].x:Positions[i+1].x;
			pObj[i]->m_FromY = (int)(i==2 || i == 5)?Positions[i].y:Positions[i+1].y;
			pObj[i]->m_StartTick = Server()->Tick();
		}
	}
	else if(m_Type == 3)
	{
		CNetObj_Laser *pObj[2];
	
		for(int i = 0; i < 2; i++)
		{
			pObj[i] = static_cast<CNetObj_Laser *>(Server()->SnapNewItem(NETOBJTYPE_LASER, m_IDs[i], sizeof(CNetObj_Laser)));

			if(!pObj[i])
				return;

			pObj[i]->m_X = (int)m_Pos.x + Server()->Tick()%20*(i*2-1);
			pObj[i]->m_Y = (int)m_Pos.y;
			pObj[i]->m_FromX = (int)m_Pos.x + Server()->Tick()%20;
			pObj[i]->m_FromY = (int)m_Pos.y;
			pObj[i]->m_StartTick = Server()->Tick();
		}

	}

	CCharacter *pOwner = GameServer()->GetPlayerChar(m_Owner);

	if(pOwner)
	{
		if(pOwner->GetPlayer()->m_AccData.m_AllWeapons)
		{

		}
	}
	
	
}
