/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SERVER_ENTITIES_PUSHAURA_H
#define GAME_SERVER_ENTITIES_PUSHAURA_H

#include <game/server/entity.h>

class CPushAura : public CEntity
{
public:
	CPushAura(CGameWorld *pGameWorld, int Owner, vec2 Pos);

	virtual void Reset();
	virtual void Tick();
	virtual void Snap(int SnappingClient);


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
