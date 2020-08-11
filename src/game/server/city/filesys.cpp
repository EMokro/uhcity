#include <string.h>
#include <fstream>
#include <time.h>
#include <sys/stat.h>
#include <engine/config.h>
#include "filesys.h"
#include "base/rapidjson/document.h"
#include "base/rapidjson/reader.h"
#include "base/rapidjson/writer.h"

using namespace rapidjson;

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
    char nameBuf[256];
    char aBuf[256];
    time_t now = time(NULL);
    tm tm = *localtime(&now);
    char aAddr[NETADDR_MAXSTRSIZE];
    FILE *File;
    str_format(nameBuf, sizeof nameBuf, "logs/login/%d-%02d-%02d.txt", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
    File = fopen(nameBuf, "a+");

    if(!File)
        system("mkdir logs/login");

    GameServer()->Server()->GetClientAddr(Player->GetCID(), aAddr, NETADDR_MAXSTRSIZE);
    str_format(aBuf, sizeof aBuf, "%02d:%02d [ %s ]: logged in as [ %s ]\n",
        tm.tm_hour,
        tm.tm_min,
        aAddr,
        Player->m_AccData.m_Username);
    fputs(aBuf, File);
    fclose(File);
}

void CFileSys::CreateDebugLog(int Level, const char *pFrom, const char *pStr) {
    char nameBuf[256];
    time_t now = time(NULL);
    tm tm = *localtime(&now);
    FILE *File;

    str_format(nameBuf, sizeof nameBuf, "logs/debug/%d-%02d-%02d.txt", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
    File = fopen(nameBuf, "a+");

    if(!File)
        system("mkdir logs/debug");
}