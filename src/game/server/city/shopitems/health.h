/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SERVER_ENTITIES_HEALTH_H
#define GAME_SERVER_ENTITIES_HEALTH_H

class CBuyHealth : public CEntity
{
public:
	CBuyHealth(CGameWorld *pGameWorld, vec2 Pos);

	virtual void Reset();
	virtual void Tick();
	virtual void Snap(int SnappingClient);

private:
	vec2 m_Pos;
	int m_IDs[2];
	
	
};

#endif
