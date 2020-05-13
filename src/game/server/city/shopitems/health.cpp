/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <game/generated/protocol.h>
#include <game/server/gamecontext.h>
#include "health.h"

CBuyHealth::CBuyHealth(CGameWorld *pGameWorld, vec2 Pos)
: CEntity(pGameWorld, CGameWorld::ENTTYPE_PROJECTILE)
{
	m_Pos = Pos;

	GameWorld()->InsertEntity(this);
		for(int i = 0; i < 2; i++)
		m_IDs[i] = Server()->SnapNewID();
}

void CBuyHealth::Reset()
{
	GameServer()->m_World.DestroyEntity(this);
	
	
	for(int i = 0; i < 2; i++)
		Server()->SnapFreeID(m_IDs[i]);
}

void CBuyHealth::Tick()
{
	for(int i = 0; i < MAX_CLIENTS; i++)
	{
		CCharacter *pUser = GameServer()->GetPlayerChar(i);

		if(!pUser)
			continue;

		int Click = pUser->MouseEvent(m_Pos-vec2(50,50));
		int Click1 = pUser->MouseEvent(m_Pos-vec2(-50,50));

		if(!Click && !Click1)
		{
			//m_LastClick = 0;
			continue;
		}
	
		pUser->Buy("Health", &pUser->GetPlayer()->m_AccData.m_Health, g_Config.m_EuHealth, Click, 500);
		pUser->Buy("Armor", &pUser->GetPlayer()->m_AccData.m_Armor, g_Config.m_EuArmor, Click1, 500);
	}
}


void CBuyHealth::Snap(int SnappingClient)
{

	CNetObj_Flag *pFlag = (CNetObj_Flag *)Server()->SnapNewItem(NETOBJTYPE_FLAG, TEAM_RED, sizeof(CNetObj_Flag));

	pFlag->m_X = m_Pos.x;
	pFlag->m_Y = m_Pos.y;
	pFlag->m_Team = TEAM_RED;

		CNetObj_Pickup *pP[2];


		pP[0] = static_cast<CNetObj_Pickup *>(Server()->SnapNewItem(NETOBJTYPE_PICKUP, m_IDs[0], sizeof(CNetObj_Pickup)));

		if(!pP[0])
			return;

		pP[0]->m_X = (int)m_Pos.x-50;
		pP[0]->m_Y = (int)m_Pos.y-50;
		pP[0]->m_Type = POWERUP_HEALTH;
		pP[0]->m_Subtype = 0;
	
		pP[1] = static_cast<CNetObj_Pickup *>(Server()->SnapNewItem(NETOBJTYPE_PICKUP, m_IDs[1], sizeof(CNetObj_Pickup)));

		if(!pP[1])
			return;

		pP[1]->m_X = (int)m_Pos.x+50;
		pP[1]->m_Y = (int)m_Pos.y-50;
		pP[1]->m_Type = POWERUP_ARMOR;
		pP[1]->m_Subtype = 0;
	
	
		
	
}
