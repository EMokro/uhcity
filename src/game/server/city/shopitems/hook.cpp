/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <game/generated/protocol.h>
#include <game/server/gamecontext.h>
#include "hook.h"

CHook::CHook(CGameWorld* pGameWorld, int Owner, vec2 Pos, int Type)
	: CEntity(pGameWorld, CGameWorld::ENTTYPE_PROJECTILE)
{
	m_Owner = Owner;
	m_Pos = Pos;
	m_Type = Type;
	m_StartTick = Server()->Tick();
	m_Visible = false;

	for (int i = 0; i < 10; i++)
		m_IDs[i] = Server()->SnapNewID();

	GameWorld()->InsertEntity(this);
}

void CHook::Reset()
{
	GameServer()->m_World.DestroyEntity(this);

	for (int i = 0; i < 10; i++)
		Server()->SnapFreeID(m_IDs[i]);
}

void CHook::Tick()
{
	CCharacter* pOwner = GameServer()->GetPlayerChar(m_Owner);

	if (!pOwner)
	{
		Reset();
		return;
	}

	if (!m_Visible)
		return;

	char aBuf[128];
	int Click = pOwner->MouseEvent(m_Pos);

	if (!Click)
	{
		m_LastClick = 0;
		return;
	}

	switch (m_Type)
	{
	case 1:
		pOwner->Buy("Endless Hook", &pOwner->GetPlayer()->m_AccData.m_EndlessHook, g_Config.m_EuHookEndless, Click, 1);
		break;
	}
}


void CHook::Snap(int SnappingClient)
{
	if (SnappingClient != m_Owner || NetworkClipped(SnappingClient, m_Pos) || !m_Visible)
		return;
	if (Server()->Tick() % 5 == 0)
		m_StartTick = Server()->Tick();

	if (m_Type == 1)// Endless
	{
		CNetObj_Laser *pObj[7];

		for (int i = 0; i < 7; i++) {
			pObj[i] = static_cast<CNetObj_Laser*>(Server()->SnapNewItem(NETOBJTYPE_LASER, m_IDs[i], sizeof(CNetObj_Laser)));

			if (!pObj[i])
				return;
		}

		pObj[0]->m_X = (int)m_Pos.x - 40;
		pObj[0]->m_Y = (int)m_Pos.y;
		pObj[0]->m_FromX = (int)m_Pos.x - 20;
		pObj[0]->m_FromY = (int)m_Pos.y - 15;

		pObj[1]->m_X = (int)m_Pos.x - 20;
		pObj[1]->m_Y = (int)m_Pos.y + 15;
		pObj[1]->m_FromX = (int)m_Pos.x - 40;
		pObj[1]->m_FromY = (int)m_Pos.y;

		pObj[2]->m_X = (int)m_Pos.x + 20;
		pObj[2]->m_Y = (int)m_Pos.y - 15;
		pObj[2]->m_FromX = (int)m_Pos.x - 20;
		pObj[2]->m_FromY = (int)m_Pos.y + 15;

		pObj[3]->m_X = (int)m_Pos.x + 40;
		pObj[3]->m_Y = (int)m_Pos.y;
		pObj[3]->m_FromX = (int)m_Pos.x + 20;
		pObj[3]->m_FromY = (int)m_Pos.y - 15;

		pObj[4]->m_X = (int)m_Pos.x + 20;
		pObj[4]->m_Y = (int)m_Pos.y + 15;
		pObj[4]->m_FromX = (int)m_Pos.x + 40;
		pObj[4]->m_FromY = (int)m_Pos.y;

		pObj[5]->m_X = (int)m_Pos.x + 20;
		pObj[5]->m_Y = (int)m_Pos.y + 15;
		pObj[5]->m_FromX = (int)m_Pos.x - 20;
		pObj[5]->m_FromY = (int)m_Pos.y - 15;

		pObj[6]->m_X = (int)m_Pos.x + 20;
		pObj[6]->m_Y = (int)m_Pos.y + 15;

		pObj[0]->m_StartTick = Server()->Tick();
		pObj[1]->m_StartTick = Server()->Tick();
		pObj[2]->m_StartTick = Server()->Tick();
		pObj[3]->m_StartTick = Server()->Tick();
		pObj[4]->m_StartTick = Server()->Tick();
		pObj[5]->m_StartTick = Server()->Tick();
	}
}
