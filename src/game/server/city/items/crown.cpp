/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <game/generated/protocol.h>
#include <game/server/gamecontext.h>
#include "crown.h"

CCrown::CCrown(CGameWorld *pGameWorld, int Owner)
: CEntity(pGameWorld, CGameWorld::ENTTYPE_LASER)
{
	m_Owner = Owner;

	for(int i = 0; i < 4; i++)
		m_IDs[i] = Server()->SnapNewID();

	GameWorld()->InsertEntity(this);
}

void CCrown::Reset()
{
	GameServer()->m_World.DestroyEntity(this);

	for(int i = 0; i < 4; i++)
		Server()->SnapFreeID(m_IDs[i]);
}

void CCrown::Tick()
{
	CCharacter *pOwner = GameServer()->GetPlayerChar(m_Owner);

	if(!pOwner)
		Reset();
	else
		m_Pos = pOwner->m_Pos;

}

void CCrown::Snap(int SnappingClient)
{
	CCharacter *pOwner = GameServer()->GetPlayerChar(m_Owner);

	if(pOwner)
	{
		if(!pOwner->GetPlayer()->m_AccData.m_Donor)
			return;

		if(pOwner->m_Invisible && SnappingClient != m_Owner)
			return;

		  if(pOwner->GetPlayer()->m_Crown)
			return;
	}

	CNetObj_Laser *pObj[4];

	for(int i = 0; i < 4; i++)
	{
		vec2 From = vec2(0, 0);
		vec2 To = vec2(0, 0);

		switch(i)
		{
		case 0:
			From.x = -20;
			From.y = -20;
			To.y = -45;
			break;
		case 1:
			From.x = +20;
			From.y = -20;
			To.y = -45;
			break;
		case 2:
			To.x = -25;
			To.y = -35;
			break;
		case 3:
			To.x = +25;
			To.y = -35;
			break;
		}

		pObj[i] = static_cast<CNetObj_Laser *>(Server()->SnapNewItem(NETOBJTYPE_LASER, m_IDs[i], sizeof(CNetObj_Laser)));

		if(!pObj[i])
			return;

		pObj[i]->m_X = (int)m_Pos.x + To.x;
		pObj[i]->m_Y = (int)m_Pos.y + To.y;
		pObj[i]->m_FromX = (int)m_Pos.x + From.x;
		pObj[i]->m_FromY = (int)m_Pos.y + From.y;
		pObj[i]->m_StartTick = Server()->Tick();
	}

}
