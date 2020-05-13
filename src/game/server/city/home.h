/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SERVER_CITY_HOME_H
#define GAME_SERVER_CITY_HOME_H

class CHome : public CEntity
{
public:
	CHome(CGameWorld *pGameWorld, vec2 Pos);

	virtual void Reset();
	virtual void Tick();
	virtual void Snap(int SnappingClient);

	void SetItemID();

private:
	int m_ItemID[4];
	
};

#endif
