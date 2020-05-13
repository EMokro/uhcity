/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <game/generated/protocol.h>
#include <game/server/gamecontext.h>
#include "grenade.h"

CGrenade::CGrenade(CGameWorld *pGameWorld, int Owner, vec2 Pos, int Type)
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

void CGrenade::Reset()
{
	GameServer()->m_World.DestroyEntity(this);

	for(int i = 0; i < 10; i++)
		Server()->SnapFreeID(m_IDs[i]);
}

void CGrenade::Tick()
{
	CCharacter *pOwner = GameServer()->GetPlayerChar(m_Owner);

	if(!pOwner)
	{
		Reset();
		return;
	}

	if(!m_Visible)
		return;

	char aBuf[128];
	int Click = pOwner->MouseEvent(m_Pos);

	if(!Click)
	{
		m_LastClick = 0;
		return;
	}
	
	switch(m_Type)
	{
	case 1:
		pOwner->Buy("Grenade spread", &pOwner->GetPlayer()->m_AccData.m_GrenadeSpread, g_Config.m_EuGrenadeSpread, Click, 5);
		break;
	case 2:
		pOwner->Buy("Grenade bounce", &pOwner->GetPlayer()->m_AccData.m_GrenadeBounce, g_Config.m_EuGrenadeBounce, Click, 5);
		break;
	case 3:
		//pOwner->Buy("Rifle Plasma", &pOwner->GetPlayer()->m_AccData.m_Shotgun, g_Config.m_EuRiflePlasma, Click, 10);
		break;
	}
}


void CGrenade::Snap(int SnappingClient)
{
	if(SnappingClient != m_Owner || NetworkClipped(SnappingClient, m_Pos) || !m_Visible)
		return;

	if(Server()->Tick()%5 == 0)
		m_StartTick = Server()->Tick();

	if(m_Type == 1)// Spread
	{
		CNetObj_Projectile *pProj[3];

		for(int i = 0; i < 3; i++)
		{
			pProj[i] = static_cast<CNetObj_Projectile *>(Server()->SnapNewItem(NETOBJTYPE_PROJECTILE, m_IDs[i], sizeof(CNetObj_Projectile)));
			if(!pProj[i])
				return;

			pProj[i]->m_X = (int)m_Pos.x - 30 + 20*i;
			pProj[i]->m_Y = (int)m_Pos.y;
			pProj[i]->m_VelX = (int)0;
			pProj[i]->m_VelY = (int)-1;
			pProj[i]->m_StartTick = m_StartTick;
			pProj[i]->m_Type = WEAPON_GRENADE;
		}
	}
	else if(m_Type == 2)// Bounce
	{
		if(Server()->Tick()%10 == 0)
			m_StartTick = Server()->Tick();

		CNetObj_Projectile *pProj[3];

		for(int i = 0; i < 2; i++)
		{
			pProj[i] = static_cast<CNetObj_Projectile *>(Server()->SnapNewItem(NETOBJTYPE_PROJECTILE, m_IDs[i], sizeof(CNetObj_Projectile)));
			if(!pProj[i])
				return;

			pProj[i]->m_X = (int)m_Pos.x + (i?-128:128);
			pProj[i]->m_Y = (int)m_Pos.y + 96 - 32*i;
			pProj[i]->m_VelX = (int)i?50:-50;
			pProj[i]->m_VelY = (int)-50;
			pProj[i]->m_StartTick = i?m_StartTick - 10:m_StartTick - 5;
			pProj[i]->m_Type = WEAPON_GRENADE;
		}

		CNetObj_Laser *pObj[4];
	
		for(int i = 0; i < 2; i++)
		{
			pObj[i] = static_cast<CNetObj_Laser *>(Server()->SnapNewItem(NETOBJTYPE_LASER, m_IDs[i+2], sizeof(CNetObj_Laser)));

			if(!pObj[i])
				return;

			pObj[i]->m_X = (int)m_Pos.x - 32;
			pObj[i]->m_Y = (int)i?m_Pos.y - 32:m_Pos.y + 32;
			pObj[i]->m_FromX = (int)m_Pos.x - 32;
			pObj[i]->m_FromY = (int)m_Pos.y - 32;
			pObj[i]->m_StartTick = Server()->Tick();

		}
	}
	/*
	CNetObj_Pickup *pP[2];

	for(int i = 0; i < 2; i++)
	{
		pP[i] = static_cast<CNetObj_Pickup *>(Server()->SnapNewItem(NETOBJTYPE_PICKUP, m_IDs[i], sizeof(CNetObj_Pickup)));

		if(!pP[i])
			return;

		pP[i]->m_X = (int)m_Pos.x - 25 + 5*i;
		pP[i]->m_Y = (int)m_Pos.y + 5*i;
		pP[i]->m_Type = i;
		pP[i]->m_Subtype = 0;
	
	}

	vec2 Positions[6];
	Positions[0] = vec2(m_Pos.x - 45, m_Pos.y);
	Positions[1] = vec2(m_Pos.x + 45, m_Pos.y);

	CNetEvent_Explosion *pEvent = (CNetEvent_Explosion *)GameServer()->m_Events.Create(NETEVENTTYPE_EXPLOSION, sizeof(CNetEvent_Explosion));
	if(pEvent)
	{
		pEvent->m_X = (int)Pos.x;
		pEvent->m_Y = (int)Pos.y;
	}

	CCharacter *pOwner = GameServer()->GetPlayerChar(m_Owner);

	if(pOwner)
	{
		if(pOwner->GetPlayer()->m_AccData.m_AllWeapons)
		{

		}
	}
	
	*/
}
