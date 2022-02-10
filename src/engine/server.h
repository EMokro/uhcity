/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef ENGINE_SERVER_H
#define ENGINE_SERVER_H
#include "kernel.h"
#include "message.h"

#include <engine/shared/protocol.h>
#include <game/generated/protocol.h>

enum
{
	CHATCATEGORY_DEFAULT=0,
	CHATCATEGORY_INFO,
	CHATCATEGORY_JOIN,
	CHATCATEGORY_GOOD,
};

class IServer : public IInterface
{
	MACRO_INTERFACE("server", 0)
protected:
	int m_CurrentGameTick;
	int m_TickSpeed;

public:
	class CLocalization* m_pLocalization;

public:
	/*
		Structure: CClientInfo
	*/
	struct CClientInfo
	{
		const char *m_pName;
		int m_Latency;

		int m_Client;
	};

	enum
	{
		CLIENT_VANILLA = 0,
		CLIENT_DDNET,
		CLIENT_KCLIENT,
		CLIENT_CUSTOM, // some custom client with 64p which we don't know
		NUM_CLIENT_TYPES,
	};

	virtual ~IServer() {};

	inline class CLocalization* Localization() { return m_pLocalization; }

	int Tick() const { return m_CurrentGameTick; }
	int TickSpeed() const { return m_TickSpeed; }

	virtual const char *ClientName(int ClientID) = 0;
	virtual const char *ClientClan(int ClientID) = 0;
	virtual int ClientCountry(int ClientID) = 0;
	virtual int ClientIdByName(const char* Name) = 0;
	virtual bool ClientIngame(int ClientID) = 0;
	virtual int GetClientInfo(int ClientID, CClientInfo *pInfo) = 0;
	virtual void GetClientAddr(int ClientID, NETADDR *pAddr) = 0;
	virtual void GetClientAddr(int ClientID, char *pAddrStr, int Size) = 0;

	virtual int SendMsg(CMsgPacker *pMsg, int Flags, int ClientID) = 0;

	template<class T>
	int SendPackMsg(T *pMsg, int Flags, int ClientID)
	{
		int Result = 0;
		T Tmp;
		if (ClientID == -1)
		{
			for (int i = 0; i < MAX_CLIENTS; i++)
			{
				if (ClientIngame(i))
				{
					mem_copy(&Tmp, pMsg, sizeof(T));
					Result = SendPackMsgTranslate(&Tmp, Flags, i);
				}
			}
		}
		else
		{
			mem_copy(&Tmp, pMsg, sizeof(T));
			Result = SendPackMsgTranslate(&Tmp, Flags, ClientID);
		}

		return Result;
	}

	template<class T>
	int SendPackMsgTranslate(T* pMsg, int Flags, int ClientID)
	{
		return SendPackMsgOne(pMsg, Flags, ClientID);
	}

	int SendPackMsgTranslate(CNetMsg_Sv_Emoticon* pMsg, int Flags, int ClientID)
	{
		return Translate(pMsg->m_ClientID, ClientID) && SendPackMsgOne(pMsg, Flags, ClientID);
	}

	char aMsgBuf[1024];
	int SendPackMsgTranslate(CNetMsg_Sv_Chat* pMsg, int Flags, int ClientID)
	{
		if (pMsg->m_ClientID >= 0 && !Translate(pMsg->m_ClientID, ClientID))
		{
			str_format(aMsgBuf, sizeof(aMsgBuf), "%s: %s", ClientName(pMsg->m_ClientID), pMsg->m_pMessage);
			pMsg->m_pMessage = aMsgBuf;
			pMsg->m_ClientID = VANILLA_MAX_CLIENTS - 1;
		}

		return SendPackMsgOne(pMsg, Flags, ClientID);
	}

	int SendPackMsgTranslate(CNetMsg_Sv_KillMsg* pMsg, int Flags, int ClientID)
	{
		if (!Translate(pMsg->m_Victim, ClientID)) return 0;
		if (!Translate(pMsg->m_Killer, ClientID)) pMsg->m_Killer = pMsg->m_Victim;
		return SendPackMsgOne(pMsg, Flags, ClientID);
	}

	template<class T>
	int SendPackMsgOne(T* pMsg, int Flags, int ClientID)
	{
		dbg_assert(ClientID != -1, "SendPackMsgOne called with -1");

		CMsgPacker Packer(pMsg->MsgID());
		if (pMsg->Pack(&Packer))
			return -1;

		return SendMsg(&Packer, Flags, ClientID);
	}

	bool Translate(int& Target, int ClientID)
	{
		CClientInfo Info;
		GetClientInfo(ClientID, &Info);
		if (Info.m_Client != CLIENT_VANILLA)
			return true;

		int* pMap = GetIdMap(ClientID);
		bool Found = false;
		for (int i = 0; i < VANILLA_MAX_CLIENTS; i++)
		{
			if (Target == pMap[i])
			{
				Target = i;
				Found = true;
				break;
			}
		}

		return Found;
	}

	bool ReverseTranslate(int& Target, int ClientID)
	{
		CClientInfo Info;
		GetClientInfo(ClientID, &Info);
		if (Info.m_Client != CLIENT_VANILLA)
			return true;

		int* pMap = GetIdMap(ClientID);
		if (pMap[Target] == -1)
			return false;

		Target = pMap[Target];
		return true;
	}

	virtual void SetClientName(int ClientID, char const *pName) = 0;
	virtual void SetClientClan(int ClientID, char const *pClan) = 0;
	virtual void SetClientCountry(int ClientID, int Country) = 0;
	virtual void SetClientScore(int ClientID, int Score) = 0;
	virtual void Logout(int ClientID) = 0;
	virtual void Police(int ClientID,int Switch) = 0;
	virtual void SetRconlvl(int ClientID,int Level) = 0;
	virtual void SetClientAccID(int ClientID, int AccID) = 0;

	virtual int SnapNewID() = 0;
	virtual void SnapFreeID(int ID) = 0;
	virtual void *SnapNewItem(int Type, int ID, int Size) = 0;

	virtual void SnapSetStaticsize(int ItemType, int Size) = 0;

	//KlickFoots stuff
	virtual bool IsAdmin(int ClientID) = 0;
	virtual bool IsMod(int ClientID) = 0;
	virtual bool IsMapper(int ClientID) = 0;
	virtual bool IsPolice(int ClientID) = 0;

	//Normales zeugs
	virtual bool IsAuthed(int ClientID) = 0;
	virtual int AuthLvl(int ClientID) = 0;
	virtual void Kick(int ClientID, const char *pReason) = 0;

	// 64 clients
	virtual int* GetIdMap(int ClientID) = 0;
	virtual void SetClient(int ClientID, int Client) = 0;
	virtual bool IsClient64(int ClientID) = 0;

	virtual void DemoRecorder_HandleAutoStart() = 0;
	
	// Localization system
	virtual const char* GetClientLanguage(int ClientID) = 0;
	virtual void SetClientLanguage(int ClientID, const char* pLanguage) = 0;
};

class IGameServer : public IInterface
{
	MACRO_INTERFACE("gameserver", 0)
protected:
public:
	virtual void OnInit() = 0;
	virtual void OnConsoleInit() = 0;
	virtual void OnShutdown() = 0;

	virtual void OnTick() = 0;
	virtual void OnPreSnap() = 0;
	virtual void OnSnap(int ClientID) = 0;
	virtual void OnPostSnap() = 0;

	virtual void OnMessage(int MsgID, CUnpacker *pUnpacker, int ClientID) = 0;

	virtual void OnClientConnected(int ClientID) = 0;
	virtual void OnClientEnter(int ClientID) = 0;
	virtual void OnClientDrop(int ClientID, const char *pReason) = 0;
	virtual void OnClientDirectInput(int ClientID, void *pInput) = 0;
	virtual void OnClientPredictedInput(int ClientID, void *pInput) = 0;

	virtual bool IsClientReady(int ClientID) = 0;
	virtual bool IsClientPlayer(int ClientID) = 0;

	virtual const char *GameType() = 0;
	virtual const char *Version() = 0;
	virtual const char *NetVersion() = 0;

	virtual void SendBroadcast_Localization(int To, int Priority, int LifeSpan, const char* pText, ...) = 0;
	virtual void SendBroadcast_Localization_P(int To, int Priority, int LifeSpan, int Number, const char* pText, ...) = 0;
	virtual void SendChatTarget(int To, const char* pText) = 0;
	virtual void SendChatTarget_Localization(int To, int Category, const char* pText, ...) = 0;
	virtual void SendChatTarget_Localization_P(int To, int Category, int Number, const char* pText, ...) = 0;
};

extern IGameServer *CreateGameServer();
#endif
