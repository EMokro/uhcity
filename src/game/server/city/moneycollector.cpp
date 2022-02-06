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

    if (m_Money > g_Config.m_SvMcPotMax)
        m_Money = g_Config.m_SvMcPotMax;

    Apply();
}

void CMoneyCollector::Tick() {
    if (!(Server()->Tick() % (Server()->TickSpeed() * m_UpdateTimer))) { // every hour
        m_Money *= 1.5;

        if (m_Money > g_Config.m_SvMcPotMax)
            m_Money = g_Config.m_SvMcPotMax;

        Apply();
    }

    if (!(Server()->Tick() % (Server()->TickSpeed() * 600))) {
        m_Price -= m_Price * 0.05;
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
        GameServer()->SendChatTarget_Localization(ClientID, CHATCATEGORY_INFO, _("Only the holder can collect this money"));
        return;
    }

    if (!m_Money) {
        GameServer()->SendChatTarget_Localization(ClientID, CHATCATEGORY_INFO, _("There is nothing to collect :("));
        return;
    }

    pP->m_AccData.m_Money += m_Money;
    GameServer()->FormatInt(m_Money, numBuf);
    m_Money = 0;

    str_format(aBuf, sizeof aBuf, "You collected %s$", numBuf);
    GameServer()->SendChatTarget_Localization(ClientID, CHATCATEGORY_INFO, _("You collected {str:m}$"), "m", numBuf, NULL);
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
        GameServer()->SendChatTarget_Localization(ClientID, CHATCATEGORY_INFO, _("You must pay more than the indicated price"));
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
    GameServer()->SendChatTarget_Localization(-1, CHATCATEGORY_INFO, _("{str:n} is the new holder of the Moneycollector with a value of {str:m}$"), "n", Server()->ClientName(ClientID), "m", numBuf, NULL);

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
