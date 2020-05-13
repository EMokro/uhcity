/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <game/generated/protocol.h>
#include <game/server/gamecontext.h>
#include <game/server/entities/projectile.h>
#include "hammerkill.h"

CHammerKill::CHammerKill(CGameWorld *pGameWorld, int Owner, int Victim)
: CEntity(pGameWorld, CGameWorld::ENTTYPE_PROJECTILE)
{
	m_Owner = Owner;
	m_Victim = Victim;

	m_VictimTick = 10*50;

	GameWorld()->InsertEntity(this);
}

void CHammerKill::Reset()
{
	CCharacter *pVictim = GameServer()->GetPlayerChar(m_Victim);

	if(pVictim)
		pVictim->m_IsHammerKilled = false;

	GameServer()->m_World.DestroyEntity(this);
}

void CHammerKill::Tick()
{	
	CCharacter *Victim = GameServer()->GetPlayerChar(m_Victim);
	CCharacter *Owner = GameServer()->GetPlayerChar(m_Owner);
		
	m_VictimTick--;

	if(Victim && Owner && m_VictimTick)
	{
		for(int i = 0; i <= 360; i+=36)
		{
			vec2 TempPos = Victim->m_Pos + normalize(GetDir(pi/180 * (i+m_VictimTick))) * (m_VictimTick/3);
			new CProjectile(GameWorld(), WEAPON_HAMMER,	m_Owner, TempPos, vec2(0, 0), 1, 1, 0, 0, -1, 0);

			for(int i = 0; i < MAX_CLIENTS; i++)
			{
				if(i == m_Victim || i == m_Owner)
					continue;

				CCharacter *pVictim = GameServer()->GetPlayerChar(i);

				if(pVictim)
				{
					if(distance(pVictim->m_Pos, TempPos) < 20)
						pVictim->Die(m_Owner, WEAPON_HAMMER);
				}
			}
		}
		
		if((float)(m_VictimTick/3) < 28)
		{
			//GameServer()->CreateExplosion(Victim->m_Pos, m_Owner, WEAPON_RIFLE, false);
			Victim->Die(m_Owner, WEAPON_HAMMER);
			GameServer()->m_World.DestroyEntity(this);
			return;
		}
			
	}
	else
		Reset();

	
}