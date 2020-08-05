/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SERVER_ENTITIES_HEALSTATE_H
#define GAME_SERVER_ENTITIES_HEALSTATE_H

#include <game/server/entity.h>


class CHealstate : public CEntity
{
public:
	CHealstate(CGameWorld *pGameWorld, int Owner);

	virtual void Reset();
	virtual void Tick();
	virtual void Snap(int SnappingClient);

private:
	int m_IDs[5];
	int m_Owner;
    vec2 m_LastPos[5];
};

#endif
