/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* Copyright � 2013 Neox.                                                                                                */
/* If you are missing that file, acquire a complete release at https://www.teeworlds.com/forum/viewtopic.php?pid=106934  */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef GAME_SERVER_ENTITIES_MONSTER_H
#define GAME_SERVER_ENTITIES_MONSTER_H

#include <game/server/entity.h>
#include <game/generated/protocol.h>

enum
{
    TYPE_HAMMER = WEAPON_HAMMER,
    TYPE_GUN = WEAPON_GUN,
    TYPE_SHOTGUN = WEAPON_SHOTGUN,
    TYPE_GRENADE = WEAPON_GRENADE,
    TYPE_LASER = WEAPON_RIFLE,
    TYPE_NINJA = WEAPON_NINJA,
};

class CMonster : public CEntity
{
    enum
    {
        ENTITY_NUM = 3,
        ENTITY_SPEED = 5,
    };
public:
	//monster's size
	static const int ms_PhysSize = 28;

	CMonster(CGameWorld *pWorld, int Type, int MonsterID, int Health, int Armor, int Difficulty);

	virtual void Reset();
	virtual void Tick();
	virtual void TickPaused();
	virtual void Snap(int SnappingClient);

	bool IsGrounded();

	void HandleNinja(bool IsPredicted = false);

	void FireWeapon();

	void Die(int Killer);
	bool TakeDamage(vec2 Force, int Dmg, int From, int Weapon, bool FromMonster = false, bool Drain = true, bool FromReflect = false);

	bool IncreaseHealth(int Amount);
	bool IncreaseArmor(int Amount);

	vec2 GetPos() const { return m_Pos; }
	void Destroy();
	void SetPerfectDir();
	void HandleWeapons();
	void Move();
	void HandleCore();
	void HandleActions();
	void Jump();
	bool CanJump();
	void ChangeDir();
	void Spawn();
	const char *MonsterName();
	int GetDifficulty() const { return m_Difficulty; }
	void OnPredictedNinja();
    
    int GetLifes() { return m_Armor + m_Health; }

private:

	// weapon info
	CEntity *m_apHitObjects[10];
	int m_NumObjectsHit;
	int m_ActiveWeapon;
	int m_ReloadTimer;
	int m_DamageTaken;

	int m_DamageTakenTick;

	int m_Health;
	int m_Armor;
	int m_MaxArmor;
	int m_MaxHealth;

	// ninja
	struct
	{
		vec2 m_ActivationDir;
		int m_ActivationTick;
		int m_CurrentMoveTime;
		int m_OldVelAmount;
	} m_Ninja;

	int m_Type;
	int m_MonsterID;
	int m_Difficulty;

    bool m_WillFire;
    bool m_WillJump;
    bool m_WillHook;

    int m_aIDs[ENTITY_NUM];
    vec2 m_aSnapPos[ENTITY_NUM];

    vec2 m_Dir;

    struct
    {
        vec2 m_Vel;

        vec2 m_HookPos;
        vec2 m_HookDir;
        int m_HookTick;
        int m_HookState;
        int m_HookedPlayer;

        int m_Jumped;
    }m_Core;

	struct
	{
        int m_Direction;
        int m_ActualDirection;

        bool m_CollideRight;
        bool m_CollideLeft;

        int m_LastJumpTick;

        vec2 m_LastPos;
        int m_StuckTick;
	}m_Path;
};

#endif
