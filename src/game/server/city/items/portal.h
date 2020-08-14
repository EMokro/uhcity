/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SERVER_ENTITIES_PORTAL_H
#define GAME_SERVER_ENTITIES_PORTAL_H

#include <game/server/entity.h>


class CPortal : public CEntity
{
public:
	CPortal(CGameWorld *pGameWorld, int Owner, vec2 Pos1, vec2 Pos2);

	virtual void Reset();
	virtual void Tick();
	virtual void Snap(int SnappingClient);

    vec2 m_Pos1;
    vec2 m_Pos2;
	bool m_Active;

private:
	int m_IDs[2];
	int m_Owner;
    int m_LifeTime;
    int m_Timer;
    
};

#endif
