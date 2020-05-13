/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SERVER_ENTITIES_WALL_H
#define GAME_SERVER_ENTITIES_WALL_H

#include <game/server/entity.h>


class CWall : public CEntity
{
public:
	CWall(CGameWorld *pGameWorld, vec2 Pos, vec2 Pos2, int Owner);

	virtual void Reset();
	virtual void Tick();
	virtual void Snap(int SnappingClient);

private:
	int m_ID2;
	int m_Owner;
	vec2 m_Pos;
	vec2 m_Pos2;
	int m_Counter;
	int m_Lifespan;
};

#endif
