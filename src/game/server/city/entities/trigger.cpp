/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <game/generated/protocol.h>
#include <game/server/gamecontext.h>
#include "trigger.h"

CTrigger::CTrigger(CGameWorld *pGameWorld, vec2 Pos, int Type)
: CEntity(pGameWorld, CGameWorld::ENTTYPE_FLAG)
{
	m_Pos = Pos;
	m_Switched = false;

	for(int i = 0; i < 4; i++)
		m_ItemID[i] = 0;

	m_Type = Type;
	GameWorld()->InsertEntity(this);
}

void CTrigger::Reset()
{
	GameServer()->m_World.DestroyEntity(this);
}

void CTrigger::TriggerNR()
{
	CCharacter *apEnts[MAX_CLIENTS];
	int Num = GameServer()->m_World.FindEntities(m_Pos, 1.0f, (CEntity**)apEnts, MAX_CLIENTS, CGameWorld::ENTTYPE_CHARACTER);
	for(int i = 0; i < Num; i++)
	{
		if (GetNumber() > 0)
			apEnts[i]->m_TriggerNR = GetNumber();
	}

	if(Num)
		m_Switched = true;
	else
		m_Switched = false;
}

void CTrigger::TriggerID()
{
	CCharacter *apEnts[MAX_CLIENTS];
	int Num = GameServer()->m_World.FindEntities(m_Pos, 1.0f, (CEntity**)apEnts, MAX_CLIENTS, CGameWorld::ENTTYPE_CHARACTER);
	for(int i = 0; i < Num; i++)
	{
		if (GetNumber() > 0 && apEnts[i]->GetPlayer()->m_AccData.m_UserID == GetNumber())
		{
			apEnts[i]->m_TriggerID = GetNumber();
			m_Switched = true;
		}
	}

	if(!Num)
		m_Switched = false;
}

void CTrigger::Tick()
{
	const int MAX_TILES = 5;

	for(int i = 0; i < 4; i++)
	{
		const int STEPS = 32;
		int X, Y;

		switch(i)
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

		if(!m_ItemID[i])
		{
			for(int j = 1; j <= MAX_TILES+1; j++)
			{
				int Number = -1;

				if(j < MAX_TILES)
				{
					vec2 TestPos = vec2(m_Pos.x+X*j, m_Pos.y +Y*j);
					Number = GameServer()->Collision()->Number(TestPos);
				}


				if(Number != -1 && j < MAX_TILES+1)
				{
					if(i == 2 || i == 3)
					{
						m_ItemID[i] += Number*pow(10.0f, MAX_TILES-j);
					}
					else
					{
						m_ItemID[i] += Number*pow(10.0f, j-1);
					}
				}
				else
				{
					if((i == 2 || i == 3) && j < MAX_TILES+1)
						m_ItemID[i] = m_ItemID[i]/pow(10.0f, MAX_TILES+1-j);

					if(!m_ItemID[i])
						m_ItemID[i] = -1;
					
					break;
				}
			}
		}
	}

	if(m_Type)
		TriggerID();
	else
		TriggerNR();
}

int CTrigger::GetNumber()
{
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

void CTrigger::Snap(int SnappingClient)
{
	if(NetworkClipped(SnappingClient, m_Pos))
		return;
	
	CNetObj_Laser *pObj = static_cast<CNetObj_Laser *>(Server()->SnapNewItem(NETOBJTYPE_LASER, m_ID, sizeof(CNetObj_Laser)));
	if(!pObj)
		return;

	pObj->m_X = (int)m_Pos.x;
	pObj->m_Y = (int)m_Pos.y;
	pObj->m_FromX = (int)m_Pos.x;
	pObj->m_FromY = (int)m_Pos.y - (m_Switched ? 48 : 0);
	pObj->m_StartTick = Server()->Tick();
}