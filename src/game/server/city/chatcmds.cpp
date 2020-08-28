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

    if (!pChr)
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

    if (!pChr)
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

    if (!pChr)
        return;

    if (pP->GetCharacter()->m_Frozen || pP->m_AccData.m_Arrested || pP->m_Insta) {
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

    if (!pChr)
        return;

    if (pP->GetCharacter()->m_Frozen || pP->m_AccData.m_Arrested || pP->m_Insta) {
        pSelf->SendChatTarget(pP->GetCID(), "You can't do this at the moment");
        return;
    }

    if (!pP->m_AccData.m_Donor) {
        pSelf->SendChatTarget(pP->GetCID(), "This is a donor feature");
        return;
    }
    
    pChr->SaveLoad(true);
}

void CGameContext::ConChatHome(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    CCharacter *pChr = pP->GetCharacter();

    if (!pChr)
        return;

    if (pP->GetCharacter()->m_Frozen || pP->m_AccData.m_Arrested || pP->m_Insta) {
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

    if (!pChr)
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

    if (!pChr)
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

    if (!pChr)
        return;

    if (pP->GetCharacter()->m_Frozen || pP->m_AccData.m_Arrested || pChr->m_GameZone || pP->m_Insta) {
        pSelf->SendChatTarget(pP->GetCID(), "You can't do this at the moment");
        return;
    }

    if (!pP->m_AccData.m_VIP) {
        pSelf->SendChatTarget(pP->GetCID(), "This is a vip feature");
        return;
    }
    
    pChr->Move(0);
}

void CGameContext::ConChatTele(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    CCharacter *pChr = pP->GetCharacter();

    if (!pChr)
        return;

    if (pP->GetCharacter()->m_Frozen || pP->m_AccData.m_Arrested || pP->m_Insta) {
        pSelf->SendChatTarget(pP->GetCID(), "You can't do this at the moment");
        return;
    }

    if (!pP->m_AccData.m_VIP) {
        pSelf->SendChatTarget(pP->GetCID(), "This is a vip feature");
        return;
    }
    
    pChr->Tele();
}

void CGameContext::ConChatDown(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    CCharacter *pChr = pP->GetCharacter();

    if (!pChr)
        return;

    if (pP->GetCharacter()->m_Frozen || pP->m_AccData.m_Arrested || pChr->m_GameZone || pP->m_Insta) {
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

    if (!pChr)
        return;

    if (pP->GetCharacter()->m_Frozen || pP->m_AccData.m_Arrested || pChr->m_GameZone || pP->m_Insta) {
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

    if (!pChr)
        return;

    if (pP->GetCharacter()->m_Frozen || pP->m_AccData.m_Arrested || pChr->m_GameZone || pP->m_Insta) {
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

    if (!pChr)
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

    if (!pChr)
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

    if (!pChr)
        return;

    if (pChr->m_Frozen || pChr->m_GameZone || pP->m_Insta) {
        pSelf->SendChatTarget(pP->GetCID(), "You can't do this at the moment");
        return;
    }

    pChr->Transfer(Money);
}

void CGameContext::ConChatPM(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];

    if (pResult->NumArguments() != 2) {
        pSelf->SendChatTarget(pResult->GetClientID(), "usage: /pm <name> <message>");
        return;
    }

    const char* Victim = pResult->GetString(0);
    const char* Msg = pResult->GetString(1);
    int ID = pSelf->Server()->ClientIdByName(Victim);

    if (ID == -1) {
        pSelf->SendChatTarget(pResult->GetClientID(), "No such user");
        return;
    }

    if (pP->GetCID() == ID) {
        pSelf->SendChatTarget(pP->GetCID(), "You can't pm yourself");
        return;
    }

    if (!pSelf->ValidID(ID)) {
        pSelf->SendChatTarget(pP->GetCID(), "Out of range");
        return;
    }

    if (!pSelf->Server()->ClientIngame(ID)) {
        pSelf->SendChatTarget(pP->GetCID(), "No such player");
        return;
    }

    pSelf->SendPrivate(pResult->GetClientID(), ID, Msg);
}

void CGameContext::ConChatSetPM(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];

    if (pResult->NumArguments() != 1) {
        pSelf->SendChatTarget(pResult->GetClientID(), "usage: /setpm <name>");
        return;
    }

    const char* Victim = pResult->GetString(0);
    int ID = pSelf->Server()->ClientIdByName(Victim);
    char aBuf[256];

    if (ID == -1) {
        pSelf->SendChatTarget(pResult->GetClientID(), "No such user");
        return;
    }

    if (pP->GetCID() == ID) {
        pSelf->SendChatTarget(pP->GetCID(), "You can't set yourself as pm partner");
        return;
    }

    if (!pSelf->ValidID(ID)) {
        pSelf->SendChatTarget(pP->GetCID(), "Out of range");
        return;
    }

    if (!pSelf->Server()->ClientIngame(ID)) {
        pSelf->SendChatTarget(pP->GetCID(), "No such player");
        return;
    }

    pP->m_PmID = ID;
    str_format(aBuf, sizeof aBuf, "%s is now your pm partner", Victim);
    pSelf->SendChatTarget(pP->GetCID(), aBuf);
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

    if (!pChr)
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
    int Amount = pResult->NumArguments() == 2 ? pResult->GetLongLong(1) : 1;
    char aBuf[256], numBuf[2][32];

    if (ID < 0) {
        str_format(aBuf, sizeof aBuf, "'%s' does not exist", Weapon);
        pSelf->SendChatTarget(pP->GetCID(), aBuf);
        return;
    }

    if (Amount > 100) {
        pSelf->SendChatTarget(pP->GetCID(), "You can't train more than 100ep at once");
        return;
    }

    if (Amount < 0) {
        pSelf->SendChatTarget(pP->GetCID(), "Bad boy...");
        return;
    }

    if (pP->m_AccData.m_ExpWeapon[ID] >= (unsigned)g_Config.m_SvWLvlMax) {
        pSelf->SendChatTarget(pP->GetCID(), "You already reached the max level");
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
    int Size = 0;

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (pSelf->BountyList(i) == -1)
            continue;

        Size++;
    }

    if (!Size) {
        pSelf->SendChatTarget(pP->GetCID(), "There are no bounties to hunt");
        return;
    }

    if (ListID * 6 > Size) {
        pSelf->SendChatTarget(pP->GetCID(), "There are no bounties here");
        return;
    }

    if (ListID * 6 < 0) {
        pSelf->SendChatTarget(pP->GetCID(), "Stop fooling around");
        return;
    }

    pSelf->SendChatTarget(pP->GetCID(), "~~~~~~ BOUNTY LIST ~~~~~~");
    for (int i = ListID * 6; (i < ListID * 6 + 6 && i < Size); i++) {
        pSelf->FormatInt(pSelf->m_apPlayers[pSelf->BountyList(i)]->m_AccData.m_Bounty, numBuf);
        str_format(aBuf, sizeof aBuf, "'%s': %s$", pSelf->Server()->ClientName(pSelf->BountyList(i)), numBuf);
        pSelf->SendChatTarget(pP->GetCID(), aBuf);
    }

    if (Size > ListID * 6 + 6) {
        str_format(aBuf, sizeof aBuf, "/bountylist %d", ListID+1);
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

    if (Amount < 1000) {
        pSelf->SendChatTarget(pP->GetCID(), "Put a bounty of at least 1.000$");
        return;
    }

    CPlayer *pTarget = pSelf->m_apPlayers[Victim];

    if (!pTarget) {
        pSelf->SendChatTarget(pP->GetCID(), "No such player");
        return;
    }

    if ((long long unsigned)Amount > pP->m_AccData.m_Money) {
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

// moneycollector
// no checks needed, mc is autonom
void CGameContext::ConChatMCBuy(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;

    if (pResult->NumArguments() != 1) {
        pSelf->SendChatTarget(pResult->GetClientID(), "Usage: /mcbuy <amount>");
        return;
    }

    if (!pSelf->Collision()->IsTile(pSelf->GetPlayerChar(pResult->GetClientID())->m_Core.m_Pos, TILE_MONEYCOLLECTOR)) {
        pSelf->SendChatTarget(pResult->GetClientID(), "You need to be inside the Moneycollector");
        return;
    }

    long long Amount = pResult->GetLongLong(0);

    if (Amount < 0) {
        pSelf->SendChatTarget(pResult->GetClientID(), "Pfff...");
        return;
    }

    pSelf->MoneyCollector()->Buy(pResult->GetClientID(), Amount);
}

void CGameContext::ConChatMCCollect(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    pSelf->MoneyCollector()->Collect(pResult->GetClientID());
}

void CGameContext::ConChatMCHelp(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    int ID = pResult->GetClientID();

    pSelf->SendChatTarget(ID, "~~ Money Collector Help ~~");
    pSelf->SendChatTarget(ID, "The Money Collector collects 4% of the farmed income on the server.");
    pSelf->SendChatTarget(ID, "This money ends up in the pot.");
    pSelf->SendChatTarget(ID, "The pot is increased by 50% every hour.");
    pSelf->SendChatTarget(ID, "The price is reduced by 5% every 10 minutes");
    pSelf->SendChatTarget(ID, "The Holder can collect the money from the pot.");
    pSelf->SendChatTarget(ID, "Checkout /mccmds to get more information.");
}

void CGameContext::ConChatMCCmds(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    int ID = pResult->GetClientID();

    pSelf->SendChatTarget(ID, "~~ Money Collector Cmds ~~");
    pSelf->SendChatTarget(ID, "/mcbuy");
    pSelf->SendChatTarget(ID, "/mccollect");
    pSelf->SendChatTarget(ID, "/mchelp");
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
        pSelf->SendChatTarget(pP->GetCID(), "/donor -- Infos about Donor");
        pSelf->SendChatTarget(pP->GetCID(), "/vip -- Infos about VIP");
        pSelf->SendChatTarget(pP->GetCID(), "/upgrcmds -- Get a list of all Upgrade commands");
        pSelf->SendChatTarget(pP->GetCID(), "/instagib -- Play insta");
        pSelf->SendChatTarget(pP->GetCID(), "/cmdlist 1 -- see more commands");
    } else if (ID == 1) {
        pSelf->SendChatTarget(pP->GetCID(), "---------- COMMAND LIST ----------");
        pSelf->SendChatTarget(pP->GetCID(), "/ids -- Display all IDs");
        pSelf->SendChatTarget(pP->GetCID(), "/event -- Gives the current event infos");
        pSelf->SendChatTarget(pP->GetCID(), "/setbounty -- Put a bounty on someone");
        pSelf->SendChatTarget(pP->GetCID(), "/checkbounty -- Check if a player has a bounty");
        pSelf->SendChatTarget(pP->GetCID(), "/bountylist -- Get a list of all bounties");
        pSelf->SendChatTarget(pP->GetCID(), "/writestats -- Writes your weaponlevel down");
    } else {
        pSelf->SendChatTarget(pP->GetCID(), "We don't have so many commands :(");
    }
}

void CGameContext::ConChatHelp(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    
    pSelf->SendChatTarget(pP->GetCID(), "---------- HELP ----------");
    pSelf->SendChatTarget(pP->GetCID(), "You need to register enter the game.");
    pSelf->SendChatTarget(pP->GetCID(), "-- /register <username> <password>");
    pSelf->SendChatTarget(pP->GetCID(), "If you already have an account you can login with");
    pSelf->SendChatTarget(pP->GetCID(), "-- /login <username> <password>");
    pSelf->SendChatTarget(pP->GetCID(), "If you have any questions you can always ask a team member.");
    pSelf->SendChatTarget(pP->GetCID(), "Join our discord to contact us https://discord.gg/Rstb8ge");
}

void CGameContext::ConChatInfo(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    
    pSelf->SendChatTarget(pP->GetCID(), "---------- INFO ----------");
    pSelf->SendChatTarget(pP->GetCID(), "UH|City is made by UrinStone.");
    pSelf->SendChatTarget(pP->GetCID(), "Join our Discord to be always informed about the latest updates.");
    pSelf->SendChatTarget(pP->GetCID(), "If you forget your password or wish to donate, please ONLY contact UrinStone#8404 on Discord.");
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
    pSelf->SendChatTarget(pP->GetCID(), "- Don't place unfair lights");
    pSelf->SendChatTarget(pP->GetCID(), "- Respect the Police");
    pSelf->SendChatTarget(pP->GetCID(), "You will be punished if you disregard the rules");
}

void CGameContext::ConChatDonor(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    
    pSelf->SendChatTarget(pP->GetCID(), "---------- DONOR ----------");
    pSelf->SendChatTarget(pP->GetCID(), "Donor costs 10€");
    pSelf->SendChatTarget(pP->GetCID(), "Contact UrinStone#8404 at discord");
    pSelf->SendChatTarget(pP->GetCID(), "Donor provides following features:");
    pSelf->SendChatTarget(pP->GetCID(), "- x5 Money and Exp");
    pSelf->SendChatTarget(pP->GetCID(), "- The nice crown");
    pSelf->SendChatTarget(pP->GetCID(), "- Rainbow");
    pSelf->SendChatTarget(pP->GetCID(), "- Your own House");
    pSelf->SendChatTarget(pP->GetCID(), "- Home teleport");
    pSelf->SendChatTarget(pP->GetCID(), "- Save and load a position");
    pSelf->SendChatTarget(pP->GetCID(), "- Exclusive 1000$ money tiles that give x6 money & exp");
    pSelf->SendChatTarget(pP->GetCID(), "Checkout /donorcmds for more information.");
}

void CGameContext::ConChatDonorCmds(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    
    pSelf->SendChatTarget(pP->GetCID(), "---------- DONORCMDS ----------");
    pSelf->SendChatTarget(pP->GetCID(), " /save -- Saves your position");
    pSelf->SendChatTarget(pP->GetCID(), " /load -- Teleports you to the saved position");
    pSelf->SendChatTarget(pP->GetCID(), " /rainbow -- Gives you rainbow colors");
    pSelf->SendChatTarget(pP->GetCID(), " /crown -- The nice crown");
    pSelf->SendChatTarget(pP->GetCID(), " /home -- Teleports you Into your Home");
    pSelf->SendChatTarget(pP->GetCID(), "Checkout /donor to get more information.");
}

void CGameContext::ConChatIDs(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    int Page = !pResult->NumArguments() ? 0 : pResult->GetInteger(0);
    int IDs[MAX_CLIENTS];
    char aBuf[128];
    int j = 0;
    
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (pSelf->Server()->ClientIngame(i)) {
            IDs[j] = i;
            j++;
        }
    }

    pSelf->SendChatTarget(pP->GetCID(), "---------- IDS ----------");
    for  (int i = Page*6; i < Page*6 + 6 && i < j; i++) {
        str_format(aBuf, sizeof aBuf, "[%d] %s", IDs[i], pSelf->Server()->ClientName(IDs[i]));
        pSelf->SendChatTarget(pP->GetCID(), aBuf);
    }

    if (Page*6+6 < j) {
        str_format(aBuf, sizeof aBuf, "/ids %d", Page+1);
        pSelf->SendChatTarget(pP->GetCID(), aBuf);
    }
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
    pSelf->SendChatTarget(pP->GetCID(), "- /tele");
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
        pSelf->SendChatTarget(pP->GetCID(), "/portal -- Opens a portal to teleport between two points");
        return;
    }

    if (!str_comp_nocase(Upgr, "gun")) {
        pSelf->SendChatTarget(pP->GetCID(), "---------- UPGRADE CMDS ----------");
        pSelf->SendChatTarget(pP->GetCID(), "/fgun -- Freeze gun");
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

    if (!str_comp_nocase(Upgr, "hook")) {
        pSelf->SendChatTarget(pP->GetCID(), "---------- UPGRADE CMDS ----------");
        pSelf->SendChatTarget(pP->GetCID(), "/bhook -- Boost other players");
        pSelf->SendChatTarget(pP->GetCID(), "/hhook -- Increases the hooked players healthregen");
        return;
    }

    if (!str_comp_nocase(Upgr, "ninja")) {
        pSelf->SendChatTarget(pP->GetCID(), "---------- UPGRADE CMDS ----------");
        pSelf->SendChatTarget(pP->GetCID(), "/nfly -- Ninja fly");
        pSelf->SendChatTarget(pP->GetCID(), "/nbomber -- Bomb your enemys away");
        return;
    }

    if (!str_comp_nocase(Upgr, "special")) {
        pSelf->SendChatTarget(pP->GetCID(), "---------- UPGRADE CMDS ----------");
        pSelf->SendChatTarget(pP->GetCID(), "/pushaura -- Pushes the other players");
        pSelf->SendChatTarget(pP->GetCID(), "/pullaura -- Pull the other players");
        return;
    }

    pSelf->SendChatTarget(pP->GetCID(), "---------- UPGRADE CMDS ----------");
    pSelf->SendChatTarget(pP->GetCID(), "Please use '/upgrcmds <item>'");
    pSelf->SendChatTarget(pP->GetCID(), "You can checkout following items:");
    pSelf->SendChatTarget(pP->GetCID(), "- hammer");
    pSelf->SendChatTarget(pP->GetCID(), "- gun");
    pSelf->SendChatTarget(pP->GetCID(), "- rifle");
    pSelf->SendChatTarget(pP->GetCID(), "- jump");
    pSelf->SendChatTarget(pP->GetCID(), "- hook");
    pSelf->SendChatTarget(pP->GetCID(), "- ninja");
    pSelf->SendChatTarget(pP->GetCID(), "- special");
}

void CGameContext::ConChatShop(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    CCharacter *pChr = pP->GetCharacter();
    const char *Upgr = !pResult->NumArguments() ? "" : pResult->GetString(0);

    if (!pChr)
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
    else if (!str_comp_nocase(Upgr, "hook"))
        pChr->m_ShopGroup = ITEM_JUMP; // should be ITEM_HOOK, but we have no specials yet
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
        pSelf->SendChatTarget(pP->GetCID(), "- hook");
    }
}   

void CGameContext::ConChatCoach(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    CCharacter *pChr = pP->GetCharacter();

    if (!pChr)
        return;

    if (!pSelf->Collision()->IsTile(pChr->m_Core.m_Pos, TILE_TRAINER)) {
        pSelf->SendChatTarget(pResult->GetClientID(), "Visit a coach first");
        return;
    }

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

void CGameContext::ConChatEvent(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    
    pSelf->GameEvent()->EventInfo(pResult->GetClientID());
}

// items
void CGameContext::ConChatWalls(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    CCharacter *pChr = pP->GetCharacter();
    char aBuf[128];

    if (!pChr)
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

    if (!pChr)
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

    if (!pChr)
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

    if (!pChr)
        return;

    if (!pP->m_AccData.m_GunFreeze) {
        pSelf->SendChatTarget(pP->GetCID(), "Buy Gunfreeze first!");
        return;
    }

    pChr->ChangeUpgrade(ITEM_GUN, UPGRADE_GUNFREEZE);
    str_format(aBuf, sizeof(aBuf), "%s Gunfreeze", pP->m_AciveUpgrade[ITEM_GUN] == UPGRADE_GUNFREEZE ?"Enabled":"Disabled");
    pSelf->SendChatTarget(pP->GetCID(), aBuf);
}

void CGameContext::ConChatRifleplasma(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    CCharacter *pChr = pP->GetCharacter();
    char aBuf[128];

    if (!pChr)
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

    if (!pChr)
        return;

    if (pP->m_Insta || pChr->m_GameZone)
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

    if (!pChr)
        return;

    if (pP->m_Insta || pChr->m_GameZone)
        return;

    if (!(pP->m_AccData.m_InfinityJumps == 2)) {
        pSelf->SendChatTarget(pP->GetCID(), "Buy Fly first!");
        return;
    }

    pChr->ChangeUpgrade(ITEM_JUMP, UPGRADE_FLY);
    str_format(aBuf, sizeof(aBuf), "%s Fly", pP->m_AciveUpgrade[ITEM_JUMP] == UPGRADE_FLY ?"Enabled":"Disabled");
    pSelf->SendChatTarget(pP->GetCID(), aBuf);
}

void CGameContext::ConChatHealHook(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    CCharacter *pChr = pP->GetCharacter();
    char aBuf[128];

    if (!pChr)
        return;

    if (pP->m_Insta || pChr->m_GameZone)
        return;

    if (!pP->m_AccData.m_HealHook) {
        pSelf->SendChatTarget(pP->GetCID(), "Buy Healhook first!");
        return;
    }

    pChr->ChangeUpgrade(ITEM_HOOK, UPGRADE_HEALHOOK);
    str_format(aBuf, sizeof(aBuf), "%s Healhook", pP->m_AciveUpgrade[ITEM_HOOK] == UPGRADE_HEALHOOK ?"Enabled":"Disabled");
    pSelf->SendChatTarget(pP->GetCID(), aBuf);
}

void CGameContext::ConChatBoostHook(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    CCharacter *pChr = pP->GetCharacter();
    char aBuf[128];

    if (!pChr)
        return;

    if (!pChr->IsAlive() || pP->m_Insta || pChr->m_GameZone)
        return;

    if (!pP->m_AccData.m_BoostHook) {
        pSelf->SendChatTarget(pP->GetCID(), "Buy Boosthook first!");
        return;
    }

    pChr->ChangeUpgrade(ITEM_HOOK, UPGRADE_BOOSTHOOK);
    pChr->m_Core.m_DisablePlayerHook ^= true;
    str_format(aBuf, sizeof(aBuf), "%s Boosthook", pP->m_AciveUpgrade[ITEM_HOOK] == UPGRADE_BOOSTHOOK ?"Enabled":"Disabled");
    pSelf->SendChatTarget(pP->GetCID(), aBuf);
}

void CGameContext::ConChatPortal(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    CCharacter *pChr = pP->GetCharacter();
    char aBuf[128];

    if (!pChr)
        return;

    if (pP->m_Insta || pChr->m_GameZone)
        return;

    if (!pP->m_AccData.m_Portal) {
        pSelf->SendChatTarget(pP->GetCID(), "Buy Portal first!");
        return;
    }

    pChr->ChangeUpgrade(ITEM_HAMMER, UPGRADE_PORTAL);
    str_format(aBuf, sizeof(aBuf), "%s Portal", pP->m_AciveUpgrade[ITEM_HAMMER] == UPGRADE_PORTAL ? "Enabled" : "Disabled");
    pSelf->SendChatTarget(pP->GetCID(), aBuf);
}

void CGameContext::ConChatPushAura(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    CCharacter *pChr = pP->GetCharacter();
    char aBuf[128];

    if (!pChr)
        return;

    if (pP->m_Insta || pChr->m_GameZone)
        return;

    if (!pP->m_AccData.m_PushAura) {
        pSelf->SendChatTarget(pP->GetCID(), "Buy PushAura first!");
        return;
    }

    if (pP->m_GravAuraCooldown) {
        str_format(aBuf, sizeof aBuf, "You can use this aura in %d seconds again", pP->m_GravAuraCooldown);
        pSelf->SendChatTarget(pP->GetCID(), aBuf);
        return;
    }

    pChr->m_GravAura = new CGravAura(pChr->GameWorld(), pP->GetCID(), 1);
    pP->m_GravAuraCooldown = g_Config.m_SvGravAuraCooldown;
}

void CGameContext::ConChatPullAura(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    CCharacter *pChr = pP->GetCharacter();
    char aBuf[128];

    if (!pChr)
        return;

    if (pP->m_Insta || pChr->m_GameZone)
        return;

    if (!pP->m_AccData.m_PullAura) {
        pSelf->SendChatTarget(pP->GetCID(), "Buy PullAura first!");
        return;
    }

    if (pP->m_GravAuraCooldown) {
        str_format(aBuf, sizeof aBuf, "You can use this aura in %d seconds again", pP->m_GravAuraCooldown);
        pSelf->SendChatTarget(pP->GetCID(), aBuf);
        return;
    }

    pChr->m_GravAura = new CGravAura(pChr->GameWorld(), pP->GetCID(), -1);
    pP->m_GravAuraCooldown = g_Config.m_SvGravAuraCooldown;
}

void CGameContext::ConChatNinjaFly(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    CCharacter *pChr = pP->GetCharacter();
    char aBuf[128];

    if (!pChr)
        return;

    if (pP->m_Insta || pChr->m_GameZone)
        return;

    if (!pP->m_AccData.m_NinjaFly) {
        pSelf->SendChatTarget(pP->GetCID(), "Buy Ninjafly first!");
        return;
    }

    pP->m_NinjaFly ^= true;
    str_format(aBuf, sizeof aBuf, "%s Ninjafly", pP->m_NinjaFly ? "Enabled" : "Disabled");
    pSelf->SendChatTarget(pP->GetCID(), aBuf);
}

void CGameContext::ConChatNinjaBomber(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    CCharacter *pChr = pP->GetCharacter();
    char aBuf[128];

    if (!pChr)
        return;

    if (pP->m_Insta || pChr->m_GameZone)
        return;

    if (!pP->m_AccData.m_NinjaBomber) {
        pSelf->SendChatTarget(pP->GetCID(), "Buy Ninjabomber first!");
        return;
    }

    pP->m_NinjaBomber ^= true;
    str_format(aBuf, sizeof aBuf, "%s Ninjabomber", pP->m_NinjaBomber ? "Enabled" : "Disabled");
    pSelf->SendChatTarget(pP->GetCID(), aBuf);
}
