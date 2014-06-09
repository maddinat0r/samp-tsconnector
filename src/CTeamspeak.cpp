#include "main.h"
#include "CTeamspeak.h"

#include <istream>

#include <boost/regex.hpp>
#include "format.h"


void CTeamspeak::Connect(char *ip, char *port)
{
	m_Socket.async_connect(tcp::endpoint(asio::ip::address::from_string(ip), 10011), boost::bind(&CTeamspeak::OnConnect, this, _1, string(port)));
	m_IoThread = new thread(boost::bind(&asio::io_service::run, boost::ref(m_IoService)));


	CommandList *cmds = new CommandList;
	cmds->push(new CCommand(str(fmt::Writer() << "use port=" << port)));
	Execute(cmds);
}

void CTeamspeak::Disconnect()
{
	m_Socket.close();
	m_IoService.stop();

	m_IoThread->join();
	delete m_IoThread;
}

void CTeamspeak::AsyncRead()
{
	asio::async_read_until(m_Socket, m_ReadStreamBuf, '\r', boost::bind(&CTeamspeak::OnRead, this, _1));
}

void CTeamspeak::AsyncWrite(string &data)
{
	m_CmdWriteBuffer = data;
	if (data.at(data.length()-1) != '\n')
		m_CmdWriteBuffer.push_back('\n');

	m_Socket.async_send(asio::buffer(m_CmdWriteBuffer), boost::bind(&CTeamspeak::OnWrite, this, _1));
}

void CTeamspeak::OnConnect(const boost::system::error_code &error_code, string port)
{
	if (error_code.value() == 0)
	{
		AsyncRead();
	}
	else
	{
		logprintf("plugin.TSConnector: Error while connecting to server: \"%s\"", error_code.message().c_str());
		Disconnect();
	}
}

void CTeamspeak::OnRead(const boost::system::error_code &error_code)
{
	if (error_code.value() == 0)
	{
		std::istream tmp_stream(&m_ReadStreamBuf);
		string read_data;
		std::getline(tmp_stream, read_data, '\r');

		//TODO: process data 'read_data'

		//regex: parse error
		static const boost::regex error_rx("error id=([0-9]+) msg=([^ \n]+)");
		boost::cmatch error_rx_result;
		if (boost::regex_search(read_data.c_str(), error_rx_result, error_rx))
		{
			boost::lock_guard<boost::mutex> queue_lock_guard(m_CmdQueueMutex);
			if (error_rx_result[1].str() == "0")
			{
				if (m_CmdQueue.empty() == false)
				{
					CommandList *cmd_list = m_CmdQueue.front();
					if (cmd_list->empty() == false)
					{
						cmd_list->pop();
						if (cmd_list->empty() == false)
							AsyncWrite(cmd_list->front()->Command);
						else
						{
							delete cmd_list;
							m_CmdQueue.pop();

							if (m_CmdQueue.empty() == false)
								AsyncWrite(m_CmdQueue.front()->front()->Command);
						}
					}
				}
			}
			else
			{
				string error_str(error_rx_result[2].str());
				UnEscapeString(error_str);

				logprintf(">> plugin.TSConnector: Error while executing \"%s\": %s (#%s)", m_CmdQueue.front()->front()->Command.c_str(), error_str.c_str(), error_rx_result[1].str().c_str());
			}
		}
	}
	else
		logprintf(">> plugin.TSConnector: Error while reading: %s (#%d)", error_code.message().c_str(), error_code.value());
	
	AsyncRead();
}

void CTeamspeak::OnWrite(const boost::system::error_code &error_code)
{
	m_CmdWriteBuffer.clear();
}

void CTeamspeak::Execute(CommandList *cmds)
{
	boost::lock_guard<boost::mutex> queue_lock_guard(m_CmdQueueMutex);
	m_CmdQueue.push(cmds);
	if (m_CmdQueue.size() == 1)
		AsyncWrite(cmds->front()->Command);
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
