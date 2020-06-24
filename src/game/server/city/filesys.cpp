
#include <engine/config.h>
#include <string.h>
#include <fstream>
#include <experimental/filesystem>
#include <time.h>
#include <sys/stat.h>
#include "filesys.h"
#include "account.h"

namespace fs = std::experimental::filesystem;

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

void CFileSys::UpdateAccounts() {
	char aBuf[256];
	dbg_msg("accounts", "Starting Account Update");
	for (auto& p: fs::directory_iterator("accounts")) {

        if (!str_comp(p.path().string().c_str(), "accounts/++UserIds++.acc"))
            continue;

        char AccUsername[32];
        char AccPassword[32];
    
        FILE *Accfile;
        Accfile = fopen(p.path().string().c_str(), "r");
        fscanf(Accfile, "%s\n%s", AccUsername, AccPassword);

        CPlayer pPlayer = CPlayer(GameServer(), 15, TEAM_SPECTATORS);
        pPlayer.m_AccData.m_UserID = 0;
        pPlayer.m_pAccount->Login(AccUsername, AccPassword);
        pPlayer.m_pAccount->Apply();


        fclose(Accfile);
	}
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