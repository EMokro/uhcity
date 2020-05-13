/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <game/generated/protocol.h>
#include <game/server/gamecontext.h>
#include "vip.h"

CVip::CVip(CGameWorld *pGameWorld, vec2 Pos)
: CEntity(pGameWorld, CGameWorld::ENTTYPE_PROJECTILE)
{
	m_Pos = Pos;

	GameWorld()->InsertEntity(this);
}

void CVip::Reset()
{
	GameServer()->m_World.DestroyEntity(this);

}

void CVip::Tick()
{
	for(int i = 0; i < MAX_CLIENTS; i++)
	{
	CCharacter *pUser = GameServer()->GetPlayerChar(i);

	if(!pUser)
		continue;

	int Click = pUser->MouseEvent(m_Pos);

	if(!Click)
	{
		//m_LastClick = 0;
		continue;
	}
	
	pUser->Buy("Vip", &pUser->GetPlayer()->m_AccData.m_VIP, g_Config.m_EuVIP, Click, 1);
}
}


void CVip::Snap(int SnappingClient)
{

	CNetObj_Flag *pFlag = (CNetObj_Flag *)Server()->SnapNewItem(NETOBJTYPE_FLAG, TEAM_BLUE, sizeof(CNetObj_Flag));

	pFlag->m_X = m_Pos.x;
	pFlag->m_Y = m_Pos.y;
	pFlag->m_Team = TEAM_BLUE;
		
	
}
