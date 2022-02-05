/* * * * * * * * * * * * * */
/* Copyright � 2014 Neox.  */
/* * * * * * * * * * * * * */

#ifndef GAME_SERVER_ENTITIES_MONSTERDEATH_H
#define GAME_SERVER_ENTITIES_MONSTERDEATH_H

#include <game/server/entity.h>

class CMonsterDeath : public CEntity
{
public:
	CMonsterDeath(CGameWorld *pGameWorld, vec2 Pos, vec2 Vel, int WeaponNum, int Killer);

	virtual void Reset();
	virtual void Tick();
	virtual void Snap(int SnappingClient);
	bool IsGrounded();
	bool OnDeathTile();

private:
    vec2 m_Pos;
    vec2 m_Vel;
    int m_WeaponNum;
    int m_Killer;
    int m_GroundedTicks;
};

#endif
