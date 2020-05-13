/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SERVER_CITY_CAROUSEL_H
#define GAME_SERVER_CITY_CAROUSEL_H

class CCarousel : public CEntity
{
public:
	CCarousel(CGameWorld *pGameWorld, vec2 Pos);

	virtual void Reset();
	virtual void Tick();
	virtual void Snap(int SnappingClient);
	
	void SetSize();

private:
	int m_IDs[6];
	int carouselX[6];
	int carouselY[6];
	int m_Size;
};

#endif
