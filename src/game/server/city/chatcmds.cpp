#include <game/server/gamecontext.h>
#include <game/server/entities/character.h>
#include <engine/shared/config.h>
#include <game/version.h>

// account
void CGameContext::ConChatLogin(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    if (pResult->NumArguments() != 2) {
        pSelf->SendChatTarget(pResult->GetClientID(), "usage: /login <username> <password>");
        return;
    }

    char Username[512];
    char Password[512];
    str_copy(Username, pResult->GetString(0), sizeof(Username));
    str_copy(Password, pResult->GetString(1), sizeof(Password));
	
    pSelf->m_apPlayers[pResult->GetClientID()]->m_pAccount->Login(Username, Password);
}

void CGameContext::ConChatRegister(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;

    if (pResult->NumArguments() != 2) {
        pSelf->SendChatTarget(pResult->GetClientID(), "usage: /register <username> <password>");
        return;
    }

    char Username[512];
    char Password[512];
    str_copy(Username, pResult->GetString(0), sizeof(Username));
    str_copy(Password, pResult->GetString(1), sizeof(Password));

    pSelf->m_apPlayers[pResult->GetClientID()]->m_pAccount->Register(Username, Password);
}

void CGameContext::ConChatLogout(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    CCharacter *pChr = pP->GetCharacter();

    if (!pChr && !pChr->IsAlive())
        return;

    if (!pP->m_AccData.m_UserID)
        return;

    pP->m_pAccount->Apply();
    pP->m_pAccount->Reset();

    pSelf->SendChatTarget(pP->GetCID(), "Logout succesful");
    pChr->Die(pP->GetCID(), WEAPON_GAME);
}

void CGameContext::ConChatChangePw(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;

    if (pResult->NumArguments() != 1) {
        pSelf->SendChatTarget(pResult->GetClientID(), "usage: /changepw <password>");
        return;
    }

    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    CCharacter *pChr = pP->GetCharacter();
    char NewPw[512];
    str_copy(NewPw, pResult->GetString(0), sizeof(NewPw));

    if (!pChr && !pChr->IsAlive())
        return;

    if (!pP->m_AccData.m_UserID)
        return;

    pP->m_pAccount->NewPassword(NewPw);
    pP->m_pAccount->Apply();

    pSelf->SendChatTarget(pP->GetCID(), "Password changed");
}

// functions
// donor
void CGameContext::ConChatSave(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    CCharacter *pChr = pP->GetCharacter();

    if (!pChr && !pChr->IsAlive() && !pP->m_Insta)
        return;

    if (pP->GetCharacter()->m_Frozen || pP->m_AccData.m_Arrested) {
        pSelf->SendChatTarget(pP->GetCID(), "You can't do this at the moment");
        return;
    }

    if (!pP->m_AccData.m_Donor) {
        pSelf->SendChatTarget(pP->GetCID(), "This is a donor feature");
        return;
    }
    
    pChr->SaveLoad(false);
}

void CGameContext::ConChatLoad(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    CCharacter *pChr = pP->GetCharacter();

    if (!pChr && !pChr->IsAlive() && !pP->m_Insta)
        return;

    if (pP->GetCharacter()->m_Frozen || pP->m_AccData.m_Arrested) {
        pSelf->SendChatTarget(pP->GetCID(), "You can't do this at the moment");
        return;
    }

    if (!pP->m_AccData.m_Donor) {
        pSelf->SendChatTarget(pP->GetCID(), "This is a donor feature");
        return;
    }
    
    pChr->SaveLoad(true);
}

void CGameContext::ConChatTele(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    CCharacter *pChr = pP->GetCharacter();

    if (!pChr && !pChr->IsAlive() && !pP->m_Insta)
        return;

    if (pP->GetCharacter()->m_Frozen || pP->m_AccData.m_Arrested) {
        pSelf->SendChatTarget(pP->GetCID(), "You can't do this at the moment");
        return;
    }

    if (!pP->m_AccData.m_Donor) {
        pSelf->SendChatTarget(pP->GetCID(), "This is a donor feature");
        return;
    }
    
    pChr->Tele();
}

void CGameContext::ConChatHome(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    CCharacter *pChr = pP->GetCharacter();

    if (!pChr && !pChr->IsAlive() && !pP->m_Insta)
        return;

    if (pP->GetCharacter()->m_Frozen || pP->m_AccData.m_Arrested) {
        pSelf->SendChatTarget(pP->GetCID(), "You can't do this at the moment");
        return;
    }

    if (!pP->m_AccData.m_Donor) {
        pSelf->SendChatTarget(pP->GetCID(), "This is a donor feature");
        return;
    }
    
    vec2 oldPos = pChr->m_Core.m_Pos;
    pChr->m_Home = pP->m_AccData.m_UserID;
    
    if (pChr->m_Core.m_Pos != oldPos)
        pSelf->SendChatTarget(pP->GetCID(), "Welcome Home :)");
}

void CGameContext::ConChatRainbow(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    CCharacter *pChr = pP->GetCharacter();
    char aBuf[128];

    if (!pChr && !pChr->IsAlive())
        return;

    if (!pP->m_AccData.m_Donor) {
        pSelf->SendChatTarget(pP->GetCID(), "This is a donor feature");
        return;
    }
    
    pP->m_Rainbow ^= true;
    str_format(aBuf, sizeof(aBuf), "%s Rainbow", pP->m_Rainbow ? "Enabled" : "Disabled");
    pSelf->SendChatTarget(pP->GetCID(), aBuf);
}

void CGameContext::ConChatCrown(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    CCharacter *pChr = pP->GetCharacter();
    char aBuf[128];

    if (!pChr && !pChr->IsAlive())
        return;

    if (!pP->m_AccData.m_Donor) {
        pSelf->SendChatTarget(pP->GetCID(), "This is a donor feature");
        return;
    }
    
    pP->m_Crown ^= true;
    str_format(aBuf, sizeof(aBuf), "%s Crown", pP->m_Crown ? "Enabled" : "Disabled");
    pSelf->SendChatTarget(pP->GetCID(), aBuf);
}

// vip
void CGameContext::ConChatUp(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    CCharacter *pChr = pP->GetCharacter();

    if (!pChr && !pChr->IsAlive() && !pP->m_Insta)
        return;

    if (pP->GetCharacter()->m_Frozen || pP->m_AccData.m_Arrested || pChr->m_GameZone) {
        pSelf->SendChatTarget(pP->GetCID(), "You can't do this at the moment");
        return;
    }

    if (!pP->m_AccData.m_VIP) {
        pSelf->SendChatTarget(pP->GetCID(), "This is a vip feature");
        return;
    }
    
    pChr->Move(0);
}

void CGameContext::ConChatDown(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    CCharacter *pChr = pP->GetCharacter();

    if (!pChr && !pChr->IsAlive() && !pP->m_Insta)
        return;

    if (pP->GetCharacter()->m_Frozen || pP->m_AccData.m_Arrested || pChr->m_GameZone) {
        pSelf->SendChatTarget(pP->GetCID(), "You can't do this at the moment");
        return;
    }

    if (!pP->m_AccData.m_VIP) {
        pSelf->SendChatTarget(pP->GetCID(), "This is a vip feature");
        return;
    }
    
    pChr->Move(2);
}

void CGameContext::ConChatLeft(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    CCharacter *pChr = pP->GetCharacter();

    if (!pChr && !pChr->IsAlive() && !pP->m_Insta)
        return;

    if (pP->GetCharacter()->m_Frozen || pP->m_AccData.m_Arrested || pChr->m_GameZone) {
        pSelf->SendChatTarget(pP->GetCID(), "You can't do this at the moment");
        return;
    }

    if (!pP->m_AccData.m_VIP) {
        pSelf->SendChatTarget(pP->GetCID(), "This is a vip feature");
        return;
    }
    
    pChr->Move(1);
}

void CGameContext::ConChatRight(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    CCharacter *pChr = pP->GetCharacter();

    if (!pChr && !pChr->IsAlive() && !pP->m_Insta)
        return;

    if (pP->GetCharacter()->m_Frozen || pP->m_AccData.m_Arrested || pChr->m_GameZone) {
        pSelf->SendChatTarget(pP->GetCID(), "You can't do this at the moment");
        return;
    }

    if (!pP->m_AccData.m_VIP) {
        pSelf->SendChatTarget(pP->GetCID(), "This is a vip feature");
        return;
    }
    
    pChr->Move(3);
}

// authed
void CGameContext::ConChatJailrifle(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    CCharacter *pChr = pP->GetCharacter();
    char aBuf[128];

    if (!pChr && !pChr->IsAlive() && !pP->m_Insta)
        return;

    if (!pSelf->Server()->AuthLvl(pP->GetCID())) {
        pSelf->SendChatTarget(pP->GetCID(), "Only for police");
        return;
    }

    pChr->m_JailRifle ^= 1;
    str_format(aBuf, sizeof(aBuf), "JailRifle %s", pChr->m_JailRifle ? "enabled" : "disabled");
    pSelf->SendChatTarget(pP->GetCID(), aBuf);
}

void CGameContext::ConChatGod(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    char aBuf[128];

    if (!(pSelf->Server()->IsMod(pP->GetCID()) || pSelf->Server()->IsAdmin(pP->GetCID())))
        return;

    pP->m_God ^= true;
    str_format(aBuf, sizeof(aBuf), "God %s", pP->m_God ? "enabled" : "disabled");
    pSelf->SendChatTarget(pP->GetCID(), aBuf);
}

// all
void CGameContext::ConChatInstagib(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    CCharacter *pChr = pP->GetCharacter();
    char aBuf[128];

    if (!g_Config.m_EnableInstagib) {
        pSelf->SendChatTarget(pP->GetCID(), "Instagib is not enabled");
        return;
    }

    if (!pChr && !pChr->IsAlive())
        return;

    if (pChr->m_Frozen || pP->m_AccData.m_Arrested || pChr->m_GameZone) {
        pSelf->SendChatTarget(pP->GetCID(), "You can't do this at the moment");
        return;
    }

    pP->m_Insta ^= 1;
    pChr->Die(pP->GetCID(), WEAPON_GAME);
    str_format(aBuf, sizeof(aBuf), "%s %s Instagib", pSelf->Server()->ClientName(pP->GetCID()), pP->m_Insta ? "joined" : "left");
    pSelf->SendChat(-1, CGameContext::CHAT_ALL, aBuf);
}

void CGameContext::ConChatTransfer(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;

    if (pResult->NumArguments() != 1) {
        pSelf->SendChatTarget(pResult->GetClientID(), "usage: /transfer <amount>");
        return;
    }

    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    CCharacter *pChr = pP->GetCharacter();
    long long Money = pResult->GetLongLong(0);

    if (!pChr && !pChr->IsAlive())
        return;

    if (pChr->m_Frozen || pChr->m_GameZone) {
        pSelf->SendChatTarget(pP->GetCID(), "You can't do this at the moment");
        return;
    }

    pChr->Transfer(Money);
}

void CGameContext::ConChatDisabledmg(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    int Victim = pResult->GetVictim();

    if (Victim < 0) {
        pSelf->SendChatTarget(pResult->GetClientID(), "usage: /disabledmg <id>");
        return;
    }

    if (pP->GetCID() == Victim) {
        pSelf->SendChatTarget(pP->GetCID(), "You can't disabledmg on yourself");
        return;
    }

    if (!pSelf->ValidID(Victim)) {
        pSelf->SendChatTarget(pP->GetCID(), "Out of range");
        return;
    }

    if (!pSelf->Server()->ClientIngame(Victim)) {
        pSelf->SendChatTarget(pP->GetCID(), "No such player");
        return;
    }

    pSelf->DisableDmg(pP->GetCID(), Victim);
}

void CGameContext::ConChatEnabledmg(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    int Victim = pResult->GetVictim();

    if (Victim < 0) {
        pSelf->SendChatTarget(pResult->GetClientID(), "usage: /enabledmg <id>");
        return;
    }

    if (pP->GetCID() == Victim) {
        pSelf->SendChatTarget(pP->GetCID(), "You can't enabledmg on yourself");
        return;
    }

    if (!pSelf->ValidID(Victim)) {
        pSelf->SendChatTarget(pP->GetCID(), "Out of range");
        return;
    }

    if (!pSelf->Server()->ClientIngame(Victim)) {
        pSelf->SendChatTarget(pP->GetCID(), "No such player");
        return;
    }

    pSelf->EnableDmg(pP->GetCID(), Victim);
}

void CGameContext::ConChatTrain(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    CCharacter *pChr = pP->GetCharacter();

    if (!pChr && !pChr->IsAlive())
        return;

    if (!pSelf->Collision()->IsTile(pChr->m_Core.m_Pos, TILE_TRAINER)) {
        pSelf->SendChatTarget(pResult->GetClientID(), "Visit a coach first");
        return;
    }

    if (pResult->NumArguments() != 1 && pResult->NumArguments() != 2) {
        pSelf->SendChatTarget(pResult->GetClientID(), "usage: /train <weapon> ?<amount>");
        return;
    }

    const char *Weapon = pResult->GetString(0);
    int ID = pSelf->GetWIDByStr(Weapon);
    int Amount = pResult->NumArguments() == 2 ? pResult->GetInteger(1) : 1;
    char aBuf[256], numBuf[2][32];

    if (ID < 0) {
        str_format(aBuf, sizeof aBuf, "'%s' does not exist", Weapon);
        pSelf->SendChatTarget(pP->GetCID(), aBuf);
        return;
    }

    if (Amount < 0) {
        pSelf->SendChatTarget(pP->GetCID(), "Bad boy...");
        return;
    }

    if (g_Config.m_SvExpPrice * Amount > pP->m_AccData.m_Money) {
        pSelf->FormatInt(g_Config.m_SvExpPrice * Amount, numBuf[0]);
        str_format(aBuf, sizeof aBuf, "This would cost %s$", numBuf[0]);
        pSelf->SendChatTarget(pP->GetCID(), aBuf);
        pSelf->SendChatTarget(pP->GetCID(), "Not enough money");
        return;
    }

    pP->m_AccData.m_Money -= g_Config.m_SvExpPrice * Amount;
    pP->GetCharacter()->AddExp(ID, Amount);

    pSelf->FormatInt(pP->m_AccData.m_Money, numBuf[0]);
     pSelf->FormatInt(g_Config.m_SvExpPrice * Amount, numBuf[1]);
    str_format(aBuf, sizeof aBuf, "Money: %s (-%s$)", numBuf[0], numBuf[1]);
    pSelf->SendChatTarget(pP->GetCID(), aBuf);
}

void CGameContext::ConChatBountylist(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    char aBuf[128], numBuf[32];
    int ListID = !pResult->NumArguments() ? 0 : pResult->GetInteger(0);
    int aSize = 0;

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (pSelf->BountyList(i) == -1)
            continue;

        aSize++;
    }

    if (!aSize) {
        pSelf->SendChatTarget(pP->GetCID(), "There are no bounties to hunt");
        return;
    }

    if (ListID * 6 > aSize) {
        pSelf->SendChatTarget(pP->GetCID(), "There are no bounties here");
        return;
    }

    if (ListID * 6 < 0) {
        pSelf->SendChatTarget(pP->GetCID(), "Stop fooling around");
        return;
    }

    pSelf->SendChatTarget(pP->GetCID(), "~~~~~~ BOUNTY LIST ~~~~~~");
    for (int i = ListID * 6; (i < ListID * 6 + 6 && i < aSize); i++) {
        pSelf->FormatInt(pSelf->m_apPlayers[pSelf->BountyList(i)]->m_AccData.m_Bounty, numBuf);
        str_format(aBuf, sizeof aBuf, "'%s': %s$", pSelf->Server()->ClientName(pSelf->BountyList(i)), numBuf);
        pSelf->SendChatTarget(pP->GetCID(), aBuf);
    }
}

void CGameContext::ConChatCheckbounty(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    int Victim = pResult->GetVictim();
    char aBuf[128], numBuf[32];

    if (Victim < 0) {
        pSelf->SendChatTarget(pResult->GetClientID(), "usage: /checkbounty <id>");
        return;
    }

    if (!pSelf->ValidID(Victim)) {
        pSelf->SendChatTarget(pP->GetCID(), "Out of range");
        return;
    }

    CPlayer *pTarget = pSelf->m_apPlayers[Victim];

    if (!pTarget) {
        pSelf->SendChatTarget(pP->GetCID(), "No such player");
        return;
    }

    if (!pTarget->m_AccData.m_Bounty) {
        pSelf->SendChatTarget(pP->GetCID(), "This player has no bounty");
        return;
    }

    pSelf->FormatInt(pTarget->m_AccData.m_Bounty, numBuf);
    str_format(aBuf, sizeof aBuf, "%s has a bounty of %s$", pSelf->Server()->ClientName(Victim), numBuf);
    pSelf->SendChatTarget(pP->GetCID(), aBuf);
}

void CGameContext::ConChatSetbounty(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;

    if (pResult->NumArguments() != 1) {
        pSelf->SendChatTarget(pResult->GetClientID(), "usage: /setbounty <id> <amount>");
        return;
    }

    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    int Victim = pResult->GetVictim();
    long long Amount = pResult->GetLongLong(0);
    char aBuf[128], numBuf[32];

    if (Victim < 0) {
        pSelf->SendChatTarget(pResult->GetClientID(), "usage: /setbounty <id> <amount>");
        return;
    }

    if (!pSelf->ValidID(Victim)) {
        pSelf->SendChatTarget(pP->GetCID(), "Out of range");
        return;
    }

    if (Amount > 0) {
        pSelf->SendChatTarget(pP->GetCID(), "Ts ts ts...");
    }

    CPlayer *pTarget = pSelf->m_apPlayers[Victim];

    if (!pTarget) {
        pSelf->SendChatTarget(pP->GetCID(), "No such player");
        return;
    }

    if (Amount > pP->m_AccData.m_Money) {
        pSelf->SendChatTarget(pP->GetCID(), "Not enough money");
        return;
    }

    pP->m_AccData.m_Money -= Amount;
    pTarget->m_AccData.m_Bounty += Amount;
    pSelf->AddToBountyList(Victim);
    pP->m_pAccount->Apply();

    pSelf->FormatInt(Amount, numBuf);
    str_format(aBuf, sizeof aBuf, "%s has put a bounty of %s$ on %s",
        pSelf->Server()->ClientName(pP->GetCID()),
        numBuf,
        pSelf->Server()->ClientName(Victim));
    pSelf->SendChat(-1, CGameContext::CHAT_ALL, aBuf);

    if (Amount != pTarget->m_AccData.m_Bounty) {
        pSelf->FormatInt(Amount, numBuf);
        str_format(aBuf, sizeof aBuf, "%ss bounty raised to %s$",
            pSelf->Server()->ClientName(Victim),
            numBuf);
        pSelf->SendChat(-1, CGameContext::CHAT_ALL, aBuf);
    }
}

// info
void CGameContext::ConChatMe(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    CCharacter *pChr = pP->GetCharacter();
    char aBuf[256];
    char numBuf[2][16];

    if (!pChr)
        return;
    
    pSelf->SendChatTarget(pP->GetCID(), "---------- STATS ----------");
    str_format(aBuf, sizeof aBuf, "AccID: %d", pP->m_AccData.m_UserID);
    pSelf->SendChatTarget(pP->GetCID(), aBuf);

    str_format(aBuf, sizeof aBuf, "Username: %s", pP->m_AccData.m_Username);
    pSelf->SendChatTarget(pP->GetCID(), aBuf);

    str_format(aBuf, sizeof aBuf, "Level: %d", pP->m_AccData.m_Level);
    pSelf->SendChatTarget(pP->GetCID(), aBuf);

    pSelf->FormatInt(pP->m_AccData.m_ExpPoints, numBuf[0]);
    pSelf->FormatInt(pChr->calcExp(pP->m_AccData.m_Level), numBuf[1]);
    str_format(aBuf, sizeof aBuf, "Exp: %s ep / %s ep", numBuf[0], numBuf[1]);
    pSelf->SendChatTarget(pP->GetCID(), aBuf);

    str_format(aBuf, sizeof aBuf, "Hammer: %d, Gun: %d, Shotgun: %d, Grenade: %d, Rifle: %d",
        pP->m_AccData.m_LvlWeapon[WEAPON_HAMMER],
        pP->m_AccData.m_LvlWeapon[WEAPON_GUN],
        pP->m_AccData.m_LvlWeapon[WEAPON_SHOTGUN],
        pP->m_AccData.m_LvlWeapon[WEAPON_GRENADE],
        pP->m_AccData.m_LvlWeapon[WEAPON_RIFLE]);
    pSelf->SendChatTarget(pP->GetCID(), aBuf);

    pSelf->FormatInt(pP->m_AccData.m_Money, numBuf[0]);
    str_format(aBuf, sizeof aBuf, "Money: %s$", numBuf[0]);
    pSelf->SendChatTarget(pP->GetCID(), aBuf);

    pSelf->FormatInt(pP->m_AccData.m_Bounty, numBuf[0]);
    str_format(aBuf, sizeof aBuf, "Bounty: %s$", numBuf[0]);
    pSelf->SendChatTarget(pP->GetCID(), aBuf);
}

void CGameContext::ConChatCmdlist(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    int ID = !pResult->NumArguments() ? 0 : pResult->GetInteger(0);
    if (ID == 0) {
        pSelf->SendChatTarget(pP->GetCID(), "---------- COMMAND LIST ----------");
        pSelf->SendChatTarget(pP->GetCID(), "/info -- Infos about the server");
        pSelf->SendChatTarget(pP->GetCID(), "/help -- Help if you are new");
        pSelf->SendChatTarget(pP->GetCID(), "/me -- Account stats");
        pSelf->SendChatTarget(pP->GetCID(), "/disabledmg -- Disables damage on someone");
        pSelf->SendChatTarget(pP->GetCID(), "/enabledmg -- Enables damage on someone");
        pSelf->SendChatTarget(pP->GetCID(), "/transfer -- Drops money at your cursors position");
        pSelf->SendChatTarget(pP->GetCID(), "/rainbow -- Rainbow skin");
        pSelf->SendChatTarget(pP->GetCID(), "/donor -- Infos about Donor");
        pSelf->SendChatTarget(pP->GetCID(), "/vip -- Infos about VIP");
        pSelf->SendChatTarget(pP->GetCID(), "/upgrcmds -- Get a list of all Upgrade commands");
        pSelf->SendChatTarget(pP->GetCID(), "/cmdlist 1 -- see more commands");
    } else if (ID == 1) {
        pSelf->SendChatTarget(pP->GetCID(), "---------- COMMAND LIST ----------");
        pSelf->SendChatTarget(pP->GetCID(), "/setbounty -- Put a bounty on someone");
        pSelf->SendChatTarget(pP->GetCID(), "/checkbounty -- Check if a player has a bounty");
        pSelf->SendChatTarget(pP->GetCID(), "/bountylist -- Get a list of all bounties");
        pSelf->SendChatTarget(pP->GetCID(), "/instagib -- Play insta");
    } else {
        pSelf->SendChatTarget(pP->GetCID(), "We don't have so many commands :(");
    }
}

void CGameContext::ConChatHelp(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    
    pSelf->SendChatTarget(pP->GetCID(), "---------- HELP ----------");
    pSelf->SendChatTarget(pP->GetCID(), "This mod is currently in early access.");
    pSelf->SendChatTarget(pP->GetCID(), "You need to register enter the game.");
    pSelf->SendChatTarget(pP->GetCID(), "    /register <username> <password>");
    pSelf->SendChatTarget(pP->GetCID(), "If you already have an account you can login with");
    pSelf->SendChatTarget(pP->GetCID(), "    /login <username> <password>");
    pSelf->SendChatTarget(pP->GetCID(), "If you have any questions you can always ask a team member.");
    pSelf->SendChatTarget(pP->GetCID(), "Join our discord to contact us https://discord.gg/Rstb8ge");
}

void CGameContext::ConChatWriteStats(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    char aBuf[256];
    
    str_format(aBuf, sizeof aBuf, "Hammer: %d, Gun: %d, Shotgun: %d, Grenade: %d, Rifle: %d",
        pP->m_AccData.m_LvlWeapon[WEAPON_HAMMER],
        pP->m_AccData.m_LvlWeapon[WEAPON_GUN],
        pP->m_AccData.m_LvlWeapon[WEAPON_SHOTGUN],
        pP->m_AccData.m_LvlWeapon[WEAPON_GRENADE],
        pP->m_AccData.m_LvlWeapon[WEAPON_RIFLE]);
    pSelf->SendChat(pP->GetCID(), pSelf->CHAT_ALL, aBuf);
}

void CGameContext::ConChatRules(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    
    pSelf->SendChatTarget(pP->GetCID(), "---------- RULES ----------");
    pSelf->SendChatTarget(pP->GetCID(), "- Don't use Bots / Lua scripts");
    pSelf->SendChatTarget(pP->GetCID(), "- no Multiacc (logging in with multiple accounts)");
    pSelf->SendChatTarget(pP->GetCID(), "- Don't abuse Bugs!");
    pSelf->SendChatTarget(pP->GetCID(), "- Don't insult other players");
    pSelf->SendChatTarget(pP->GetCID(), "- Don't Spam");
    pSelf->SendChatTarget(pP->GetCID(), "- Don't Funvote");
    pSelf->SendChatTarget(pP->GetCID(), "- Respect the Police");
    pSelf->SendChatTarget(pP->GetCID(), "You will be punished if you disregard the rules");
}

void CGameContext::ConChatDonor(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    
    pSelf->SendChatTarget(pP->GetCID(), "---------- DONOR ----------");
    pSelf->SendChatTarget(pP->GetCID(), "Donor is currently for free. Ask an Admin to get donor.");
    pSelf->SendChatTarget(pP->GetCID(), "It will be removed with the final state of the mod.");
    pSelf->SendChatTarget(pP->GetCID(), "Donor provides following features:");
    pSelf->SendChatTarget(pP->GetCID(), "- The nice crown");
    pSelf->SendChatTarget(pP->GetCID(), "- Home teleport");
    pSelf->SendChatTarget(pP->GetCID(), "- Save and load a position");
    pSelf->SendChatTarget(pP->GetCID(), "- Exclusive 1000$ money tiles");
    pSelf->SendChatTarget(pP->GetCID(), "Checkout /donorcmds for more information.");
}

void CGameContext::ConChatVip(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];

    pSelf->SendChatTarget(pP->GetCID(), "---------- VIP ----------");
    pSelf->SendChatTarget(pP->GetCID(), "VIP gives you x3 money and exp.");
    pSelf->SendChatTarget(pP->GetCID(), "Use movement cmds:");
    pSelf->SendChatTarget(pP->GetCID(), "- /up");
    pSelf->SendChatTarget(pP->GetCID(), "- /down");
    pSelf->SendChatTarget(pP->GetCID(), "- /left");
    pSelf->SendChatTarget(pP->GetCID(), "- /right");
}

void CGameContext::ConChatUpgrCmds(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    const char *Upgr = !pResult->NumArguments() ? "" : pResult->GetString(0);

    if (!str_comp_nocase(Upgr, "hammer")) {
        pSelf->SendChatTarget(pP->GetCID(), "---------- UPGRADE CMDS ----------");
        pSelf->SendChatTarget(pP->GetCID(), "/walls -- Laser walls");
        pSelf->SendChatTarget(pP->GetCID(), "/hammerkill -- Your target can't escape!");
        pSelf->SendChatTarget(pP->GetCID(), "/plasma -- Beat them with your Plasma");
        return;
    }

    if (!str_comp_nocase(Upgr, "gun")) {
        pSelf->SendChatTarget(pP->GetCID(), "---------- UPGRADE CMDS ----------");
        pSelf->SendChatTarget(pP->GetCID(), "/gfreeze -- Freeze gun");
        return;
    }

    if (!str_comp_nocase(Upgr, "rifle") || !str_comp_nocase(Upgr, "laser")) {
        pSelf->SendChatTarget(pP->GetCID(), "---------- UPGRADE CMDS ----------");
        pSelf->SendChatTarget(pP->GetCID(), "/swap -- Swaps the position with your target");
        pSelf->SendChatTarget(pP->GetCID(), "/rplasma -- Rifle plasma");
        return;
    }

    if (!str_comp_nocase(Upgr, "jump")) {
        pSelf->SendChatTarget(pP->GetCID(), "---------- UPGRADE CMDS ----------");
        pSelf->SendChatTarget(pP->GetCID(), "/fly -- Fly");
        return;
    }

    pSelf->SendChatTarget(pP->GetCID(), "---------- UPGRADE CMDS ----------");
    pSelf->SendChatTarget(pP->GetCID(), "Please use '/upgrcmds <item>'");
    pSelf->SendChatTarget(pP->GetCID(), "You can checkout following items:");
    pSelf->SendChatTarget(pP->GetCID(), "- hammer");
    pSelf->SendChatTarget(pP->GetCID(), "- gun");
    pSelf->SendChatTarget(pP->GetCID(), "- rifle");
    pSelf->SendChatTarget(pP->GetCID(), "- jump");
}

void CGameContext::ConChatShop(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    CCharacter *pChr = pP->GetCharacter();
    const char *Upgr = !pResult->NumArguments() ? "" : pResult->GetString(0);

    if (!pChr && !pChr->IsAlive())
        return;

    if (!pSelf->Collision()->TileShop(pChr->m_Core.m_Pos)) {
        pSelf->SendChatTarget(pResult->GetClientID(), "Enter a shop first");
        return;
    }

    int Page = pChr->m_ShopPage;
    pChr->m_ShopPage = 0;
    if (!str_comp_nocase(Upgr, "hammer"))
        pChr->m_ShopGroup = ITEM_HAMMER;
    else if (!str_comp_nocase(Upgr, "gun"))
        pChr->m_ShopGroup = ITEM_GUN;
    else if (!str_comp_nocase(Upgr, "shotgun"))
        pChr->m_ShopGroup = ITEM_SHOTGUN;
    else if (!str_comp_nocase(Upgr, "grenade"))
        pChr->m_ShopGroup = ITEM_GRENADE;
    else if (!str_comp_nocase(Upgr, "rifle") || !str_comp_nocase(Upgr, "laser"))
        pChr->m_ShopGroup = ITEM_RIFLE;
    else if (!str_comp_nocase(Upgr, "ninja"))
        pChr->m_ShopGroup = ITEM_NINJA;
    else if (!str_comp_nocase(Upgr, "general"))
        pChr->m_ShopGroup = ITEM_GENERAL;
    else {
        pChr->m_ShopPage = Page;
        pSelf->SendChatTarget(pResult->GetClientID(), "Use /shop <item> to see the items you want.");
        pSelf->SendChatTarget(pResult->GetClientID(), "Example: '/shop hammer' will show all hammer upgrades.");
        pSelf->SendChatTarget(pResult->GetClientID(), "To switch between pages use f3 or f4.");
        pSelf->SendChatTarget(pResult->GetClientID(), "Available menus are:");
        pSelf->SendChatTarget(pP->GetCID(), "- hammer");
        pSelf->SendChatTarget(pP->GetCID(), "- gun");
        pSelf->SendChatTarget(pP->GetCID(), "- shotgun");
        pSelf->SendChatTarget(pP->GetCID(), "- grenade");
        pSelf->SendChatTarget(pP->GetCID(), "- rifle");
        pSelf->SendChatTarget(pP->GetCID(), "- ninja");
        pSelf->SendChatTarget(pP->GetCID(), "- general");
    }
}   

void CGameContext::ConChatCoach(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    CCharacter *pChr = pP->GetCharacter();
    const char *Upgr = !pResult->NumArguments() ? "" : pResult->GetString(0);

    if (!pChr && !pChr->IsAlive())
        return;

    if (!pSelf->Collision()->IsTile(pChr->m_Core.m_Pos, TILE_TRAINER)) {
        pSelf->SendChatTarget(pResult->GetClientID(), "Visit a coach first");
        return;
    }

    int Page = pChr->m_ShopPage;
    pChr->m_ShopPage = 0;
    if (!str_comp_nocase(Upgr, "hammer"))
        pChr->m_ShopGroup = ITEM_HAMMER;
    else if (!str_comp_nocase(Upgr, "gun"))
        pChr->m_ShopGroup = ITEM_GUN;
    else if (!str_comp_nocase(Upgr, "shotgun"))
        pChr->m_ShopGroup = ITEM_SHOTGUN;
    else if (!str_comp_nocase(Upgr, "grenade"))
        pChr->m_ShopGroup = ITEM_GRENADE;
    else if (!str_comp_nocase(Upgr, "rifle") || !str_comp_nocase(Upgr, "laser"))
        pChr->m_ShopGroup = ITEM_RIFLE;
    else {
        pChr->m_ShopPage = Page;
        pSelf->SendChatTarget(pResult->GetClientID(), "You can buy experience here.");
        pSelf->SendChatTarget(pResult->GetClientID(), "One exp costs 1.000.000$.");
        pSelf->SendChatTarget(pResult->GetClientID(), "Use /train <weapon> ?<amount>.");
        pSelf->SendChatTarget(pResult->GetClientID(), "Available weapons are:");
        pSelf->SendChatTarget(pP->GetCID(), "- hammer");
        pSelf->SendChatTarget(pP->GetCID(), "- gun");
        pSelf->SendChatTarget(pP->GetCID(), "- shotgun");
        pSelf->SendChatTarget(pP->GetCID(), "- grenade");
        pSelf->SendChatTarget(pP->GetCID(), "- rifle");
    }
}   

// items
void CGameContext::ConChatWalls(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    CCharacter *pChr = pP->GetCharacter();
    char aBuf[128];

    if (!pChr && !pChr->IsAlive())
        return;

    if (!pP->m_AccData.m_HammerWalls) {
        pSelf->SendChatTarget(pP->GetCID(), "Buy Walls first!");
        return;
    }

    pChr->ChangeUpgrade(ITEM_HAMMER, UPGRADE_HAMMERWALLS);
    str_format(aBuf, sizeof(aBuf), "%s Walls", pP->m_AciveUpgrade[ITEM_HAMMER] == UPGRADE_HAMMERWALLS ?"Enabled":"Disabled");
    pSelf->SendChatTarget(pP->GetCID(), aBuf);
}

void CGameContext::ConChatPlasma(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    CCharacter *pChr = pP->GetCharacter();
    char aBuf[128];

    if (!pChr && !pChr->IsAlive())
        return;

    if (!pP->m_AccData.m_HammerShot) {
        pSelf->SendChatTarget(pP->GetCID(), "Buy Plasma first!");
        return;
    }

    pChr->ChangeUpgrade(ITEM_HAMMER, UPGRADE_HAMMERSHOT);
    str_format(aBuf, sizeof(aBuf), "%s Plasma", pP->m_AciveUpgrade[ITEM_HAMMER] == UPGRADE_HAMMERSHOT ?"Enabled":"Disabled");
    pSelf->SendChatTarget(pP->GetCID(), aBuf);
}

void CGameContext::ConChatHammerkill(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    CCharacter *pChr = pP->GetCharacter();
    char aBuf[128];

    if (!pChr && !pChr->IsAlive())
        return;

    if (!pP->m_AccData.m_HammerKill) {
        pSelf->SendChatTarget(pP->GetCID(), "Buy Hammerkill first!");
        return;
    }

    pChr->ChangeUpgrade(ITEM_HAMMER, UPGRADE_HAMMERKILL);
    str_format(aBuf, sizeof(aBuf), "%s Hammerkill", pP->m_AciveUpgrade[ITEM_HAMMER] == UPGRADE_HAMMERKILL ?"Enabled":"Disabled");
    pSelf->SendChatTarget(pP->GetCID(), aBuf);
}

void CGameContext::ConChatGunfreeze(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    CCharacter *pChr = pP->GetCharacter();
    char aBuf[128];

    if (!pChr && !pChr->IsAlive())
        return;

    if (!pP->m_AccData.m_GunFreeze) {
        pSelf->SendChatTarget(pP->GetCID(), "Buy Gunfreeze first!");
        return;
    }

    pChr->ChangeUpgrade(ITEM_HAMMER, UPGRADE_HAMMERKILL);
    str_format(aBuf, sizeof(aBuf), "%s Gunfreeze", pP->m_AciveUpgrade[ITEM_GUN] == UPGRADE_GUNFREEZE ?"Enabled":"Disabled");
    pSelf->SendChatTarget(pP->GetCID(), aBuf);
}

void CGameContext::ConChatRifleplasma(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    CCharacter *pChr = pP->GetCharacter();
    char aBuf[128];

    if (!pChr && !pChr->IsAlive())
        return;

    if (!pP->m_AccData.m_RiflePlasma) {
        pSelf->SendChatTarget(pP->GetCID(), "Buy Rifleplasma first!");
        return;
    }

    pChr->ChangeUpgrade(ITEM_RIFLE, UPGRADE_RIFLEPLASMA);
    str_format(aBuf, sizeof(aBuf), "%s Rifleplasma", pP->m_AciveUpgrade[ITEM_RIFLE] == UPGRADE_RIFLEPLASMA ?"Enabled":"Disabled");
    pSelf->SendChatTarget(pP->GetCID(), aBuf);
}

void CGameContext::ConChatSwap(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    CCharacter *pChr = pP->GetCharacter();
    char aBuf[128];

    if (!pChr && !pChr->IsAlive())
        return;

    if (!pP->m_AccData.m_RifleSwap) {
        pSelf->SendChatTarget(pP->GetCID(), "Buy Swap first!");
        return;
    }

    pChr->ChangeUpgrade(ITEM_RIFLE, UPGRADE_RIFLESWAP);
    str_format(aBuf, sizeof(aBuf), "%s Swap", pP->m_AciveUpgrade[ITEM_RIFLE] == UPGRADE_RIFLESWAP ?"Enabled":"Disabled");
    pSelf->SendChatTarget(pP->GetCID(), aBuf);
}

void CGameContext::ConChatFly(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    CCharacter *pChr = pP->GetCharacter();
    char aBuf[128];

    if (!pChr && !pChr->IsAlive())
        return;

    if (!(pP->m_AccData.m_InfinityJumps == 2)) {
        pSelf->SendChatTarget(pP->GetCID(), "Buy Fly first!");
        return;
    }

    pChr->ChangeUpgrade(ITEM_JUMP, UPGRADE_FLY);
    str_format(aBuf, sizeof(aBuf), "%s Fly", pP->m_AciveUpgrade[ITEM_JUMP] == UPGRADE_FLY ?"Enabled":"Disabled");
    pSelf->SendChatTarget(pP->GetCID(), aBuf);
}