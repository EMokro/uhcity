/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <game/generated/protocol.h>
#include <game/server/gamecontext.h>
#include "healstate.h"

CHealstate::CHealstate(CGameWorld *pGameWorld, int Owner)
: CEntity(pGameWorld, CGameWorld::ENTTYPE_LASER)
{
	m_Owner = Owner;

	for(int i = 0; i < 5; i++)
		m_IDs[i] = Server()->SnapNewID();

    for (int i = 0; i < 5; i++)
		m_LastPos[i] = vec2(0, ((rand() % 64) - 32));

	GameWorld()->InsertEntity(this);
}

void CHealstate::Reset()
{
	GameServer()->m_World.DestroyEntity(this);

	for(int i = 0; i < 5; i++)
		Server()->SnapFreeID(m_IDs[i]);
}

void CHealstate::Tick()
{
	CCharacter *pOwner = GameServer()->GetPlayerChar(m_Owner);

	if(!pOwner)
		Reset();
	else
		m_Pos = pOwner->m_Pos;
}

void CHealstate::Snap(int SnappingClient)
{
	CCharacter *pOwner = GameServer()->GetPlayerChar(m_Owner);
    
	if(pOwner)
	{
		if(pOwner->m_Invisible && SnappingClient != m_Owner)
			return; 

        if(!pOwner->m_ExternalHeal)
            return;
	}
    
	CNetObj_Pickup *pObj[5];
    int SpawnBound = 64;
    // this doesnt look rly nice :(
    for (int i = 0; i < 5; i++) {
        pObj[i] = static_cast<CNetObj_Pickup*>(Server()->SnapNewItem(NETOBJTYPE_PICKUP, m_IDs[i], sizeof(CNetObj_Pickup)));

        if (!pObj[i])
            return;
        
        if (!m_LastPos[i].y)
            m_LastPos[i].x = ((rand() % SpawnBound) - SpawnBound/2);
            
        pObj[i]->m_X = (int)pOwner->m_Core.m_Pos.x + (int)m_LastPos[i].x;
        pObj[i]->m_Y = (int)pOwner->m_Core.m_Pos.y - (int)m_LastPos[i].y;
        pObj[i]->m_Type = 0;
        pObj[i]->m_Subtype = 0;
        m_LastPos[i].y += 2;

        if (m_LastPos[i].y > SpawnBound)
            m_LastPos[i].y = 0;
    }
}
