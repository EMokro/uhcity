
#include "gameevent.h"
#include <engine/shared/config.h>
#include <stdlib.h>
#include "math.h"

IServer *CGameEvent::Server() const { return m_pGameServer->Server(); }

CGameEvent::CGameEvent(CGameContext *pGameServer) {
    m_pGameServer = pGameServer;
    m_Timer = (rand() % 60) + 50;

    m_isEvent = false;
}

void CGameEvent::Tick() {
    if (Server()->Tick() % 50 == 0) {
        if (!m_Timer && !m_isEvent) {
            Create(rand() % AMOUNT_EVENT, (rand() % (g_Config.m_SvEventDurationMax - g_Config.m_SvEventDurationMin)) + g_Config.m_SvEventDurationMin);
            m_isEvent = true;
        } else if ((!m_Timer && m_isEvent) || m_Escape) {
            char aEvent[32];

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

void CGameEvent::Create(int type, int duration) {
    char aEvent[32];

    switch (type)
    {
    case EVENT_BOUNTY:
        GameServer()->SendChat(-1, CGameContext::CHAT_ALL, "~~~~~ BOUNTY EVENT ~~~~~");
        GameServer()->SendChat(-1, CGameContext::CHAT_ALL, "We accept only dead tees.");

        Bounty();
        break;
    }

    m_CurrentEvent = type;
    m_Timer = duration;
    GetEventStr(type, aEvent, sizeof aEvent);
    dbg_msg("event", "'%s' Event started for %d seconds", aEvent, duration);
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
    
    default:
        str_format(Out, Size, "Unknown");
        break;
    }
}

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
