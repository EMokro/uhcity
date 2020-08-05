/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <game/generated/protocol.h>
#include <game/server/gamecontext.h>
#include "portal.h"

CPortal::CPortal(CGameWorld *pGameWorld, int Owner, vec2 Pos1, vec2 Pos2)
: CEntity(pGameWorld, CGameWorld::ENTTYPE_LASER)
{
	m_Owner = Owner;
    m_Pos1 = Pos1;
    m_Pos2 = Pos2;
    m_Timer = 25;
    m_LifeTime = 3000; // 1min

	// for(int i = 0; i < 2; i++)
	// 	m_IDs[i] = Server()->SnapNewID();

	GameWorld()->InsertEntity(this);
}

void CPortal::Reset()
{
	GameServer()->m_World.DestroyEntity(this);

	// for(int i = 0; i < 2; i++)
	// 	Server()->SnapFreeID(m_IDs[i]);
}

void CPortal::Tick()
{
	CCharacter *pOwner = GameServer()->GetPlayerChar(m_Owner);

	if(!pOwner)
		Reset();

    m_LifeTime--;

    if (m_LifeTime <= 0)
        Reset();

    if (m_Timer < 1) {
        CCharacter* pChr1 = GameWorld()->ClosestCharacter(m_Pos1, 32, NULL);

        if (!pChr1)
            return;

        if (pChr1) {
            pChr1->m_Core.m_Pos = m_Pos2;
            m_Timer = 25;
        }
            
        CCharacter* pChr2 = GameWorld()->ClosestCharacter(m_Pos2, 32, NULL);
        if (pChr2) {
            pChr2->m_Core.m_Pos = m_Pos1;
            m_Timer = 25;
        }
    } else 
        m_Timer--;
    
}

void CPortal::Snap(int SnappingClient)
{
	CCharacter *pOwner = GameServer()->GetPlayerChar(m_Owner);

	if(pOwner)
	{
		if(!pOwner->GetPlayer()->m_AccData.m_Portal)
			return;
	}

    if (!m_Pos1 || !m_Pos2) {
        return;
    }
        

	CNetEvent_Spawn *pObj[2];

	for(int i = 0; i < 2; i++)
	{
		pObj[i] = (CNetEvent_Spawn*)GameServer()->m_Events.Create(NETEVENTTYPE_SPAWN, sizeof(CNetEvent_Spawn));

        if (!pObj[i])
            continue;

        switch (i)
        {
        case 0:
            pObj[i]->m_X = (int)m_Pos1.x;
            pObj[i]->m_Y = (int)m_Pos1.y;
            break;
        case 1:
            pObj[i]->m_X = (int)m_Pos2.x;
            pObj[i]->m_Y = (int)m_Pos2.y;
            break;
        default:
            break;
        }
	}

}
