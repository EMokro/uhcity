/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SERVER_ENTITIES_GRAVAURA_H
#define GAME_SERVER_ENTITIES_GRAVAURA_H

#include <game/server/entity.h>


class CGravAura : public CEntity
{
public:
	CGravAura(CGameWorld *pGameWorld, int Owner, int Mode = 1);

	virtual void Reset();
	virtual void Tick();
	virtual void Snap(int SnappingClient);

    int m_Mode;

private:

    enum {
        MAX_PROJECTILS = 16
    };

	int m_IDs[MAX_PROJECTILS];
	int m_Owner;
    int m_LifeTime;

    // anim
    int m_SpawnTimer;
    int m_StartPos[MAX_PROJECTILS];
    int m_StartRadius;

};

#endif
