#include <game/server/gamecontext.h>
#include <game/server/entities/character.h>
#include <engine/shared/config.h>
#include <game/version.h>
#include <engine/server/crypt.h>

// account
void CGameContext::ConChatLogin(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    if (pResult->NumArguments() != 2) {
        pSelf->SendChatTarget_Localization(pResult->GetClientID(), CHATCATEGORY_INFO, _("usage: /login <username> <password>"));
        return;
    }

    char Username[512];
    char Password[512];
    str_copy(Username, pResult->GetString(0), sizeof(Username));
    str_copy(Password, pResult->GetString(1), sizeof(Password));

    char aHash[64]; //Result
	mem_zero(aHash, sizeof(aHash));
	Crypt(Password, (const unsigned char*) "d9", 1, 14, aHash);
	
    pSelf->m_apPlayers[pResult->GetClientID()]->m_pAccount->Login(Username, aHash);
}

void CGameContext::ConChatRegister(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;

    if (pResult->NumArguments() != 2) {
        pSelf->SendChatTarget_Localization(pResult->GetClientID(), CHATCATEGORY_INFO, _("usage: /register <username> <password>"));
        return;
    }

    char Username[512];
    char Password[512];
    str_copy(Username, pResult->GetString(0), sizeof(Username));
    str_copy(Password, pResult->GetString(1), sizeof(Password));

    char aHash[64];
	Crypt(Password, (const unsigned char*) "d9", 1, 14, aHash);

    pSelf->m_apPlayers[pResult->GetClientID()]->m_pAccount->Register(Username, aHash, Password);
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

    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("Logout succesful"));
    pChr->Die(pP->GetCID(), WEAPON_GAME);
}

void CGameContext::ConChatChangePw(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;

    if (pResult->NumArguments() != 1) {
        pSelf->SendChatTarget_Localization(pResult->GetClientID(), CHATCATEGORY_INFO, _("usage: /changepw <password>"));
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

    char aHash[64];
	Crypt(NewPw, (const unsigned char*) "d9", 1, 16, aHash);

    pP->m_pAccount->NewPassword(aHash);
    pP->m_pAccount->Apply();

    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("Password changed"));
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

    if (pP->GetCharacter()->m_Frozen || pP->m_AccData.m_Arrested || pP->m_Insta || pP->m_onMonster) {
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("You can't do this at the moment"));
        return;
    }

    if (!pP->m_AccData.m_Donor) {
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("This is a donor feature"));
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

    if (pP->GetCharacter()->m_Frozen || pP->m_AccData.m_Arrested || pP->m_Insta || pP->m_onMonster) {
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("You can't do this at the moment"));
        return;
    }

    if (!pP->m_AccData.m_Donor) {
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("This is a donor feature"));
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

    if (pP->GetCharacter()->m_Frozen || pP->m_AccData.m_Arrested || pP->m_Insta || pP->m_onMonster) {
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("You can't do this at the moment"));
        return;
    }

    if (!pP->m_AccData.m_Donor) {
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("This is a donor feature"));
        return;
    }
    
    vec2 oldPos = pChr->m_Core.m_Pos;
    pChr->m_Home = pP->m_AccData.m_HouseID;
    
    if (pChr->m_Core.m_Pos != oldPos)
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("Welcome Home :)"));
}

void CGameContext::ConChatRainbow(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    CCharacter *pChr = pP->GetCharacter();

    if (!pChr)
        return;

    if (!pP->m_AccData.m_Donor) {
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("This is a donor feature"));
        return;
    }
    
    pP->m_Rainbow ^= true;
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("{str:RD} Rainbow"), "RD", pP->m_Rainbow ? "Enable" : "Disable", NULL);
}

void CGameContext::ConChatCrown(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    CCharacter *pChr = pP->GetCharacter();

    if (!pChr)
        return;

    if (!pP->m_AccData.m_Donor) {
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("This is a donor feature"));
        return;
    }
    
    pP->m_Crown ^= true;
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("{str:RD} Crown"), "RD", pP->m_Crown ? "Enable" : "Disable", NULL);
}

// vip
void CGameContext::ConChatUp(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    CCharacter *pChr = pP->GetCharacter();

    if (!pChr)
        return;

    if (pP->GetCharacter()->m_Frozen || pP->m_AccData.m_Arrested || pChr->m_GameZone || pP->m_Insta || pP->m_onMonster) {
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("You can't do this at the moment"));
        return;
    }

    if (!pP->m_AccData.m_VIP) {
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("This is a vip feature"));
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

    if (pP->GetCharacter()->m_Frozen || pP->m_AccData.m_Arrested || pP->m_Insta || pP->m_onMonster) {
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("You can't do this at the moment"));
        return;
    }

    if (!pP->m_AccData.m_VIP) {
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("This is a vip feature"));
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

    if (pP->GetCharacter()->m_Frozen || pP->m_AccData.m_Arrested || pChr->m_GameZone || pP->m_Insta || pP->m_onMonster) {
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("You can't do this at the moment"));
        return;
    }

    if (!pP->m_AccData.m_VIP) {
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("This is a vip feature"));
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

    if (pP->GetCharacter()->m_Frozen || pP->m_AccData.m_Arrested || pChr->m_GameZone || pP->m_Insta || pP->m_onMonster) {
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("You can't do this at the moment"));
        return;
    }

    if (!pP->m_AccData.m_VIP) {
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("This is a vip feature"));
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

    if (pP->GetCharacter()->m_Frozen || pP->m_AccData.m_Arrested || pChr->m_GameZone || pP->m_Insta || pP->m_onMonster) {
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("You can't do this at the moment"));
        return;
    }

    if (!pP->m_AccData.m_VIP) {
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("This is a vip feature"));
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
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("Only for police"));
        return;
    }

    pChr->m_JailRifle ^= 1;
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("Jailrifle {str:RD}"), "RD", pChr->m_JailRifle ? "Enable" : "Disable", NULL);
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
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("God {str:RD}"), "RD", pP->m_God ? "Enable" : "Disable");
}

// all
void CGameContext::ConChatInstagib(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    CCharacter *pChr = pP->GetCharacter();
    char aBuf[128];

    if (!g_Config.m_EnableInstagib) {
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("Instagib is not enabled"));
        return;
    }

    if (!pChr || !pP)
        return;

    if (pChr->m_Frozen || pP->m_AccData.m_Arrested || pChr->m_GameZone || pP->m_onMonster) {
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("You can't do this at the moment"));
        return;
    }

    pP->m_Insta ^= 1;
    pChr->Die(pP->GetCID(), WEAPON_GAME);
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("{str:PName} {str:RD} Instagib"), "PName", pSelf->Server()->ClientName(pP->GetCID()), "RD", pP->m_Insta ? "joined" : "left", NULL);
}

void CGameContext::ConChatMonster(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    CCharacter *pChr = pP->GetCharacter();
    char aBuf[128];

    if (!g_Config.m_EnableMonster) {
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("Zomb is not enabled"));
        return;
    }

    if (!pChr || !pP)
        return;

    if (pChr->m_Frozen || pP->m_AccData.m_Arrested || pChr->m_GameZone || pP->m_Insta) {
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("You can't do this at the moment"));
        return;
    }

    pP->m_onMonster ^= 1;
    pChr->Die(pP->GetCID(), WEAPON_GAME);
    str_format(aBuf, sizeof(aBuf), "%s %s Monster", pSelf->Server()->ClientName(pP->GetCID()), pP->m_onMonster ? "joined" : "left");
    pSelf->SendChat(-1, CGameContext::CHAT_ALL, aBuf);
}

void CGameContext::ConChatTransfer(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;

    if (pResult->NumArguments() != 1) {
        pSelf->SendChatTarget_Localization(pResult->GetClientID(), CHATCATEGORY_INFO, _("usage: /transfer <amount>"));
        return;
    }

    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    CCharacter *pChr = pP->GetCharacter();
    long long Money = pResult->GetLongLong(0);

    if (!pChr)
        return;

    if (pChr->m_Frozen || pChr->m_GameZone || pP->m_Insta) {
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("You can't do this at the moment"));
        return;
    }

    pChr->Transfer(Money);
}

void CGameContext::ConChatPM(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];

    if (pResult->NumArguments() != 2) {
        pSelf->SendChatTarget_Localization(pResult->GetClientID(), CHATCATEGORY_INFO, _("usage: /pm <name> <message>"));
        return;
    }

    const char* Victim = pResult->GetString(0);
    const char* Msg = pResult->GetString(1);
    int ID = pSelf->Server()->ClientIdByName(Victim);

    if (ID == -1) {
        pSelf->SendChatTarget_Localization(pResult->GetClientID(), CHATCATEGORY_INFO, _("No such user"));
        return;
    }

    if (pP->GetCID() == ID) {
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("You can't pm yourself"));
        return;
    }

    if (!pSelf->ValidID(ID)) {
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("Out of range"));
        return;
    }

    if (!pSelf->Server()->ClientIngame(ID)) {
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("No such player"));
        return;
    }

    pSelf->SendPrivate(pResult->GetClientID(), ID, Msg);
}

void CGameContext::ConChatSetPM(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];

    if (pResult->NumArguments() != 1) {
        pSelf->SendChatTarget_Localization(pResult->GetClientID(), CHATCATEGORY_INFO, _("usage: /setpm <name>"));
        return;
    }

    const char* Victim = pResult->GetString(0);
    int ID = pSelf->Server()->ClientIdByName(Victim);
    char aBuf[256];

    if (ID == -1) {
        pSelf->SendChatTarget_Localization(pResult->GetClientID(), CHATCATEGORY_INFO, _("No such user"));
        return;
    }

    if (pP->GetCID() == ID) {
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("You can't set yourself as pm partner"));
        return;
    }

    if (!pSelf->ValidID(ID)) {
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("Out of range"));
        return;
    }

    if (!pSelf->Server()->ClientIngame(ID)) {
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("No such player"));
        return;
    }

    pP->m_PmID = ID;
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("{str:VT} is now your pm partner"), "VT", Victim, NULL);
}

void CGameContext::ConChatDisabledmg(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    int Victim = pResult->GetVictim();

    if (Victim < 0) {
        pSelf->SendChatTarget_Localization(pResult->GetClientID(), CHATCATEGORY_INFO, _("usage: /disabledmg <id>"));
        return;
    }

    if (pP->GetCID() == Victim) {
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("You can't disabledmg on yourself"));
        return;
    }

    if (!pSelf->ValidID(Victim)) {
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("Out of range"));
        return;
    }

    if (!pSelf->Server()->ClientIngame(Victim)) {
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("No such player"));
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
        pSelf->SendChatTarget_Localization(pResult->GetClientID(), CHATCATEGORY_INFO, _("usage: /enabledmg <id>"));
        return;
    }

    if (pP->GetCID() == Victim) {
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("You can't enabledmg on yourself"));
        return;
    }

    if (!pSelf->ValidID(Victim)) {
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("Out of range"));
        return;
    }

    if (!pSelf->Server()->ClientIngame(Victim)) {
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("No such player"));
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
        pSelf->SendChatTarget_Localization(pResult->GetClientID(), CHATCATEGORY_INFO, _("Visit a coach first"));
        return;
    }

    if (pResult->NumArguments() != 1 && pResult->NumArguments() != 2) {
        pSelf->SendChatTarget_Localization(pResult->GetClientID(), CHATCATEGORY_INFO, _("usage: /train <weapon> ?<amount>"));
        return;
    }

    const char *Weapon = pResult->GetString(0);
    int ID = pSelf->GetWIDByStr(Weapon);
    int Amount = pResult->NumArguments() == 2 ? pResult->GetLongLong(1) : 1;
    char aBuf[256], numBuf[2][32];

    if (ID < 0) {
        str_format(aBuf, sizeof aBuf, "'%s' does not exist", Weapon);
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("'{str:W}' does not exist"), "W", Weapon, NULL);
        return;
    }

    if (Amount > 100) {
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("You can't train more than 100ep at once"));
        return;
    }

    if (Amount < 0) {
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("Bad boy..."));
        return;
    }

    if (pP->m_AccData.m_ExpWeapon[ID] >= (unsigned)g_Config.m_SvWLvlMax) {
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("You already reached the max level"));
        return;
    }

    if (g_Config.m_SvExpPrice * Amount > pP->m_AccData.m_Money) {
        pSelf->FormatInt(g_Config.m_SvExpPrice * Amount, numBuf[0]);
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("This would cost {str:D}$"), "D", numBuf[0], NULL);
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("Not enough money"));
        return;
    }

    pP->m_AccData.m_Money -= g_Config.m_SvExpPrice * Amount;
    pP->GetCharacter()->AddExp(ID, Amount);

    pSelf->FormatInt(pP->m_AccData.m_Money, numBuf[0]);
     pSelf->FormatInt(g_Config.m_SvExpPrice * Amount, numBuf[1]);
    str_format(aBuf, sizeof aBuf, "Money: %s (-%s$)", numBuf[0], numBuf[1]);
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("Money: {str:Num0} (-{str:Num1}$)"), "Num0", numBuf[0], "Num1", numBuf[1], NULL);
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
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("There are no bounties to hunt"));
        return;
    }

    if (ListID * 6 > Size) {
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("There are no bounties here"));
        return;
    }

    if (ListID * 6 < 0) {
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("Stop fooling around"));
        return;
    }

    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("~~~~~~ BOUNTY LIST ~~~~~~"));
    for (int i = ListID * 6; (i < ListID * 6 + 6 && i < Size); i++) {
        pSelf->FormatInt(pSelf->m_apPlayers[pSelf->BountyList(i)]->m_AccData.m_Bounty, numBuf);
        str_format(aBuf, sizeof aBuf, "'%s': %s$", pSelf->Server()->ClientName(pSelf->BountyList(i)), numBuf);
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("'{str:CN}': {str:BL}$"), "CN", pSelf->Server()->ClientName(pSelf->BountyList(i)), "BL", numBuf, NULL);
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
        pSelf->SendChatTarget_Localization(pResult->GetClientID(), CHATCATEGORY_INFO, _("usage: /checkbounty <id>"));
        return;
    }

    if (!pSelf->ValidID(Victim)) {
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("Out of range"));
        return;
    }

    CPlayer *pTarget = pSelf->m_apPlayers[Victim];

    if (!pTarget) {
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("No such player"));
        return;
    }

    if (!pTarget->m_AccData.m_Bounty) {
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("This player has no bounty"));
        return;
    }

    pSelf->FormatInt(pTarget->m_AccData.m_Bounty, numBuf);
    str_format(aBuf, sizeof aBuf, "%s has a bounty of %s$", pSelf->Server()->ClientName(Victim), numBuf);
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("{str:CN} has a bounty of {str:nB}$"), "CN", pSelf->Server()->ClientName(Victim), "nB", numBuf, NULL);

    
}

void CGameContext::ConChatSetbounty(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;

    if (pResult->NumArguments() != 1) {
        pSelf->SendChatTarget_Localization(pResult->GetClientID(), CHATCATEGORY_INFO, _("usage: /setbounty <id> <amount>"));
        return;
    }

    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    int Victim = pResult->GetVictim();
    long long Amount = pResult->GetLongLong(0);
    char aBuf[128], numBuf[32];

    if (Victim < 0) {
        pSelf->SendChatTarget_Localization(pResult->GetClientID(), CHATCATEGORY_INFO, _("usage: /setbounty <id> <amount>"));
        return;
    }

    if (!pSelf->ValidID(Victim)) {
        pSelf->SendChatTarget_Localization(pResult->GetClientID(), CHATCATEGORY_INFO, _("Out of range"));
        return;
    }

    if (Amount < 1000) {
        pSelf->SendChatTarget_Localization(pResult->GetClientID(), CHATCATEGORY_INFO, _("Put a bounty of at least 1.000$"));
        return;
    }

    CPlayer *pTarget = pSelf->m_apPlayers[Victim];

    if (!pTarget) {
        pSelf->SendChatTarget_Localization(pResult->GetClientID(), CHATCATEGORY_INFO, _("No such player"));
        return;
    }

    if ((long long unsigned)Amount > pP->m_AccData.m_Money) {
        pSelf->SendChatTarget_Localization(pResult->GetClientID(), CHATCATEGORY_INFO, _("Not enough money"));
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
    pSelf->SendChatTarget_Localization(-1, CHATCATEGORY_INFO, _("{str:PP} has put a bounty of {str:m}$ on {str:vt}"), "PP", pSelf->Server()->ClientName(pP->GetCID()), "m", numBuf, "vt", pSelf->Server()->ClientName(Victim), NULL);

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
        pSelf->SendChatTarget_Localization(pResult->GetClientID(), CHATCATEGORY_INFO, _("Usage: /mcbuy <amount>"));
        return;
    }

    if (!pSelf->Collision()->IsTile(pSelf->GetPlayerChar(pResult->GetClientID())->m_Core.m_Pos, TILE_MONEYCOLLECTOR)) {
        pSelf->SendChatTarget_Localization(pResult->GetClientID(), CHATCATEGORY_INFO, _("You need to be inside the Moneycollector"));
        return;
    }

    long long Amount = pResult->GetLongLong(0);

    if (Amount < 0) {
        pSelf->SendChatTarget_Localization(pResult->GetClientID(), CHATCATEGORY_INFO, _("Pfff..."));
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

    pSelf->SendChatTarget_Localization(ID, CHATCATEGORY_INFO, _("~~ Money Collector Help ~~"));
    pSelf->SendChatTarget_Localization(ID, CHATCATEGORY_INFO, _("The Money Collector collects 4% of the farmed income on the server."));
    pSelf->SendChatTarget_Localization(ID, CHATCATEGORY_INFO, _("This money ends up in the pot."));
    pSelf->SendChatTarget_Localization(ID, CHATCATEGORY_INFO, _("The pot is increased by 50% every hour."));
    pSelf->SendChatTarget_Localization(ID, CHATCATEGORY_INFO, _("The price is reduced by 5% every 10 minutes"));
    pSelf->SendChatTarget_Localization(ID, CHATCATEGORY_INFO, _("The Holder can collect the money from the pot."));
    pSelf->SendChatTarget_Localization(ID, CHATCATEGORY_INFO, _("Checkout /mccmds to get more information."));
}

void CGameContext::ConChatMCCmds(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    int ID = pResult->GetClientID();

    pSelf->SendChatTarget_Localization(ID, CHATCATEGORY_INFO, _("~~ Money Collector Cmds ~~"));
    pSelf->SendChatTarget_Localization(ID, CHATCATEGORY_INFO, _("/mcbuy"));
    pSelf->SendChatTarget_Localization(ID, CHATCATEGORY_INFO, _("/mccollect"));
    pSelf->SendChatTarget_Localization(ID, CHATCATEGORY_INFO, _("/mchelp"));
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
    
    if(!pP->m_AccData.m_UserID)
        return;
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("---------- STATS ----------"));
    str_format(aBuf, sizeof aBuf, "AccID: %d", pP->m_AccData.m_UserID);
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("AccID: {int:id}"), "id", &pP->m_AccData.m_UserID, NULL);

    str_format(aBuf, sizeof aBuf, "Username: %s", pP->m_AccData.m_Username);
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("Username: {str:un}"), "un", pP->m_AccData.m_Username, NULL);

    str_format(aBuf, sizeof aBuf, "Level: %d", pP->m_AccData.m_Level);
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("Level: {int:lv}"), "lv", &pP->m_AccData.m_Level, NULL);

    pSelf->FormatInt(pP->m_AccData.m_ExpPoints, numBuf[0]);
    pSelf->FormatInt(pChr->calcExp(pP->m_AccData.m_Level), numBuf[1]);
    str_format(aBuf, sizeof aBuf, "Exp: %s ep / %s ep", numBuf[0], numBuf[1]);
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("Exp: {str:exp} / {str:nB}"), "exp", numBuf[0], "nB", numBuf[1], NULL);

    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("Hammer: {int:h} Gun: {int:g}, Shotgun: {int:sg}, Grenade: {int:gr}, Rifle: {int:r}"),
        "h", &pP->m_AccData.m_LvlWeapon[WEAPON_HAMMER],
        "g", &pP->m_AccData.m_LvlWeapon[WEAPON_GUN],
        "sg", &pP->m_AccData.m_LvlWeapon[WEAPON_SHOTGUN],
        "gr", &pP->m_AccData.m_LvlWeapon[WEAPON_GRENADE],
        "r" ,&pP->m_AccData.m_LvlWeapon[WEAPON_RIFLE], NULL);

    pSelf->FormatInt(pP->m_AccData.m_Money, numBuf[0]);
    str_format(aBuf, sizeof aBuf, "Money: %s$", numBuf[0]);
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("Money: {str:Money}$"), "Money", numBuf[0], NULL);

    pSelf->FormatInt(pP->m_AccData.m_Bounty, numBuf[0]);
    str_format(aBuf, sizeof aBuf, "Bounty: %s$", numBuf[0]);
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("Bounty: {str:B}$"), "B", numBuf[0], NULL);
}

void CGameContext::ConChatCmdlist(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    int ID = !pResult->NumArguments() ? 0 : pResult->GetInteger(0);
    if (ID == 0) {
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("---------- COMMAND LIST ----------"));
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("/info -- Infos about the server"));
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("/help -- Help if you are new"));
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("/me -- Account stats"));
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("/disabledmg -- Disables damage on someone"));
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("/enabledmg -- Enables damage on someone"));
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("/transfer -- Drops money at your cursors position"));
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("/donor -- Infos about Donor"));
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("/vip -- Infos about VIP"));
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("/upgrcmds -- Get a list of all Upgrade commands"));
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("/instagib -- Play insta"));
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("/cmdlist 1 -- see more commands"));
    } else if (ID == 1) {
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("---------- COMMAND LIST ----------"));
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("/ids -- Display all IDs"));
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("/event -- Gives the current event infos"));
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("/setbounty -- Put a bounty on someone"));
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("/checkbounty -- Check if a player has a bounty"));
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("/bountylist -- Get a list of all bounties"));
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("/writestats -- Writes your weaponlevel down"));
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("/lang cn|en -- Change you server language"));
    } else {
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("We don't have so many commands :("));
    }
}

void CGameContext::ConChatHelp(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("---------- HELP ----------"));
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("You need to register enter the game."));
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("-- /register <username> <password>"));
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("If you already have an account you can login with"));
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("-- /login <username> <password>"));
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("If you have any questions you can always ask a team member."));
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("Join our discord to contact us https://discord.gg/Rstb8ge"));
}

void CGameContext::ConChatInfo(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("---------- INFO ----------"));
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("UH|City is made by UrinStone."));
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("Join our Discord to be always informed about the latest updates."));
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("If you forget your password or wish to donate, please ONLY contact UrinStone#8404 on Discord."));
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
    
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("---------- RULES ----------"));
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("- Don't use Bots / Lua scripts"));
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("- no Multiacc (logging in with multiple accounts)"));
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("- Don't abuse Bugs!"));
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("- Don't insult other players"));
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("- Don't Spam"));
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("- Don't Funvote"));
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("- Don't place unfair lights"));
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("- Respect the Police"));
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("You will be punished if you disregard the rules"));
}

void CGameContext::ConChatDonor(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("---------- DONOR ----------"));
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("Donor costs 10€"));
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("Contact UrinStone#8404 at discord"));
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("Donor provides following features:"));
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("- x5 Money and Exp"));
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("- The nice crown"));
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("- Rainbow"));
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("- Your own House"));
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("- Home teleport"));
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("- Save and load a position"));
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("- Exclusive 1000$ money tiles that give x6 money & exp"));
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("Checkout /donorcmds for more information."));
}

void CGameContext::ConChatDonorCmds(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("---------- DONORCMDS ----------"));
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _(" /save -- Saves your position"));
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _(" /load -- Teleports you to the saved position"));
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _(" /rainbow -- Gives you rainbow colors"));
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _(" /crown -- The nice crown"));
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _(" /home -- Teleports you Into your Home"));
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("Checkout /donor to get more information."));
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

    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("---------- IDS ----------"));
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

    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("---------- VIP ----------"));
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("VIP gives you x3 money and exp."));
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("Use movement cmds:"));
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("- /up"));
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("- /down"));
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("- /left"));
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("- /right"));
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("- /tele"));
}

void CGameContext::ConChatUpgrCmds(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *) pUserData;
    CPlayer *pP = pSelf->m_apPlayers[pResult->GetClientID()];
    const char *Upgr = !pResult->NumArguments() ? "" : pResult->GetString(0);

    if (!str_comp_nocase(Upgr, "hammer")) {
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("---------- UPGRADE CMDS ----------"));
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("/walls -- Laser walls"));
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("/hammerkill -- Your target can't escape!"));
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("/plasma -- Beat them with your Plasma"));
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("/portal -- Opens a portal to teleport between two points"));
        return;
    }

    if (!str_comp_nocase(Upgr, "gun")) {
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("---------- UPGRADE CMDS ----------"));
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("/fgun -- Freeze gun"));
        return;
    }

    if (!str_comp_nocase(Upgr, "rifle") || !str_comp_nocase(Upgr, "laser")) {
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("---------- UPGRADE CMDS ----------"));
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("/swap -- Swaps the position with your target"));
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("/rplasma -- Rifle plasma"));
        return;
    }

    if (!str_comp_nocase(Upgr, "jump")) {
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("---------- UPGRADE CMDS ----------"));
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("/fly -- Fly"));
        return;
    }

    if (!str_comp_nocase(Upgr, "hook")) {
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("---------- UPGRADE CMDS ----------"));
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("/bhook -- Boost other players"));
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("/hhook -- Increases the hooked players healthregen"));
        return;
    }

    if (!str_comp_nocase(Upgr, "ninja")) {
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("---------- UPGRADE CMDS ----------"));
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("/nfly -- Ninja fly"));
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("/nbomber -- Bomb your enemys away"));
        return;
    }

    if (!str_comp_nocase(Upgr, "special")) {
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("---------- UPGRADE CMDS ----------"));
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("/pushaura -- Pushes the other players"));
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("/pullaura -- Pull the other players"));
        return;
    }

    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("---------- UPGRADE CMDS ----------"));
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("Please use '/upgrcmds <item>'"));
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("You can checkout following items:"));
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("- hammer"));
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("- gun"));
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("- rifle"));
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("- jump"));
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("- hook"));
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("- ninja"));
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("- special"));
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
        pSelf->SendChatTarget_Localization(pResult->GetClientID(), CHATCATEGORY_INFO, _("Enter a shop first"));
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
        pSelf->SendChatTarget_Localization(pResult->GetClientID(), CHATCATEGORY_INFO, _("Use /shop <item> to see the items you want."));
        pSelf->SendChatTarget_Localization(pResult->GetClientID(), CHATCATEGORY_INFO, _("Example: '/shop hammer' will show all hammer upgrades."));
        pSelf->SendChatTarget_Localization(pResult->GetClientID(), CHATCATEGORY_INFO, _("To switch between pages use f3 or f4."));
        pSelf->SendChatTarget_Localization(pResult->GetClientID(), CHATCATEGORY_INFO, _("Available menus are:"));
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("- hammer"));
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("- gun"));
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("- shotgun"));
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("- grenade"));
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("- rifle"));
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("- ninja"));
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("- general"));
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("- hook"));
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
        pSelf->SendChatTarget_Localization(pResult->GetClientID(), CHATCATEGORY_INFO, _("Visit a coach first"));
        return;
    }

    pSelf->SendChatTarget_Localization(pResult->GetClientID(), CHATCATEGORY_INFO, _("You can buy experience here."));
    pSelf->SendChatTarget_Localization(pResult->GetClientID(), CHATCATEGORY_INFO, _("One exp costs 1.000.000$."));
    pSelf->SendChatTarget_Localization(pResult->GetClientID(), CHATCATEGORY_INFO, _("Use /train <weapon> ?<amount>."));
    pSelf->SendChatTarget_Localization(pResult->GetClientID(), CHATCATEGORY_INFO, _("Available weapons are:"));
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("- hammer"));
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("- gun"));
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("- shotgun"));
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("- grenade"));
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("- rifle"));
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
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("Buy Walls first!"));
        return;
    }

    pChr->ChangeUpgrade(ITEM_HAMMER, UPGRADE_HAMMERWALLS);
    str_format(aBuf, sizeof(aBuf), "%s Walls", pP->m_AciveUpgrade[ITEM_HAMMER] == UPGRADE_HAMMERWALLS ?"Enabled":"disabled");
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("{str:RD} Walls"), "RD", pP->m_AciveUpgrade[ITEM_HAMMER] == UPGRADE_HAMMERWALLS ?"Enabled":"Disabled", NULL);
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
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("Buy Plasma first!"));
        return;
    }

    pChr->ChangeUpgrade(ITEM_HAMMER, UPGRADE_HAMMERSHOT);
    str_format(aBuf, sizeof(aBuf), "%s Plasma", pP->m_AciveUpgrade[ITEM_HAMMER] == UPGRADE_HAMMERSHOT ?"Enabled":"disabled");
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("{str:plasma} Plasma"), "plasma", pP->m_AciveUpgrade[ITEM_HAMMER] == UPGRADE_HAMMERSHOT ?"Enabled":"Disabled", NULL);
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
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("Buy Hammerkill first!"));
        return;
    }

    pChr->ChangeUpgrade(ITEM_HAMMER, UPGRADE_HAMMERKILL);
    str_format(aBuf, sizeof(aBuf), "%s Hammerkill", pP->m_AciveUpgrade[ITEM_HAMMER] == UPGRADE_HAMMERKILL ?"Enabled":"disabled");
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("{str:hk} Hammerkill"), "hk", pP->m_AciveUpgrade[ITEM_HAMMER] == UPGRADE_HAMMERKILL ?"Enabled":"Disabled", NULL);
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
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("Buy Gunfreeze first!"));
        return;
    }

    pChr->ChangeUpgrade(ITEM_GUN, UPGRADE_GUNFREEZE);
    str_format(aBuf, sizeof(aBuf), "%s Gunfreeze", pP->m_AciveUpgrade[ITEM_GUN] == UPGRADE_GUNFREEZE ?"Enabled":"disabled");
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("{str:gf} Gunfreeze"), "gf", pP->m_AciveUpgrade[ITEM_GUN] == UPGRADE_GUNFREEZE ? "Enabled" : "Disabled", NULL);
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
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("Buy Rifleplasma first!"));
        return;
    }

    pChr->ChangeUpgrade(ITEM_RIFLE, UPGRADE_RIFLEPLASMA);
    str_format(aBuf, sizeof(aBuf), "%s Rifleplasma", pP->m_AciveUpgrade[ITEM_RIFLE] == UPGRADE_RIFLEPLASMA ?"Enabled":"disabled");
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("{str:rp} Rifleplasma"), "rp", pP->m_AciveUpgrade[ITEM_RIFLE] == UPGRADE_RIFLEPLASMA ?"Enabled":"Disabled", NULL);
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
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("Buy Swap first!"));
        return;
    }

    pChr->ChangeUpgrade(ITEM_RIFLE, UPGRADE_RIFLESWAP);
    str_format(aBuf, sizeof(aBuf), "%s Swap", pP->m_AciveUpgrade[ITEM_RIFLE] == UPGRADE_RIFLESWAP ?"Enabled":"disabled");
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("{str:ed} Swap"), "ed", pP->m_AciveUpgrade[ITEM_RIFLE] == UPGRADE_RIFLESWAP ?"Enabled":"Disabled", NULL);
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
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("Buy Fly first!"));
        return;
    }

    pChr->ChangeUpgrade(ITEM_JUMP, UPGRADE_FLY);
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("{str:ed} Fly"), "ed", pP->m_AciveUpgrade[ITEM_JUMP] == UPGRADE_FLY ?"Enabled":"Disabled", NULL);
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
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("Buy Healhook first!"));
        return;
    }

    pChr->ChangeUpgrade(ITEM_HOOK, UPGRADE_HEALHOOK);
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("{str:ed} Healhook"), "ed", pP->m_AciveUpgrade[ITEM_HOOK] == UPGRADE_HEALHOOK ?"Enabled":"disabled", NULL);
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
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("Buy Boosthook first!"));
        return;
    }

    pChr->ChangeUpgrade(ITEM_HOOK, UPGRADE_BOOSTHOOK);
    pChr->m_Core.m_DisablePlayerHook ^= true;
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("{str:ed} Boosthook"), "ed", pP->m_AciveUpgrade[ITEM_HOOK] == UPGRADE_BOOSTHOOK ?"Enabled":"disabled", NULL);
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
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("Buy Portal first!"));
        return;
    }

    pChr->ChangeUpgrade(ITEM_HAMMER, UPGRADE_PORTAL);
    str_format(aBuf, sizeof(aBuf), "%s Portal", pP->m_AciveUpgrade[ITEM_HAMMER] == UPGRADE_PORTAL ? "Enabled" : "disabled");
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("{str:ed} Portal"), "ed", pP->m_AciveUpgrade[ITEM_HAMMER] == UPGRADE_PORTAL ? "Enabled" : "disabled", NULL);
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
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("Buy PushAura first!"));
        return;
    }

    if (pP->m_GravAuraCooldown) {
        str_format(aBuf, sizeof aBuf, "You can use this aura in %d seconds again", pP->m_GravAuraCooldown);
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("You can use this aura in {int:cd} seconds again"), "cd", &pP->m_GravAuraCooldown);
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
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("Buy PullAura first!"));
        return;
    }

    if (pP->m_GravAuraCooldown) {
        str_format(aBuf, sizeof aBuf, "You can use this aura in %d seconds again", pP->m_GravAuraCooldown);
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("You can use this aura in {int:cd} seconds again"), "cd", &pP->m_GravAuraCooldown, NULL);
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
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("Buy Ninjafly first!"));
        return;
    }

    pP->m_NinjaFly ^= true;
    str_format(aBuf, sizeof aBuf, "%s Ninjafly", pP->m_NinjaFly ? "Enabled" : "disabled");
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("{str:ED} Ninjafly"), "ED", pP->m_NinjaFly ? "Enable" : "Disable", NULL);
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
        pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("Buy Ninjabomber first!"));
        return;
    }

    pP->m_NinjaBomber ^= true;
    str_format(aBuf, sizeof aBuf, "%s Ninjabomber", pP->m_NinjaBomber ? "Enabled" : "disabled");
    pSelf->SendChatTarget_Localization(pP->GetCID(), CHATCATEGORY_INFO, _("{str:ed} Ninjabomber"), "ed", pP->m_NinjaBomber ? "Enabled" : "disabled", NULL);
}

void CGameContext::ConLanguage(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;

	int ClientID = pResult->GetClientID();

	const char *pLanguageCode = (pResult->NumArguments()>0) ? pResult->GetString(0) : 0x0;
	char aFinalLanguageCode[8];
	aFinalLanguageCode[0] = 0;

	if(pLanguageCode)
	{
		if(str_comp_nocase(pLanguageCode, "ua") == 0)
			str_copy(aFinalLanguageCode, "uk", sizeof(aFinalLanguageCode));
		else
		{
			for(int i=0; i<pSelf->Server()->Localization()->m_pLanguages.size(); i++)
			{
				if(str_comp_nocase(pLanguageCode, pSelf->Server()->Localization()->m_pLanguages[i]->GetFilename()) == 0)
					str_copy(aFinalLanguageCode, pLanguageCode, sizeof(aFinalLanguageCode));
			}
		}
	}
	
	if(aFinalLanguageCode[0])
	{
		pSelf->SetClientLanguage(ClientID, aFinalLanguageCode);
	}
	else
	{
		const char* pLanguage = pSelf->m_apPlayers[ClientID]->GetLanguage();
		const char* pTxtUnknownLanguage = pSelf->Server()->Localization()->Localize(pLanguage, _("Unknown language"));
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "language", pTxtUnknownLanguage);	
		
		dynamic_string BufferList;
		int BufferIter = 0;
		for(int i=0; i<pSelf->Server()->Localization()->m_pLanguages.size(); i++)
		{
			if(i>0)
				BufferIter = BufferList.append_at(BufferIter, ", ");
			BufferIter = BufferList.append_at(BufferIter, pSelf->Server()->Localization()->m_pLanguages[i]->GetFilename());
		}
		
		dynamic_string Buffer;
		pSelf->Server()->Localization()->Format_L(Buffer, pLanguage, _("Available languages: {str:ListOfLanguage}"), "ListOfLanguage", BufferList.buffer(), NULL);
		
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "language", Buffer.buffer());

        pSelf->SendChatTarget(ClientID, Buffer.buffer());
    }
	
	return;
}