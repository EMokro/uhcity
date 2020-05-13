/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <game/generated/protocol.h>
#include <game/server/gamecontext.h>
#include "healthregen.h"

CHealthRegen::CHealthRegen(CGameWorld *pGameWorld, int Owner, vec2 Pos)
: CEntity(pGameWorld, CGameWorld::ENTTYPE_PROJECTILE)
{
	m_Owner = Owner;
	m_Pos = Pos;

	m_Visible = false;

	for(int i = 0; i < 10; i++)
		m_IDs[i] = Server()->SnapNewID();

	GameWorld()->InsertEntity(this);
}

void CHealthRegen::Reset()
{
	GameServer()->m_World.DestroyEntity(this);

	for(int i = 0; i < 10; i++)
		Server()->SnapFreeID(m_IDs[i]);
}

void CHealthRegen::Tick()
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
	
	pOwner->Buy("Health regeneration", &pOwner->GetPlayer()->m_AccData.m_HealthRegen, g_Config.m_EuHealthRegen, Click, 25);
}


void CHealthRegen::Snap(int SnappingClient)
{
	if(SnappingClient != m_Owner || NetworkClipped(SnappingClient, m_Pos) || !m_Visible)
		return;

	CNetObj_Pickup *pP[2];

	for(int i = 0; i < 2; i++)
	{
		pP[i] = static_cast<CNetObj_Pickup *>(Server()->SnapNewItem(NETOBJTYPE_PICKUP, m_IDs[i], sizeof(CNetObj_Pickup)));

		if(!pP[i])
			return;

		pP[i]->m_X = (int)m_Pos.x - 32 + 5*i;
		pP[i]->m_Y = (int)m_Pos.y + 5*i;
		pP[i]->m_Type = i;
		pP[i]->m_Subtype = 0;
	
	}

	CNetObj_Laser *pObj[4];
	
	for(int i = 0; i < 4; i++)
	{
		pObj[i] = static_cast<CNetObj_Laser *>(Server()->SnapNewItem(NETOBJTYPE_LASER, m_IDs[i+2], sizeof(CNetObj_Laser)));

		if(!pObj[i])
			return;

		pObj[i]->m_X = (int)m_Pos.x + 16 *((i+1)%2?0:(i==3?1:-1));
		pObj[i]->m_Y = (int)m_Pos.y + 16 * (i%2?0:(!i?-1:1));
		pObj[i]->m_FromX = (int)m_Pos.x;
		pObj[i]->m_FromY = (int)m_Pos.y;
		pObj[i]->m_StartTick = Server()->Tick();

	}

	CCharacter *pOwner = GameServer()->GetPlayerChar(m_Owner);

	if(pOwner)
	{
		if(pOwner->GetPlayer()->m_AccData.m_AllWeapons)
		{

		}
	}
	
	
}
