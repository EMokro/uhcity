/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <game/generated/protocol.h>
#include <game/server/gamecontext.h>
#include "projectile.h"
#include "game/server/city/plasma.h"


CProjectile::CProjectile(CGameWorld *pGameWorld, int Type, int Owner, vec2 Pos, vec2 Dir, int Span,
		int Damage, bool Explosive, float Force, int SoundImpact, int Weapon)
: CEntity(pGameWorld, CGameWorld::ENTTYPE_PROJECTILE)
{
	m_Type = Type;
	m_Pos = Pos;
	m_Direction = Dir;
	m_LifeSpan = Span;
	m_Owner = Owner;
	m_Force = Force;
	m_Damage = Damage;
	m_SoundImpact = SoundImpact;
	m_Weapon = Weapon;
	m_StartTick = Server()->Tick();
	m_Explosive = Explosive;

	m_Bounces = 0;

	GameWorld()->InsertEntity(this);
}

void CProjectile::Reset()
{
	GameServer()->m_World.DestroyEntity(this);
}

vec2 CProjectile::GetPos(float Time)
{
	float Curvature = 0;
	float Speed = 0;

	switch(m_Type)
	{
		case WEAPON_GRENADE:
			Curvature = GameServer()->Tuning()->m_GrenadeCurvature;
			Speed = GameServer()->Tuning()->m_GrenadeSpeed;
			break;

		case WEAPON_SHOTGUN:
			Curvature = GameServer()->Tuning()->m_ShotgunCurvature;
			Speed = GameServer()->Tuning()->m_ShotgunSpeed;
			break;

		case WEAPON_GUN:
			Curvature = GameServer()->Tuning()->m_GunCurvature;
			Speed = GameServer()->Tuning()->m_GunSpeed;
			break;
	}


	return CalcPos(m_Pos, m_Direction, Curvature, Speed, Time);
}


void CProjectile::Tick()
{
	
	float Pt = (Server()->Tick()-m_StartTick-1)/(float)Server()->TickSpeed();
	float Ct = (Server()->Tick()-m_StartTick)/(float)Server()->TickSpeed();
	vec2 PrevPos = GetPos(Pt);
	vec2 CurPos = GetPos(Ct);
	// City
	int Collide = GameServer()->Collision()->IntersectLine(PrevPos, CurPos, &CurPos, &m_BouncePos);
	CCharacter *OwnerChar = GameServer()->GetPlayerChar(m_Owner);
	CCharacter *TargetChr = GameServer()->m_World.IntersectCharacter(PrevPos, CurPos, 6.0f, CurPos, OwnerChar);

	if(TargetChr)
	{
		if(TargetChr->GetPlayer()->m_Insta)
			return;
	}

	m_LifeSpan--;

	bool Destroy = false;
	const int MAX_PLASMA = 4*16;

	CPlasma *apEnts[MAX_PLASMA];
	int Num = GameServer()->m_World.FindEntities(CurPos, 1024, (CEntity**)apEnts,
					MAX_PLASMA, CGameWorld::ENTTYPE_PLASMA);

	for(int i = 0; i < Num; i++)
	{
		CPlasma *pTarget = apEnts[i];

		if(pTarget)
		{
			if(distance(pTarget->m_Pos, CurPos) < 16)
			{
				pTarget->Reset();
				Destroy = true;
			}
		}
	}

	int Bounces;

	// City
	if(OwnerChar && TargetChr)
	{
		if(OwnerChar->GetPlayer()->m_AccData.m_GunFreeze && m_Type == WEAPON_GUN && !TargetChr->m_God && !TargetChr->Protected())
			TargetChr->Freeze(OwnerChar->GetPlayer()->m_AccData.m_GunFreeze * Server()->TickSpeed());
	}

	if(OwnerChar)
	{
		Bounces = OwnerChar->Protected()?0:OwnerChar->GetPlayer()->m_AccData.m_GrenadeBounce;

		if(Collide && m_Weapon == WEAPON_GRENADE && m_Bounces <= Bounces)
		{
			vec2 TempPos = m_BouncePos;
			vec2 TempDir = m_Direction * 4.0f;

			GameServer()->Collision()->MovePoint(&TempPos, &TempDir, 1.0f, 0);
			m_Pos = TempPos;
			m_Direction = normalize(TempDir);
			m_StartTick = Server()->Tick();
			m_Bounces++;
		}
	}


	if(TargetChr || Collide || m_LifeSpan < 0 || GameLayerClipped(CurPos) || Destroy)
	{
		if(m_LifeSpan >= 0 || m_Weapon == WEAPON_GRENADE)
			GameServer()->CreateSound(CurPos, m_SoundImpact);

		if(m_Explosive)
			GameServer()->CreateExplosion(CurPos, m_Owner, m_Weapon, false);

		else if(TargetChr)
			TargetChr->TakeDamage(m_Direction * max(0.001f, m_Force), m_Damage, m_Owner, m_Weapon);

		if((Bounces && (!Collide || m_Weapon != WEAPON_GRENADE || m_LifeSpan < 0 || m_Bounces > Bounces || !OwnerChar)) || !Bounces || Destroy)
			GameServer()->m_World.DestroyEntity(this);
		
	}
}

void CProjectile::FillInfo(CNetObj_Projectile *pProj)
{
	pProj->m_X = (int)m_Pos.x;
	pProj->m_Y = (int)m_Pos.y;
	pProj->m_VelX = (int)(m_Direction.x*100.0f);
	pProj->m_VelY = (int)(m_Direction.y*100.0f);
	pProj->m_StartTick = m_StartTick;
	pProj->m_Type = m_Type;
}

void CProjectile::Snap(int SnappingClient)
{
	float Ct = (Server()->Tick()-m_StartTick)/(float)Server()->TickSpeed();

	if(NetworkClipped(SnappingClient, GetPos(Ct)))
		return;

	

	CCharacter *pOwner = GameServer()->GetPlayerChar(m_Owner);

	if(pOwner)
	{
		if(pOwner->m_Invisible == 2 && !Server()->IsAuthed(SnappingClient) && SnappingClient != m_Owner)
			return;
	}
	CNetObj_Projectile *pProj = static_cast<CNetObj_Projectile *>(Server()->SnapNewItem(NETOBJTYPE_PROJECTILE, m_ID, sizeof(CNetObj_Projectile)));
	if(pProj)
		FillInfo(pProj);
	


	/*if(SnappingClient == m_Owner)
	{
		CNetEvent_Spawn *ev = (CNetEvent_Spawn *)GameServer()->m_Events.Create(NETEVENTTYPE_SPAWN, sizeof(CNetEvent_Spawn));
		if(ev)
		{
			ev->m_X = (int)m_Pos.x;
			ev->m_Y = (int)m_Pos.y;
		}
	}*/
		
}
