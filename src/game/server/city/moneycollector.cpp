#include "moneycollector.h"

#include <fstream>

IServer *CMoneyCollector::Server() const { return m_pGameServer->Server(); }

CMoneyCollector::CMoneyCollector(CGameContext *pGameServer) {
    m_pGameServer = pGameServer;

    m_Money = 0;
    m_Price = 0;
    m_HolderID = 0;
    m_UpdateTimer = 3600;
    m_CollectPercentage = 0.04;
    str_copy(m_aHolderName, "", sizeof m_aHolderName);

    FILE *pFile = fopen("moneycollector.data", "r");

    if (pFile) {
        fscanf(pFile, "%llu\n%llu\n%d\n%s",
            &m_Money,
            &m_Price,
            &m_HolderID,
            m_aHolderName
        );
        fclose(pFile);
    }
}

void CMoneyCollector::AddMoney(long long unsigned Money) {
    m_Money += Money * m_CollectPercentage;
    Apply();
}

void CMoneyCollector::Tick() {
    if (!(Server()->Tick() % (Server()->TickSpeed() * m_UpdateTimer))) { // every hour
        m_Money *= 1.5;
        m_Price -= m_Price * 0.01;

        clamp(m_Money, (unsigned long long)0, (unsigned long long)10000000000);
        Apply();
    }
}

void CMoneyCollector::Collect(int ClientID) {
    CPlayer *pP = GameServer()->m_apPlayers[ClientID];
    char aBuf[256], numBuf[32];

    if (!GameServer()->ValidID(ClientID))
        return;

    if (!pP->m_AccData.m_UserID)
        return;

    if (pP->m_AccData.m_UserID != m_HolderID) {
        GameServer()->SendChatTarget(ClientID, "Only the holder can collect this money");
        return;
    }

    if (!m_Money) {
        GameServer()->SendChatTarget(ClientID, "There is nothing to collect :(");
        return;
    }

    pP->m_AccData.m_Money += m_Money;
    GameServer()->FormatInt(m_Money, numBuf);
    m_Money = 0;

    str_format(aBuf, sizeof aBuf, "You collected %s$", numBuf);
    GameServer()->SendChatTarget(ClientID, aBuf);
    Apply();
}

void CMoneyCollector::Buy(int ClientID, long long unsigned Amount) {
    CPlayer *pP = GameServer()->m_apPlayers[ClientID];
    char aBuf[256], numBuf[32];

    if (!GameServer()->ValidID(ClientID))
        return;

    if (!pP->m_AccData.m_UserID)
        return;

    if (Amount <= m_Price) {
        GameServer()->SendChatTarget(ClientID, "You must pay more than the indicated price");
        return;
    }

    if (pP->m_AccData.m_Money < Amount) {
        GameServer()->SendChatTarget(ClientID, "You don't have enough money");
        return;
    }

    pP->m_AccData.m_Money -= Amount;

    m_Price = Amount;
    m_HolderID = pP->m_AccData.m_UserID;
    str_copy(m_aHolderName, Server()->ClientName(ClientID), sizeof m_aHolderName);

    GameServer()->FormatInt(Amount, numBuf);
    str_format(aBuf, sizeof aBuf, "%s is the new holder of the Moneycollector with a value of %s$", Server()->ClientName(ClientID), numBuf);
    GameServer()->SendChat(-1, CGameContext::CHAT_ALL, aBuf);

    Apply();
}

void CMoneyCollector::Apply() {
    std::remove("moneycollector.data");

    FILE *pFile = fopen("moneycollector.data", "a+");
    char aBuf[256];

    str_format(aBuf, sizeof aBuf, "%llu\n%llu\n%d\n%s", m_Money, m_Price, m_HolderID, m_aHolderName);
    fputs(aBuf, pFile);
    fclose(pFile);
}
