/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <game/generated/protocol.h>
#include <game/server/gamecontext.h>
#include <game/server/entities/projectile.h>
#include "carousel.h"
#include <game/server/entities/laser.h>

CCarousel::CCarousel(CGameWorld *pGameWorld, vec2 Pos)
: CEntity(pGameWorld, CGameWorld::ENTTYPE_FLAG)
{
	m_Pos = Pos;

	for(int i = 0; i < 6; i++)
		m_IDs[i] = Server()->SnapNewID();

	GameWorld()->InsertEntity(this);

}

void CCarousel::Reset()
{
	GameServer()->m_World.DestroyEntity(this);

	for(int i = 0; i < 6; i++)
		Server()->SnapFreeID(m_IDs[i]);
}

void CCarousel::SetSize()
{
	const int MAX_TILES = 4;

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

		if(!m_Size)
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
						m_Size += Number*pow(10.0f, MAX_TILES-j);
					}
					else
					{
						m_Size += Number*pow(10.0f, j-1);
					}
				}
				else
				{
					if((i == 2 || i == 3) && j < MAX_TILES+1)
						m_Size = m_Size/pow(10.0f, MAX_TILES+1-j);

					if(!m_Size)
						m_Size = 300;
					
					return;
				}
			}
		}
	}

}

void CCarousel::Tick()
{
	if(!m_Size)
		SetSize();

	if(!m_Size)
		return;

	for(int i = 0; i < 6; i++)
	{
		vec2 TestPos = m_Pos + normalize(GetDir(pi/180 * ((Server()->Tick()+i*60)%360+1))) * m_Size;

		carouselX[i] = TestPos.x;
		carouselY[i] = TestPos.y;
	
		CCharacter *pC = static_cast<CCharacter *>(GameServer()->m_World.ClosestCharacter(TestPos, 64, NULL));
	
		if(pC && !pC->GetPlayer()->m_ReleaseCarousel)
			pC->m_Core.m_Vel = vec2((TestPos.x-pC->m_Core.m_Pos.x)/3, (TestPos.y-pC->m_Core.m_Pos.y)/3);
	}
}


void CCarousel::Snap(int SnappingClient)
{
	CNetObj_Laser *pObj[6];
	
	for(int i = 0; i < 6; i++)
	{
		pObj[i] = static_cast<CNetObj_Laser *>(Server()->SnapNewItem(NETOBJTYPE_LASER, m_IDs[i], sizeof(CNetObj_Laser)));

		if(!pObj[i])
			return;

		pObj[i]->m_X = carouselX[i];
		pObj[i]->m_Y = carouselY[i];
		pObj[i]->m_FromX = carouselX[i];
		pObj[i]->m_FromY = carouselY[i];
		pObj[i]->m_StartTick = Server()->Tick();
	}

	
}
