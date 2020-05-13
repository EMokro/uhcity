/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <game/generated/protocol.h>
#include <game/server/gamecontext.h>
#include "fastreload.h"

CFastReload::CFastReload(CGameWorld *pGameWorld, int Owner, vec2 Pos)
: CEntity(pGameWorld, CGameWorld::ENTTYPE_PROJECTILE)
{
	m_Owner = Owner;
	m_Pos = Pos;

	m_Visible = false;

	for(int i = 0; i < 3; i++)
		m_IDs[i] = Server()->SnapNewID();

	GameWorld()->InsertEntity(this);
}

void CFastReload::Reset()
{
	GameServer()->m_World.DestroyEntity(this);

	for(int i = 0; i < 3; i++)
		Server()->SnapFreeID(m_IDs[i]);
}

void CFastReload::Tick()
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
	
	pOwner->Buy("Fast reload", &pOwner->GetPlayer()->m_AccData.m_FastReload, g_Config.m_EuFastReload, Click, 10);
}


void CFastReload::Snap(int SnappingClient)
{
	if(SnappingClient != m_Owner || NetworkClipped(SnappingClient, m_Pos) || !m_Visible)
		return;

	CNetObj_Pickup *pP = static_cast<CNetObj_Pickup *>(Server()->SnapNewItem(NETOBJTYPE_PICKUP, m_ID, sizeof(CNetObj_Pickup)));

	if(!pP)
		return;

	pP->m_X = (int)m_Pos.x - 20;
	pP->m_Y = (int)m_Pos.y;
	pP->m_Type = POWERUP_WEAPON;
	pP->m_Subtype = WEAPON_GUN;
	
	CNetObj_Projectile *pProj[3];

	for(int i = 0; i < 3; i++)
	{
		pProj[i] = static_cast<CNetObj_Projectile *>(Server()->SnapNewItem(NETOBJTYPE_PROJECTILE, m_IDs[i], sizeof(CNetObj_Projectile)));

		if(!pProj[i])
			return;

		pProj[i]->m_X = (int)m_Pos.x + i*15;
		pProj[i]->m_Y = (int)m_Pos.y;
		pProj[i]->m_VelX = (int)10;
		pProj[i]->m_VelY = (int)-2;
		pProj[i]->m_StartTick = Server()->Tick()-2;
		pProj[i]->m_Type = WEAPON_GUN;

	}

	CCharacter *pOwner = GameServer()->GetPlayerChar(m_Owner);

	if(pOwner)
	{
		if(pOwner->GetPlayer()->m_AccData.m_AllWeapons)
		{

		}
	}
	
	
}
