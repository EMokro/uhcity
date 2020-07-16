/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */

#ifndef GAME_SERVER_ACCOUNT_H
#define GAME_SERVER_ACCOUNT_H
#include <engine/server.h> // ?
#include <game/server/player.h>

class CAccount
{
public:
	CAccount(CPlayer *pPlayer, CGameContext *pGameServer);

	void Login(char *Username, char *Password);
	bool OldLogin(char *Username, char *Password);
	void Register(char *Username, char *Password);
	void Apply();
	void Reset();
	void Delete();
	void NewPassword(char *NewPassword);
	void SetAuth(char *Username, int lvl);
	bool Exists(const char * Username);

	static const int MAX_SERVER = 2;
	int NextID();
	
	
	//bool LoggedIn(const char * Username);
	//int NameToID(const char * Username);

private:
	class CPlayer *m_pPlayer;
	class CGameContext *m_pGameServer;
	CGameContext *GameServer() const { return m_pGameServer; }
};

#endif
