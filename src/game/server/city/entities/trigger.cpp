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
		for(int j = 0; j < 4; j++)
		{
			if(m_ItemID[j] > 0)
				apEnts[i]->m_TriggerNR[j] = m_ItemID[j];
		}
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
		for(int j = 0; j < 4; j++)
		{
			if(m_ItemID[j] > 0 && apEnts[i]->GetPlayer()->m_AccData.m_UserID == m_ItemID[j])
			{
				apEnts[i]->m_TriggerID[j] = m_ItemID[j];
				m_Switched = true;
			}
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


void CTrigger::Snap(int SnappingClient)
{
	if(NetworkClipped(SnappingClient, m_Pos))
		return;

	
	CNetObj_Laser *pObj[1];

	for(int i = 0; i < 1; i++)
	{
		pObj[i] = static_cast<CNetObj_Laser *>(Server()->SnapNewItem(NETOBJTYPE_LASER, m_ID, sizeof(CNetObj_Laser)));

		if(!pObj[i])
			return;

		pObj[i]->m_X = (int)m_Pos.x;
		pObj[i]->m_Y = (int)m_Pos.y;
		pObj[i]->m_FromX = (int)m_Pos.x;
		pObj[i]->m_FromY = (int)m_Pos.y - (m_Switched?48:0);
		pObj[i]->m_StartTick = Server()->Tick();

	}
}
