#include <stdio.h>
#include <string.h>

#include <engine/server.h>
#include <game/version.h>
#include <locale.h>
#include "cmds.h"
#include "account.h"


CCmd::CCmd(CPlayer *pPlayer, CGameContext *pGameServer)
{
	m_pPlayer = pPlayer;
	m_pGameServer = pGameServer;
}

void CCmd::ChatCmd(CNetMsg_Cl_Say *Msg)
{
	// if(!str_comp_nocase(Msg->m_pMessage, "/right")) {
	// if(!strncmp(Msg->m_pMessage, "/login", 6))
	// if(sscanf(Msg->m_pMessage, "/login %s %s", name, pass) != 2)
	setlocale(LC_NUMERIC, "");

	if(!strncmp(Msg->m_pMessage, "/login", 6))
	{
		LastChat();

		char Username[512];
		char Password[512];
		if(sscanf(Msg->m_pMessage, "/login %s %s", Username, Password) != 2)
		{
			GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Please, use '/login <username> <password>'");
			return;
		}
		m_pPlayer->m_pAccount->Login(Username, Password);
		return;
	}
	else if(!str_comp_nocase(Msg->m_pMessage, "/logout"))
	{
		LastChat();

		CCharacter* pOwner = GameServer()->GetPlayerChar(m_pPlayer->GetCID());

		if (!(pOwner && pOwner->IsAlive()))
			return;

		if(!m_pPlayer->m_AccData.m_UserID)
		{
			GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Not logged in");
			return;
		}

		m_pPlayer->m_pAccount->Apply();
		m_pPlayer->m_pAccount->Reset();

		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Logout succesful");

		pOwner->Die(m_pPlayer->GetCID(), WEAPON_GAME);
		return;
	}
	else if(!str_comp_nocase(Msg->m_pMessage, "/save"))
	{
		LastChat();

		CCharacter* pOwner = GameServer()->GetPlayerChar(m_pPlayer->GetCID());

		if (!(pOwner && pOwner->IsAlive() && !m_pPlayer->m_Insta))
			return;

		if(GameServer()->Server()->AuthLvl(m_pPlayer->GetCID()) < 1)
		{
			if(!m_pPlayer->m_AccData.m_Donor)
			{
				GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Access denied");
				return;
			}
		}

		pOwner->SaveLoad(false);
		return;
	}
	else if(!str_comp_nocase(Msg->m_pMessage, "/load"))
	{
		LastChat();

		CCharacter* pOwner = GameServer()->GetPlayerChar(m_pPlayer->GetCID());

		if (!(pOwner && pOwner->IsAlive() && !m_pPlayer->m_Insta))
			return;

		if(GameServer()->Server()->AuthLvl(m_pPlayer->GetCID()) < 1)
		{
			if (!m_pPlayer->m_AccData.m_Donor
				|| m_pPlayer->GetCharacter()->m_Frozen
				|| m_pPlayer->m_AccData.m_Arrested)
			{
				GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Access denied");
				return;
			}
		}

		pOwner->SaveLoad(true);
		return;
	}
	else if(!str_comp_nocase(Msg->m_pMessage, "/me") || !str_comp_nocase(Msg->m_pMessage, "/status") || !str_comp_nocase(Msg->m_pMessage, "/stats"))
	{
		LastChat();
		char aBuf[256];
		char numBuf[2][16];
		CCharacter *pOwner = GameServer()->GetPlayerChar(m_pPlayer->GetCID());

		if (!pOwner)
			return;

		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "---------- STATS ----------");
		str_format(aBuf, sizeof aBuf, "AccID: %d", m_pPlayer->m_AccData.m_UserID);
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), aBuf);

		str_format(aBuf, sizeof aBuf, "Username: %s", m_pPlayer->m_AccData.m_Username);
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), aBuf);

		str_format(aBuf, sizeof aBuf, "Username: %d", m_pPlayer->m_AccData.m_Level);
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), aBuf);

		GameServer()->FormatInt(m_pPlayer->m_AccData.m_ExpPoints, numBuf[0]);
		GameServer()->FormatInt(pOwner->calcExp(m_pPlayer->m_AccData.m_Level), numBuf[1]);
		str_format(aBuf, sizeof aBuf, "Exp: %s ep / %s ep", numBuf[0], numBuf[1]);
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), aBuf);

		GameServer()->FormatInt(m_pPlayer->m_AccData.m_Money, numBuf[0]);
		str_format(aBuf, sizeof aBuf, "Money: %s$", numBuf[0]);
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), aBuf);

		return;
	}
	else if(!str_comp_nocase(Msg->m_pMessage, "/tele"))
	{
		LastChat();
		CCharacter* pOwner = GameServer()->GetPlayerChar(m_pPlayer->GetCID());

		if (!(pOwner && pOwner->IsAlive()))
			return;

		if (m_pPlayer->m_Insta)
			return;

		if (!m_pPlayer->m_AccData.m_Donor
			|| m_pPlayer->GetCharacter()->m_Frozen
			|| m_pPlayer->m_AccData.m_Arrested)
		{
			GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Access denied");
			return;
		}

		pOwner->Tele();
		return;
	}
	else if (!str_comp_nocase(Msg->m_pMessage, "/up")) {
		LastChat();
		CCharacter* pOwner = GameServer()->GetPlayerChar(m_pPlayer->GetCID());

		if (!(pOwner && pOwner->IsAlive()))
			return;

		if (m_pPlayer->m_Insta)
			return;

		if (!m_pPlayer->m_AccData.m_Donor
			|| m_pPlayer->GetCharacter()->m_Frozen
			|| m_pPlayer->m_AccData.m_Arrested)
		{
			GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Access denied");
			return;
		}

		pOwner->Move(0);
		return;
	}
	else if (!str_comp_nocase(Msg->m_pMessage, "/down")) {
		LastChat();
		CCharacter* pOwner = GameServer()->GetPlayerChar(m_pPlayer->GetCID());

		if (!(pOwner && pOwner->IsAlive()))
			return;

		if (m_pPlayer->m_Insta)
			return;

		if (!m_pPlayer->m_AccData.m_Donor
			|| m_pPlayer->GetCharacter()->m_Frozen
			|| m_pPlayer->m_AccData.m_Arrested)
		{
			GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Access denied");
			return;
		}

		pOwner->Move(2);
		return;
	}
	else if (!str_comp_nocase(Msg->m_pMessage, "/left")) {
		LastChat();
		CCharacter* pOwner = GameServer()->GetPlayerChar(m_pPlayer->GetCID());

		if (!(pOwner && pOwner->IsAlive()))
			return;

		if (m_pPlayer->m_Insta)
			return;

		if (!m_pPlayer->m_AccData.m_Donor
			|| m_pPlayer->GetCharacter()->m_Frozen
			|| m_pPlayer->m_AccData.m_Arrested)
		{
			GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Access denied");
			return;
		}

		pOwner->Move(1);
		return;
	}
	else if (!str_comp_nocase(Msg->m_pMessage, "/right")) {
		LastChat();
		CCharacter* pOwner = GameServer()->GetPlayerChar(m_pPlayer->GetCID());

		if (!(pOwner && pOwner->IsAlive()))
			return;

		if (m_pPlayer->m_Insta)
			return;

		if (!m_pPlayer->m_AccData.m_Donor
			|| m_pPlayer->GetCharacter()->m_Frozen
			|| m_pPlayer->m_AccData.m_Arrested)
		{
			GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Access denied");
			return;
		}

		pOwner->Move(3);
		return;
	}
	else if(!str_comp_nocase(Msg->m_pMessage, "/home") || !str_comp_nocase(Msg->m_pMessage, "/house"))
	{
		LastChat();
		CCharacter* pOwner = GameServer()->GetPlayerChar(m_pPlayer->GetCID());

		if (!(pOwner && pOwner->IsAlive()))
			return;

		if (m_pPlayer->m_Insta 
			|| m_pPlayer->GetCharacter()->m_Frozen
			|| m_pPlayer->m_AccData.m_Arrested)
			return;

		if (!m_pPlayer->m_AccData.m_Donor)
		{
			GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Only for Donors!");
			return;
		}

		
		pOwner->m_Home = m_pPlayer->m_AccData.m_UserID;
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Welcome Home :)");
		dbg_msg("-.-", "/home: %i", pOwner->m_Home);

		return;

	}
	else if(!str_comp_nocase(Msg->m_pMessage, "/jailrifle"))
	{
		LastChat();

		CCharacter* pOwner = GameServer()->GetPlayerChar(m_pPlayer->GetCID());

		if (!(pOwner && pOwner->IsAlive()))
			return;

		char aBuf[200];
		
		if(!GameServer()->Server()->AuthLvl(m_pPlayer->GetCID()))
		{
			GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Only for Police");
			return;
		}
		m_pPlayer->GetCharacter()->m_JailRifle^=1;

		str_format(aBuf, sizeof(aBuf), "JailRifle %s", m_pPlayer->GetCharacter()->m_JailRifle?"enabled":"disabled");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), aBuf);

		return;
	}
	else if(!strncmp(Msg->m_pMessage, "/register", 9))
	{
		LastChat();

		char Username[512];
		char Password[512];
		if(sscanf(Msg->m_pMessage, "/register %s %s", Username, Password) != 2)
		{
			GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Please, use '/register <username> <password>'");
			return;
		}
		m_pPlayer->m_pAccount->Register(Username, Password);
		return;
	}
	else if(!str_comp_nocase(Msg->m_pMessage, "/invi") || !str_comp_nocase(Msg->m_pMessage, "/invisible") || !str_comp_nocase(Msg->m_pMessage, "/invis"))
	{
		LastChat();

		if(m_pPlayer->m_Insta)
		{
			GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Cmd is not allowed while playing instagib");
			return;
		}

		CCharacter *pOwner = GameServer()->GetPlayerChar(m_pPlayer->GetCID());

		if(pOwner)
		{
			pOwner->m_Invisible++;

			if(pOwner->m_Invisible == 1)
				GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Invisibility enabled I");
			else if(pOwner->m_Invisible == 2)
				GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Invisibility enabled II");
			else if(pOwner->m_Invisible > 2)
			{
				GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Invisibility disabled");
				pOwner->m_Invisible = 0;
			}
		}
		return;
	}
	else if(!str_comp_nocase(Msg->m_pMessage, "/rainbow"))
	{
		LastChat();
		char aBuf[200];

		m_pPlayer->m_Rainbow^=true;
		str_format(aBuf, sizeof(aBuf), "%s Rainbow", m_pPlayer->m_Rainbow ? "Enabled" : "Disabled");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), aBuf);

		return;
	}
	else if(!str_comp_nocase(Msg->m_pMessage, "/crown"))
	{
		LastChat();

		char aBuf[200];

		if (m_pPlayer->m_AccData.m_Donor) {
			m_pPlayer->m_Crown ^= true;
			str_format(aBuf, sizeof(aBuf), "%s Crown", m_pPlayer->m_Crown ? "Enabled" : "Disabled");
			GameServer()->SendChatTarget(m_pPlayer->GetCID(), aBuf);
		}

		return;
	}
	else if(!str_comp_nocase(Msg->m_pMessage, "/del") || !str_comp_nocase(Msg->m_pMessage, "/delete"))
	{
		LastChat();
		m_pPlayer->m_pAccount->Delete();
		return;
	}
	else if(!strncmp(Msg->m_pMessage, "/password", 9))
	{
		LastChat();
		char NewPassword[512];
		if(sscanf(Msg->m_pMessage, "/password %s", NewPassword) != 1)
		{
			GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Please use '/password <password>'");
			return;
		}
		m_pPlayer->m_pAccount->NewPassword(NewPassword);
		return;
	}
	if(!strncmp(Msg->m_pMessage, "/newname", 8) || !strncmp(Msg->m_pMessage, "/changename", 11))
	{
		LastChat();
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Please, use /rename <newname>");
		return;
	}
	else if(!strncmp(Msg->m_pMessage, "/rename", 7))
	{
		LastChat();
		char NewUsername[512];
		if(sscanf(Msg->m_pMessage, "/rename %s", NewUsername) != 1)
		{
			GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Please use '/rename <newname>'");
			return;
		}
		m_pPlayer->m_pAccount->NewUsername(NewUsername);
		return;
	}
	else if(!str_comp_nocase(Msg->m_pMessage, "/info"))
    {
		LastChat();

		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "---------- INFO ----------");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "UH | City is made by UrinStone and Nohack.");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "It's still in early access, but we are updating almoast daily.");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "--");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Always remember your password. Admins or police won't ask for it!");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "If you have any problems you can contact us on discord https://discord.gg/Rstb8ge");

		return;
    }
	else if(!str_comp_nocase(Msg->m_pMessage, "/instagib") || !str_comp_nocase(Msg->m_pMessage, "/insta"))
    {
		LastChat();
		char aBuf[200];	
		if(!g_Config.m_EnableInstagib)
		{
			GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Instagib is not enabled on this server");
			return;
		}
		if(m_pPlayer->m_AccData.m_Arrested)
		{
			GameServer()->SendChatTarget(m_pPlayer->GetCID(), "You are not permitted to join instagib.");
			return;
		}
		CCharacter *pOwner = GameServer()->GetPlayerChar(m_pPlayer->GetCID());
		if(pOwner && pOwner->IsAlive())
		{
			m_pPlayer->m_Insta^=1;
			str_format(aBuf, sizeof(aBuf), "%s %s Instagib",GameServer()->Server()->ClientName(m_pPlayer->GetCID()),m_pPlayer->m_Insta?"joined":"left");
			m_pPlayer->GetCharacter()->Die(m_pPlayer->GetCID(), WEAPON_GAME);
			GameServer()->SendChat(-1, CGameContext::CHAT_ALL, aBuf);
		}
		else
		{
			m_pPlayer->m_Insta^=1;
			str_format(aBuf, sizeof(aBuf), "%s %s Instagib",GameServer()->Server()->ClientName(m_pPlayer->GetCID()),m_pPlayer->m_Insta?"joined":"left");
			GameServer()->SendChat(-1, CGameContext::CHAT_ALL, aBuf);
		}
		return;
    }
	if(!strncmp(Msg->m_pMessage, "/transfer", 9))
	{
		LastChat();
		int Money;

		if(sscanf(Msg->m_pMessage, "/transfer %i", &Money) != 1)
		{
			GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Please, use '/transfer <money>'");
			return;
		}
		
		CCharacter *pUser = GameServer()->GetPlayerChar(m_pPlayer->GetCID());

		if (pUser) {
			pUser->Transfer(Money);
			GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Welcome Home :)");
		}

		return;
	}
	else if(!str_comp_nocase(Msg->m_pMessage, "/cmdlist"))
	{
		LastChat();

		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "---------- DONOR COMMANDS ----------");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "/info -- Infos about the server");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "/help -- Help if you are new");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "/me -- Account stats");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "/transfer -- Sends money");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "/rainbow -- Rainbow skin");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "/rainbow -- Rainbow");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "/donor -- Infos about Donor");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "/vip -- Infos about VIP");

		return;
	}
	else if(!str_comp_nocase(Msg->m_pMessage, "/help"))
	{
		LastChat();

		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "---------- HELP ----------");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "This mod is currently in early access.");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "You need to register enter the game.");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "    /register username password");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "If you already have an account you can login with");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "    /login username password");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "If you have any questions you can always ask a team member.");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Join our discord to contact us https://discord.gg/Rstb8ge");

		return;
	}
	else if(!str_comp_nocase(Msg->m_pMessage, "/donor"))
	{
		LastChat();

		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "---------- DONOR ----------");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Donor is currently for free. Ask an Admin to get donor.");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "It will be removed with the final state of the mod.");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Donor provides following features:");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "- The nice crown");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "- Home teleport");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "- Save and load a position");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "- Exclusive 1000$ money tiles");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Checkout /donorcmds for more information.");

		return;
	}
	else if (!str_comp_nocase(Msg->m_pMessage, "/vip"))
	{
		LastChat();

		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "---------- VIP ----------");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "VIP gives you x3 money and exp.");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Use movement cmds:");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "- /up");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "- /down");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "- /left");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "- /right");

		return;
	}
	else if (!str_comp_nocase(Msg->m_pMessage, "/donorcmds"))
	{
		LastChat();

		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "---------- DONOR COMMANDS ----------");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "/tele -- Teleports you to your cursor");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "/crown -- The nice crown");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "/save -- Saves your position");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "/load -- Teleports to the saved position");
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "/home -- Teleports to your home");

		return;
	}
	if(!strncmp(Msg->m_pMessage, "/login", 6))
	{
		LastChat();
		char Username[512];
		char Password[512];
		if(sscanf(Msg->m_pMessage, "/login %s %s", Username, Password) != 2)
		{
			GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Please, use '/login <username> <password>'");
			return;
		}
		m_pPlayer->m_pAccount->Login(Username, Password);
		return;
	}
	else if(!str_comp_nocase(Msg->m_pMessage, "/acc") || !str_comp_nocase(Msg->m_pMessage, "/account"))
	{
		LastChat();

		return;
	}
	else if(!str_comp_nocase(Msg->m_pMessage, "/god"))
    {
		char aBuf[200];
		if(!GameServer()->Server()->IsAdmin(m_pPlayer->GetCID()))
		{
			GameServer()->SendChatTarget(m_pPlayer->GetCID(), "You are not admin! Access denied");
			return;
		}

		CCharacter *pOwner = GameServer()->GetPlayerChar(m_pPlayer->GetCID());

		if(GameServer()->Server()->IsAdmin(m_pPlayer->GetCID()))
		{
			pOwner->m_God^=true;
			str_format(aBuf, sizeof(aBuf), "%s Godmode", pOwner->m_God?"Enabled":"Disabled");
			GameServer()->SendChatTarget(m_pPlayer->GetCID(), aBuf);
		}

		return;
    } // Item cmds
	else if (!str_comp_nocase(Msg->m_pMessage, "/walls")) {
		CCharacter* pOwner = GameServer()->GetPlayerChar(m_pPlayer->GetCID());

		if (pOwner->GetPlayer()->m_AccData.m_HammerWalls) {
			if (pOwner->SetActiveUpgrade(WEAPON_HAMMER, 3)) 
				GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Walls Enabled");
			else
				GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Walls Disabled");
		} else
			GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Buy Walls first");

		return;
	}
	else if (!str_comp_nocase(Msg->m_pMessage, "/plasma")) {
		CCharacter* pOwner = GameServer()->GetPlayerChar(m_pPlayer->GetCID());

		if (pOwner->GetPlayer()->m_AccData.m_HammerWalls) {
			if (pOwner->SetActiveUpgrade(WEAPON_HAMMER, 2))
				GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Plasma Enabled");
			else
				GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Plasma Disabled");
		}
		else
			GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Buy Plasma first");

		return;
	}

	if(!strncmp(Msg->m_pMessage, "/", 1))
	{
		LastChat();
		GameServer()->SendChatTarget(m_pPlayer->GetCID(), "Wrong CMD, see /cmdlist");
	}

}

void CCmd::LastChat()
{
	 m_pPlayer->m_LastChat = GameServer()->Server()->Tick();
}