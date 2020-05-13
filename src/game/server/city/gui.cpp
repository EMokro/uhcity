/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <game/generated/protocol.h>
#include <game/server/gamecontext.h>

#include <game/server/city/shopitems/shotgun.h>
#include <game/server/city/shopitems/grenade.h>
#include <game/server/city/shopitems/rifle.h>
#include <game/server/city/shopitems/gun.h>
#include <game/server/city/shopitems/hammer.h>
#include <game/server/city/shopitems/ninja.h>

#include <game/server/city/shopitems/healthregen.h>
#include <game/server/city/shopitems/infjumps.h>
#include <game/server/city/shopitems/infammo.h>
#include <game/server/city/shopitems/allweapons.h>
#include <game/server/city/shopitems/fastreload.h>
#include <game/server/city/shopitems/noselfdmg.h>

#include "gui.h"


CGui::CGui(CGameWorld *pGameWorld, int Owner)
: CEntity(pGameWorld, CGameWorld::ENTTYPE_PROJECTILE)
{
	m_Owner = Owner;
	m_Visible = false;
	m_Pos = vec2(0, 0);

	// ITEMS
	m_apShop[0] = new CHammer(GameWorld(), m_Owner, m_Pos, 1);
	m_apShop[1] = new CHammer(GameWorld(), m_Owner, m_Pos, 2);
	m_apShop[2] = new CHammer(GameWorld(), m_Owner, m_Pos, 3);
		
	m_apShop[3] = new CGun(GameWorld(), m_Owner, m_Pos, 1);
	m_apShop[4] = new CGun(GameWorld(), m_Owner, m_Pos, 2);
	m_apShop[5] = new CGun(GameWorld(), m_Owner, m_Pos, 3);

	m_apShop[6] = new CShotgun(pGameWorld, m_Owner, m_Pos, 1);
	m_apShop[7] = new CShotgun(pGameWorld, m_Owner, m_Pos, 2);
//  m_apShop[8] = new CShotgun(pGameWorld, m_Owner, m_Pos, 3);

	m_apShop[9] = new CGrenade(pGameWorld, m_Owner, m_Pos, 1);
	m_apShop[10] = new CGrenade(pGameWorld, m_Owner, m_Pos, 2);
//	m_apShop[11] = new CGrenade(pGameWorld, m_Owner, m_Pos, 3);
	
	m_apShop[12] = new CRifle(GameWorld(), m_Owner, m_Pos, 3);
	m_apShop[13] = new CRifle(GameWorld(), m_Owner, m_Pos, 2);
	m_apShop[14] = new CRifle(GameWorld(), m_Owner, m_Pos, 1);

	m_apShop[15] = new CNinja(GameWorld(), m_Owner, m_Pos, 1);
	m_apShop[16] = new CNinja(GameWorld(), m_Owner, m_Pos, 2);
	m_apShop[17] = new CNinja(GameWorld(), m_Owner, m_Pos, 3);

	m_apShop[18] = new CInfAmmo(pGameWorld, m_Owner, m_Pos);
	m_apShop[19] = new CAllWeapons(pGameWorld, m_Owner, m_Pos);
	m_apShop[20] = new CFastReload(pGameWorld, m_Owner, m_Pos);
	m_apShop[21] = new CHealthRegen(pGameWorld, m_Owner, m_Pos);
	m_apShop[22] = new CNoSelfDMG(pGameWorld, m_Owner, m_Pos);
	m_apShop[23] = new CInfJumps(pGameWorld, m_Owner, m_Pos);
	
	//m_Menu = 1;
	for(int i = 0; i < 24; i++)
	{
		if(i == 11 || i == 8)
			continue;

		m_apShop[i]->m_Visible = true;
	}

	GameWorld()->InsertEntity(this);
}

CGui::~CGui()
{
	for(int i = 0; i < 24; i++)
		delete m_apShop[i];
	//m_apShop = 0;

}
void CGui::Switch()
{
	for(int i = 0; i < 24; i++)
	{
		if(i == 11 || i == 8)
			continue;

		m_apShop[i]->m_Visible ^= true;
	}
}

void CGui::Reset()
{
	GameServer()->m_World.DestroyEntity(this);
	/*delete m_apShop;
	m_apShop[] = 0;*/
}

void CGui::Tick()
{
	CCharacter *pOwner = GameServer()->GetPlayerChar(m_Owner);

	if(!pOwner)
	{
		Reset();
		return;
	}

	if(GameServer()->Collision()->TileShop(pOwner->m_Pos) && !pOwner->m_Menu)
		pOwner->m_Menu = 1;
	else if(!GameServer()->Collision()->TileShop(pOwner->m_Pos) && pOwner->m_Menu)
		pOwner->m_Menu = 0;

}

void CGui::CheckKlick(vec2 Pos)
{
	for(int i = 0; i < 24; i++)
	{
		if(length(m_apShop[i]->m_Pos-Pos) < 96)
			m_apShop[i]->m_Visible ^= true;
	}


}
void CGui::Menu()
{
	CCharacter *pOwner = GameServer()->GetPlayerChar(m_Owner);

	if(!pOwner)
		return;
	
	int Menu = pOwner->m_Menu;

	for(int i = 0; i < 24; i++)
	{
		if(i == 11 || i == 8)
			continue;

		m_apShop[i]->m_Visible = false;
	}

	int Case = 0;
	int Start, End;

	if(Menu < 7 && Menu)
	{
		Start = (Menu-1)*3;
		End = 3*Menu;

		Case = 1;

		if(Menu == 3 || Menu == 4)
			Case = 2;
	}
	else if(Menu > 6 && Menu)
	{
		Start = 18;
		End = 24;
		Case = 3;
	}
	else
		return;
	

	for(int i = Start; i < End; i++)
	{
		if(i == 11 || i == 8)
			continue;

		m_apShop[i]->m_Visible = true;
		
		if(Case == 1)
			m_apShop[i]->m_Pos = pOwner->m_Pos + normalize(GetDir(pi/180 * 270-5+(i-Start)*5))*250;
		else if(Case == 2)
			m_apShop[i]->m_Pos = pOwner->m_Pos + normalize(GetDir(pi/180 * 270-1+(i-Start)*2))*250;
		else if(Case == 3)
			m_apShop[i]->m_Pos = pOwner->m_Pos + normalize(GetDir(pi/180 * 270-3*1+(i-Start)*1))*250;
	}

}

void CGui::Snap(int SnappingClient)
{
	if(SnappingClient != m_Owner)
		return;

	Menu();
	/*CCharacter *pOwner = GameServer()->GetPlayerChar(m_Owner);

	if(pOwner)
	{
		
		for(int i = 0; i < 24; i++)
		{
			if(i == 11 || i == 8)
				continue;

			m_apShop[i]->m_Pos = pOwner->m_Pos + normalize(GetDir(pi/180 * ((180 + i*(i?-1:1)*15)%360)))*250;
		}
	}*/




	
	
}
