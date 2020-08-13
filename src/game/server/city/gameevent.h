#ifndef GAME_SERVER_GAMEEVENT_H
#define GAME_SERVER_GAMEEVENT_H

#include <engine/server.h>

#include "game/server/gamecontext.h"

class CGameEvent {
    private:
        CGameContext *m_pGameServer;

        IServer *Server() const;
        CGameContext *GameServer() const { return m_pGameServer; }

        // Resets all Event effekts
        void Reset();

        // Events
        void Bounty();
        void MoneyExp(int Amount);
        void RisingMC();

        bool m_Escape;
        bool m_SilentEscape;
        bool m_isEvent;

    public:
        CGameEvent(CGameContext *pGameServer);
        
        void Tick();
        void Create(int Type, int Duration);
        void EventInfo(int ClientID = -1);
        void Abort();
        void GetEventStr(int ID, char *Out, int Size);

        int m_Timer;
        int m_CurrentEvent;
        int m_Multiplier;

        enum {
            EVENT_BOUNTY = 0,
            EVENT_MONEYEXP,
            EVENT_RISINGMC,

            AMOUNT_EVENT
        };
};

#endif