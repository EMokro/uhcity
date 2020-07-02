/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <game/generated/protocol.h>
#include <game/server/gamecontext.h>
#include "door.h"

CDoor::CDoor(CGameWorld* pGameWorld, vec2 Pos, int Type)
	: CEntity(pGameWorld, CGameWorld::ENTTYPE_FLAG)
{
	m_Pos = Pos;

	for (int i = 0; i < 4; i++)
	{
		m_aLength[i] = 0;
		m_ItemID[i] = 0;
	}

	for (int i = 0; i < 5; i++)
		m_IDs[i] = Server()->SnapNewID();

	m_Type = Type;
	GameWorld()->InsertEntity(this);
}

void CDoor::Reset()
{
	GameServer()->m_World.DestroyEntity(this);

	for (int i = 0; i < 5; i++)
		Server()->SnapFreeID(m_IDs[i]);
}

void CDoor::DoorID()
{
	if (GetNumber() < 0)
		return;

	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		CCharacter* pChar = GameServer()->GetPlayerChar(i);

		if (!pChar)
			continue;

		if (pChar->m_TriggerID <= 0)
			continue;

		if (pChar->m_TriggerID == GetNumber() && pChar->m_LastSwitch + Server()->TickSpeed() < Server()->Tick())
		{
			GameServer()->Collision()->DoorOpen(GetNumber(), !GameServer()->Collision()->IsDoorOpen(GetNumber()));
			pChar->m_LastSwitch = Server()->Tick();
			break;
		}

		if (pChar->m_LastSwitch + 15 < Server()->Tick() && pChar->m_LastSwitch + Server()->TickSpeed() >= Server()->Tick())
			pChar->m_TriggerID = 0;
	}
}

void CDoor::DoorNR()
{
	if (GetNumber() < 0)
		return;

	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		CCharacter* pChar = GameServer()->GetPlayerChar(i);

		if (!pChar)
			continue;

		if (pChar->m_TriggerNR <= 0)
			continue;

		if (pChar->m_TriggerNR == GetNumber() && pChar->m_LastSwitch + Server()->TickSpeed() < Server()->Tick())
		{
			GameServer()->Collision()->DoorOpen(GetNumber(), !GameServer()->Collision()->IsDoorOpen(GetNumber()));
			pChar->m_LastSwitch = Server()->Tick();
		}

		if (pChar->m_LastSwitch + 15 < Server()->Tick() && pChar->m_LastSwitch + Server()->TickSpeed() >= Server()->Tick())
			pChar->m_TriggerNR = 0;
	}
}

void CDoor::ItemID()
{
	const int MAX_TILES = 5;

	for (int i = 0; i < 4; i++)
	{
		const int STEPS = 32;
		int X, Y;

		switch (i)
		{
		case 0:
			X = -STEPS;
			Y = 0;
			break;
		case 1:
			X = 0;
			Y = -STEPS;
			break;
		case 2:
			X = STEPS;
			Y = 0;
			break;
		case 3:
			X = 0;
			Y = STEPS;
			break;
		}

		if (!m_ItemID[i])
		{
			for (int j = 1; j <= MAX_TILES + 1; j++)
			{
				int Number = -1;

				if (j < MAX_TILES)
				{
					vec2 TestPos = vec2(m_Pos.x + X * j, m_Pos.y + Y * j);
					Number = GameServer()->Collision()->Number(TestPos);
				}

				if (Number != -1 && j < MAX_TILES + 1)
				{
					if (i == 2 || i == 3)
						m_ItemID[i] += Number * pow(10.0f, MAX_TILES - j);
					else
						m_ItemID[i] += Number * pow(10.0f, j - 1);
				}
				else
				{
					if ((i == 2 || i == 3) && j < MAX_TILES + 1)
						m_ItemID[i] = m_ItemID[i] / pow(10.0f, MAX_TILES + 1 - j);

					if (!m_ItemID[i])
						m_ItemID[i] = -1;

					break;
				}
			}
		}
	}
}

void CDoor::Stop()
{
	const int MAX_TILES_STOP = 16;
	bool Stop[4] = { false };

	for (int i = 0; i < 4; i++)
	{
		if (Stop[i])
			continue;

		const int STEPS = 32;
		int X, Y;

		switch (i)
		{
		case 0:
			X = -STEPS;
			Y = 0;
			break;
		case 1:
			X = 0;
			Y = -STEPS;
			break;
		case 2:
			X = STEPS;
			Y = 0;
			break;
		case 3:
			X = 0;
			Y = STEPS;
			break;
		}

		for (int j = 1; j <= MAX_TILES_STOP + 1; j++)
		{
			if (j < MAX_TILES_STOP)
			{
				vec2 TestPos = vec2(m_Pos.x + X * j, m_Pos.y + Y * j);
				m_aLength[i] = 0;

				if (GameServer()->Collision()->IsTile(TestPos, TILE_DOOR_END))// STOP
				{
					Stop[i] = true;
					m_aLength[i] = distance(TestPos, m_Pos);
					break;
				}
				else if (GameServer()->Collision()->CheckPoint(TestPos) && !GameServer()->Collision()->GetDoorAt(TestPos.x, TestPos.y))
				{
					Stop[i] = true;
					break;
				}
			}
		}
	}
}

void CDoor::Tick()
{
	ItemID();
	Stop();

	if (m_Type)
		DoorID();
	else
		DoorNR();

	vec2 At;
	for (int i = 0; i < 4; i++)
	{
		vec2 ToPos = m_Pos;
		if (m_aLength[i])
		{
			if (!(i % 2))
				ToPos.x = (int)m_Pos.x + m_aLength[i] * (i == 0 ? -1 : 1);
			else
				ToPos.y = (int)m_Pos.y + m_aLength[i] * (i == 3 ? 1 : -1);

			GameServer()->Collision()->SetDoorAt(m_Pos, ToPos, GetNumber());

			if (g_Config.m_SvDoorType == 2) // not needed for "killer doors"
				continue;

			CCharacter *pHit = GameServer()->m_World.IntersectCharacter(m_Pos, ToPos, 16.0f, At, 0x0);

			if (!pHit)
				continue;
			else
			{
				if (GameServer()->Collision()->IsDoorOpen(GetNumber()))
					return;

				pHit->m_Core.m_Vel += -normalize(At - pHit->m_Pos) * 3;

				/*if (!(Hit->ActiveWeapon() == WEAPON_NINJA && (Hit->m_Input.m_Fire & 1) == 1)) {
					Hit->m_Core.m_Pos = Hit->m_DesiredPos;
					Hit->m_Core.m_Vel = vec2(0, 0);
				}
				else {
					Hit->Die(Hit->GetPlayer()->GetCID(), WEAPON_GAME);
				}*/
			}
		}
	}
}

int CDoor::GetNumber()
{
	// omg that's hacky
	char aNumber[32];
	mem_zero(&aNumber, sizeof(aNumber));
	for (int i = 0; i < 4; i++)
	{
		if (m_ItemID[i] <= 0)
			continue;

		char aBuf[32];
		str_format(aBuf, sizeof(aBuf), "%d", m_ItemID[i]);
		str_append(aNumber, aBuf, sizeof(aNumber));
	}

	if (!aNumber[0])
		return -1;

	int Number = str_toint(aNumber);
	if (Number <= 0)
		return -1;

	return Number;
}

void CDoor::Snap(int SnappingClient)
{
	if(NetworkClipped(SnappingClient, m_Pos))
		return;

	CNetObj_Laser* pObj[5];

	for (int i = 0; i < 5; i++)
	{
		pObj[i] = static_cast<CNetObj_Laser*>(Server()->SnapNewItem(NETOBJTYPE_LASER, m_IDs[i], sizeof(CNetObj_Laser)));

		if (!pObj[i])
			return;

		if (i < 5 && !m_aLength[i])
			continue;

		pObj[i]->m_X = (int)m_Pos.x;
		pObj[i]->m_Y = (int)m_Pos.y;
		pObj[i]->m_FromX = (int)m_Pos.x;
		pObj[i]->m_FromY = (int)m_Pos.y;
		pObj[i]->m_StartTick = Server()->Tick();

		if (!(i % 2) && i < 4)
			pObj[i]->m_X = (int)m_Pos.x + m_aLength[i] * (i == 0 ? -1 : 1);
		else
			pObj[i]->m_Y = (int)m_Pos.y + m_aLength[i] * (i == 3 ? 1 : -1);

		if (GameServer()->Collision()->IsDoorOpen(GetNumber()))
		{
			if (!(i % 2) && i < 4)
				pObj[i]->m_FromX = (int)m_Pos.x + m_aLength[i] * (i == 0 ? -1 : 1);
			else
				pObj[i]->m_FromY = (int)m_Pos.y + m_aLength[i] * (i == 3 ? 1 : -1);
		}
	}
}
