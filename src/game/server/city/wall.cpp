/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <game/generated/protocol.h>
#include <game/server/gamecontext.h>
#include "wall.h"

CWall::CWall(CGameWorld *pGameWorld, vec2 Pos, vec2 Pos2, int Owner)
: CEntity(pGameWorld, CGameWorld::ENTTYPE_LASER)
{
	m_Pos = Pos;
	m_Pos2 = Pos2;
	m_Owner = Owner;
	m_ID2 = Server()->SnapNewID();
	m_Lifespan = (float)(20*50);
	GameWorld()->InsertEntity(this);
}

void CWall::Reset()
{
	CCharacter *pOwner = GameServer()->GetPlayerChar(m_Owner);

	if(pOwner)
		pOwner->m_Walls--;

	GameServer()->m_World.DestroyEntity(this);
	Server()->SnapFreeID(m_ID2);
}

void CWall::Tick()
{
	CCharacter *pOwner = GameServer()->GetPlayerChar(m_Owner);

	m_Lifespan--;

	if(!pOwner || m_Lifespan<=0)
	{
		Reset();
		return;
	}	

	if(m_Counter >= pOwner->GetPlayer()->m_AccData.m_HammerWalls)
	{
		Reset();
		return;
	}	

	vec2 Intersection;
	CCharacter *pTargetChr = GameServer()->m_World.IntersectCharacter(m_Pos, m_Pos2, 1.0f, Intersection, 0x0);

	if(pTargetChr)
	{
		if(pTargetChr->Protected() || (pTargetChr == pOwner && pTargetChr->ActiveWeapon() == WEAPON_GUN))
		{
			Reset();
			return;
		}

		if(pTargetChr != pOwner && !pTargetChr->Protected() && !pTargetChr->m_God)
		{
			pTargetChr->Die(m_Owner, WEAPON_HAMMER);
			m_Counter++;
		}
	}

}

void CWall::Snap(int SnappingClient)
{
	CNetObj_Laser *pObj = static_cast<CNetObj_Laser *>(Server()->SnapNewItem(NETOBJTYPE_LASER, m_ID, sizeof(CNetObj_Laser)));
	if(!pObj)
		return;

	pObj->m_X = (int)m_Pos2.x;
	pObj->m_Y = (int)m_Pos2.y;
	pObj->m_FromX = (int)m_Pos.x;
	pObj->m_FromY = (int)m_Pos.y;
	pObj->m_StartTick = Server()->Tick();

	
	CNetObj_Laser *pObj2 = static_cast<CNetObj_Laser *>(Server()->SnapNewItem(NETOBJTYPE_LASER, m_ID2, sizeof(CNetObj_Laser)));
	if(!pObj2)
		return;

	pObj2->m_X = (int)m_Pos.x;
	pObj2->m_Y = (int)m_Pos.y;
	pObj2->m_FromX = (int)m_Pos.x;
	pObj2->m_FromY = (int)m_Pos.y;
	pObj2->m_StartTick = Server()->Tick();
	
}
