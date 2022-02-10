/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* Copyright � 2013 Neox.                                                                                                */
/* If you are missing that file, acquire a complete release at https://www.teeworlds.com/forum/viewtopic.php?pid=106934  */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <new>
#include <engine/shared/config.h>
#include <game/server/gamecontext.h>
#include <game/mapitems.h>

#include "../laser.h"
#include "../projectile.h"
#include "monster.h"
#include "monsterdeath.h"
#include <base/math.h>

CMonster::CMonster(CGameWorld *pWorld, int Type, int MonsterID, int Health, int Armor, int Difficulty)
: CEntity(pWorld, CGameWorld::ENTTYPE_MONSTER)
{
    m_MonsterID = MonsterID;
    m_Type = Type;
	m_ProximityRadius = ms_PhysSize;
    m_MaxArmor = Armor;
	m_Armor = Armor;
    m_Health = Health;
	m_MaxHealth = Health;
	m_Difficulty = Difficulty;

	Spawn();

    m_ActiveWeapon = m_Type;

    for(int i = 0; i < ENTITY_NUM; i ++)
        m_aIDs[i] = Server()->SnapNewID();

	GameWorld()->InsertEntity(this);
}

void CMonster::Reset()
{
	GameServer()->OnMonsterDeath(m_MonsterID);
}

void CMonster::Destroy()
{
    GameWorld()->DestroyEntity(this);

    for(int i = 0; i < ENTITY_NUM; i ++)
        Server()->SnapFreeID(m_aIDs[i]);
}

void CMonster::Spawn()
{
    if(GameServer()->m_pController->m_AliveMonsters >= 10)
        return;
	if(!GameServer()->m_pController->m_MonsterSpawnNum)
		return;
	m_Pos = vec2(0, 0);
	m_Core.m_Vel = vec2(0, 0);
	m_Core.m_HookPos = vec2(0, 0);
	m_Core.m_HookDir = vec2(0, 0);
	m_Core.m_HookTick = 0;
	m_Core.m_HookState = HOOK_IDLE;
	m_Core.m_HookedPlayer = -1;
	m_Core.m_Jumped = 0;
	m_Path.m_ActualDirection = rand()%2;
	if(!m_Path.m_ActualDirection)
        m_Path.m_ActualDirection = -1;

    if(m_Path.m_ActualDirection == 1)
    {
        m_Path.m_CollideLeft = true;
        m_Path.m_CollideRight = false;
    }
    else
    {
        m_Path.m_CollideLeft = false;
        m_Path.m_CollideRight = true;
    }

    float Distance = 0;
    int FurthestNum = -1;

    for(int i = 0; i < GameServer()->m_pController->m_MonsterSpawnNum; i ++)
    {
        CMonster* pMonst = GameWorld()->ClosestMonster(GameServer()->m_pController->m_aMonsterSpawnPos[i], 100000, this);
        if(pMonst)
        {
            float Distance2 = distance(GameServer()->m_pController->m_aMonsterSpawnPos[i], pMonst->m_Pos);
            if(Distance < Distance2)
            {
                Distance = Distance2;
                FurthestNum = i;
            }
        }
    }

    vec2 SpawnPos; // Can be replaced with m_Pos, but better to read

    if(FurthestNum == -1)
    {
        FurthestNum = GameServer()->m_pController->m_MonsterSpawnCurrentNum;
        GameServer()->m_pController->m_MonsterSpawnCurrentNum ++;
        GameServer()->m_pController->m_MonsterSpawnCurrentNum %= GameServer()->m_pController->m_MonsterSpawnNum;
        bool Found = false;
        for(int i = 0; i < 4; i ++)
        {
            vec2 Pos = GameServer()->m_pController->m_aMonsterSpawnPos[FurthestNum]; // Done just because it's shorter to type
            switch(i)
            {
                case 0: Pos.x += 64.f; break;
                case 1: Pos.x -= 64.f; break;
                case 2: Pos.y += 64.f; break;
                case 3: Pos.y -= 64.f; break;
            }

            if(GameServer()->Collision()->CheckPoint(Pos))
                continue;

            CMonster* pClosest = GameWorld()->ClosestMonster(Pos, 100, this);
            if(!pClosest)
            {
                Found = true;
                SpawnPos = Pos;
                break;
            }
            if(distance(pClosest->m_Pos, Pos) >= 32)
            {
                Found = true;
                SpawnPos = Pos;
                break;
            }
        }
        if(!Found)
            SpawnPos = GameServer()->m_pController->m_aMonsterSpawnPos[FurthestNum];
    }
    else
        SpawnPos = GameServer()->m_pController->m_aMonsterSpawnPos[FurthestNum];

    m_Pos = SpawnPos;

    //GameServer()->CreateMonsterSpawn(m_Pos); ...just kidding
    GameServer()->CreatePlayerSpawn(m_Pos);
}

bool CMonster::IsGrounded()
{
	if(GameServer()->Collision()->CheckPoint(m_Pos.x+m_ProximityRadius/2, m_Pos.y+m_ProximityRadius/2+5))
		return true;
	if(GameServer()->Collision()->CheckPoint(m_Pos.x-m_ProximityRadius/2, m_Pos.y+m_ProximityRadius/2+5))
		return true;
	return false;
}


void CMonster::HandleNinja(bool IsPredicted)
{
	m_Ninja.m_CurrentMoveTime--;

	if (m_Ninja.m_CurrentMoveTime == 0)
	{
		// reset velocity
		m_Core.m_Vel = m_Ninja.m_ActivationDir*m_Ninja.m_OldVelAmount;
	}

	if (m_Ninja.m_CurrentMoveTime > 0)
	{
		// Set velocity
		m_Core.m_Vel = m_Ninja.m_ActivationDir * g_pData->m_Weapons.m_Ninja.m_Velocity;
		vec2 OldPos = m_Pos;
		GameServer()->Collision()->MoveBox(&m_Pos, &m_Core.m_Vel, vec2(m_ProximityRadius, m_ProximityRadius), 0.f);

		// reset velocity so the client doesn't predict stuff
		m_Core.m_Vel = vec2(0.f, 0.f);

		if(IsPredicted)
            return;

		// check if we Hit anything along the way
		{
			CCharacter *aEnts[MAX_CLIENTS];
			vec2 Dir = m_Pos - OldPos;
			float Radius = m_ProximityRadius * 2.0f;
			vec2 Center = OldPos + Dir * 0.5f;
			int Num = GameServer()->m_World.FindEntities(Center, Radius, (CEntity**)aEnts, MAX_CLIENTS, CGameWorld::ENTTYPE_CHARACTER);

			for (int i = 0; i < Num; ++i)
			{
				// make sure we haven't Hit this object before
				bool bAlreadyHit = false;
				for (int j = 0; j < m_NumObjectsHit; j++)
				{
					if (m_apHitObjects[j] == aEnts[i])
						bAlreadyHit = true;
				}
				if (bAlreadyHit)
					continue;

				// check so we are sufficiently close
				if (distance(aEnts[i]->m_Pos, m_Pos) > (m_ProximityRadius * 2.0f))
					continue;

				// Hit a player, give him damage and stuffs...
				GameServer()->CreateSound(aEnts[i]->m_Pos, SOUND_NINJA_HIT);
				// set his velocity to fast upward (for now)
				if(m_NumObjectsHit < 10)
					m_apHitObjects[m_NumObjectsHit++] = aEnts[i];

                int Damage = g_pData->m_Weapons.m_Ninja.m_pBase->m_Damage;

                Damage += (m_Difficulty - 1) * 3; // + 3 damage every 3 difficulty

				aEnts[i]->TakeDamage(vec2(0, -10.0f), Damage, m_MonsterID, m_ActiveWeapon, true);
			}

			CMonster *aMonsts[MAX_MONSTERS];
			int Num2 = GameServer()->m_World.FindEntities(Center, Radius, (CEntity**)aMonsts, MAX_MONSTERS, CGameWorld::ENTTYPE_MONSTER);

			for (int i = 0; i < Num2; ++i)
			{
			    if(aMonsts[i]->m_MonsterID == m_MonsterID)
                    continue;

				// make sure we haven't Hit this object before
				bool bAlreadyHit = false;
				for (int j = 0; j < m_NumObjectsHit; j++)
				{
					if (m_apHitObjects[j] == aMonsts[i])
						bAlreadyHit = true;
				}
				if (bAlreadyHit)
					continue;

				// check so we are sufficiently close
				if (distance(aMonsts[i]->m_Pos, m_Pos) > (m_ProximityRadius * 2.0f))
					continue;

				// Hit a player, give him damage and stuffs...
				GameServer()->CreateSound(aMonsts[i]->m_Pos, SOUND_NINJA_HIT);
				// set his velocity to fast upward (for now)
				if(m_NumObjectsHit < 10)
					m_apHitObjects[m_NumObjectsHit++] = aMonsts[i];

				aMonsts[i]->TakeDamage(vec2(0, -10.0f), g_pData->m_Weapons.m_Ninja.m_pBase->m_Damage, m_MonsterID, m_ActiveWeapon, true);
			}
		}
	}
}

void CMonster::Move()
{
	float RampValue = VelocityRamp(length(m_Core.m_Vel)*50, GameServer()->m_World.m_Core.m_Tuning.m_VelrampStart, GameServer()->m_World.m_Core.m_Tuning.m_VelrampRange, GameServer()->m_World.m_Core.m_Tuning.m_VelrampCurvature);

	m_Core.m_Vel.x = m_Core.m_Vel.x*RampValue;

	GameServer()->Collision()->MoveBox(&m_Pos, &m_Core.m_Vel, vec2(m_ProximityRadius, m_ProximityRadius), 0);

	m_Core.m_Vel.x = m_Core.m_Vel.x*(1.0f/RampValue);
}

void CMonster::HandleCore()
{
	float PhysSize = m_ProximityRadius;

	// get ground state
	bool Grounded = IsGrounded();

	CCharacter* pVict = GameWorld()->ClosestCharacter(m_Pos, 10000, 0x0, false);

	vec2 TargetDirection = pVict ? normalize(pVict->m_Pos - m_Pos) : vec2(0, 0);

	m_Core.m_Vel.y += GameServer()->m_World.m_Core.m_Tuning.m_Gravity;

	float MaxSpeed = Grounded ? GameServer()->m_World.m_Core.m_Tuning.m_GroundControlSpeed : GameServer()->m_World.m_Core.m_Tuning.m_AirControlSpeed;
	float Accel = Grounded ? GameServer()->m_World.m_Core.m_Tuning.m_GroundControlAccel : GameServer()->m_World.m_Core.m_Tuning.m_AirControlAccel;
	float Friction = Grounded ? GameServer()->m_World.m_Core.m_Tuning.m_GroundFriction : GameServer()->m_World.m_Core.m_Tuning.m_AirFriction;

   // handle jump
    if(m_WillJump)
    {
        if(m_Core.m_Jumped < 2)
        {
            if(Grounded)
            {
                m_Core.m_Vel.y = -GameServer()->m_World.m_Core.m_Tuning.m_GroundJumpImpulse;
                m_Core.m_Jumped = 1;
                GameServer()->CreateSound(m_Pos, SOUND_PLAYER_JUMP);
            }
            else
            {
                m_Core.m_Vel.y = -GameServer()->m_World.m_Core.m_Tuning.m_AirJumpImpulse;
                m_Core.m_Jumped = 2;
                GameServer()->CreateSound(m_Pos, SOUND_PLAYER_AIRJUMP);
            }
        }
    }
    else
    {
        if(Grounded)
            m_Core.m_Jumped = 0;
        else if(m_Core.m_Jumped < 1)
            m_Core.m_Jumped = 1;
    }

    // handle hook
    if(m_WillHook)
    {
        if(m_Core.m_HookState == HOOK_IDLE)
        {
            m_Core.m_HookState = HOOK_FLYING;
            m_Core.m_HookPos = m_Pos+TargetDirection*PhysSize*1.5f;
            m_Core.m_HookDir = TargetDirection;
            m_Core.m_HookedPlayer = -1;
            m_Core.m_HookTick = 0;
        }
    }
    else
    {
        m_Core.m_HookedPlayer = -1;
        m_Core.m_HookState = HOOK_IDLE;
        m_Core.m_HookPos = m_Pos;
    }

	// add the speed modification according to players wanted direction
	if(m_Path.m_Direction < 0)
		m_Core.m_Vel.x = SaturatedAdd(-MaxSpeed, MaxSpeed, m_Core.m_Vel.x, -Accel);
	if(m_Path.m_Direction > 0)
		m_Core.m_Vel.x = SaturatedAdd(-MaxSpeed, MaxSpeed, m_Core.m_Vel.x, Accel);
	if(m_Path.m_Direction == 0)
		m_Core.m_Vel.x *= Friction;

	// handle jumping
	// 1 bit = to keep track if a jump has been made on this input
	// 2 bit = to keep track if a air-jump has been made
	if(Grounded)
		m_Core.m_Jumped &= ~2;

	// do hook
	if(m_Core.m_HookState == HOOK_IDLE)
	{
		m_Core.m_HookedPlayer = -1;
		m_Core.m_HookState = HOOK_IDLE;
		m_Core.m_HookPos = m_Pos;
	}
	else if(m_Core.m_HookState >= HOOK_RETRACT_START && m_Core.m_HookState < HOOK_RETRACT_END)
	{
		m_Core.m_HookState++;
	}
	else if(m_Core.m_HookState == HOOK_RETRACT_END)
	{
		m_Core.m_HookState = HOOK_RETRACTED;
	}
	else if(m_Core.m_HookState == HOOK_FLYING)
	{
		vec2 NewPos = m_Core.m_HookPos+m_Core.m_HookDir*GameServer()->m_World.m_Core.m_Tuning.m_HookFireSpeed;
		if(distance(m_Pos, NewPos) > GameServer()->m_World.m_Core.m_Tuning.m_HookLength)
		{
			m_Core.m_HookState = HOOK_RETRACT_START;
			NewPos = m_Pos + normalize(NewPos-m_Pos) * GameServer()->m_World.m_Core.m_Tuning.m_HookLength;
		}

		// make sure that the hook doesn't go though the ground
		bool GoingToHitGround = false;
		bool GoingToRetract = false;
		int Hit = GameServer()->Collision()->IntersectLine(m_Core.m_HookPos, NewPos, &NewPos, 0);
		if(Hit)
		{
			if(Hit&CCollision::COLFLAG_NOHOOK)
				GoingToRetract = true;
			else
				GoingToHitGround = true;
		}

		// Check against other players first
        float Distance = 0.0f;
        for(int i = 0; i < MAX_CLIENTS; i++)
        {
            CCharacter *pChar = GameServer()->GetPlayerChar(i);
            if(!pChar)
                continue;

            vec2 ClosestPoint = closest_point_on_line(m_Core.m_HookPos, NewPos, pChar->m_Pos);
            if(distance(pChar->m_Pos, ClosestPoint) < PhysSize+2.0f)
            {
                if (m_Core.m_HookedPlayer == -1 || distance(m_Core.m_HookPos, pChar->m_Pos) < Distance)
                {
                    m_Core.m_HookState = HOOK_GRABBED;
                    m_Core.m_HookedPlayer = i;
                    Distance = distance(m_Core.m_HookPos, pChar->m_Pos);
                    GameServer()->CreateSound(m_Pos, SOUND_HOOK_ATTACH_PLAYER);
                }
            }
        }

		if(m_Core.m_HookState == HOOK_FLYING)
		{
			// check against ground
			if(GoingToHitGround)
				m_Core.m_HookState = HOOK_GRABBED;
			else if(GoingToRetract)
				m_Core.m_HookState = HOOK_RETRACT_START;

			m_Core.m_HookPos = NewPos;
		}
	}

	if(m_Core.m_HookState == HOOK_GRABBED)
	{
		if(m_Core.m_HookedPlayer != -1)
		{
			CCharacter *pChar = GameServer()->GetPlayerChar(m_Core.m_HookedPlayer);
			if(pChar)
				m_Core.m_HookPos = pChar->m_Pos;
			else
			{
				// release hook
				m_Core.m_HookedPlayer = -1;
				m_Core.m_HookState = HOOK_RETRACTED;
				m_Core.m_HookPos = m_Pos;
			}

			// keep players hooked for a max of 1.5sec
			//if(Server()->Tick() > hook_tick+(Server()->TickSpeed()*3)/2)
				//release_hooked();
		}
		else
            m_WillHook = false;

		// don't do this hook rutine when we are hook to a player
		if(m_Core.m_HookedPlayer == -1 && distance(m_Core.m_HookPos, m_Pos) > 46.0f)
		{
			vec2 HookVel = normalize(m_Core.m_HookPos-m_Pos)*GameServer()->m_World.m_Core.m_Tuning.m_HookDragAccel;
			// the hook as more power to drag you up then down.
			// this makes it easier to get on top of an platform
			if(HookVel.y > 0)
				HookVel.y *= 0.3f;

			// the hook will boost it's power if the player wants to move
			// in that direction. otherwise it will dampen everything abit
			if((HookVel.x < 0 && m_Path.m_Direction < 0) || (HookVel.x > 0 && m_Path.m_Direction > 0))
				HookVel.x *= 0.95f;
			else
				HookVel.x *= 0.75f;

			vec2 NewVel = m_Core.m_Vel+HookVel;

			// check if we are under the legal limit for the hook
			if(length(NewVel) < GameServer()->m_World.m_Core.m_Tuning.m_HookDragSpeed || length(NewVel) < length(m_Core.m_Vel))
				m_Core.m_Vel = NewVel; // no problem. apply

		}

		// release hook (max hook time is 1.25
		m_Core.m_HookTick++;
		if(m_Core.m_HookedPlayer != -1 && (m_Core.m_HookTick > Server()->TickSpeed()+Server()->TickSpeed()/5 || !GameServer()->GetPlayerChar(m_Core.m_HookedPlayer)))
		{
			m_Core.m_HookedPlayer = -1;
			m_Core.m_HookState = HOOK_RETRACTED;
			m_Core.m_HookPos = m_Pos;
		}
	}

	if(m_Core.m_HookState == HOOK_RETRACTED)
        m_Core.m_HookState = HOOK_IDLE;

    CCharacter *pChar = GameServer()->GetPlayerChar(m_Core.m_HookedPlayer);

    if(pChar)
    {
        float Distance = distance(m_Pos, pChar->m_Pos);
        vec2 Dir = normalize(m_Pos - pChar->m_Pos);

        if(Distance > PhysSize*1.50f) // TODO: fix tweakable variable
        {
            float Accel = GameServer()->m_World.m_Core.m_Tuning.m_HookDragAccel * (Distance/GameServer()->m_World.m_Core.m_Tuning.m_HookLength);
            float DragSpeed = GameServer()->m_World.m_Core.m_Tuning.m_HookDragSpeed;

            // add force to the hooked player
            pChar->m_Core.m_Vel.x = SaturatedAdd(-DragSpeed, DragSpeed, pChar->m_Core.m_Vel.x, Accel*Dir.x*1.5f);
            pChar->m_Core.m_Vel.y = SaturatedAdd(-DragSpeed, DragSpeed, pChar->m_Core.m_Vel.y, Accel*Dir.y*1.5f);

            // add a little bit force to the guy who has the grip
            m_Core.m_Vel.x = SaturatedAdd(-DragSpeed, DragSpeed, m_Core.m_Vel.x, -Accel*Dir.x*0.25f);
            m_Core.m_Vel.y = SaturatedAdd(-DragSpeed, DragSpeed, m_Core.m_Vel.y, -Accel*Dir.y*0.25f);
        }
    }

	// clamp the velocity to something sane
	if(length(m_Core.m_Vel) > 6000)
		m_Core.m_Vel = normalize(m_Core.m_Vel) * 6000;

    if(m_Core.m_HookState == HOOK_GRABBED && m_Core.m_HookedPlayer == -1)
    {
        m_Core.m_HookState = HOOK_IDLE;
		m_Core.m_HookPos = m_Pos;
    }
}

void CMonster::HandleWeapons()
{
    m_WillFire = false;
    HandleNinja();

	if(m_ReloadTimer)
        m_ReloadTimer --;

    if(!m_ReloadTimer)
    {
        CCharacter* pVict = GameWorld()->ClosestCharacter(m_Pos, 10000, 0x0, false);
        if(pVict)
        {
            m_WillFire = true;
            if(pVict->m_Pos.x - m_Pos.x > 400 && m_ActiveWeapon == WEAPON_GRENADE)
            {
                if(IsGrounded())
                    m_Dir = vec2(32 * (m_Path.m_Direction * -2), 32);
                else
                {
                    m_Pos.y += 5;
                    if(IsGrounded())
                        m_Dir = vec2(32 * (m_Path.m_Direction * -2), 32);
                    else
                        m_WillFire = false;
                    m_Pos.y -= 5;
                }
            }
            else
                m_Dir = pVict->m_Pos - m_Pos;
        }
        else
        {
            m_Dir = vec2(0, 0);
            if(m_Type == TYPE_NINJA && g_Config.m_HardNinja)
                OnPredictedNinja();
        }
    }

    if(m_Dir.x != 0 || m_Dir.y != 0)
        FireWeapon();
}

void CMonster::OnPredictedNinja() // Ninja's smart part =)
{
    CCharacter* pClosest = GameWorld()->ClosestCharacter(m_Pos, 1000, 0x0);

    if(pClosest)
    {
        vec2 OldVel = m_Core.m_Vel;
        vec2 OldPos = m_Pos;
        vec2 OldActivationDir = m_Ninja.m_ActivationDir;
        int OldCurrentMoveTime = m_Ninja.m_CurrentMoveTime;
        int OldVelAmount = m_Ninja.m_OldVelAmount;
        vec2 Diff = pClosest->m_Pos - m_Pos;
        Diff.y = 0;
        if(Diff.x > 0)
            Diff.x = 1;
        if(Diff.x < 0)
            Diff.x = -1;
        bool DirChanged = false;
        if(Diff.x != m_Path.m_Direction)
        {
            DirChanged = true;
            ChangeDir();
            m_Path.m_Direction = Diff.x;
        }
        m_Ninja.m_ActivationDir = normalize(Diff);
        m_Ninja.m_CurrentMoveTime = g_pData->m_Weapons.m_Ninja.m_Movetime * Server()->TickSpeed() / 1000;
        m_Ninja.m_OldVelAmount = length(m_Core.m_Vel);
        for(int PredictedTicks = 0; PredictedTicks < Server()->TickSpeed(); PredictedTicks ++)
        {
            HandleNinja(true);

            Move();

            if(GameWorld()->ClosestCharacter(m_Pos, 10000, 0x0, false))
            {
                m_Dir = Diff;
                m_Core.m_Vel = OldVel;
                m_Pos = OldPos;
                m_Ninja.m_ActivationDir = OldActivationDir;
                m_Ninja.m_CurrentMoveTime = OldCurrentMoveTime;
                m_Ninja.m_OldVelAmount = OldVelAmount;
                m_WillFire = true;
                return;
            }
        }
        if(DirChanged)
        {
            ChangeDir();
            m_Path.m_Direction = -Diff.x;
        }
        m_Core.m_Vel = OldVel;
        m_Pos = OldPos;
        m_Ninja.m_ActivationDir = OldActivationDir;
        m_Ninja.m_CurrentMoveTime = OldCurrentMoveTime;
        m_Ninja.m_OldVelAmount = OldVelAmount;
        Diff = pClosest->m_Pos - m_Pos;
        Diff.x = 0;
        m_Ninja.m_ActivationDir = normalize(Diff);
        m_Ninja.m_CurrentMoveTime = g_pData->m_Weapons.m_Ninja.m_Movetime * Server()->TickSpeed() / 1000;
        m_Ninja.m_OldVelAmount = length(m_Core.m_Vel);
        for(int PredictedTicks = 0; PredictedTicks < Server()->TickSpeed(); PredictedTicks ++)
        {
            HandleNinja(true);

            Move();

            if(GameWorld()->ClosestCharacter(m_Pos, 10000, 0x0, false))
            {
                m_Dir = Diff;
                m_Core.m_Vel = OldVel;
                m_Pos = OldPos;
                m_Ninja.m_ActivationDir = OldActivationDir;
                m_Ninja.m_CurrentMoveTime = OldCurrentMoveTime;
                m_Ninja.m_OldVelAmount = OldVelAmount;
                m_WillFire = true;
                return;
            }
        }
        m_Core.m_Vel = OldVel;
        m_Pos = OldPos;
        m_Ninja.m_ActivationDir = OldActivationDir;
        m_Ninja.m_CurrentMoveTime = OldCurrentMoveTime;
        m_Ninja.m_OldVelAmount = OldVelAmount;
        m_Dir = vec2(0, 0);
    }
}

void CMonster::FireWeapon()
{
	if(m_ReloadTimer != 0 || !m_WillFire)
		return;

    vec2 Direction = normalize(m_Dir);

	vec2 ProjStartPos = m_Pos+Direction*m_ProximityRadius*0.75f;

	switch(m_ActiveWeapon)
	{
		case WEAPON_HAMMER:
		{
			// reset objects Hit
			m_NumObjectsHit = 0;
			GameServer()->CreateSound(m_Pos, SOUND_HAMMER_FIRE);

			CCharacter *apEnts[MAX_CLIENTS];
			int Hits = 0;
			int Num = GameServer()->m_World.FindEntities(ProjStartPos, m_ProximityRadius*0.5f, (CEntity**)apEnts,
														MAX_CLIENTS, CGameWorld::ENTTYPE_CHARACTER);

			for (int i = 0; i < Num; ++i)
			{
				CCharacter *pTarget = apEnts[i];

				if (GameServer()->Collision()->IntersectLine(ProjStartPos, pTarget->m_Pos, NULL, NULL))
					continue;

				// set his velocity to fast upward (for now)
				if(length(pTarget->m_Pos-ProjStartPos) > 0.0f)
					GameServer()->CreateHammerHit(pTarget->m_Pos-normalize(pTarget->m_Pos-ProjStartPos)*m_ProximityRadius*0.5f);
				else
					GameServer()->CreateHammerHit(ProjStartPos);

				vec2 Dir;
				if (length(pTarget->m_Pos - m_Pos) > 0.0f)
					Dir = normalize(pTarget->m_Pos - m_Pos);
				else
					Dir = vec2(0.f, -1.f);

				pTarget->TakeDamage(vec2(0.f, -1.f) + normalize(Dir + vec2(0.f, -1.1f)) * 10.0f, g_pData->m_Weapons.m_Hammer.m_pBase->m_Damage, m_MonsterID, m_ActiveWeapon, true);
				Hits++;
			}

			CMonster *apMonsts[MAX_MONSTERS];
			int Num2 = GameServer()->m_World.FindEntities(ProjStartPos, m_ProximityRadius*0.5f, (CEntity**)apMonsts,
														MAX_MONSTERS, CGameWorld::ENTTYPE_MONSTER);

			for (int i = 0; i < Num2; ++i)
			{
				CMonster *pTarget = apMonsts[i];

				if(!pTarget)
					continue;

				if(pTarget == this)
                    continue;

				if(GameServer()->Collision()->IntersectLine(ProjStartPos, pTarget->m_Pos, NULL, NULL))
					continue;

				// set his velocity to fast upward (for now)
				if(length(pTarget->m_Pos-ProjStartPos) > 0.0f)
					GameServer()->CreateHammerHit(pTarget->m_Pos-normalize(pTarget->m_Pos-ProjStartPos)*m_ProximityRadius*0.5f);
				else
					GameServer()->CreateHammerHit(ProjStartPos);

				vec2 Dir;
				if (length(pTarget->m_Pos - m_Pos) > 0.0f)
					Dir = normalize(pTarget->m_Pos - m_Pos);
				else
					Dir = vec2(0.f, -1.f);

				pTarget->TakeDamage(vec2(0.f, -1.f) + normalize(Dir + vec2(0.f, -1.1f)) * 10.0f, g_pData->m_Weapons.m_Hammer.m_pBase->m_Damage, m_MonsterID, m_ActiveWeapon, true);
				Hits++;
			}

			// if we Hit anything, we have to wait for the reload
			if(Hits)
				m_ReloadTimer = Server()->TickSpeed()/3;

		} break;

		case WEAPON_GUN:
		{
		    int ShotSpread = 0;

            if(m_Difficulty >= 4)
                ShotSpread = 2;
		    else if(m_Difficulty >= 2)
                ShotSpread = 1;

			CMsgPacker Msg(NETMSGTYPE_SV_EXTRAPROJECTILE);

            Msg.AddInt(ShotSpread*2+1);

            if(ShotSpread)
            {
                for(int i = -ShotSpread; i <= ShotSpread; ++i)
                {
                    float Spreading[5] = {-0.185f, -0.070f, 0, 0.070f, 0.185f};
                    Spreading[0] = i * 0.1f;

                    float a = GetAngle(Direction);

                    a += Spreading[0];
                    float v = 1-(absolute(i)/(float)ShotSpread);
                    float Speed = mix((float)GameServer()->Tuning()->m_ShotgunSpeeddiff, 1.0f, v);
                    CProjectile *pProj = new CProjectile(GameWorld(), WEAPON_GUN,
                        m_MonsterID,
                        ProjStartPos,
                        vec2(cosf(a), sinf(a))*Speed,
                        (int)(Server()->TickSpeed()*GameServer()->Tuning()->m_GunLifetime),
                        1, 0, 0, -1, WEAPON_GUN, true);

                    CNetObj_Projectile p;
                    pProj->FillInfo(&p);

                    for(unsigned i = 0; i < sizeof(CNetObj_Projectile)/sizeof(int); i++)
                        Msg.AddInt(((int *)&p)[i]);
                }
            }
            else
            {
                CProjectile* pProj = new CProjectile(GameWorld(), WEAPON_GUN,
                    m_MonsterID,
                    ProjStartPos,
                    Direction,
                    (int)(Server()->TickSpeed()*GameServer()->Tuning()->m_GunLifetime),
                    1, 0, 0, -1, WEAPON_GUN, true);

                    CNetObj_Projectile p;
                    pProj->FillInfo(&p);

                    for(unsigned i = 0; i < sizeof(CNetObj_Projectile)/sizeof(int); i++)
                        Msg.AddInt(((int *)&p)[i]);
            }

			Server()->SendMsg(&Msg, 0, -1);

			GameServer()->CreateSound(m_Pos, SOUND_GUN_FIRE);
		} break;

		case WEAPON_SHOTGUN:
		{
		    int Spread = 0;
		    if(m_Difficulty >= 4)
                Spread = 4;
            else if(m_Difficulty >= 2)
                Spread = 2;
			int ShotSpread = Spread + 2;

			CMsgPacker Msg(NETMSGTYPE_SV_EXTRAPROJECTILE);
			Msg.AddInt(ShotSpread*2+1);

			for(int i = -ShotSpread; i <= ShotSpread; ++i)
			{
				float Spreading[] = {-0.185f, -0.070f, 0, 0.070f, 0.185f};
                if(Spread)
                    Spreading[0] = i * 0.1f;

                float a = GetAngle(Direction);

                if(Spread)
                    a += Spreading[0];
                else
                    a += Spreading[i+2];
				float v = 1-(absolute(i)/(float)ShotSpread);
				float Speed = mix((float)GameServer()->Tuning()->m_ShotgunSpeeddiff, 1.0f, v);
				CProjectile *pProj = new CProjectile(GameWorld(), WEAPON_SHOTGUN,
					m_MonsterID,
					ProjStartPos,
					vec2(cosf(a), sinf(a))*Speed,
					(int)(Server()->TickSpeed()*GameServer()->Tuning()->m_ShotgunLifetime),
					1, 0, 0, -1, WEAPON_SHOTGUN, true);

				// pack the Projectile and send it to the client Directly
				CNetObj_Projectile p;
				pProj->FillInfo(&p);

				for(unsigned i = 0; i < sizeof(CNetObj_Projectile)/sizeof(int); i++)
					Msg.AddInt(((int *)&p)[i]);
			}

			Server()->SendMsg(&Msg, 0, -1);

			GameServer()->CreateSound(m_Pos, SOUND_SHOTGUN_FIRE);
		} break;

		case WEAPON_GRENADE:
		{
			int GrenadeSpread = 0;
		    if(m_Difficulty >= 4)
                GrenadeSpread = 2;
            else if(m_Difficulty >= 2)
                GrenadeSpread = 1;
			CMsgPacker Msg(NETMSGTYPE_SV_EXTRAPROJECTILE);

            Msg.AddInt(GrenadeSpread);

            if(GrenadeSpread)
            {
                for(int i = -GrenadeSpread; i <= GrenadeSpread; ++i)
                {
                    float Spreading[5] = {-0.185f, -0.070f, 0, 0.070f, 0.185f};
                    Spreading[0] = i * 0.2f;

                    float a = GetAngle(Direction);

                    a += Spreading[0];
                    float v = 1-(absolute(i)/(float)GrenadeSpread);
                    float Speed = mix((float)GameServer()->Tuning()->m_ShotgunSpeeddiff, 1.0f, v);
                    CProjectile *pProj = 0;

                    pProj = new CProjectile(GameWorld(), WEAPON_GRENADE,
                    m_MonsterID,
                    ProjStartPos,
                    vec2(cosf(a), sinf(a))*Speed,
                    (int)(Server()->TickSpeed()*GameServer()->Tuning()->m_GrenadeLifetime),
                    1, true, 0, SOUND_GRENADE_EXPLODE, WEAPON_GRENADE, true);

                    // pack the Projectile and send it to the client Directly
                    CNetObj_Projectile p;
                    pProj->FillInfo(&p);

                    for(unsigned i = 0; i < sizeof(CNetObj_Projectile)/sizeof(int); i++)
                        Msg.AddInt(((int *)&p)[i]);
                }
            }
            else
            {
                CProjectile *pProj = new CProjectile(GameWorld(), WEAPON_GRENADE,
                    m_MonsterID,
                    ProjStartPos,
                    Direction,
                    (int)(Server()->TickSpeed()*GameServer()->Tuning()->m_GrenadeLifetime),
                    1, true, 0, SOUND_GRENADE_EXPLODE, WEAPON_GRENADE, true);

                // pack the Projectile and send it to the client Directly
                CNetObj_Projectile p;
                pProj->FillInfo(&p);

                for(unsigned i = 0; i < sizeof(CNetObj_Projectile)/sizeof(int); i++)
                    Msg.AddInt(((int *)&p)[i]);
            }
			Server()->SendMsg(&Msg, 0, -1);

			GameServer()->CreateSound(m_Pos, SOUND_GRENADE_FIRE);
		} break;

		case WEAPON_RIFLE:
		{
		    int ShotSpread = 0;
		    if(m_Difficulty >= 4)
                ShotSpread = 3;
            else if(m_Difficulty >= 2)
                ShotSpread = 1;
			if(ShotSpread)
            {
                for(int i = -ShotSpread; i <= ShotSpread; ++i)
                {
                    float Spreading = i * 0.1f;

                    float a = GetAngle(Direction);
                    a += Spreading;
                    float v = 1-(absolute(i)/(float)ShotSpread);
                    float Speed = mix((float)GameServer()->Tuning()->m_ShotgunSpeeddiff, 1.0f, v);
                    new CLaser(GameWorld(), m_Pos, vec2(cosf(a), sinf(a))*Speed, GameServer()->Tuning()->m_LaserReach, m_MonsterID, true);
                }
            }
            else
                new CLaser(GameWorld(), m_Pos, Direction, GameServer()->Tuning()->m_LaserReach, m_MonsterID, true);
			GameServer()->CreateSound(m_Pos, SOUND_RIFLE_FIRE);
		} break;

		case WEAPON_NINJA:
		{
			// reset Hit objects
			m_NumObjectsHit = 0;

			m_Ninja.m_ActivationDir = Direction;
			m_Ninja.m_CurrentMoveTime = g_pData->m_Weapons.m_Ninja.m_Movetime * Server()->TickSpeed() / 1000;
			m_Ninja.m_OldVelAmount = length(m_Core.m_Vel);

			GameServer()->CreateSound(m_Pos, SOUND_NINJA_FIRE);
		} break;
	}

	if(!m_ReloadTimer)
		m_ReloadTimer = g_pData->m_Weapons.m_aId[m_ActiveWeapon].m_Firedelay * Server()->TickSpeed() / 1000;

    if(m_Difficulty >= 4)
        m_ReloadTimer /= clamp(m_Difficulty - 2, 2, 4);
}

void CMonster::Tick()
{
	// handle death-tiles and leaving gamelayer
	if(GameServer()->Collision()->GetCollisionAt(m_Pos.x+m_ProximityRadius/3.f, m_Pos.y-m_ProximityRadius/3.f)&CCollision::COLFLAG_DEATH ||
		GameServer()->Collision()->GetCollisionAt(m_Pos.x+m_ProximityRadius/3.f, m_Pos.y+m_ProximityRadius/3.f)&CCollision::COLFLAG_DEATH ||
		GameServer()->Collision()->GetCollisionAt(m_Pos.x-m_ProximityRadius/3.f, m_Pos.y-m_ProximityRadius/3.f)&CCollision::COLFLAG_DEATH ||
		GameServer()->Collision()->GetCollisionAt(m_Pos.x-m_ProximityRadius/3.f, m_Pos.y+m_ProximityRadius/3.f)&CCollision::COLFLAG_DEATH ||
		GameLayerClipped(m_Pos))
	{
		Die(-1);
		return;
	}

    for(int i = 0; i < ENTITY_NUM; i ++)
        m_aSnapPos[i] = m_Pos + normalize(GetDir(pi/180 * ((Server()->Tick() + i*(360/ENTITY_NUM)%360+1) * ENTITY_SPEED)))*(m_ProximityRadius/2);

	HandleActions();
	HandleWeapons();
	HandleCore();
	Move();
}

void CMonster::HandleActions() // This is the monsters AI, it has been decreased because if too many calculations, it creates hard lags/crashes
{
    if(m_Path.m_LastPos.x == m_Pos.x && m_Path.m_LastPos.y == m_Pos.y) // This is done because monsters are SOMETIMES stuck on a corner and can't move because of it
    {
        m_Path.m_StuckTick ++;
        if(m_Path.m_StuckTick >= Server()->TickSpeed() * 3)
        {
            ChangeDir();
            m_Path.m_StuckTick = 0;
        }
    }
    else
        m_Path.m_StuckTick = 0;

    m_Path.m_LastPos = m_Pos;

	const int DIRECTION_RIGHT = 1;
	const int DIRECTION_LEFT = -1;
	const int DIRECTION_ZERO = 0;
	const int TILES_TO_CHECK = 7;
	const int LINES_TO_CHECK = 30;
	const int JUMP_LIMIT = 11;
	//const int AIRJUMP_LIMIT = 6;
	const int TS = 32; // TS = TileSize, not TeamSpeak.
    const float MaxSpeed = IsGrounded() ? GameServer()->m_World.m_Core.m_Tuning.m_GroundControlSpeed : GameServer()->m_World.m_Core.m_Tuning.m_AirControlSpeed;
    const float Accel = IsGrounded() ? GameServer()->m_World.m_Core.m_Tuning.m_GroundControlAccel : GameServer()->m_World.m_Core.m_Tuning.m_AirControlAccel;
    const float Friction = IsGrounded() ? GameServer()->m_World.m_Core.m_Tuning.m_GroundFriction : GameServer()->m_World.m_Core.m_Tuning.m_AirFriction;

	m_WillJump = false;

    CCharacter* pVict = GameWorld()->ClosestCharacter(m_Pos, 10000, 0x0, false);

    if(m_Path.m_CollideRight)
        m_Path.m_ActualDirection = DIRECTION_LEFT;

    if(m_Path.m_CollideLeft)
        m_Path.m_ActualDirection = DIRECTION_RIGHT;

    for(int j = 0; j < 2; j ++)
    {
        if(GameServer()->Collision()->IntersectLine(m_Pos, vec2(m_Pos.x + TS * (j + 1), m_Pos.y), 0x0, 0x0)) // Check collision on the right
        {
            if(CanJump())
                Jump();
            vec2 At;
            bool FoundEmpty = GameServer()->Collision()->EmptyOnLine(vec2(m_Pos.x + TS, m_Pos.y), vec2(m_Pos.x + TS, m_Pos.y - TS * TILES_TO_CHECK), &At, 0x0);
            //if(!FoundEmpty) // This makes monsters going on the right and left, but not on the center, just try it by adding it back
                //FoundEmpty = GameServer()->Collision()->EmptyOnLine(vec2(m_Pos.x + TS, m_Pos.y), vec2(m_Pos.x + TS, m_Pos.y + TS * TILES_TO_CHECK), &At, 0x0);
            if(FoundEmpty && j == 1)
            {
                if(GameServer()->Collision()->CheckPoint(At + vec2(-TS, 0)))
                    FoundEmpty = false;
            }
            if(GameServer()->Collision()->IntersectLine(m_Pos, vec2(m_Pos.x + TS * (j + 1), m_Pos.y), 0x0, 0x0) && !FoundEmpty && m_Core.m_Jumped >= 2)
            {
                m_Path.m_CollideRight = true;
                m_Path.m_CollideLeft = false;
            }
            if(!FoundEmpty) // We don't need to check again
                break;
        }
        else if(GameServer()->Collision()->IntersectLine(m_Pos, vec2(m_Pos.x - TS * (j + 1), m_Pos.y), 0x0, 0x0)) // Check collision on the left
        {
            if(CanJump())
                Jump();
            vec2 At;
            bool FoundEmpty = GameServer()->Collision()->EmptyOnLine(vec2(m_Pos.x - TS, m_Pos.y), vec2(m_Pos.x - TS, m_Pos.y - TS * TILES_TO_CHECK), &At, 0x0);
            //if(!FoundEmpty) // This makes monsters going on the right and left, but not on the center, just try it by adding it back
                //FoundEmpty = GameServer()->Collision()->EmptyOnLine(vec2(m_Pos.x - TS, m_Pos.y), vec2(m_Pos.x - TS, m_Pos.y + TS * TILES_TO_CHECK), &At, 0x0);
            if(FoundEmpty && j == 1)
            {
                if(GameServer()->Collision()->CheckPoint(At + vec2(TS, 0)))
                    FoundEmpty = false;
            }
            if(GameServer()->Collision()->IntersectLine(m_Pos, vec2(m_Pos.x - TS * (j + 1), m_Pos.y), 0x0, 0x0) && !FoundEmpty && m_Core.m_Jumped >= 2)
            {
                m_Path.m_CollideRight = false;
                m_Path.m_CollideLeft = true;
            }
            if(!FoundEmpty) // We don't need to check again
                break;
        }
    }

    if(m_Path.m_CollideLeft)
    {
        if(!GameServer()->Collision()->CheckPoint(m_Pos + vec2(TS, TS))) // Check if the next tile under the monster is not a solid tile
        {
            bool KeepDirection = false;
            for(int i = 1; i < LINES_TO_CHECK+1; i ++)
            {
                // Check 30 * 30 tiles on the bottom right of the monster
                if(GameServer()->Collision()->IntersectLine(m_Pos + vec2(i * TS, 0), m_Pos + vec2(TS, LINES_TO_CHECK * TS), 0x0, 0x0))
                {
                    KeepDirection = true;
                    break;
                }
            }
            if(!KeepDirection)
                ChangeDir();
            else if(IsGrounded())
                Jump();
            else if(!GameServer()->Collision()->IntersectLine(m_Pos, m_Pos + vec2(0, TS * JUMP_LIMIT), 0x0, 0x0) && CanJump() && pVict)
                Jump();
        }

        // This is made because monsters are often stuck if "the top and the bottom are too close" and "the monster can't go to his direction (because of a wall)"
        if(m_Path.m_CollideLeft) // Check if the direction isn't changed
        {
            // Check collision on the top and right
            if(GameServer()->Collision()->CheckPoint(m_Pos + vec2(0, -TS)) && GameServer()->Collision()->CheckPoint(m_Pos + vec2(TS, 0)))
            {
                 // Check 11 tiles on the bottom (limit of a jump + airjump of a player)
                if(GameServer()->Collision()->IntersectLine(m_Pos, m_Pos + vec2(0, TS * 11), 0x0, 0x0)) // If found a solid tile, change the direction
                    ChangeDir();
            }
        }
    }
    else
    {
        if(!GameServer()->Collision()->CheckPoint(m_Pos + vec2(-TS, TS))) // Check if the next tile under the monster is not a solid tile
        {
            bool KeepDirection = false;
            for(int i = 1; i < LINES_TO_CHECK+1; i ++)
            {
                // Check 30 * 30 tiles on the bottom left of the monster
                if(GameServer()->Collision()->IntersectLine(m_Pos + vec2(i * -TS, 0), m_Pos + vec2(-TS, LINES_TO_CHECK * TS), 0x0, 0x0))
                {
                    KeepDirection = true;
                    break;
                }
            }
            if(!KeepDirection)
                ChangeDir();
            else if(IsGrounded())
                Jump();
            else if(!GameServer()->Collision()->IntersectLine(m_Pos, m_Pos + vec2(0, TS * JUMP_LIMIT), 0x0, 0x0) && CanJump() && pVict)
                Jump();
        }

        // This is made because monsters are often stuck if "the top and the bottom are too close" and "the monster can't go to his direction (because of a wall)"
        if(m_Path.m_CollideRight) // Check only if the direction isn't changed
        {
            // Check collision on the top and left
            if(GameServer()->Collision()->CheckPoint(m_Pos + vec2(0, -TS)) && GameServer()->Collision()->CheckPoint(m_Pos + vec2(-TS, 0)))
            {
                 // Check 11 tiles on the bottom (limit of a jump + airjump of a player)
                if(GameServer()->Collision()->IntersectLine(m_Pos, m_Pos + vec2(0, TS * JUMP_LIMIT), 0x0, 0x0)) // If found a solid tile, change the direction
                    ChangeDir();
            }
        }
    }

    if(pVict)
    {
        vec2 Diff = pVict->m_Pos - m_Pos;
        if(Diff.x >= 1)
        {
            m_Path.m_ActualDirection = DIRECTION_RIGHT;
            m_Path.m_CollideLeft = true;
            m_Path.m_CollideRight = false;
        }
        else if(Diff.x <= -1)
        {
            m_Path.m_ActualDirection = DIRECTION_LEFT;
            m_Path.m_CollideLeft = false;
            m_Path.m_CollideRight = true;
        }
        else
        {
            m_Path.m_ActualDirection = DIRECTION_ZERO;
            m_Path.m_CollideLeft = true;
            m_Path.m_CollideRight = false;
        }

        if(distance(pVict->m_Pos, m_Pos) < 800)
        {
            if(m_Difficulty >= 3 || m_Type == TYPE_HAMMER)
                m_WillHook = true;
            bool WillJump = CanJump();
            if(WillJump) // Here we check if when the monster will jump, it will still see the victim or not. If not, make like it doesn't jump
            {
                vec2 OldVel = m_Core.m_Vel; // Create a copy of the velocity

                if(IsGrounded())
                {
                    m_Core.m_Vel.y = -GameServer()->m_World.m_Core.m_Tuning.m_GroundJumpImpulse; // Simulate ground jumping
                    for(int PredictedTicks = 0; PredictedTicks < Server()->TickSpeed(); PredictedTicks ++) // Simulate ticks for 1 virtual second
                    {
                        if(PredictedTicks == Server()->TickSpeed()/2) // Simulate air jumping
                            m_Core.m_Vel.y = -GameServer()->m_World.m_Core.m_Tuning.m_AirJumpImpulse;

                        Move();

                        if(GameServer()->Collision()->IntersectLine(pVict->m_Pos, m_Pos, 0x0, 0x0)) // If the monster doesn't see the player after jumping
                        {
                            WillJump = false;
                            break;
                        }
                        m_Core.m_Vel.y += GameServer()->m_World.m_Core.m_Tuning.m_Gravity;
                        if(m_Path.m_ActualDirection < 0)
                            m_Core.m_Vel.x = SaturatedAdd(-MaxSpeed, MaxSpeed, m_Core.m_Vel.x, -Accel);
                        if(m_Path.m_ActualDirection > 0)
                            m_Core.m_Vel.x = SaturatedAdd(-MaxSpeed, MaxSpeed, m_Core.m_Vel.x, Accel);
                        if(m_Path.m_ActualDirection == 0)
                            m_Core.m_Vel.x *= Friction;
                    }
                }
                else
                {
                    m_Core.m_Vel.y = -GameServer()->m_World.m_Core.m_Tuning.m_AirJumpImpulse; // Simulate air jumping
                    for(int PredictedTicks = 0; PredictedTicks < Server()->TickSpeed()/2; PredictedTicks ++) // Simulate ticks for a half virtual second
                    {
                        Move();

                        if(GameServer()->Collision()->IntersectLine(pVict->m_Pos, m_Pos, 0x0, 0x0))
                        {
                            WillJump = false;
                            break;
                        }
                        m_Core.m_Vel.y += GameServer()->m_World.m_Core.m_Tuning.m_Gravity;
                        if(m_Path.m_ActualDirection < 0)
                            m_Core.m_Vel.x = SaturatedAdd(-MaxSpeed, MaxSpeed, m_Core.m_Vel.x, -Accel);
                        if(m_Path.m_ActualDirection > 0)
                            m_Core.m_Vel.x = SaturatedAdd(-MaxSpeed, MaxSpeed, m_Core.m_Vel.x, Accel);
                        if(m_Path.m_ActualDirection == 0)
                            m_Core.m_Vel.x *= Friction;
                    }
                }
                // The pos and the velocity have been modified, make like they aren't
                m_Pos = m_Path.m_LastPos;
                m_Core.m_Vel = OldVel;
            }
            if(WillJump)
                Jump();
        }
    }
    else
    {
        // Here we check if when the monster will jump, there will not be a collision between a player and it (so the monster has found a victim)
        bool WillJump = CanJump();
        if(WillJump)
        {
            vec2 OldVel = m_Core.m_Vel; // Create a copy of the velocity

            if(IsGrounded())
            {
                m_Core.m_Vel.y = -GameServer()->m_World.m_Core.m_Tuning.m_GroundJumpImpulse; // Simulate ground jumping
                for(int PredictedTicks = 0; PredictedTicks <= Server()->TickSpeed() * 2; PredictedTicks ++) // Simulate ticks for 2 virtual seconds
                {
                    if(PredictedTicks == Server()->TickSpeed()/2) // Simulate air jumping
                        m_Core.m_Vel.y = -GameServer()->m_World.m_Core.m_Tuning.m_AirJumpImpulse;

                    Move();

                    if(PredictedTicks % Server()->TickSpeed()/2 == 0) // Do not check always because of the lags
                    {
                        if(GameWorld()->ClosestCharacter(m_Pos, 10000, 0x0, false)) // If the monster will find a player
                        {
                            WillJump = true;
                            break;
                        }
                        else
                            WillJump = false;
                    }
                    m_Core.m_Vel.y += GameServer()->m_World.m_Core.m_Tuning.m_Gravity;
                    if(m_Path.m_ActualDirection < 0)
                        m_Core.m_Vel.x = SaturatedAdd(-MaxSpeed, MaxSpeed, m_Core.m_Vel.x, -Accel);
                    if(m_Path.m_ActualDirection > 0)
                        m_Core.m_Vel.x = SaturatedAdd(-MaxSpeed, MaxSpeed, m_Core.m_Vel.x, Accel);
                    if(m_Path.m_ActualDirection == 0)
                        m_Core.m_Vel.x *= Friction;
                }
            }
            else
            {
                m_Core.m_Vel.y = -GameServer()->m_World.m_Core.m_Tuning.m_AirJumpImpulse; // Simulate air jumping
                for(int PredictedTicks = 0; PredictedTicks <= Server()->TickSpeed(); PredictedTicks ++) // Simulate ticks 1 virtual second
                {
                    Move();

                    if(PredictedTicks % Server()->TickSpeed()/2 == 0) // Do not check always because of the lags
                    {
                        if(GameWorld()->ClosestCharacter(m_Pos, 10000, 0x0, false)) // If the monster will find a player
                        {
                            WillJump = true;
                            break;
                        }
                        else
                            WillJump = false;
                    }
                    m_Core.m_Vel.y += GameServer()->m_World.m_Core.m_Tuning.m_Gravity;
                    if(m_Path.m_ActualDirection < 0)
                        m_Core.m_Vel.x = SaturatedAdd(-MaxSpeed, MaxSpeed, m_Core.m_Vel.x, -Accel);
                    if(m_Path.m_ActualDirection > 0)
                        m_Core.m_Vel.x = SaturatedAdd(-MaxSpeed, MaxSpeed, m_Core.m_Vel.x, Accel);
                    if(m_Path.m_ActualDirection == 0)
                        m_Core.m_Vel.x *= Friction;
                }
            }
            // The pos and the velocity have been modified, make like they aren't
            m_Pos = m_Path.m_LastPos;
            m_Core.m_Vel = OldVel;
        }
        if(WillJump)
            Jump();
        m_WillHook = false;
    }

    m_Path.m_Direction = m_Path.m_ActualDirection;
}

void CMonster::TickPaused()
{
	++m_DamageTakenTick;
	++m_Ninja.m_ActivationTick;
}

void CMonster::Jump()
{
    const int Now = Server()->Tick();
    m_Path.m_LastJumpTick = Now;
    m_WillJump = true;
}

void CMonster::ChangeDir()
{
    m_Path.m_CollideRight = m_Path.m_CollideRight ? false : true;
    m_Path.m_CollideLeft = m_Path.m_CollideLeft ? false : true;
}

bool CMonster::CanJump()
{
    const int Now = Server()->Tick();
    const int WAIT_TICK = Server()->TickSpeed()/2;

    if(m_Path.m_LastJumpTick + WAIT_TICK < Now && m_Core.m_Jumped < 2)
        return true;

    return false;
}

bool CMonster::IncreaseHealth(int Amount)
{
	if(m_Health >= m_MaxHealth)
		return false;
	m_Health = clamp(m_Health+Amount, 0, m_MaxHealth);
	return true;
}

bool CMonster::IncreaseArmor(int Amount)
{
	if(m_Armor >= m_MaxArmor)
		return false;
	m_Armor = clamp(m_Armor+Amount, 0, m_MaxArmor);
	return true;
}

void CMonster::Die(int Killer)
{
    if(g_Config.m_SvDeathAnimation)
        new CMonsterDeath(GameWorld(), m_Pos, m_Core.m_Vel, m_Type, Killer);
    GameServer()->CreateSound(m_Pos, SOUND_HOOK_LOOP);
    if(GameServer()->IsValidPlayer(Killer))
    {
    	// send the kill message; works only for ddnet client :/
    	CNetMsg_Sv_KillMsg Msg;
    	Msg.m_Killer = Killer;
    	Msg.m_Victim = 16;
    	Msg.m_Weapon = m_Type;
    	Msg.m_ModeSpecial = 0;
    	Server()->SendPackMsg(&Msg, MSGFLAG_VITAL, -1);
    	
        int Score;
        switch(m_Type)
        {
            case TYPE_HAMMER: Score = 50000; break;
            case TYPE_GUN: Score = 50000; break;
            case TYPE_SHOTGUN: Score = 70000; break;
            case TYPE_GRENADE: Score = 70000; break;
            case TYPE_LASER: Score = 100000; break;
            case TYPE_NINJA: Score = 500000; break;
            default: Score = 1; break; // Security :p
        }
        GameServer()->m_apPlayers[Killer]->m_AccData.m_ExpPoints += Score;
        GameServer()->m_apPlayers[Killer]->m_AccData.m_Money += Score;
        GameServer()->m_apPlayers[Killer]->m_AccData.m_LvlWeapon[m_Type] += 2;
        GameServer()->m_apPlayers[Killer]->GetCharacter()->IncreaseHealth(GameServer()->m_apPlayers[Killer]->m_AccData.m_Health-10);
        char aBuf[256];
        str_format(aBuf, sizeof(aBuf), "ep + %d || Money + %d$", Score, Score);
        GameServer()->SendChatTarget(Killer, aBuf);
        GameServer()->CreateDeath(m_Pos, Killer);
        Reset();
        GameServer()->m_apPlayers[Killer]->GetCharacter()->CheckLevelUp(Killer);
        GameServer()->m_apPlayers[Killer]->GetCharacter()->HandleCity();
        GameServer()->m_pController->m_AliveMonsters--;
        
    }
}

const char *CMonster::MonsterName()
{
    switch(m_Type)
    {
        case TYPE_HAMMER: return "smasher"; break;
        case TYPE_GUN: return "soldier"; break;
        case TYPE_SHOTGUN: return "hunter"; break;
        case TYPE_GRENADE: return "bomber"; break;
        case TYPE_LASER: return "sniper"; break;
        case TYPE_NINJA: return "ninja"; break;
        default: return "INVALID"; break;
    }
}

bool CMonster::TakeDamage(vec2 Force, int Dmg, int From, int Weapon, bool FromMonster, bool Drain, bool FromReflect)
{
	m_Core.m_Vel += Force;

	if(FromMonster)
        return false;

    if(FromReflect && m_Health == 1 && !m_Armor)
        return false;

    int DrainedAmount = 0;

    if(GameServer()->IsValidPlayer(From))
    {
        Dmg += GameServer()->m_apPlayers[From]->m_AccData.m_Level;
        if(GameServer()->m_apPlayers[From]->m_AccData.m_Kills && Drain && Dmg)
            DrainedAmount = Dmg/2;
    }

	m_DamageTaken++;

	// create healthmod indicator
	if(Server()->Tick() < m_DamageTakenTick+25)
	{
		// make sure that the damage indicators doesn't group together
		GameServer()->CreateDamageInd(m_Pos, m_DamageTaken*0.25f, Dmg);
	}
	else
	{
		m_DamageTaken = 0;
		GameServer()->CreateDamageInd(m_Pos, 0, Dmg);
	}

	if(Dmg)
	{
		if(m_Armor)
		{
			if(Dmg > 1)
			{
				m_Health--;
				Dmg--;
			}

			if(Dmg > m_Armor)
			{
				Dmg -= m_Armor;
				m_Armor = 0;
			}
			else
			{
				m_Armor -= Dmg;
				Dmg = 0;
			}
		}

		m_Health -= Dmg;
	}

	m_DamageTakenTick = Server()->Tick();

	// do damage Hit sound
	if(From >= 0 && From < MAX_CLIENTS && GameServer()->m_apPlayers[From])
	{
		int Mask = CmaskOne(From);
		for(int i = 0; i < MAX_CLIENTS; i++)
		{
			if(GameServer()->m_apPlayers[i] && GameServer()->m_apPlayers[i]->GetTeam() == TEAM_SPECTATORS && GameServer()->m_apPlayers[i]->m_SpectatorID == From)
				Mask |= CmaskOne(i);
		}
		GameServer()->CreateSound(GameServer()->m_apPlayers[From]->m_ViewPos, SOUND_HIT, Mask);
	}

	if(DrainedAmount && Drain && GameServer()->GetPlayerChar(From))
    {
        if(DrainedAmount + GameServer()->GetPlayerChar(From)->GetHealth() > GameServer()->m_apPlayers[From]->m_AccData.m_Health)
        {
            int ArmorToGive = DrainedAmount;
            ArmorToGive -= GameServer()->m_apPlayers[From]->m_AccData.m_Health - GameServer()->GetPlayerChar(From)->GetHealth();
            GameServer()->GetPlayerChar(From)->IncreaseHealth(DrainedAmount);
            GameServer()->GetPlayerChar(From)->IncreaseArmor(ArmorToGive);
        }
        else
            GameServer()->GetPlayerChar(From)->IncreaseHealth(DrainedAmount);
    }

	// check for death
	if(m_Health <= 0)
	{
	    if(FromReflect)
        {
            m_Health = 1;
            goto here;
        }
		// set attacker's face to happy (taunt!)
		if (From >= 0 && From < MAX_CLIENTS && GameServer()->m_apPlayers[From])
		{
			CCharacter *pChr = GameServer()->m_apPlayers[From]->GetCharacter();
			if (pChr)
				pChr->SetEmote(EMOTE_HAPPY, Server()->Tick() + Server()->TickSpeed());
		}

		Die(From);

		return false;
	}

	here:

	if (Dmg > 2)
		GameServer()->CreateSound(m_Pos, SOUND_PLAYER_PAIN_LONG);
	else
		GameServer()->CreateSound(m_Pos, SOUND_PLAYER_PAIN_SHORT);

	return true;
}

void CMonster::Snap(int SnappingClient)
{
    if(NetworkClipped(SnappingClient))
		return;

    if(m_Core.m_HookState != HOOK_IDLE)
    {
        CNetObj_Laser *pObj = static_cast<CNetObj_Laser *>(Server()->SnapNewItem(NETOBJTYPE_LASER, m_ID, sizeof(CNetObj_Laser)));
        if(!pObj)
            return;

        pObj->m_X = (int)m_Core.m_HookPos.x;
        pObj->m_Y = (int)m_Core.m_HookPos.y;
        pObj->m_FromX = (int)m_Pos.x;
        pObj->m_FromY = (int)m_Pos.y;
        pObj->m_StartTick = Server()->Tick();
    }

    CNetObj_Pickup *apObjs[ENTITY_NUM];
    for(int i = 0; i < ENTITY_NUM; i ++)
    {
        apObjs[i] = static_cast<CNetObj_Pickup *>(Server()->SnapNewItem(NETOBJTYPE_PICKUP, m_aIDs[i], sizeof(CNetObj_Pickup)));
        if(!apObjs[i])
            return;

        apObjs[i]->m_X = (int)m_aSnapPos[i].x;
        apObjs[i]->m_Y = (int)m_aSnapPos[i].y;
        apObjs[i]->m_Type = POWERUP_WEAPON;
        apObjs[i]->m_Subtype = m_Type;
    }
}
