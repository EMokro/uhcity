/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <game/generated/protocol.h>
#include <game/server/gamecontext.h>
#include "allweapons.h"

CAllWeapons::CAllWeapons(CGameWorld *pGameWorld, int Owner, vec2 Pos)
: CEntity(pGameWorld, CGameWorld::ENTTYPE_PROJECTILE)
{
	m_Owner = Owner;
	m_Pos = Pos;

	m_Visible = false;

	for(int i = 0; i < 10; i++)
		m_IDs[i] = Server()->SnapNewID();

	GameWorld()->InsertEntity(this);
}

void CAllWeapons::Reset()
{
	GameServer()->m_World.DestroyEntity(this);

	for(int i = 0; i < 10; i++)
		Server()->SnapFreeID(m_IDs[i]);
}

void CAllWeapons::Tick()
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
	
	pOwner->Buy("All weapons", &pOwner->GetPlayer()->m_AccData.m_AllWeapons, g_Config.m_EuAllWeapons, Click, 1);
}


void CAllWeapons::Snap(int SnappingClient)
{
	if(SnappingClient != m_Owner || NetworkClipped(SnappingClient, m_Pos) || !m_Visible)
		return;

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
			pP[i]->m_Type = POWERUP_WEAPON;
			pP[i]->m_Subtype = WEAPON_HAMMER;
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
