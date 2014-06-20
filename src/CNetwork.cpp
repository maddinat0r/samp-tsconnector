#include "main.h"
#include "CNetwork.h"
#include "CServer.h"

#include <istream>

#include <boost/regex.hpp>
#include "format.h"


void CNetwork::Connect(char *ip, unsigned short port, unsigned short query_port)
{
	m_SocketDest = tcp::endpoint(asio::ip::address::from_string(ip), query_port);
	m_ServerPort = port;
	AsyncConnect();
	m_IoThread = new thread(boost::bind(&asio::io_service::run, boost::ref(m_IoService)));
}

void CNetwork::Disconnect()
{
	m_Socket.close();
	m_IoService.stop();
	
	m_IoThread->join();
	delete m_IoThread;
}

void CNetwork::AsyncRead()
{
	asio::async_read_until(m_Socket, m_ReadStreamBuf, '\r', boost::bind(&CNetwork::OnRead, this, _1));
}

void CNetwork::AsyncWrite(string &data)
{
	m_CmdWriteBuffer = data;
	if (data.at(data.length()-1) != '\n')
		m_CmdWriteBuffer.push_back('\n');

	m_Socket.async_send(asio::buffer(m_CmdWriteBuffer), boost::bind(&CNetwork::OnWrite, this, _1));
}

void CNetwork::AsyncConnect()
{
	if (m_Socket.is_open())
		m_Socket.close();

	m_Socket.async_connect(m_SocketDest, boost::bind(&CNetwork::OnConnect, this, _1));
}

void CNetwork::OnConnect(const boost::system::error_code &error_code)
{
	if (error_code.value() == 0)
	{
		AsyncRead();

		/*CCommands *cmds = CCommands::Create();
		cmds->AddCommand(fmt::Writer() << "use port=" << m_ServerPort);
		Execute(cmds);*/
		Execute(fmt::Writer() << "use port=" << m_ServerPort, 
			boost::bind(&CServer::OnPortSet, CServer::Get(), _1));
	}
	else
	{
		logprintf("plugin.TSConnector: Error while connecting to server: \"%s\"", error_code.message().c_str());
		Disconnect();
	}
}


/*
	- result data is sent as a string which ends with "\n\r"
	- the Teamspeak3 server can send multiple strings
	- the end of a result set is always an error result string
*/
void CNetwork::OnRead(const boost::system::error_code &error_code)
{
	if (error_code.value() == 0)
	{
		static vector<string> captured_data;
		std::istream tmp_stream(&m_ReadStreamBuf);
		string read_data;
		std::getline(tmp_stream, read_data, '\r');

		logprintf(">>> %s", read_data.c_str());

		//regex: parse error
		//if this is an error message, it means that no other result data will come
		static const boost::regex error_rx("error id=([0-9]+) msg=([^ \n]+)");
		boost::smatch error_rx_result;
		if (boost::regex_match(read_data, error_rx_result, error_rx))
		{
			if (error_rx_result[1].str() == "0")
			{
				for (vector<string>::iterator i, end = captured_data.end(); i != end; ++i)
				{
					string &data = *i;
					if (data.find('|') != string::npos) //multiple data rows with '|' as delimiter
					{
						vector<string> result_set;
						size_t delim_pos = 0;
						do
						{
							string row = data.substr(delim_pos, (delim_pos = data.find('|', delim_pos)));
							result_set.push_back(row);
						} while (delim_pos != string::npos);

						captured_data.insert(i, result_set.begin(), result_set.end());
					}
				}
				
				//call callback and send next command
				boost::lock_guard<boost::mutex> queue_lock_guard(m_CmdQueueMutex);
				if (m_CmdQueue.empty() == false)
				{
					m_CmdQueue.front().get<1>()(captured_data); //calls the callback
					m_CmdQueue.pop();

					if (m_CmdQueue.empty() == false)
						AsyncWrite(m_CmdQueue.front().get<0>());
				}
			}
			else
			{
				string error_str(error_rx_result[2].str());
				UnEscapeString(error_str);

				//TODO: print sent command in error message
				logprintf(">> plugin.TSConnector: Error while executing \"%%s\": %s (#%s)", /*m_CmdQueue.front()->Front()->Command.c_str(),*/ error_str.c_str(), error_rx_result[1].str().c_str());
			}

			captured_data.clear();
		}
		else
		{
			//stack the result data if it is not an error message
			captured_data.push_back(read_data);
		}

		AsyncRead();
	}
	else if (error_code == asio::error::eof)
	{
		AsyncConnect();
	}
	else
		logprintf(">> plugin.TSConnector: Error while reading: %s (#%d)", error_code.message().c_str(), error_code.value());
}

void CNetwork::OnWrite(const boost::system::error_code &error_code)
{
	logprintf("<<< %s", m_CmdWriteBuffer.c_str());
	m_CmdWriteBuffer.clear();
	if (error_code.value() != 0)
		logprintf(">> plugin.TSConnector: Error while writing: %s (#%d)", error_code.message().c_str(), error_code.value());
}

void CNetwork::Execute(fmt::Writer &cmd_writer, ReadCallback_t callback)
{
	boost::lock_guard<boost::mutex> queue_lock_guard(m_CmdQueueMutex);
	m_CmdQueue.push(boost::make_tuple(fmt::str(cmd_writer), boost::move(callback)));
	if (m_CmdQueue.size() == 1)
		AsyncWrite(m_CmdQueue.front().get<0>());
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

void CNetwork::EscapeString(string &str) 
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

void CNetwork::UnEscapeString(string &str) 
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
