/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SERVER_ENTITIES_CROWN_H
#define GAME_SERVER_ENTITIES_CROWN_H

#include <game/server/entity.h>


class CCrown : public CEntity
{
public:
	CCrown(CGameWorld *pGameWorld, int Owner);

	virtual void Reset();
	virtual void Tick();
	virtual void Snap(int SnappingClient);

private:
	int m_IDs[4];
	int m_Owner;
};

#endif
