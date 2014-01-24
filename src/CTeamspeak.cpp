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
