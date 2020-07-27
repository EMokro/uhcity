/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SERVER_ENTITIES_GUI_H
#define GAME_SERVER_ENTITIES_GUI_H

#include <list>

class CGui : public CEntity
{
public:
	CGui(CGameWorld *pGameWorld, int Owner);
	~CGui();

	virtual void Reset();
	virtual void Tick();
	virtual void Snap(int SnappingClient);

	void Switch();
	void CheckClick(vec2 Pos);
	void Menu();

	bool m_Visible;
	

private:
	vec2 m_Pos;
	int m_Owner;
	int m_StartTick;
	bool m_ItrFlag;
	bool m_MsgFlag;
	int m_Group;
	int m_Page;
	vec2 m_LastPlayerPos;

	// max page size is 6
	// groups are ITEM_** (ITEM_HAMMER)
	std::list<std::list<CEntity*>> m_aShop; // 2d list containing a list of item groups
};

#endif
