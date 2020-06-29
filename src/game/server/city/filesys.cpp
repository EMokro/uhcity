#include <string.h>
#include <fstream>
#include <time.h>
#include <sys/stat.h>
#include <engine/config.h>
#include "filesys.h"

CFileSys::CFileSys(CGameContext *pGameServer) {
    m_pGameServer = pGameServer;
}

void CFileSys::BackupAccounts() {
	dbg_msg("filesys", "Creating a backup");

	char aBuf[256];
	time_t now = time(NULL);
	tm tm = *localtime(&now);

	str_format(aBuf, sizeof aBuf, "mkdir -p ./backups/account && cp -r ./accounts ./backups/account/%d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);

	system(aBuf);
}

void CFileSys::CreateRconLog() {

}

void CFileSys::CreateLoginLog(CPlayer *Player) {
    // char aBuf[256];
    // char aAddr[NETADDR_MAXSTRSIZE];
    // FILE *Logfile;
    // Logfile = fopen("logs/login.txt", "r");

    // if(!Logfile)
    // {
    //     system("mkdir logs & touch logs/login.txt");
    //     Logfile = fopen("logs/login.txt", "r");
    // }

    // GameServer()->Server()->GetClientAddr(Player->GetCID(), aAddr, NETADDR_MAXSTRSIZE);
    // str_format(aBuf, sizeof aBuf, "[ %s ]: [ %s ] logged in as %d",
    //     aAddr,
    //     Player->m_AccData.m_Username,
    //     GameServer()->Server()->AuthLvl(Player->GetCID()));
    
    // fputs(aBuf, Logfile);
    // fclose(Logfile);
}