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



class CNetwork : public CSingleton<CNetwork>
{
	friend class CSingleton<CNetwork>;
public: //definitions
	typedef vector<string> ResultSet_t;
	typedef boost::function<void(ResultSet_t &)> ReadCallback_t;
	typedef tuple<string, ReadCallback_t> CmdTuple_t;

private: //variables
	asio::io_service m_IoService;
	thread *m_IoThread;

	tcp::socket m_Socket;
	tcp::endpoint m_SocketDest;
	unsigned short m_ServerPort;

	asio::streambuf m_ReadStreamBuf;
	string m_CmdWriteBuffer;

	boost::mutex m_CmdQueueMutex;
	queue<CmdTuple_t> m_CmdQueue;


private: //constructor / deconstructor
	CNetwork() :
		m_IoThread(NULL),

		m_Socket(m_IoService)
	{}

	~CNetwork()
	{
		Disconnect();
	}


public: //functions
	void Connect(char *ip, unsigned short port, unsigned short query_port = 10011);
	void Disconnect();

	void EscapeString(string &str);
	void UnEscapeString(string &str);

	void Execute(fmt::Writer &cmd_writer, ReadCallback_t callback = ReadCallback_t());


private: //handlers
	void OnConnect(const boost::system::error_code &error_code);
	void OnRead(const boost::system::error_code &error_code);
	void OnWrite(const boost::system::error_code &error_code);


private: //functions
	void AsyncRead();
	void AsyncWrite(string &data);
	void AsyncConnect();

};


#endif // INC_CNETWORK_H
