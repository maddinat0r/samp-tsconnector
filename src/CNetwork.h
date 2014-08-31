#pragma once
#ifndef INC_CNETWORK_H
#define INC_CNETWORK_H


#include <vector>
#include <queue>
#include <string>
#include <list>
#include <boost/thread/thread.hpp>
#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/regex.hpp>
#include <boost/atomic.hpp>

#include "format.h"

#include "CSingleton.h"

using std::vector;
using std::queue;
using std::string;
using std::list;
using boost::thread;
namespace asio = boost::asio;
using asio::ip::tcp;
using boost::tuple;
using boost::atomic;



class CNetwork : public CSingleton<CNetwork>
{
	friend class CSingleton<CNetwork>;
public: //definitions
	typedef vector<string> ResultSet_t;
	typedef boost::function<void(ResultSet_t &)> ReadCallback_t;
	typedef tuple<string, ReadCallback_t> CmdTuple_t;

	typedef boost::function<void(boost::smatch &result)> EventCallback_t;
	typedef tuple<boost::regex, EventCallback_t> EventTuple_t;

private: //variables
	asio::io_service m_IoService;
	thread *m_IoThread = nullptr;

	tcp::socket m_Socket;
	tcp::endpoint m_SocketDest;
	atomic<bool> m_Connected;
	unsigned short m_ServerPort = 9987;

	asio::deadline_timer m_AliveTimer;

	asio::streambuf m_ReadStreamBuf;
	string m_CmdWriteBuffer;

	boost::mutex m_CmdQueueMutex;
	queue<CmdTuple_t> m_CmdQueue;

	vector<EventTuple_t> m_EventList;


private: //constructor / deconstructor
	CNetwork() :
		m_Socket(m_IoService),
		m_Connected(false),
		m_AliveTimer(m_IoService)
	{
		NetAlive(boost::system::error_code(), false);
	}

	~CNetwork()
	{
		Disconnect();
	}


public: //functions
	bool Connect(string hostname, unsigned short port, unsigned short query_port = 10011);
	bool Disconnect();

	inline unsigned short GetServerPort() const
	{
		return m_ServerPort;
	}
	inline bool IsConnected() const
	{
		return m_Connected;
	}

	void Execute(string cmd, ReadCallback_t callback = ReadCallback_t());

	inline void RegisterEvent(boost::regex &&event_rx, EventCallback_t &&callback)
	{
		m_EventList.push_back(boost::make_tuple(event_rx, callback));
	}


private: //handlers
	void OnConnect(const boost::system::error_code &error_code);
	void OnRead(const boost::system::error_code &error_code);
	void OnWrite(const boost::system::error_code &error_code);

	void NetAlive(const boost::system::error_code &error_code, bool from_write);

private: //functions
	void AsyncRead();
	void AsyncWrite(string &data);
	void AsyncConnect();

};


#endif // INC_CNETWORK_H
