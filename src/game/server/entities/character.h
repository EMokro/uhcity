/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SERVER_ENTITIES_CHARACTER_H
#define GAME_SERVER_ENTITIES_CHARACTER_H

#include <game/server/entity.h>
#include <game/generated/server_data.h>
#include <game/generated/protocol.h>

#include <game/gamecore.h>

enum
{
	WEAPON_GAME = -3, // team switching etc
	WEAPON_SELF = -2, // console kill command
	WEAPON_WORLD = -1, // death tiles etc
};

class CCharacter : public CEntity
{
	MACRO_ALLOC_POOL_ID()

public:
	//character's size
	static const int ms_PhysSize = 28;

	CCharacter(CGameWorld *pWorld);

	virtual void Reset();
	virtual void Destroy();
	virtual void Tick();
	virtual void TickDefered();
	virtual void Snap(int SnappingClient);

	bool IsGrounded();

	void SetWeapon(int W);
	void HandleWeaponSwitch();
	void DoWeaponSwitch();

	void HandleWeapons();
	void HandleNinja();

	void OnPredictedInput(CNetObj_PlayerInput *pNewInput);
	void OnDirectInput(CNetObj_PlayerInput *pNewInput);
	void ResetInput();
	void FireWeapon();

	void Die(int Killer, int Weapon);
	bool TakeDamage(vec2 Force, int Dmg, int From, int Weapon);

	bool Spawn(class CPlayer *pPlayer, vec2 Pos);
	bool Remove();

	bool IncreaseHealth(int Amount);
	bool IncreaseArmor(int Amount);

	bool GiveWeapon(int Weapon, int Ammo);
	void GiveNinja();

	void SetEmote(int Emote, int Tick);

	bool IsAlive() const { return m_Alive; }
	class CPlayer *GetPlayer() { return m_pPlayer; }

	// City
	void SaveLoad(int Value);
	void Tele();
	void Buy(const char *Name, int *Upgrade, int Price, int Click, int Max);
	int MouseEvent(vec2 Pos);
	int NewPlasma();
	int ActiveWeapon();
	void ChangeUpgrade(int Value);
	void HealthRegeneration();
	void HandleCity();
	void Booster();
	void SetPosition(vec2 Pos);
	void Freeze(int Seconds);
	void Unfreeze();
	void Transfer(int Value);
	bool Protected();
//KlickFoots Water zeugs 
	bool m_SingleWater;
	bool m_Water;
//KlickFoots Space zeugs
	bool m_SingleSpace;
	bool m_Space;
	//Beides 
	int m_Luft;
	//zInsta
	int m_InstaKills;
	//Police
	int m_JailRifle;
	//Life
	int m_NeedArmor;
	int m_NeedHealth;
	int m_ArmorCost;
	int m_HealthCost;
	int m_LifeCost;
	//Save/Load
	vec2 m_SavePos;
	

	bool m_God;
	bool m_Protected;
	bool m_GameZone;
	bool m_IsHammerKilled;
	int m_Home;
	int m_Invisible;
	int m_Walls;
	int m_Plasma;
	int m_Frozen;
	int m_Emote;
	int m_EmoteType;
	int m_EmoteStop;
	int m_Menu;
	int m_FreezeWeapon;
	bool m_FreezeEnd;
	int64 m_FreezeTick;
	int64 m_LastBroadcast;
	int64 m_BuyTick;
	int64 m_SpawnProtection;
	vec2 m_HammerPos1;
	vec2 m_HammerPos2;

	// Doors
	int m_TriggerID[4];
	int m_TriggerNR[4];
	int64 m_LastSwitch;


	// the player core for the physics
	CCharacterCore m_Core;

	CNetObj_PlayerInput m_PrevInput;
	CNetObj_PlayerInput m_Input;
		
	int m_Health;
	int m_Armor;

private:
	// player controlling this character
	class CPlayer *m_pPlayer;

	bool m_Alive;

	// weapon info
	CEntity *m_apHitObjects[10];
	int m_NumObjectsHit;

	struct WeaponStat
	{
		int m_AmmoRegenStart;
		int m_Ammo;
		int m_Ammocost;
		bool m_Got;

	} m_aWeapons[NUM_WEAPONS];

	int m_ActiveWeapon;
	int m_LastWeapon;
	int m_QueuedWeapon;

	int m_ReloadTimer;
	int m_AttackTick;

	int m_DamageTaken;

	// last tick that the player took any action ie some input
	int m_LastAction;

	// these are non-heldback inputs
	CNetObj_PlayerInput m_LatestPrevInput;
	CNetObj_PlayerInput m_LatestInput;

	// input
	int m_NumInputs;
	int m_Jumped;

	int m_DamageTakenTick;



	// ninja
	struct
	{
		vec2 m_ActivationDir;
		int m_ActivationTick;
		int m_CurrentMoveTime;
		int m_OldVelAmount;
	} m_Ninja;

	

	// info for dead reckoning
	int m_ReckoningTick; // tick that we are performing dead reckoning From
	CCharacterCore m_SendCore; // core that we should send
	CCharacterCore m_ReckoningCore; // the dead reckoning core

};

#endif
