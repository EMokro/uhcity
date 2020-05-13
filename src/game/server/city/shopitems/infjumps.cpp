/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <game/generated/protocol.h>
#include <game/server/gamecontext.h>
#include "infjumps.h"

CInfJumps::CInfJumps(CGameWorld *pGameWorld, int Owner, vec2 Pos)
: CEntity(pGameWorld, CGameWorld::ENTTYPE_PROJECTILE)
{
	m_Owner = Owner;
	m_Pos = Pos;

	m_Visible = false;

	for(int i = 0; i < 10; i++)
		m_IDs[i] = Server()->SnapNewID();

	GameWorld()->InsertEntity(this);
}

void CInfJumps::Reset()
{
	GameServer()->m_World.DestroyEntity(this);

	for(int i = 0; i < 10; i++)
		Server()->SnapFreeID(m_IDs[i]);
}

void CInfJumps::Tick()
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
	
	pOwner->Buy("Infinity jumps", &pOwner->GetPlayer()->m_AccData.m_InfinityJumps, g_Config.m_EuInfJumps, Click, 2);
}

void CInfJumps::Snap(int SnappingClient)
{
	if(SnappingClient != m_Owner || NetworkClipped(SnappingClient, m_Pos) || !m_Visible)
		return;
	vec2 Positions[6];

	Positions[0] = vec2(m_Pos.x - 16, m_Pos.y);
	Positions[1] = Positions[1+3] = vec2(m_Pos.x, m_Pos.y - 32);
	Positions[2] = Positions[2+3] = vec2(m_Pos.x, m_Pos.y + 32);

	Positions[3] = vec2(m_Pos.x + 16, m_Pos.y);
	CCharacter *pOwner = GameServer()->GetPlayerChar(m_Owner);

	if(pOwner)
	{
		for(int i = 0; i < 6; i++)
		{
			if(!pOwner->GetPlayer()->m_AccData.m_InfinityJumps)
				break;

			Positions[i].x -= 8 * ((i<3) * 2 - 1);
		}
	}
	
	CNetObj_Laser *pObj[4];
	
	for(int i = 0; i < 6; i++)
	{
		pObj[i] = static_cast<CNetObj_Laser *>(Server()->SnapNewItem(NETOBJTYPE_LASER, m_IDs[i], sizeof(CNetObj_Laser)));

		if(!pObj[i])
			return;

		pObj[i]->m_X = (int)i<3?Positions[1].x:Positions[4].x;
		pObj[i]->m_Y = (int)i<3?Positions[1].y:Positions[4].y;
		pObj[i]->m_FromX = (int)Positions[i].x;
		pObj[i]->m_FromY = (int)Positions[i].y;
		pObj[i]->m_StartTick = Server()->Tick();

	}

}
