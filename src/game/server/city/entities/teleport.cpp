/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <game/generated/protocol.h>
#include <game/server/gamecontext.h>
#include <game/server/entities/projectile.h>
#include "teleport.h"

CTeleport::CTeleport(CGameWorld *pGameWorld, vec2 Pos, int Type)
: CEntity(pGameWorld, CGameWorld::ENTTYPE_FLAG)
{
	m_Pos = Pos;

	for(int i = 0; i < 4; i++)
		m_ItemID[i] = 0;

	m_Type = Type;
	GameWorld()->InsertEntity(this);
}

void CTeleport::Reset()
{
	GameServer()->m_World.DestroyEntity(this);
}

void CTeleport::To()
{	
	for(int i = 0; i < MAX_CLIENTS; i++)
	{
		CCharacter *pChar = GameServer()->GetPlayerChar(i);

		if(pChar)
		{
			for(int j = 0; j < 4; j++)
			{
				if(GameServer()->m_TeleNR[i] == m_ItemID[j] && m_ItemID[j] > 0)
				{
					if(GameServer()->m_TeleNum > 0)
					{
						if(!(GameServer()->Server()->Tick()%((rand()%20)+1)))
						{
							pChar->SetPosition(m_Pos);
							break;
						}
					}
					else
					{
						pChar->SetPosition(m_Pos);
						break;
					}
				}
			}
		}
	}

}

void CTeleport::From()
{	
	CCharacter *apEnts[MAX_CLIENTS];
	int Num = GameServer()->m_World.FindEntities(m_Pos, 0.0f, (CEntity**)apEnts, MAX_CLIENTS, CGameWorld::ENTTYPE_CHARACTER);
	for(int i = 0; i < Num; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			if(m_ItemID[j] > 0)
			{
				GameServer()->m_TeleNR[apEnts[i]->GetPlayer()->GetCID()] = m_ItemID[j];
				break;
			}
		}
	}	
}

void CTeleport::Tick()
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

	/*for(int j = 0; j < 4; j++)
	{
		if(m_ItemID[j] > 0)
			dbg_msg("...", "%s %d) %d", m_Type?"From":"To", j, m_ItemID[j]);
	}*/

	if(m_Type)
		From();
	else
		To();
}


void CTeleport::Snap(int SnappingClient)
{
	if(NetworkClipped(SnappingClient, m_Pos))
		return;

	
}
