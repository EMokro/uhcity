/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SERVER_CITY_DOOR_H
#define GAME_SERVER_CITY_DOOR_H

class CDoor : public CEntity
{
public:
	CDoor(CGameWorld *pGameWorld, vec2 Pos, int Type);

	virtual void Reset();
	virtual void Tick();
	virtual void Snap(int SnappingClient);

private:
	void DoorID();
	void DoorNR();
	bool m_Open;
	int m_aLength[4];
	int m_ItemID[4];
	int m_Type;
	int m_IDs[5];
};

#endif
