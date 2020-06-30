#ifndef GAME_SERVER_CITY_FILESYS_H
#define GAME_SERVER_CITY_FILESYS_H

#include <engine/server.h>
#include "game/server/gamecontext.h"
#include "base/rapidjson/document.h"

class CFileSys 
{
public:
    CFileSys(CGameContext *pGameServer);

    void Init();
    void BackupAccounts();
    void CreateRconLog();
    void CreateLoginLog(CPlayer *Player);
    void CreateNote(char *Msg, char *OwnerID);

private:
	CGameContext *m_pGameServer;
	CGameContext *GameServer() const { return m_pGameServer; }
};

#endif