/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SERVER_ENTITIES_PLASMA_H
#define GAME_SERVER_ENTITIES_PLASMA_H

#include <game/server/entity.h>
#include <game/server/entities/projectile.h>
//#include "path.h"

class CPlasma : public CEntity
{
public:
	CPlasma(CGameWorld *pGameWorld, int Type, vec2 Pos, int Owner, int Victim);
	CPlasma(CGameWorld *pGameWorld, int Type, vec2 Pos, int Owner, vec2 Dir);

	virtual void Reset();
	virtual void Tick();
	virtual void Snap(int SnappingClient);

	int m_Owner;
	int m_Victim;
	vec2 m_Pos;
	vec2 m_Pos2;

	bool NextPos(vec2 *NextPos);
	void GetPath();

private:
	void RiflePlasma();
	void HammerPlasma();
	int m_ID2;
	float m_Angle;
	int m_Type;
	vec2 NextPosition;
	int m_LifeSpan;

	vec2 TempPos[100];
	vec2 m_NodePos[100];
	vec2 m_LastPos;
	int m_Nodes;
};

#endif
