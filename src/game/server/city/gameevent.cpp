
#include "gameevent.h"
#include <engine/shared/config.h>
#include <stdlib.h>
#include "math.h"

IServer *CGameEvent::Server() const { return m_pGameServer->Server(); }

CGameEvent::CGameEvent(CGameContext *pGameServer) {
    m_pGameServer = pGameServer;
    m_Timer = (rand() % 60) + 50;

    m_isEvent = false;
    m_Multiplier = 1;
}

void CGameEvent::Tick() {
    if (Server()->Tick() % 50 == 0) {
        if (!m_Timer && !m_isEvent) {
            Create(rand() % AMOUNT_EVENT, (rand() % (g_Config.m_SvEventDurationMax - g_Config.m_SvEventDurationMin)) + g_Config.m_SvEventDurationMin);
            m_isEvent = true;
        } else if ((!m_Timer && m_isEvent) || m_Escape) {
            char aEvent[32];

            Reset();

            GetEventStr(m_CurrentEvent, aEvent, sizeof aEvent);
            dbg_msg("event", "'%s' Event ended", aEvent);

            m_CurrentEvent = -1;
            m_isEvent = false;
            m_Escape = false;
            m_Timer = (rand() % (g_Config.m_SvEventTimerMax - g_Config.m_SvEventTimerMin)) + g_Config.m_SvEventTimerMin;

            if (!m_SilentEscape)
                GameServer()->SendChat(-1, CGameContext::CHAT_ALL, "Event Over!");
            else
                m_SilentEscape = false;
        } else
        {
            m_Timer--;
        }
    }
}

void CGameEvent::Create(int Type, int Duration) {
    char aEvent[32], aBuf[256];

    switch (Type)
    {
    case EVENT_BOUNTY:
        Bounty();
        break;
    case EVENT_MONEYEXP:
        MoneyExp((rand() % 3) + 2);
        break;
    case EVENT_RISINGMC:
        RisingMC();
        break;
    default:
        break;
    }

    m_CurrentEvent = Type;
    m_Timer = Duration;
    EventInfo();
    GetEventStr(Type, aEvent, sizeof aEvent);
    dbg_msg("event", "'%s' Event started for %d seconds", aEvent, Duration);
}

void CGameEvent::EventInfo(int ClientID) {
    char aBuf[256];
    if (ClientID == -1) {
        switch (m_CurrentEvent)
        {
        case EVENT_BOUNTY:
            GameServer()->SendChat(-1, CGameContext::CHAT_ALL, "~~~~~ BOUNTY EVENT ~~~~~");
            GameServer()->SendChat(-1, CGameContext::CHAT_ALL, "We accept only dead tees.");
            return;
        case EVENT_MONEYEXP:
            GameServer()->SendChat(-1, CGameContext::CHAT_ALL, "~~~~~ MONEY&EXP EVENT ~~~~~");
            str_format(aBuf, sizeof aBuf, "Duration: %d minute%s %d seconds", m_Timer/60, (m_Timer/60) == 1 ? "" : "s", m_Timer%60);
            GameServer()->SendChat(-1, CGameContext::CHAT_ALL, aBuf);
            str_format(aBuf, sizeof aBuf, "Money: x%d", m_Multiplier);
            GameServer()->SendChat(-1, CGameContext::CHAT_ALL, aBuf);
            str_format(aBuf, sizeof aBuf, "Exp: x%d", m_Multiplier);
            GameServer()->SendChat(-1, CGameContext::CHAT_ALL, aBuf);
            return;
        case EVENT_RISINGMC:
            GameServer()->SendChat(-1, CGameContext::CHAT_ALL, "~~~~~ RISING MC ~~~~~");
            GameServer()->SendChat(-1, CGameContext::CHAT_ALL, "The Moneycollectors pot will rise");
            str_format(aBuf, sizeof aBuf, "Duration: %d minutes %d seconds", m_Timer/60, m_Timer%60);
            GameServer()->SendChat(-1, CGameContext::CHAT_ALL, aBuf);
            return;
        }
    } else {
        switch (m_CurrentEvent)
        {
        case EVENT_BOUNTY:
            GameServer()->SendChatTarget(ClientID, "~~~~~ BOUNTY EVENT ~~~~~");
            GameServer()->SendChatTarget(ClientID, "We accept only dead tees.");
            return;
        case EVENT_MONEYEXP:
            GameServer()->SendChatTarget(ClientID, "~~~~~ MONEY&EXP EVENT ~~~~~");
            str_format(aBuf, sizeof aBuf, "Duration: %d minutes %d seconds", m_Timer/60, m_Timer%60);
            GameServer()->SendChatTarget(ClientID, aBuf);
            str_format(aBuf, sizeof aBuf, "Money: x%d", m_Multiplier);
            GameServer()->SendChatTarget(ClientID, aBuf);
            str_format(aBuf, sizeof aBuf, "Exp: x%d", m_Multiplier);
            GameServer()->SendChatTarget(ClientID, aBuf);
            return;
        case EVENT_RISINGMC:
            GameServer()->SendChatTarget(ClientID, "~~~~~ RISING MC ~~~~~");
            GameServer()->SendChatTarget(ClientID, "The Moneycollectors pot will rise");
            str_format(aBuf, sizeof aBuf, "Duration: %d minutes %d seconds", m_Timer/60, m_Timer%60);
            GameServer()->SendChatTarget(ClientID, aBuf);
            return;
        default:
            GameServer()->SendChatTarget(ClientID, "There is no event :(");
        }
    }
}

void CGameEvent::Reset() {
    m_Multiplier = 1;
    GameServer()->MoneyCollector()->m_UpdateTimer = 3600;
    GameServer()->MoneyCollector()->m_CollectPercentage = 0.04;
}

void CGameEvent::Abort() {
    if (!m_isEvent)
        return;
    
    m_Escape = true;
}

void CGameEvent::GetEventStr(int ID, char *Out, int Size) {
    switch (ID)
    {
    case EVENT_BOUNTY:
        str_format(Out, Size, "Bounty");
        break;
    case EVENT_MONEYEXP:
        str_format(Out, Size, "Money&Exp");
        break;
    case EVENT_RISINGMC:
        str_format(Out, Size, "RisngMC");
        break;
    default:
        str_format(Out, Size, "Unknown");
        break;
    }
}

// Events

void CGameEvent::Bounty() {
    char aBuf[128], numBuf[16];
    int Amount;

    for (int i = 0; i < 16; i++) {
        if (!GameServer()->m_apPlayers[i])
            continue;

        if (!GameServer()->m_apPlayers[i]->m_AccData.m_UserID)
            continue;
        
        Amount = (rand() % (g_Config.m_SvEventBountyMax - g_Config.m_SvEventBountyMin)) + g_Config.m_SvEventBountyMin;

        if (!GameServer()->m_apPlayers[i]->m_AccData.m_Bounty)
            GameServer()->AddToBountyList(i);

        GameServer()->m_apPlayers[i]->m_AccData.m_Bounty += Amount;
        GameServer()->FormatInt(Amount, numBuf);
        str_format(aBuf, sizeof aBuf, "You got a bounty of %s$", numBuf);
        GameServer()->SendChatTarget(i, aBuf);
    }

    m_Escape = true;
    m_SilentEscape = true;
}

void CGameEvent::MoneyExp(int Amount) {
    m_Multiplier = Amount;
}

void CGameEvent::RisingMC() {
    GameServer()->MoneyCollector()->m_CollectPercentage = 1.5;
}
