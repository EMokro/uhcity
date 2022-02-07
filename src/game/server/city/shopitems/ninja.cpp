/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <game/generated/protocol.h>
#include <game/server/gamecontext.h>
#include "ninja.h"

CNinja::CNinja(CGameWorld *pGameWorld, int Owner, vec2 Pos, int Type)
: CEntity(pGameWorld, CGameWorld::ENTTYPE_PROJECTILE)
{
	m_Owner = Owner;
	m_Pos = Pos;
	m_Type = Type;
	m_StartTick = Server()->Tick();
	m_Visible = false;

	for(int i = 0; i < 10; i++)
		m_IDs[i] = Server()->SnapNewID();

	GameWorld()->InsertEntity(this);
}

void CNinja::Reset()
{
	GameServer()->m_World.DestroyEntity(this);

	for(int i = 0; i < 10; i++)
		Server()->SnapFreeID(m_IDs[i]);
}

void CNinja::Tick()
{
	CCharacter *pOwner = GameServer()->GetPlayerChar(m_Owner);

	if(!pOwner || !pOwner->IsAlive())
	{
		Reset();
		return;
	}

	if(!m_Visible)
		return;

	int Click = pOwner->MouseEvent(m_Pos);

	if(!Click)
	{
		m_LastClick = 0;
		return;
	}

	const char* pLanguage = pOwner->GetPlayer()->GetLanguage();
	dynamic_string Buffer;

	switch(m_Type)
	{
	case 1:
		Server()->Localization()->Format_L(Buffer, pLanguage, _("Ninja Permanent"));
		pOwner->Buy(Buffer.buffer(), &pOwner->GetPlayer()->m_AccData.m_NinjaPermanent, g_Config.m_EuNinjaPermanent, Click, 1);
		break;
	case 2:
		Server()->Localization()->Format_L(Buffer, pLanguage, _("Ninja at Start"));
		pOwner->Buy(Buffer.buffer(), &pOwner->GetPlayer()->m_AccData.m_NinjaStart, g_Config.m_EuNinjaStart, Click, 1);
		break;
	case 3:
		Server()->Localization()->Format_L(Buffer, pLanguage, _("Ninja Switch"));
		pOwner->Buy(Buffer.buffer(), &pOwner->GetPlayer()->m_AccData.m_NinjaSwitch, g_Config.m_EuNinjaSwitch, Click, 1);
		break;
	case 4:
		Server()->Localization()->Format_L(Buffer, pLanguage, _("Ninja fly"));
		pOwner->Buy(Buffer.buffer(), &pOwner->GetPlayer()->m_AccData.m_NinjaFly, g_Config.m_EuNinjaFly, Click, 1);
		break;
	case 5:
		Server()->Localization()->Format_L(Buffer, pLanguage, _("Ninja Bomber"));
		pOwner->Buy(Buffer.buffer(), &pOwner->GetPlayer()->m_AccData.m_NinjaBomber, g_Config.m_EuNinjaBomber, Click, 1);
		break;
	}
}


void CNinja::Snap(int SnappingClient)
{
	if(SnappingClient != m_Owner || NetworkClipped(SnappingClient, m_Pos) || !m_Visible)
		return;

	if(m_Type == 1)//Permanent
	{
		CNetObj_Pickup *pP = static_cast<CNetObj_Pickup *>(Server()->SnapNewItem(NETOBJTYPE_PICKUP, m_ID, sizeof(CNetObj_Pickup)));

		if(!pP)
			return;

		pP->m_X = (int)m_Pos.x;
		pP->m_Y = (int)m_Pos.y;
		pP->m_Type = POWERUP_NINJA;
		pP->m_Subtype = WEAPON_NINJA;
	}
	else if(m_Type == 2)// Start
	{
		CNetObj_Pickup *pP = static_cast<CNetObj_Pickup *>(Server()->SnapNewItem(NETOBJTYPE_PICKUP, m_ID, sizeof(CNetObj_Pickup)));

		if(!pP)
			return;

		pP->m_X = (int)m_Pos.x;
		pP->m_Y = (int)m_Pos.y;
		pP->m_Type = POWERUP_WEAPON;
		pP->m_Subtype = WEAPON_NINJA;
			
		if(Server()->Tick()%100 == 0)
		{
			CNetEvent_Spawn *ev = (CNetEvent_Spawn *)GameServer()->m_Events.Create(NETEVENTTYPE_SPAWN, sizeof(CNetEvent_Spawn));
			if(ev)
			{
				ev->m_X = (int)m_Pos.x;
				ev->m_Y = (int)m_Pos.y;
			}
		}
		
	} else if(m_Type == 3) // Switch
	{
		CNetObj_Pickup *pP[6];

		for(int i = 0; i < 5; i++)
		{
			pP[i] = static_cast<CNetObj_Pickup *>(Server()->SnapNewItem(NETOBJTYPE_PICKUP, m_IDs[i], sizeof(CNetObj_Pickup)));

			if(!pP[i])
				return;

			if(i != 4)
			{
				pP[i]->m_X = (int)m_Pos.x;
				pP[i]->m_Y = (int)m_Pos.y + 10 + 20 - i * 20;
				pP[i]->m_Type = POWERUP_WEAPON;
				pP[i]->m_Subtype = i+1;
			}
			else
			{
				pP[i]->m_X = (int)m_Pos.x;
				pP[i]->m_Y = (int)m_Pos.y;
				pP[i]->m_Type = POWERUP_NINJA;
				pP[i]->m_Subtype = 0;
			}

		}

	} else if(m_Type == 4) { // fly
		CNetObj_Pickup *pP[3];
		int LifeTime = 25;

		if (m_StartTick + LifeTime * 3 < Server()->Tick()) {
			m_StartTick = Server()->Tick();
		}	
			
			
		for (int i = 0; i < 3; i++) {
			if (Server()->Tick() < m_StartTick + LifeTime * (i+1) && Server()->Tick() > m_StartTick + LifeTime * i) {

				pP[i] = static_cast<CNetObj_Pickup *>(Server()->SnapNewItem(NETOBJTYPE_PICKUP, m_ID, sizeof(CNetObj_Pickup)));

				if(!pP[i])
					return;

				pP[i]->m_X = (int)m_Pos.x + (40*i - 40);
				pP[i]->m_Y = (int)m_Pos.y;
				pP[i]->m_Type = POWERUP_NINJA;
				pP[i]->m_Subtype = WEAPON_NINJA;
			}
		}
	} else if(m_Type == 5) { // fly
		CNetObj_Pickup *pP;
		CNetObj_Projectile *pProj;

		if(Server()->Tick()%20 == 0)
			m_StartTick = Server()->Tick();

		pP = static_cast<CNetObj_Pickup *>(Server()->SnapNewItem(NETOBJTYPE_PICKUP, m_IDs[0], sizeof(CNetObj_Pickup)));

		if(!pP)
			return;

		pP->m_X = (int)m_Pos.x;
		pP->m_Y = (int)m_Pos.y;
		pP->m_Type = POWERUP_NINJA;
		pP->m_Subtype = WEAPON_NINJA;

		pProj = static_cast<CNetObj_Projectile *>(Server()->SnapNewItem(NETOBJTYPE_PROJECTILE, m_IDs[1], sizeof(CNetObj_Projectile)));

		if(!pProj)
			return;

		pProj->m_X = (int)m_Pos.x - 32;
		pProj->m_Y = (int)m_Pos.y;
		pProj->m_VelX = (int)0;
		pProj->m_VelY = (int)10;
		pProj->m_StartTick = m_StartTick;
		pProj->m_Type = WEAPON_GRENADE;
	}
}
