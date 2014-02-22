#include "main.h"
#include "CTeamspeak.h"


CTeamspeak *CTeamspeak::m_Instance = NULL;


void CTeamspeak::Connect(char *ip, char *port) 
{
	if (m_Connected == true)
		Disconnect();

	m_Socket.connect(tcp::endpoint(asio::ip::address::from_string(ip), 10011), m_Error);
	if (m_Error.value() != 0)
	{
		logprintf("plugin.TSConnector: Error while connecting to server: \"%s\"", m_Error.message().c_str());
		return ;
	}

	asio::socket_base::non_blocking_io option(true);
	m_Socket.io_control(option);

	PingTest();

	m_NetThread = new thread(boost::bind(&CTeamspeak::NetThreadFunc, this));


	CommandList *cmdlist = new CommandList; 

	string port_cmd;
	karma::generate(std::back_insert_iterator<string>(port_cmd), 
		lit("use port=") << karma::string(port)
	);
	cmdlist->push(new CCommand(port_cmd));

	PushCommandList(cmdlist);
	

	m_Connected = true;

	m_Ip.assign(ip);
	m_Port.assign(port);
}

void CTeamspeak::Disconnect() 
{
	m_Connected = false;

	m_Socket.close();

	if (m_NetThread != NULL)
	{
		m_NetThreadRunning = false;
		m_NetThread->join();
		delete m_NetThread;
		m_NetThread = NULL;
	}
}

void CTeamspeak::PingTest()
{
	asio::streambuf tmp_sbuf;
	do
	{
		this_thread::sleep_for(m_Ping);
		asio::read(m_Socket, tmp_sbuf, m_Error);
	} while (m_Error != asio::error::would_block);
	tmp_sbuf.consume(tmp_sbuf.size());
	

	asio::socket_base::non_blocking_io option_disable(false);
	m_Socket.io_control(option_disable);


	string cmd("whoami\n");
	chrono::steady_clock::time_point m_PingTimePoint(chrono::steady_clock::now());

	asio::write(m_Socket, asio::buffer(cmd), m_Error);
	asio::read_until(m_Socket, tmp_sbuf, "msg=ok", m_Error);

	m_Ping = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - m_PingTimePoint) + chrono::milliseconds(200);


	asio::socket_base::non_blocking_io option_enable(true);
	m_Socket.io_control(option_enable);
}

bool CTeamspeak::Write(string &cmd)
{
	m_Error.clear();
	cmd.push_back('\n');
	asio::write(m_Socket, asio::buffer(cmd), m_Error);
	//create a delay to give the Teamspeak server some time for a response
	this_thread::sleep_for(m_Ping);
	return (m_Error.value() == 0);
}

bool CTeamspeak::Read()
{
	m_Error.clear();
	m_ReadBuf.fill('\0');
	asio::read(m_Socket, asio::buffer(m_ReadBuf), m_Error);
	return (m_Error.value() == 0);
}


static const char CharEscapeTable[][2][5] = 
{
	{ " ", "\\s" },
	{ "\\", "\\\\" },
	{ "/", "\\/" },
	{ "|", "\\p" },
	{ "\a", "\\a" },
	{ "\b", "\\b" },
	{ "\f", "\\f" },
	{ "\n", "\\n" },
	{ "\r", "\\r" },
	{ "\t", "\\t" },
	{ "\v", "\\v" },

	{ "ä", "\xc3\xa4" },
	{ "ö", "\xc3\xb6" },
	{ "ü", "\xc3\xbc" },
	{ "Ä", "\xc3\x84" },
	{ "Ö", "\xc3\x96" },
	{ "Ü", "\xc3\x9c" }
};

void CTeamspeak::EscapeString(string &str) 
{
	if (str.length())
	{
		for(size_t i = 0; i < str.length(); ++i)
		{
			const char char_val = str.at(i);
			for (size_t ci = 0; ci < 17; ++ci)
			{
				if (CharEscapeTable[ci][0][0] == char_val)
				{
					str.replace(i++, 1, CharEscapeTable[ci][1], 2);
					break;
				}
			}
		}
	}
}

void CTeamspeak::UnEscapeString(string &str) 
{
	if (str.length() >= 2)
	{
		for (size_t ci = 0; ci < 17; ++ci)
		{
			size_t find_pos = 0;
			while ((find_pos = str.find(CharEscapeTable[ci][1])) != string::npos)
				str.replace(find_pos, 2, CharEscapeTable[ci][0]);
		}
	}
}
