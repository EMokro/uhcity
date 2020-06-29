/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_COLLISION_H
#define GAME_COLLISION_H

#include <base/vmath.h>

class CCollision
{
	class CTile *m_pTiles;
	class CTile *m_pCityTiles[5];
	int m_Width;
	int m_Height;
	class CLayers *m_pLayers;

	bool IsTileSolid(int x, int y);
	int GetTile(int x, int y);

public:
	enum
	{
		COLFLAG_SOLID=1,
		COLFLAG_DEATH=2,
		COLFLAG_NOHOOK=4,
	};

	CCollision();
	~CCollision();
	void Init(class CLayers *pLayers);
	bool CheckPoint(float x, float y) { return IsTileSolid(round_to_int(x), round_to_int(y)); }
	bool CheckPoint(vec2 Pos) { return CheckPoint(Pos.x, Pos.y); }
	int GetCollisionAt(float x, float y) { return GetTile(round_to_int(x), round_to_int(y)); }
	int GetWidth() { return m_Width; };
	int GetHeight() { return m_Height; };
	int IntersectLine(vec2 Pos0, vec2 Pos1, vec2 *pOutCollision, vec2 *pOutBeforeCollision);
	void MovePoint(vec2 *pInoutPos, vec2 *pInoutVel, float Elasticity, int *pBounces);
	void MoveBox(vec2 *pInoutPos, vec2 *pInoutVel, vec2 Size, float Elasticity);
	bool TestBox(vec2 Pos, vec2 Size);

	// City
	void Dest();
	int Number(int x, int y);
	int Number(vec2 Pos) { return Number(Pos.x, Pos.y); }

	int IsTile(int x, int y, int Type);
	int IsTile(vec2 Pos, int Type) { return IsTile(Pos.x, Pos.y, Type); }

	int TileShop(int x, int y);
	int TileShop(vec2 Pos) { return TileShop(Pos.x, Pos.y); }

	int TileMoney(int x, int y);
	int TileMoney(vec2 Pos) { return TileMoney(Pos.x, Pos.y); }

private:
	class CTile *m_pSubGameLayer[4];
};

#endif
