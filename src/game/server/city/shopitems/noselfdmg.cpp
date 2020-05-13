/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <game/generated/protocol.h>
#include <game/server/gamecontext.h>
#include "noselfdmg.h"

CNoSelfDMG::CNoSelfDMG(CGameWorld *pGameWorld, int Owner, vec2 Pos)
: CEntity(pGameWorld, CGameWorld::ENTTYPE_PROJECTILE)
{
	m_Owner = Owner;
	m_Pos = Pos;

	m_Visible = false;

	for(int i = 0; i < 10; i++)
		m_IDs[i] = Server()->SnapNewID();

	GameWorld()->InsertEntity(this);
}

void CNoSelfDMG::Reset()
{
	GameServer()->m_World.DestroyEntity(this);

	for(int i = 0; i < 10; i++)
		Server()->SnapFreeID(m_IDs[i]);
}

void CNoSelfDMG::Tick()
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
	
	pOwner->Buy("No self damage", &pOwner->GetPlayer()->m_AccData.m_NoSelfDMG, g_Config.m_EuNoSelfDMG, Click, 1);
		
}


void CNoSelfDMG::Snap(int SnappingClient)
{
	if(SnappingClient != m_Owner || NetworkClipped(SnappingClient, m_Pos) || !m_Visible)
		return;

	CNetObj_Projectile *pProj = static_cast<CNetObj_Projectile *>(Server()->SnapNewItem(NETOBJTYPE_PROJECTILE, m_ID, sizeof(CNetObj_Projectile)));
	if(!pProj)
		return;

	if(Server()->Tick()%5 == 0)
			m_StartTick = Server()->Tick();

	pProj->m_X = (int)m_Pos.x + 25;
	pProj->m_Y = (int)m_Pos.y;
	pProj->m_VelX = (int)-2;
	pProj->m_VelY = (int)-10;
	pProj->m_StartTick = m_StartTick;
	pProj->m_Type = WEAPON_GRENADE;

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

	CNetObj_Laser *pObj[4];
	
	for(int i = 0; i < 2; i++)
	{
		pObj[i] = static_cast<CNetObj_Laser *>(Server()->SnapNewItem(NETOBJTYPE_LASER, m_IDs[i+2], sizeof(CNetObj_Laser)));

		if(!pObj[i])
			return;

		pObj[i]->m_X = (int)(i?Positions[1].x:Positions[0].x);
		pObj[i]->m_Y = (int)(i?Positions[1].y:Positions[0].y);
		pObj[i]->m_FromX = (int)Positions[1].x;
		pObj[i]->m_FromY = (int)Positions[1].y;
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
