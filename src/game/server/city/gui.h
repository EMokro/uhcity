/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SERVER_ENTITIES_GUI_H
#define GAME_SERVER_ENTITIES_GUI_H

class CGui : public CEntity
{
public:
	CGui(CGameWorld *pGameWorld, int Owner);
	~CGui();

	virtual void Reset();
	virtual void Tick();
	virtual void Snap(int SnappingClient);

	void Switch();
	void CheckKlick(vec2 Pos);
	void Menu();

	bool m_Visible;
	CEntity *m_apShop[24];

private:
	vec2 m_Pos;
	int m_Owner;
	int m_StartTick;
	

};

#endif
