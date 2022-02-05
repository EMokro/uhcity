/* * * * * * * * * * * * * */
/* Copyright � 2014 Neox.  */
/* * * * * * * * * * * * * */

#include <engine/shared/config.h>
#include <game/generated/protocol.h>
#include <game/server/gamecontext.h>
#include "game/server/gamecontroller.h"
#include "monsterdeath.h"

CMonsterDeath::CMonsterDeath(CGameWorld *pGameWorld, vec2 Pos, vec2 Vel, int WeaponNum, int Killer)
: CEntity(pGameWorld, CGameWorld::ENTTYPE_PICKUP)
{
	m_Pos = Pos;
	m_Vel = Vel;
	m_WeaponNum = WeaponNum;
	m_Killer = Killer;
	m_ProximityRadius = 28;

	GameWorld()->InsertEntity(this);
}

void CMonsterDeath::Reset()
{
    GameServer()->CreateDeath(m_Pos, m_Killer);
	GameWorld()->DestroyEntity(this);
}

void CMonsterDeath::Tick()
{
    float Gravity = GameServer()->m_World.m_Core.m_Tuning.m_Gravity;
    m_Vel.y += Gravity;

    if(m_GroundedTicks <= Server()->TickSpeed())
        GameServer()->Collision()->MoveBox(&m_Pos, &m_Vel, vec2(m_ProximityRadius, m_ProximityRadius), 0.5f);
    else
        m_Vel = vec2(0, 0);

    if(IsGrounded())
    {
        if(m_Vel.x >= 1)
            m_Vel.x --;
        else if(m_Vel.x <= -1)
            m_Vel.x ++;
    }
    if((IsGrounded() && (int)m_Vel.x == 0 && (int)m_Vel.y == 0))
        m_GroundedTicks ++;
    if(OnDeathTile() || m_GroundedTicks >= Server()->TickSpeed() * 5)
        Reset();
}

bool CMonsterDeath::IsGrounded()
{
	if(GameServer()->Collision()->CheckPoint(m_Pos.x+m_ProximityRadius/2, m_Pos.y+m_ProximityRadius/2+5))
		return true;
	if(GameServer()->Collision()->CheckPoint(m_Pos.x-m_ProximityRadius/2, m_Pos.y+m_ProximityRadius/2+5))
		return true;
	return false;
}

bool CMonsterDeath::OnDeathTile()
{
    if(GameServer()->Collision()->GetCollisionAt(m_Pos.x+m_ProximityRadius/3.f, m_Pos.y-m_ProximityRadius/3.f)&CCollision::COLFLAG_DEATH ||
		GameServer()->Collision()->GetCollisionAt(m_Pos.x+m_ProximityRadius/3.f, m_Pos.y+m_ProximityRadius/3.f)&CCollision::COLFLAG_DEATH ||
		GameServer()->Collision()->GetCollisionAt(m_Pos.x-m_ProximityRadius/3.f, m_Pos.y-m_ProximityRadius/3.f)&CCollision::COLFLAG_DEATH ||
		GameServer()->Collision()->GetCollisionAt(m_Pos.x-m_ProximityRadius/3.f, m_Pos.y+m_ProximityRadius/3.f)&CCollision::COLFLAG_DEATH ||
		GameLayerClipped(m_Pos))
	{
		return true;
	}

	return false;
}

void CMonsterDeath::Snap(int SnappingClient)
{
    if(m_GroundedTicks >= Server()->TickSpeed())
    {
        if(m_GroundedTicks <= Server()->TickSpeed() * 2)
        {
            if(15 - m_GroundedTicks % 15 < 5)
                return;
        }
        else if(m_GroundedTicks <= Server()->TickSpeed() * 2 + Server()->TickSpeed() / 2)
        {
            if(12 - m_GroundedTicks % 12 < 5)
                return;
        }
        else if(m_GroundedTicks <= Server()->TickSpeed() * 3 + Server()->TickSpeed() / 2)
        {
            if(9 - m_GroundedTicks % 9 < 5)
                return;
        }
        else if(m_GroundedTicks <= Server()->TickSpeed() * 5)
        {
            if(Server()->Tick() % 3 == 0)
                return;
        }
    }
	CNetObj_Pickup *pObj;

    pObj = static_cast<CNetObj_Pickup *>(Server()->SnapNewItem(NETOBJTYPE_PICKUP, m_ID, sizeof(CNetObj_Pickup)));

    if(!pObj)
        return;

    pObj->m_X = (int)m_Pos.x;
    pObj->m_Y = (int)m_Pos.y;
    pObj->m_Type = POWERUP_WEAPON;
    pObj->m_Subtype = m_WeaponNum;
}
