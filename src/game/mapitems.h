/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_MAPITEMS_H
#define GAME_MAPITEMS_H

// layer types
enum
{
	LAYERTYPE_INVALID=0,
	LAYERTYPE_GAME, // not used
	LAYERTYPE_TILES,
	LAYERTYPE_QUADS,

	MAPITEMTYPE_VERSION=0,
	MAPITEMTYPE_INFO,
	MAPITEMTYPE_IMAGE,
	MAPITEMTYPE_ENVELOPE,
	MAPITEMTYPE_GROUP,
	MAPITEMTYPE_LAYER,
	MAPITEMTYPE_ENVPOINTS,


	CURVETYPE_STEP=0,
	CURVETYPE_LINEAR,
	CURVETYPE_SLOW,
	CURVETYPE_FAST,
	CURVETYPE_SMOOTH,
	NUM_CURVETYPES,

	// game layer tiles
	ENTITY_NULL=0,
	ENTITY_HOME,
	ENTITY_SPAWN,
	ENTITY_JAIL,
	ENTITY_INSTA_SPAWN,
	ENTITY_FLAGSTAND_RED,
	ENTITY_FLAGSTAND_BLUE,
	ENTITY_ARMOR_1,
	ENTITY_HEALTH_1,
	ENTITY_WEAPON_SHOTGUN,
	ENTITY_WEAPON_GRENADE,
	ENTITY_POWERUP_NINJA,
	ENTITY_WEAPON_RIFLE,
	ENTITY_WEAPON_HAMMER,
	ENTITY_WEAPON_GUN,

	// City
	ENTITY_CAROUSEL,
	ENTITY_POWERUP,

	ENTITY_AFK,
	ENTITY_MEVENT,

	ENTITY_DOOR_ID=27,
	ENTITY_DOOR_NR,
	ENTITY_TRIGGER_ID,
	ENTITY_TRIGGER_NR,
	ENTITY_FROM,
	ENTITY_TO,
	ENTITY_MONSTER,
	ENTITY_MONSTER_PSPAWN,

	NUM_ENTITIES,

	TILE_AIR=0,
	TILE_SOLID,
	TILE_DEATH,
	TILE_NOHOOK,

	TILE_SHOP,
	TILE_MONEY100,
	TILE_MONEY200,
	TILE_MONEY500,
	TILE_MONEY_DONOR,

	TILE_KILL,
	TILE_PROTECT,
	TILE_LIFE,
	TILE_POLICE,
	TILE_ADMIN,
	TILE_DONOR,
	TILE_VIP,
	
	TILE_BOOST_DOWN,
	TILE_BOOST_UP,
	TILE_BOOST_RIGHT,
	TILE_BOOST_LEFT,
	TILE_BOOST_HORI,
	TILE_BOOST_VERT,
	TILE_BOOST_ALL,

	TILE_BOOSTF_DOWN,
	TILE_BOOSTF_UP,
	TILE_BOOSTF_RIGHT,
	TILE_BOOSTF_LEFT,
	TILE_BOOSTF_HORI,
	TILE_BOOSTF_VERT,
	TILE_BOOSTF_ALL,

	TILE_ANTI_TELE,	
	TILE_DOOR_END,

	TILE_BOOSTW_DOWN,
	TILE_BOOSTW_UP,
	TILE_BOOSTW_RIGHT,
	TILE_BOOSTW_LEFT,
	TILE_BOOSTW_HORI,
	TILE_BOOSTW_VERT,

	TILE_UNFREEZE,
	TILE_SINGLE_FREEZE,
	TILE_FREEZE,
	TILE_NOFREEZE,
	TILE_WATER,
	TILE_RACE_START,
	TILE_RACE_END,
	TILE_TRAINER,

	TILE_INSTA_START,
	TILE_INSTA_STOP,

	TILE_BOOSTFW_DOWN,
	TILE_BOOSTFW_UP,
	TILE_BOOSTFW_RIGHT,
	TILE_BOOSTFW_LEFT,
	TILE_BOOSTFW_HORI,
	TILE_BOOSTFW_VERT,

	TILE_0,
	TILE_1,
	TILE_2,
	TILE_3,
	TILE_4,
	TILE_5,
	TILE_6,
	TILE_7,
	TILE_8,
	TILE_9,

	TILE_GAMEZONE_START,
	TILE_GAMEZONE_END,

	//money tiles
	TILE_MONEY50,
	TILE_MONEY1000,
	
	TILE_AFK,
	TILE_NORMAL_GRAVITY,
	TILE_SPACE_GRAVITY,
	TILE_MONEYCOLLECTOR,

	TILEFLAG_VFLIP=1,
	TILEFLAG_HFLIP=2,
	TILEFLAG_OPAQUE=4,
	TILEFLAG_ROTATE=8,

	LAYERFLAG_DETAIL=1,
	TILESLAYERFLAG_GAME=1,

	ENTITY_OFFSET=255-16*4,
};

struct CPoint
{
	int x, y; // 22.10 fixed point
};

struct CColor
{
	int r, g, b, a;
};

struct CQuad
{
	CPoint m_aPoints[5];
	CColor m_aColors[4];
	CPoint m_aTexcoords[4];

	int m_PosEnv;
	int m_PosEnvOffset;

	int m_ColorEnv;
	int m_ColorEnvOffset;
};

class CTile
{
public:
	unsigned char m_Index;
	unsigned char m_Flags;
	unsigned char m_Skip;
	unsigned char m_Reserved;
};

class CDoorTile
{
public:
	unsigned char m_Index;
	unsigned char m_Flags;
	unsigned char m_Skip;
	int m_DoorID; // account/door id
};

struct CMapItemImage
{
	int m_Version;
	int m_Width;
	int m_Height;
	int m_External;
	int m_ImageName;
	int m_ImageData;
} ;

struct CMapItemGroup_v1
{
	int m_Version;
	int m_OffsetX;
	int m_OffsetY;
	int m_ParallaxX;
	int m_ParallaxY;

	int m_StartLayer;
	int m_NumLayers;
} ;


struct CMapItemGroup : public CMapItemGroup_v1
{
	enum { CURRENT_VERSION=3 };

	int m_UseClipping;
	int m_ClipX;
	int m_ClipY;
	int m_ClipW;
	int m_ClipH;

	int m_aName[3];
} ;

struct CMapItemLayer
{
	int m_Version;
	int m_Type;
	int m_Flags;
} ;

struct CMapItemLayerTilemap
{
	CMapItemLayer m_Layer;
	int m_Version;

	int m_Width;
	int m_Height;
	int m_Flags;

	CColor m_Color;
	int m_ColorEnv;
	int m_ColorEnvOffset;

	int m_Image;
	int m_Data;

	int m_aName[3];
} ;

struct CMapItemLayerQuads
{
	CMapItemLayer m_Layer;
	int m_Version;

	int m_NumQuads;
	int m_Data;
	int m_Image;

	int m_aName[3];
} ;

struct CMapItemVersion
{
	int m_Version;
} ;

struct CEnvPoint
{
	int m_Time; // in ms
	int m_Curvetype;
	int m_aValues[4]; // 1-4 depending on envelope (22.10 fixed point)

	bool operator<(const CEnvPoint &Other) { return m_Time < Other.m_Time; }
} ;

struct CMapItemEnvelope
{
	int m_Version;
	int m_Channels;
	int m_StartPoint;
	int m_NumPoints;
	int m_aName[8];
} ;

// city
class CSubGameLayer {
	public:
		unsigned char m_Type;
} ;

#endif
