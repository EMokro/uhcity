/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SERVER_ENTITIES_INFJUMPS_H
#define GAME_SERVER_ENTITIES_INFJUMPS_H

class CInfJumps : public CEntity
{
public:
	CInfJumps(CGameWorld *pGameWorld, int Owner, vec2 Pos);

	virtual void Reset();
	virtual void Tick();
	virtual void Snap(int SnappingClient);


private:
	int m_Owner;
	int m_IDs[30];
	
};

#endif
