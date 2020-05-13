/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SERVER_ENTITIES_RIFLE_H
#define GAME_SERVER_ENTITIES_RIFLE_H

class CRifle : public CEntity
{
public:
	CRifle(CGameWorld *pGameWorld, int Owner, vec2 Pos, int Type);

	virtual void Reset();
	virtual void Tick();
	virtual void Snap(int SnappingClient);

	
	

private:
	int m_Type;
	int m_StartTick;
	int m_Owner;
	int m_IDs[30];
	
};

#endif
