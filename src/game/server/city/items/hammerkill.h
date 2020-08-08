/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SERVER_ENTITIES_HAMMERKILL_H
#define GAME_SERVER_ENTITIES_HAMMERKILL_H

class CHammerKill : public CEntity
{
public:
	CHammerKill(CGameWorld *pGameWorld, int Owner, int Victim);

	virtual void Reset();
	virtual void Tick();

private:
	int m_Owner;
	int m_Victim;
	int m_VictimTick;
};

#endif
