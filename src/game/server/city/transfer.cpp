/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <game/generated/protocol.h>
#include <game/server/gamecontext.h>
#include "transfer.h"
#include <game/server/entities/projectile.h>

CTransfer::CTransfer(CGameWorld *pGameWorld, int Value, vec2 Pos)
: CEntity(pGameWorld, CGameWorld::ENTTYPE_TRANSFER)
{
	m_Value = Value;
	m_Pos = Pos;
	m_Vel = vec2(0, 0);

	for(int i = 0; i < 2; i++)
		m_IDs[i] = Server()->SnapNewID();

	GameWorld()->InsertEntity(this);
}

void CTransfer::Reset()
{
	GameServer()->m_World.DestroyEntity(this);

	for(int i = 0; i < 2; i++)
		Server()->SnapFreeID(m_IDs[i]);
}

void CTransfer::Tick()
{
	char aBuf[256];
	m_Vel.y += GameServer()->m_World.m_Core.m_Tuning.m_Gravity;
	GameServer()->Collision()->MoveBox(&m_Pos, &m_Vel, vec2(28, 28), 0.5f);

	//if(!m_Vel.x && !m_Vel.x)

	if(GameServer()->Collision()->GetCollisionAt(m_Pos.x, m_Pos.y)&CCollision::COLFLAG_DEATH ||GameLayerClipped(m_Pos))
	{
		Reset();
		return;
	}

	for(int i = 0; i <= 360; i+=36)
	{
		vec2 TempPos = m_Pos + normalize(GetDir(pi/180*i)) * 32;
		new CProjectile(GameWorld(), WEAPON_HAMMER,	-1, TempPos, vec2(0, 0), 1, 1, 0, 0, -1, 0);
	}

	CCharacter *apEnts[MAX_CLIENTS];
	int Num = GameServer()->m_World.FindEntities(m_Pos, 5.0f, (CEntity**)apEnts, MAX_CLIENTS, CGameWorld::ENTTYPE_CHARACTER);
	for(int i = 0; i < Num; i++)
	{
		apEnts[i]->GetPlayer()->m_AccData.m_Money += m_Value;
		str_format(aBuf, sizeof(aBuf), "Money-Transfer-Object (%i) | +%i", apEnts[i]->GetPlayer()->m_AccData.m_Money, m_Value);
		GameServer()->SendBroadcast(aBuf, apEnts[i]->GetPlayer()->GetCID());
		m_Value = 0;
		Reset();
		return;
	}

	const int MAX_TRANSFER = 1000;

	CTransfer *apSameEnts[MAX_TRANSFER];
	Num = GameServer()->m_World.FindEntities(m_Pos, 1024, (CEntity**)apSameEnts,
					MAX_TRANSFER, CGameWorld::ENTTYPE_TRANSFER);

	for(int i = 0; i < Num; i++)
	{
		CTransfer *pTarget = apSameEnts[i];

		if(pTarget && pTarget != this && pTarget->m_Value && m_Value)
		{
			if(distance(pTarget->m_Pos, m_Pos) < 128)
			{
				m_Value += pTarget->m_Value;
				pTarget->m_Value = 0;
				pTarget->Reset();
			}
		}
	}
}


void CTransfer::Snap(int SnappingClient)
{
	if(NetworkClipped(SnappingClient, m_Pos))
		return;

	CNetObj_Projectile *pProj[2];

	for(int i = 0; i < 2; i++)
	{
		pProj[i] = static_cast<CNetObj_Projectile *>(Server()->SnapNewItem(NETOBJTYPE_PROJECTILE, m_IDs[i], sizeof(CNetObj_Projectile)));
		
		if(!pProj[i])
			return;

		pProj[i]->m_X = (int)m_Pos.x + (i*2-1) * 8;
		pProj[i]->m_Y = (int)m_Pos.y;
		pProj[i]->m_VelX = (int)0;
		pProj[i]->m_VelY = (int)0;
		pProj[i]->m_StartTick = Server()->Tick()-100;
		pProj[i]->m_Type = WEAPON_RIFLE;

	}

	
	
	
}
