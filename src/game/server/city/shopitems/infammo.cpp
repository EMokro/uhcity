/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <game/generated/protocol.h>
#include <game/server/gamecontext.h>
#include "infammo.h"

CInfAmmo::CInfAmmo(CGameWorld *pGameWorld, int Owner, vec2 Pos)
: CEntity(pGameWorld, CGameWorld::ENTTYPE_PROJECTILE)
{
	m_Owner = Owner;
	m_Pos = Pos;
	m_StartTick = Server()->Tick();

	m_Visible = false;

	for(int i = 0; i < 4; i++)
		m_IDs[i] = Server()->SnapNewID();

	GameWorld()->InsertEntity(this);
}

void CInfAmmo::Reset()
{
	GameServer()->m_World.DestroyEntity(this);

	for(int i = 0; i < 4; i++)
		Server()->SnapFreeID(m_IDs[i]);
}

void CInfAmmo::Tick()
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
	
	pOwner->Buy("Infinity ammo", &pOwner->GetPlayer()->m_AccData.m_InfinityAmmo, g_Config.m_EuInfAmmo, Click, 1);
}


void CInfAmmo::Snap(int SnappingClient)
{
	if(SnappingClient != m_Owner || NetworkClipped(SnappingClient, m_Pos) || !m_Visible)
		return;

	if(Server()->Tick()%5 == 0)
		m_StartTick = Server()->Tick();

	/*vec2 Positions[4];

	Positions[0] = vec2(m_Pos.x - 28, m_Pos.y);
	Positions[1] = vec2(m_Pos.x + 28, m_Pos.y);
	Positions[2] = vec2(m_Pos.x, m_Pos.y + 32);
	Positions[3] = vec2(m_Pos.x, m_Pos.y - 32);*/

	CNetObj_Projectile *pProj[4];

	for(int i = 0; i < 4; i++)
	{
		pProj[i] = static_cast<CNetObj_Projectile *>(Server()->SnapNewItem(NETOBJTYPE_PROJECTILE, m_IDs[i], sizeof(CNetObj_Projectile)));

		if(!pProj[i])
			return;

		int X = ((i+1)%2?0:(i==3?28:-28));
		int Y = (i%2?0:(!i?-28:28));

		pProj[i]->m_X = (int)(m_Pos.x + X);
		pProj[i]->m_Y = (int)(m_Pos.y + Y);
		pProj[i]->m_VelX = (int)0;
		pProj[i]->m_VelY = (int)-1;
		pProj[i]->m_StartTick = i==3?Server()->Tick():m_StartTick;
		pProj[i]->m_Type = i+1;

	}

	CCharacter *pOwner = GameServer()->GetPlayerChar(m_Owner);

	if(pOwner)
	{
		if(pOwner->GetPlayer()->m_AccData.m_AllWeapons)
		{

		}
	}
	
	
}
