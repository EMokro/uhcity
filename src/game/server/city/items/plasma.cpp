/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <game/generated/protocol.h>
#include <game/server/gamecontext.h>
#include "plasma.h"

CPlasma::CPlasma(CGameWorld *pGameWorld, int Type, vec2 Pos, int Owner, int Victim)
: CEntity(pGameWorld, CGameWorld::ENTTYPE_PLASMA)
{
	m_Pos = Pos;
	m_Owner = Owner;
	m_Victim = Victim;
	m_Type = Type;
	GameWorld()->InsertEntity(this);

	CCharacter *OwnerChar = GameServer()->GetPlayerChar(m_Owner);

	if(!m_Type && OwnerChar)
	{
		m_LifeSpan = 50*OwnerChar->GetPlayer()->m_AccData.m_HammerShot*10;
		OwnerChar->m_Plasma++;
	}
}

CPlasma::CPlasma(CGameWorld *pGameWorld, int Type, vec2 Pos, int Owner, vec2 Dir)
: CEntity(pGameWorld, CGameWorld::ENTTYPE_LASER)
{
	m_Pos = Pos;
	m_Pos2 = Pos;
	m_Owner = Owner;
	m_Type = Type;
	m_Angle = GetAngle(Dir);
	m_LifeSpan = 25*GameServer()->m_apPlayers[m_Owner]->m_AccData.m_RiflePlasma;
	m_ID2 = Server()->SnapNewID();
	GameWorld()->InsertEntity(this);
}

void CPlasma::Reset()
{
	CCharacter *pOwner = GameServer()->GetPlayerChar(m_Owner);

	if(!m_Type && pOwner)
		pOwner->m_Plasma--;

	GameServer()->m_World.DestroyEntity(this);

	if(m_Type)
		Server()->SnapFreeID(m_ID2);
}

void CPlasma::HammerPlasma()
{
	m_LifeSpan--;
	CCharacter *OwnerChar = GameServer()->GetPlayerChar(m_Owner);
	CCharacter *VictimChar = GameServer()->GetPlayerChar(m_Victim);
	if(!OwnerChar || !VictimChar || GameServer()->Collision()->CheckPoint(m_Pos) || !m_LifeSpan)
	{
		Reset();
		return;
	}
	GetPath();
	NextPos(&NextPosition);
	vec2 LastPos = m_Pos;

	if(NextPosition.x || NextPosition.x)
		m_Pos += normalize(NextPosition - m_Pos)*(8 + 2 * GameServer()->m_apPlayers[m_Owner]->m_AccData.m_HammerShot);

	CCharacter *TargetChr = GameServer()->m_World.IntersectCharacter(m_Pos, LastPos, 6.0f, LastPos, OwnerChar);

	if(TargetChr)
	{
		TargetChr->TakeDamage(vec2(0, 0), 20, m_Owner, m_Type);
		Reset();
		return;
	}
	
}
void CPlasma::RiflePlasma()
{
	m_LifeSpan--;

	CCharacter *OwnerChar = GameServer()->GetPlayerChar(m_Owner);
	float d = distance(m_Pos, m_Pos2) / 3;
	if(!OwnerChar/* || GameServer()->Collision()->IntersectLine(m_Pos+normalize(m_Pos2-m_Pos)*d, m_Pos2+normalize(m_Pos-m_Pos2)*d, 0x0, 0x0)*/ || !m_LifeSpan)
	{
		Reset();
		return;
	}

	vec2 TestPos = m_Pos;
	vec2 TestPos2 = m_Pos2;
	bool WallPos = false;
	bool WallPos2 = false;

	TestPos += normalize(vec2(cosf(m_Angle+0.070f), sinf(m_Angle+0.070f)))*10;
	TestPos2 += normalize(vec2(cosf(m_Angle-0.070f), sinf(m_Angle-0.070f)))*10;

	if(GameServer()->Collision()->IntersectLine(m_Pos, TestPos, 0x0, &TestPos))
	{
		m_Pos = TestPos;
		WallPos = true;
	}
	
		

	if(GameServer()->Collision()->IntersectLine(m_Pos2, TestPos2, 0x0, &TestPos2))
	{
		m_Pos2 = TestPos2;
		WallPos2 = true;
	}

	if(WallPos && WallPos2)
	{
		Reset();
		return;
	}

	m_Pos = TestPos;
	m_Pos2 = TestPos2;

	vec2 Intersection;
	CCharacter *TargetChr = GameServer()->m_World.IntersectCharacter(m_Pos, m_Pos2, 20.0f, Intersection, OwnerChar);

	if(TargetChr)
		TargetChr->TakeDamage(vec2(0, 0), 20, m_Owner, m_Type);
}
void CPlasma::Tick()
{
	if(m_Type == WEAPON_HAMMER)
		HammerPlasma();
	else if(m_Type == WEAPON_RIFLE)
		RiflePlasma();
	else
		Reset();
}
bool CPlasma::NextPos(vec2 *NextPos)
{
	CCharacter *VictimChr = GameServer()->GetPlayerChar(m_Victim);

	if(!VictimChr)
	{
		Reset();
		return false;
	}

	for(int i = 99; i >= 0; i--)
	{
		if(!GameServer()->Collision()->IntersectLine(m_Pos, VictimChr->m_Pos, 0x0, 0x0))
		{
			if(NextPos && (m_Pos.x || m_Pos.y))
				*NextPos = VictimChr->m_Pos;

			return true;
		}
		
		if(!m_NodePos[i].x && !m_NodePos[i].y)
			continue;

		if(!GameServer()->Collision()->IntersectLine(m_Pos, m_NodePos[i], 0x0, 0x0))
		{
			if(NextPos)
				*NextPos = m_NodePos[i];

			return true;
		}
	}
	return false;
}
void CPlasma::GetPath()
{
	CCharacter *OwnerChr = GameServer()->GetPlayerChar(m_Owner);
	CCharacter *VictimChr = GameServer()->GetPlayerChar(m_Victim);

	if(!OwnerChr || !VictimChr)
	{
		Reset();
		return;
	}

	if(!GameServer()->Collision()->IntersectLine(m_Pos, VictimChr->m_Pos, 0x0, 0x0))
	{
		for(int i = 0; i < 100; i++)
		{
			m_NodePos[i] = vec2(0, 0);
			TempPos[i] = vec2(0, 0);
		}
		m_NodePos[0] = m_Pos;
		TempPos[0] = m_Pos;
	}
	
	m_Nodes = 0;
	for(int i = 1; i < 100; i++)
	{
		if(TempPos[i].x || TempPos[i].y)
			continue;

		if(!GameServer()->Collision()->IntersectLine(TempPos[i-1], VictimChr->m_Pos, 0x0, 0x0))
		{
			m_LastPos = VictimChr->m_Pos;
			m_Nodes = i;
			break;
		}
		else
		{
			TempPos[i] = m_LastPos;
			m_Nodes = i;
			break;
		}
	}

	int FirstNode;
	for(int i = m_Nodes; i >= 0; i--)
	{
		if(!GameServer()->Collision()->IntersectLine(TempPos[i], m_Pos, 0x0, 0x0) && (m_Pos.x || m_Pos.y))
		{
			FirstNode = i;
			break;
		}
	}

	for(int i = FirstNode; i < 100; i++)
	{
		if(!TempPos[i].x && !TempPos[i].y)
			break;

		m_NodePos[i - FirstNode] = TempPos[i];
	}

}

void CPlasma::Snap(int SnappingClient)
{
	if(NetworkClipped(SnappingClient, m_Pos))
		return;

	CNetObj_Laser *pObj = static_cast<CNetObj_Laser *>(Server()->SnapNewItem(NETOBJTYPE_LASER, m_ID, sizeof(CNetObj_Laser)));
	if(!pObj)
		return;

	pObj->m_X = m_Type?(int)m_Pos2.x:(int)m_Pos.x;
	pObj->m_Y = m_Type?(int)m_Pos2.y:(int)m_Pos.y;
	pObj->m_FromX = (int)m_Pos.x;
	pObj->m_FromY = (int)m_Pos.y;
	pObj->m_StartTick = Server()->Tick();

	if(m_Type == WEAPON_RIFLE)
	{
		CNetObj_Laser *pObj2 = static_cast<CNetObj_Laser *>(Server()->SnapNewItem(NETOBJTYPE_LASER, m_ID2, sizeof(CNetObj_Laser)));
		if(!pObj2)
			return;

		pObj2->m_X = (int)m_Pos.x;
		pObj2->m_Y = (int)m_Pos.y;
		pObj2->m_FromX = (int)m_Pos.x;
		pObj2->m_FromY = (int)m_Pos.y;
		pObj2->m_StartTick = Server()->Tick();
	}
}


/*bool Destroy = false;
const int MAX_PICKUPS = 300;

CProjectile *apEnts[MAX_PICKUPS];
int Num = GameServer()->m_World.FindEntities(m_Pos, 1024, (CEntity**)apEnts,
				MAX_PICKUPS, CGameWorld::ENTTYPE_PICKUP);

for (int i = 0; i < Num; i++)
{
	CPickup *pTarget = apEnts[i];

	if(pTarget)
	{
		if(distance(pTarget->m_Pos, m_Pos) < 32 && pTarget->m_Type == POWERUP_ARMOR)
		{
			pTarget->Reset();
			Destroy = true;
		}
	}
}

if(Destroy)
{
	Reset();
	return;
}*/
	