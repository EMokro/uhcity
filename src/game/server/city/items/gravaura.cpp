/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <game/generated/protocol.h>
#include <game/server/gamecontext.h>
#include <game/server/entities/projectile.h>
#include "gravaura.h"

CGravAura::CGravAura(CGameWorld *pGameWorld, int Owner, int Mode)
: CEntity(pGameWorld, CGameWorld::ENTTYPE_PROJECTILE)
{
	m_Owner = Owner;
    m_Mode = Mode;
    m_LifeTime = 3000;
    m_StartRadius = 48;

	for(int i = 0; i < MAX_PROJECTILS; i++) {
        m_IDs[i] = Server()->SnapNewID();
        m_StartPos[i] = rand() % (g_Config.m_SvGravAuraRadius - m_StartRadius);
    }
		
	GameWorld()->InsertEntity(this);
}

void CGravAura::Reset()
{
	GameServer()->m_World.DestroyEntity(this);

	for(int i = 0; i < MAX_PROJECTILS; i++)
		Server()->SnapFreeID(m_IDs[i]);
}

void CGravAura::Tick()
{
	CCharacter *pOwner = GameServer()->GetPlayerChar(m_Owner);

	if(!pOwner) {
        Reset();
        return;
    }

    m_LifeTime--;

    if (m_LifeTime <= 0) {
        Reset();
        return;
    }

    for (int i = 0; i < MAX_CLIENTS; i++) {

        if (i == m_Owner)
            continue;

        CCharacter* pTarget = GameServer()->GetPlayerChar(i);

        if (!pTarget)
            continue;

        if (pTarget->GetPlayer()->m_Insta || pTarget->m_GameZone || pTarget->GetPlayer()->m_God || pTarget->Protected())
            return;

        vec2 Dist = pTarget->m_Core.m_Pos - pOwner->m_Core.m_Pos;

        if (length(Dist) > g_Config.m_SvGravAuraRadius)
            continue;

        pTarget->m_Core.m_Vel += vec2(Dist.x / g_Config.m_SvGravAuraRadius, Dist.y / g_Config.m_SvGravAuraRadius) * g_Config.m_SvGravAuraForce * m_Mode;
    }
    
}

void CGravAura::Snap(int SnappingClient)
{
    CCharacter* pChr = GameServer()->GetPlayerChar(m_Owner);

    if (!pChr)
        return;

    CNetObj_Projectile *pObj[MAX_PROJECTILS];
    for(int i = 0; i < MAX_PROJECTILS; i++)
    {
        pObj[i] = static_cast<CNetObj_Projectile *>(Server()->SnapNewItem(NETOBJTYPE_PROJECTILE, m_IDs[i], sizeof(CNetObj_Projectile)));

        vec2 TempPos;
        if (m_Mode > 0) {
            TempPos = pChr->m_Pos + 
                normalize(GetDir((pi/180) * (i * 360/MAX_PROJECTILS))) * 
                (((3000 - m_LifeTime) + m_StartPos[i]) % g_Config.m_SvGravAuraRadius);
        } else if (m_Mode < 0) {
            TempPos = pChr->m_Pos + 
                normalize(GetDir((pi/180) * (i * 360/MAX_PROJECTILS))) * 
                ((((m_LifeTime - 3000) + m_StartPos[i]) % g_Config.m_SvGravAuraRadius) + g_Config.m_SvGravAuraRadius);
        }
        
        pObj[i]->m_X = m_Pos.x + TempPos.x;
		pObj[i]->m_Y = m_Pos.y + TempPos.y;
		pObj[i]->m_Type = WEAPON_HAMMER;
		pObj[i]->m_StartTick = Server()->Tick();
    }
}
