
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

            m_CurrentEvent = EVENT_NONE;
            m_isEvent = false;
            m_Escape = false;
            m_Timer = (rand() % (g_Config.m_SvEventTimerMax - g_Config.m_SvEventTimerMin)) + g_Config.m_SvEventTimerMin;

            if (!m_SilentEscape)
                GameServer()->SendChatTarget_Localization(-1, CHATCATEGORY_DEFAULT, _("Event Over!"));
            else
                m_SilentEscape = false;
        } else
        {
            m_Timer--;
        }
    }
}

void CGameEvent::Create(int Type, int Duration) {
    char aEvent[32];

    Reset(); // make sure we always have only one event effect

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
            GameServer()->SendChatTarget_Localization(-1, CHATCATEGORY_INFO, _("~~~~~ BOUNTY EVENT ~~~~~"));
            GameServer()->SendChatTarget_Localization(-1, CHATCATEGORY_INFO, _("We accept only dead tees."));
            return;
        case EVENT_MONEYEXP:
            GameServer()->SendChatTarget_Localization(-1, CHATCATEGORY_INFO, _("~~~~~ MONEY&EXP EVENT ~~~~~"));
            GameServer()->SendChatTarget_Localization(-1, CHATCATEGORY_JOIN, _("Duration: {sec:s} "), "s", &m_Timer, NULL);
            str_format(aBuf, sizeof aBuf, "Money: x%d", m_Multiplier);
            GameServer()->SendChat(-1, CGameContext::CHAT_ALL, aBuf);
            str_format(aBuf, sizeof aBuf, "Exp: x%d", m_Multiplier);
            GameServer()->SendChat(-1, CGameContext::CHAT_ALL, aBuf);
            return;
        case EVENT_RISINGMC:
            GameServer()->SendChatTarget_Localization(-1, CHATCATEGORY_INFO, _("~~~~~ RISING MC ~~~~~"));
            GameServer()->SendChatTarget_Localization(-1, CHATCATEGORY_INFO, _("The Moneycollectors pot will rise"));
            GameServer()->SendChatTarget_Localization(-1, CHATCATEGORY_JOIN, _("Duration: {sec:s}"), "s", &m_Timer, NULL);
            return;
        case EVENT_MONSTER:
            GameServer()->SendChatTarget_Localization(-1, CHATCATEGORY_INFO, _("~~~~~ ZOMB COMING!! ~~~~~"));
            GameServer()->SendChatTarget_Localization(-1, CHATCATEGORY_INFO, _("Zombie is coming! Kill them!"));
            GameServer()->SendChatTarget_Localization(-1, CHATCATEGORY_JOIN, _("Duration: {sec:s}"), "s", &m_Timer, NULL);
        }
    } else {
        switch (m_CurrentEvent)
        {
        case EVENT_BOUNTY:
            GameServer()->SendChatTarget_Localization(ClientID, CHATCATEGORY_INFO, _("~~~~~ BOUNTY EVENT ~~~~~"));
            GameServer()->SendChatTarget_Localization(ClientID, CHATCATEGORY_INFO, _("We accept only dead tees."));
            return;
        case EVENT_MONEYEXP:
            GameServer()->SendChatTarget_Localization(ClientID, CHATCATEGORY_INFO, _("~~~~~ MONEY&EXP EVENT ~~~~~"));
            GameServer()->SendChatTarget_Localization(ClientID, CHATCATEGORY_JOIN, _("Duration: {sec:s}"), "s", &m_Timer, NULL);
            str_format(aBuf, sizeof aBuf, "Money: x%d", m_Multiplier);
            GameServer()->SendChatTarget(ClientID, aBuf);
            str_format(aBuf, sizeof aBuf, "Exp: x%d", m_Multiplier);
            GameServer()->SendChatTarget(ClientID, aBuf);
            return;
        case EVENT_RISINGMC:
            GameServer()->SendChatTarget_Localization(ClientID, CHATCATEGORY_INFO, _("~~~~~ RISING MC ~~~~~"));
            GameServer()->SendChatTarget_Localization(ClientID, CHATCATEGORY_INFO, _("The Moneycollectors pot will rise"));
            GameServer()->SendChatTarget_Localization(ClientID, CHATCATEGORY_JOIN, _("Duration: {sec:s}"), "s", &m_Timer, NULL);
            return;
        case EVENT_MONSTER:
            GameServer()->SendChatTarget_Localization(ClientID, CHATCATEGORY_INFO, _("~~~~~ ZOMB COMING!! ~~~~~"));
            GameServer()->SendChatTarget_Localization(ClientID, CHATCATEGORY_INFO, _("Zombie is coming! Kill them!"));
            GameServer()->SendChatTarget_Localization(ClientID, CHATCATEGORY_JOIN, _("Duration: {sec:s}"), "s", &m_Timer, NULL);
            return;
        default:
            GameServer()->SendChatTarget_Localization(ClientID, CHATCATEGORY_INFO, _("There is no event :("));
        }
    }
}

void CGameEvent::Reset() {
    m_Multiplier = 1;
    GameServer()->MoneyCollector()->m_CollectPercentage = 0.04;
    GameServer()->m_pController->m_MonsterEvent = false;
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
    case EVENT_MONSTER:
        str_format(Out, Size, "Zomb");
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

    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (!GameServer()->m_apPlayers[i])
            continue;

        if (!GameServer()->m_apPlayers[i]->m_AccData.m_UserID)
            continue;
        
        Amount = (rand() % (g_Config.m_SvEventBountyMax - g_Config.m_SvEventBountyMin)) + g_Config.m_SvEventBountyMin;

        if (!GameServer()->m_apPlayers[i]->m_AccData.m_Bounty)
            GameServer()->AddToBountyList(i);

        GameServer()->m_apPlayers[i]->m_AccData.m_Bounty += Amount;
        GameServer()->FormatInt(Amount, numBuf);
        GameServer()->SendChatTarget_Localization(i, CHATCATEGORY_INFO, _("You got a bounty of {str:m}$"), "m", numBuf, NULL);
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

void CGameEvent::Monster()
{
    GameServer()->m_pController->m_MonsterEvent = true;
}