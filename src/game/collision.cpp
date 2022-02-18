/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <math.h>

#include <base/system.h>
#include <base/math.h>
//#include <base/vmath.h>

#include <engine/map.h>
#include <engine/kernel.h>
#include <engine/shared/config.h>

#include <game/mapitems.h>
#include <game/layers.h>
#include <game/collision.h>

CCollision::CCollision()
{
	m_pTiles = 0;
	m_Width = 0;
	m_Height = 0;
	m_pLayers = 0;

	// City
	m_pCityTiles[0] = 0;

	for (int i = 0; i < 4; i++) {
		m_pSubGameLayer[i] = 0;
		m_pCityTiles[i+1] = 0;
	}
}

CCollision::~CCollision()
{
   /* delete[] m_pCityTiles;
    m_pCityTiles = 0;*/

    //delete[] m_pEnteties;
    //m_pEnteties = 0;
}

void CCollision::Init(class CLayers *pLayers)
{
	Dest();
	m_pLayers = pLayers;
	m_Width = m_pLayers->GameLayer()->m_Width;
	m_Height = m_pLayers->GameLayer()->m_Height;
	m_pTiles = static_cast<CTile *>(m_pLayers->Map()->GetData(m_pLayers->GameLayer()->m_Data));

	m_pCityTiles[0] = new CTile[m_Width*m_Height];
	mem_copy(m_pCityTiles[0], m_pTiles, sizeof(CTile)*m_Width*m_Height);

	m_pDoorTiles = new CDoorTile[m_Width * m_Height];
	for (int i = 0; i < 1024; i++)
		m_DoorOpen[i] = false;
	
	for (int i = 0; i < 4; i++) {
		if (m_pLayers->SubGameLayer(i)) {
			
			unsigned int Size = m_pLayers->Map()->GetUncompressedDataSize(m_pLayers->SubGameLayer(i)->m_Data);
			
			if (Size >= m_Width*m_Height*sizeof(CTile)) {
				m_pSubGameLayer[i] = static_cast<CTile *>(m_pLayers->Map()->GetData(m_pLayers->SubGameLayer(i)->m_Data));

				m_pCityTiles[i+1] = new CTile[m_Width*m_Height];
				mem_copy(m_pCityTiles[i+1], m_pSubGameLayer[i], sizeof(CTile)*m_Width*m_Height);
			}
			
		}
	}

	for(int i = 0; i < m_Width*m_Height; i++)
	{
		int Index = m_pTiles[i].m_Index;

		m_pDoorTiles[i].m_Index = 0;
		m_pDoorTiles[i].m_DoorID = 0;

		if(Index > 128)
			continue;

		switch(Index)
		{
		case TILE_DEATH:
			m_pTiles[i].m_Index = COLFLAG_DEATH;
			break;
		case TILE_SOLID:
			m_pTiles[i].m_Index = COLFLAG_SOLID;
			break;
		case TILE_NOHOOK:
			m_pTiles[i].m_Index = COLFLAG_SOLID|COLFLAG_NOHOOK;
			break;
		default:
			m_pTiles[i].m_Index = 0;
		}
	}
}

int CCollision::TileMoney(int x, int y)
{
	if(IsTile(x, y, TILE_MONEY50))
		return 500;
	else if(IsTile(x, y, TILE_MONEY100))
		return 100;
	else if(IsTile(x, y, TILE_MONEY200))
		return 200;
	else if(IsTile(x, y, TILE_MONEY500))
		return 500;
	else if(IsTile(x, y, TILE_MONEY1000))
		return 1000;

	return 0;
}

int CCollision::Number(int x, int y)
{
	int Nx = clamp(x/32, 0, m_Width-1);
	int Ny = clamp(y/32, 0, m_Height-1);

	for (int i = 0; i < 5; i ++) {
		if (m_pCityTiles[i]) {
			if (m_pCityTiles[i][Ny*m_Width+Nx].m_Index >= TILE_0 && m_pCityTiles[i][Ny*m_Width+Nx].m_Index <= TILE_9)
				return m_pCityTiles[i][Ny*m_Width+Nx].m_Index-TILE_0;
		} else 
			break;
	}

	return -1;
}

int CCollision::TileShop(int x, int y)
{
	int Nx = clamp(x/32, 0, m_Width-1);
	int Ny = clamp(y/32, 0, m_Height-1);

	for (int i = 0; i < 5; i ++) {
		if (m_pCityTiles[i]) {
			if (m_pCityTiles[i][Ny*m_Width+Nx].m_Index == TILE_SHOP)
				return 1;
		} else 
			break;
	}

	return 0;
}

int CCollision::IsTile(int x, int y, int Type)
{
	int Nx = clamp(x/32, 0, m_Width-1);
	int Ny = clamp(y/32, 0, m_Height-1);

	for (int i = 0; i < 5; i ++) {
		if (m_pCityTiles[i]) {
			if (m_pCityTiles[i][Ny*m_Width+Nx].m_Index == Type)
				return 1;
		} else 
			break;
	}


	return 0;
}

int CCollision::GetTile(int x, int y)
{
	int Nx = clamp(x/32, 0, m_Width-1);
	int Ny = clamp(y/32, 0, m_Height-1);

	if (GetDoorAt(x, y))
	{
		if (g_Config.m_SvDoorType == 1)
			return COLFLAG_SOLID | COLFLAG_NOHOOK;
		else if (g_Config.m_SvDoorType == 2)
			return COLFLAG_DEATH;
		else
			return COLFLAG_SOLID;
	}

	return m_pTiles[Ny*m_Width+Nx].m_Index > 128 ? 0 : m_pTiles[Ny*m_Width+Nx].m_Index;
}

bool CCollision::IsTileSolid(int x, int y)
{
	return GetTile(x, y)&COLFLAG_SOLID;
}

// TODO: rewrite this smarter!
int CCollision::IntersectLine(vec2 Pos0, vec2 Pos1, vec2 *pOutCollision, vec2 *pOutBeforeCollision)
{
	float Distance = distance(Pos0, Pos1);
	int End(Distance+1);
	vec2 Last = Pos0;

	for(int i = 0; i < End; i++)
	{
		float a = i/Distance;
		vec2 Pos = mix(Pos0, Pos1, a);
		if(CheckPoint(Pos.x, Pos.y))
		{
			if(pOutCollision)
				*pOutCollision = Pos;
			if(pOutBeforeCollision)
				*pOutBeforeCollision = Last;
			return GetCollisionAt(Pos.x, Pos.y);
		}
		Last = Pos;
	}
	if(pOutCollision)
		*pOutCollision = Pos1;
	if(pOutBeforeCollision)
		*pOutBeforeCollision = Pos1;
	return 0;
}

// TODO: OPT: rewrite this smarter!
void CCollision::MovePoint(vec2 *pInoutPos, vec2 *pInoutVel, float Elasticity, int *pBounces)
{
	if(pBounces)
		*pBounces = 0;

	vec2 Pos = *pInoutPos;
	vec2 Vel = *pInoutVel;
	if(CheckPoint(Pos + Vel))
	{
		int Affected = 0;
		if(CheckPoint(Pos.x + Vel.x, Pos.y))
		{
			pInoutVel->x *= -Elasticity;
			if(pBounces)
				(*pBounces)++;
			Affected++;
		}

		if(CheckPoint(Pos.x, Pos.y + Vel.y))
		{
			pInoutVel->y *= -Elasticity;
			if(pBounces)
				(*pBounces)++;
			Affected++;
		}

		if(Affected == 0)
		{
			pInoutVel->x *= -Elasticity;
			pInoutVel->y *= -Elasticity;
		}
	}
	else
	{
		*pInoutPos = Pos + Vel;
	}
}

bool CCollision::TestBox(vec2 Pos, vec2 Size)
{
	Size *= 0.5f;
	if(CheckPoint(Pos.x-Size.x, Pos.y-Size.y))
		return true;
	if(CheckPoint(Pos.x+Size.x, Pos.y-Size.y))
		return true;
	if(CheckPoint(Pos.x-Size.x, Pos.y+Size.y))
		return true;
	if(CheckPoint(Pos.x+Size.x, Pos.y+Size.y))
		return true;
	return false;
}

void CCollision::MoveBox(vec2 *pInoutPos, vec2 *pInoutVel, vec2 Size, float Elasticity)
{
	// do the move
	vec2 Pos = *pInoutPos;
	vec2 Vel = *pInoutVel;

	float Distance = length(Vel);
	int Max = (int)Distance;

	if(Distance > 0.00001f)
	{
		//vec2 old_pos = pos;
		float Fraction = 1.0f/(float)(Max+1);
		for(int i = 0; i <= Max; i++)
		{
			//float amount = i/(float)max;
			//if(max == 0)
				//amount = 0;

			vec2 NewPos = Pos + Vel*Fraction; // TODO: this row is not nice

			if(TestBox(vec2(NewPos.x, NewPos.y), Size))
			{
				int Hits = 0;

				if(TestBox(vec2(Pos.x, NewPos.y), Size))
				{
					NewPos.y = Pos.y;
					Vel.y *= -Elasticity;
					Hits++;
				}

				if(TestBox(vec2(NewPos.x, Pos.y), Size))
				{
					NewPos.x = Pos.x;
					Vel.x *= -Elasticity;
					Hits++;
				}

				// neither of the tests got a collision.
				// this is a real _corner case_!
				if(Hits == 0)
				{
					NewPos.y = Pos.y;
					Vel.y *= -Elasticity;
					NewPos.x = Pos.x;
					Vel.x *= -Elasticity;
				}
			}

			Pos = NewPos;
		}
	}

	*pInoutPos = Pos;
	*pInoutVel = Vel;
}

// City

void CCollision::Dest() {
	m_pTiles = 0;
	m_Width = 0;
	m_Height = 0;
	m_pLayers = 0;

	for (int i = 0; i < 4; i++)
		m_pSubGameLayer[i] = 0;
}

bool CCollision::DoorOpen(int DoorNumber, bool Open)
{
	m_DoorOpen[DoorNumber] = Open;
	return m_DoorOpen[DoorNumber];
}

bool CCollision::GetDoorAt(int x, int y)
{
	int Nx = clamp(x / 32, 0, m_Width - 1);
	int Ny = clamp(y / 32, 0, m_Height - 1);

	if (m_pDoorTiles[Ny * m_Width + Nx].m_Index == 7)
	{
		if (m_DoorOpen[m_pDoorTiles[Ny * m_Width + Nx].m_DoorID])
			return false;
		else
			return true;
	}

	return false;
}

int CCollision::SetDoorAt(vec2 From, vec2 To, int Number)
{
	float Distance = distance(From, To);
	int End(Distance + 1);

	for (int i = 0; i < End; i++)
	{
		float a = i / Distance;
		vec2 Pos = mix(From, To, a);

		int Nx = clamp((int)Pos.x / 32, 0, m_Width - 1);
		int Ny = clamp((int)Pos.y / 32, 0, m_Height - 1);

		if (Number)
		{
			m_pDoorTiles[Ny * m_Width + Nx].m_DoorID = Number;
			m_pDoorTiles[Ny * m_Width + Nx].m_Index = 7;
		}
		else
		{
			m_pDoorTiles[Ny * m_Width + Nx].m_DoorID = 0;
			m_pDoorTiles[Ny * m_Width + Nx].m_Index = 0;
		}
	}

	return 1;
}

bool CCollision::IntersectLine2(vec2 Pos0, vec2 Pos1) // Done to avoid lags (checks every tiles instead of every single pos, going 32 by 32 instead of 1 by 1)
{
    vec2 Dir = normalize(Pos1 - Pos0) * 32;

    float Distance = distance(Pos0, Pos1);

    while(Distance > 32)
    {
        Pos0 += Dir;
        if(CheckPoint(Pos0))
            return true;
        Distance = distance(Pos0, Pos1);
    }

	return false;
}

bool CCollision::EmptyOnLine(vec2 Pos0, vec2 Pos1, vec2 *pOutCollision, vec2 *pOutBeforeCollision)
{
	float Distance = distance(Pos0, Pos1);
	int End(Distance+1);
	vec2 Last = Pos0;

	for(int i = 0; i < End; i++)
	{
		float a = i/Distance;
		vec2 Pos = mix(Pos0, Pos1, a);
		if(!CheckPoint(Pos.x, Pos.y))
		{
			if(pOutCollision)
				*pOutCollision = Pos;
			if(pOutBeforeCollision)
				*pOutBeforeCollision = Last;
			return true;
		}
		Last = Pos;
	}
	if(pOutCollision)
		*pOutCollision = Pos1;
	if(pOutBeforeCollision)
		*pOutBeforeCollision = Pos1;
	return false;
}

bool CCollision::CheckTiles(vec2 Pos, int Deepness)
{
	for(int i = 0; i < Deepness; i++)
	{
		if(CheckPoint(Pos.x, Pos.y + i * 32))
			return true;
	}
	return false;
}

bool CCollision::CheckParable(vec2 Pos, int Wideness, int Direction)
{
	/*
	I need to tranlate the code with 32* in tiles
	Thinking that a normal jump is 23 blocks wide and the jumping space is P0/0, S(11.5/5)
	->St(368/160)
	y = a * (x - 368)^2 +160
	->a = -0.001181
	this parabel has in this coordinate system that form: ^ 
	so i don't need to change it
	*/
	for(int i = 2; i < Wideness; i++)
	{
		int x = Direction * 32 * i;//Tranlate from code to tiles
		if(CheckPlatform(vec2(Pos.x + x, Pos.y + 0.0021626 * (x - Direction * 272) * (x - Direction * 272) - 160), 7))//with other directions: St(-368/160)
			return true;//Found Solid, return true
	}
	return false;
}

bool CCollision::CheckPlatform(vec2 Pos, int Deepness)
{
	for(int i = 1; i < Deepness; i++)
	{
		if(CheckPoint(Pos.x, Pos.y + i * 32) && !CheckPoint(Pos.x, Pos.y))//landing place must be there
			return true;
		if(CheckPoint(Pos.x, Pos.y + i * 32) && CheckPoint(Pos.x, Pos.y))//For the case, landing space could be behind a wall
			return false;
	}
	return false;
}

bool CCollision::IntersectTile(vec2 Pos0, vec2 Pos1)
{
	float Distance = distance(Pos0, Pos1);
	int End = (Distance+1) / 16;//save some CPU

	for(int i = 0; i < End; i++)
	{
		float a = i * 16 / Distance;
		vec2 Pos = mix(Pos0, Pos1, a);
		if(CheckPoint(Pos.x, Pos.y))
		{
			return true;
		}

	}
	return false;
}
