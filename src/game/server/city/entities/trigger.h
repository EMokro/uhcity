/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SERVER_CITY_TRIGGER_H
#define GAME_SERVER_CITY_TRIGGER_H

class CTrigger : public CEntity
{
public:
	CTrigger(CGameWorld *pGameWorld, vec2 Pos, int Type);

	virtual void Reset();
	virtual void Tick();
	virtual void Snap(int SnappingClient);
	

private:
	void TriggerNR();
	void TriggerID();

	int m_ItemID[4];
	int m_Type;
	bool m_Switched;
};

#endif
