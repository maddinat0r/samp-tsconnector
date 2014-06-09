#pragma once
#ifndef INC_CTEAMSPEAK_H
#define INC_CTEAMSPEAK_H


#include <queue>
#include <string>
#include <boost/thread/thread.hpp>
#include <boost/asio.hpp>

#include "CSingleton.h"

using std::queue;
using std::string;
using boost::thread;
namespace asio = boost::asio;
using asio::ip::tcp;


struct CCommand
{
	CCommand(string &cmd) : Command(cmd) {}

	string Command;
	string KeySearch;
};
typedef queue<CCommand *> CommandList;


class CTeamspeak : public CSingleton<CTeamspeak>
{
	friend class CSingleton < CTeamspeak > ;
private: //variables
	asio::io_service m_IoService;
	tcp::socket m_Socket;
	thread *m_IoThread;
	asio::streambuf m_ReadStreamBuf;

	boost::mutex m_CmdQueueMutex;
	queue<CommandList *> m_CmdQueue;

	string m_CmdWriteBuffer;


private: //constructor / deconstructor
	CTeamspeak() :
		m_IoThread(NULL),

		m_Socket(m_IoService)
	{}

	~CTeamspeak()
	{
		Disconnect();
	}


public: //functions
	void Connect(char *ip, char *port);
	void Disconnect();

	void EscapeString(string &str);
	void UnEscapeString(string &str);

	void Execute(CommandList *cmds);


private: //handlers
	void OnConnect(const boost::system::error_code &error_code, string port);
	void OnRead(const boost::system::error_code &error_code);
	void OnWrite(const boost::system::error_code &error_code);


private: //functions
	void AsyncRead();
	void AsyncWrite(string &data);

};


enum E_CHANNEL_TYPES
{
	CHANNEL_TYPE_PERMANENT = 1,
	CHANNEL_TYPE_SEMI_PERMANENT,
	CHANNEL_TYPE_TEMPORARY
};

enum E_KICK_TYPES 
{
	KICK_TYPE_CHANNEL = 1,
	KICK_TYPE_SERVER
};


#endif // INC_CTEAMSPEAK_H
