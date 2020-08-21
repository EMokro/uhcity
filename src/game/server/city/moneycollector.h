#ifndef GAME_SERVER_MONEYCOLLECTOR_H
#define GAME_SERVER_MONEYCOLLECTOR_H

#include <engine/server.h>

#include "game/server/gamecontext.h"

class CMoneyCollector {
    private:
        CGameContext *m_pGameServer;

        IServer *Server() const;
        CGameContext *GameServer() const { return m_pGameServer; }

        void Apply();
        int m_HolderID;
    public:
        CMoneyCollector(CGameContext *pGameServer);
        
        void Tick();
        void Buy(int ClientID, long long unsigned Amount);
        void Collect(int ClientID);

        void AddMoney(long long unsigned Money);

        long long unsigned m_Price;
        long long unsigned m_Money;
        int m_UpdateTimer;
        float m_CollectPercentage;
        char m_aHolderName[256];
};

#endif