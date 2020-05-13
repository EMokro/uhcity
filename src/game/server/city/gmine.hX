/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SERVER_ENTITIES_MINEZ_H
#define GAME_SERVER_ENTITIES_MINEZ_H

#include <game/server/entity.h>
#include "character.h"

class CMine : public CEntity
{
public:
	CMine(CGameWorld *pGameWorld, vec2 Pos, int Owner, int Damage;
	
	virtual void Reset();
	virtual void Tick();
	virtual void Snap(int SnappingClient);

	virtual void HitCharacter();
	
	virtual void Collide();
private:
	
	vec2 m_Vel;
	int m_Owner;
	int m_Damage;
	
	bool m_GoUp;
};

#endif