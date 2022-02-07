/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <game/generated/protocol.h>
#include <game/server/gamecontext.h>
#include <game/server/entities/projectile.h>
#include "pushaura.h"

CPushAura::CPushAura(CGameWorld *pGameWorld, int Owner, vec2 Pos)
: CEntity(pGameWorld, CGameWorld::ENTTYPE_PROJECTILE)
{
	m_Owner = Owner;
    m_Pos = Pos;
    m_LifeTime = 3000;
	m_StartRadius = 48;

	for(int i = 0; i < MAX_PROJECTILS; i++) {
        m_IDs[i] = Server()->SnapNewID();
        m_StartPos[i] = rand() % (g_Config.m_SvGravAuraRadius - m_StartRadius);
    }
		
	GameWorld()->InsertEntity(this);
}

void CPushAura::Reset()
{
	GameServer()->m_World.DestroyEntity(this);

	for(int i = 0; i < MAX_PROJECTILS; i++)
		Server()->SnapFreeID(m_IDs[i]);
}

void CPushAura::Tick()
{
	CCharacter *pOwner = GameServer()->GetPlayerChar(m_Owner);

	if(!pOwner || !pOwner->IsAlive()) {
		Reset();
		return;
	}

    if(!m_Visible)
		return;

	m_LifeTime--;

	int Click = pOwner->MouseEvent(m_Pos);

	if(!Click)
	{
		m_LastClick = 0;
		return;
	}

	if (m_LifeTime <= 0)
		m_LifeTime = 3000;
	const char* pLanguage = pOwner->GetPlayer()->GetLanguage();
	dynamic_string Buffer;
	Server()->Localization()->Format_L(Buffer, pLanguage, _("Pushaura"));
	
	pOwner->Buy(Buffer.buffer(), &pOwner->GetPlayer()->m_AccData.m_PushAura, g_Config.m_EuPushAura, Click, 1);
}

void CPushAura::Snap(int SnappingClient)
{
    if(SnappingClient != m_Owner || !m_Visible)
		return;

	CNetObj_Projectile *pObj[MAX_PROJECTILS];

    for(int i = 0; i < MAX_PROJECTILS; i++)
    {
		pObj[i] = static_cast<CNetObj_Projectile *>(Server()->SnapNewItem(NETOBJTYPE_PROJECTILE, m_IDs[i], sizeof(CNetObj_Projectile)));

		if (!pObj[i])
            continue;

		vec2 TempPos = normalize(GetDir((pi/180) * (i * 360/MAX_PROJECTILS))) * (((3000 - m_LifeTime) + m_StartPos[i]) % 80);

		pObj[i]->m_X = m_Pos.x + TempPos.x;
		pObj[i]->m_Y = m_Pos.y + TempPos.y;
		pObj[i]->m_Type = WEAPON_HAMMER;
		pObj[i]->m_StartTick = Server()->Tick();
    }
}
