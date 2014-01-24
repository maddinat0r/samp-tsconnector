#pragma once
#ifndef INC_CTEAMSPEAK_H
#define INC_CTEAMSPEAK_H

#define _SCL_SECURE_NO_WARNINGS


#include "main.h"

#include <queue>
#include <string>
#include <boost/unordered_map.hpp>
#include <boost/atomic.hpp>
#include <boost/thread/thread.hpp>
#include <boost/asio.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/karma.hpp>
#include <boost/lockfree/spsc_queue.hpp>
#include <boost/chrono.hpp>

using std::queue;
using std::string;
using boost::unordered_map;
using boost::atomic;
using boost::thread;
namespace this_thread = boost::this_thread;
namespace asio = boost::asio;
using asio::ip::tcp;
using namespace boost::spirit;
namespace chrono = boost::chrono;



struct CCommand 
{
	CCommand(string cmd, string valname=string()) :
		Command(cmd), ValName(valname) {}
	
	string Command;
	//recv only
	string ValName;

	//multiple search
	string MFind;
};


typedef queue<CCommand*> CommandList;


struct SClientInfo 
{
	string UID;
	string LastChannelName;
};


class CTeamspeak 
{
private: //singleton
	static CTeamspeak *m_Instance;

public: //singleton
	static inline CTeamspeak *Get()
	{
		if (m_Instance == NULL)
			m_Instance = new CTeamspeak;
		return m_Instance;
	}
	inline void Destroy()
	{
		Disconnect();
		m_Instance = NULL;
		delete this;
	}

private: //variables

	asio::io_service m_NetService;
	tcp::socket
		m_Socket;

	boost::array<char, 4096U> m_ReadBuf;
	boost::system::error_code m_Error;

	thread *m_NetThread;
	atomic<bool> m_NetThreadRunning;

	bool m_Connected;
	bool m_LoggedIn;
	
	
	boost::lockfree::spsc_queue<
		CommandList *,
		boost::lockfree::fixed_sized<true>,
		boost::lockfree::capacity<32678>
	> m_CmdListQueue;


	//internal data storage
	unordered_map<int, SClientInfo*> m_ClientInfo;
	string
		m_Ip,
		m_Port;
	

private: //functions

	CTeamspeak() :
		m_NetService(),
		m_Socket(m_NetService),

		m_NetThread(NULL),
		m_NetThreadRunning(true),

		m_Connected(false),
		m_LoggedIn(false)
	{ }

	~CTeamspeak()
	{ }
	
	void NetThreadFunc();

	void RegisterNotifyEvents();
	void UpdateInternalData();
	void CheckCallbacks(string &str);

	void OnClientConnect(int channelid, int clientid, string uid, string nickname);
	void OnClientDisconnect(int clientid);
	void OnClientServerText(string nickname, string uid, string msg);
	void OnClientChannelText(int clientid, string nickname, string uid, string msg);
	void OnClientChannelMove(int clientid, int channelid);


	inline bool Write(string &cmd)
	{
		m_Error.clear();
		cmd.push_back('\n');
		asio::write(m_Socket, asio::buffer(cmd), m_Error);
		//create a delay to give the Teamspeak server some time for a response
		this_thread::sleep_for(chrono::milliseconds(70));
		return (m_Error.value() == 0);
	}
	inline bool Read()
	{
		m_Error.clear();
		m_ReadBuf.fill('\0');
		asio::read(m_Socket, asio::buffer(m_ReadBuf), m_Error);
		return (m_Error.value() == 0);
	}

public: //functions

	void Connect(char *ip, char *port);
	void Disconnect();
	
	inline void PushCommandList(CommandList *cmdlist)
	{
		m_CmdListQueue.push(cmdlist);
	}

	void EscapeString(string &str);
	void UnEscapeString(string &str);

	inline string GetPort() const
	{
		return m_Port;
	}
};


enum ChannelTypes {
	CHANNEL_TYPE_PERMANENT = 1,
	CHANNEL_TYPE_SEMI_PERMANENT,
	CHANNEL_TYPE_TEMPORARY
};

enum KickTypes {
	KICK_TYPE_CHANNEL = 1,
	KICK_TYPE_SERVER
};


#endif // INC_CTEAMSPEAK_H
