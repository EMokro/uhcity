/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <game/generated/protocol.h>
#include <game/server/gamecontext.h>
#include <game/server/entities/projectile.h>
#include "home.h"

CHome::CHome(CGameWorld *pGameWorld, vec2 Pos)
: CEntity(pGameWorld, CGameWorld::ENTTYPE_FLAG)
{
	m_Pos = Pos;

	for(int i = 0; i < 4; i++)
		m_ItemID[i] = 0;

	GameWorld()->InsertEntity(this);
}

void CHome::Reset()
{
	GameServer()->m_World.DestroyEntity(this);
}

void CHome::SetItemID()
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
}

void CHome::Tick()
{
	bool RefreshNeeded = true;
	for(int i = 0; i < 4; i++)
	{
		if(m_ItemID[i] > 0)
			RefreshNeeded = false;
	}

	if(RefreshNeeded)
		SetItemID();

	for(int p = 0; p < MAX_CLIENTS; p++)
	{
		CCharacter *pChar = GameServer()->GetPlayerChar(p);

		if(!pChar)
			continue;

		for(int i = 0; i < 4; i++)
		{
			if(m_ItemID[i] <= 0)
				continue;

			if(pChar->m_Home == m_ItemID[i])
				pChar->SetPosition(m_Pos);
		}
	}
}


void CHome::Snap(int SnappingClient)
{
	if(NetworkClipped(SnappingClient, m_Pos))
		return;

	
}
