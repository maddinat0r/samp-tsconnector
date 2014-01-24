#include "main.h"
#include "CTeamspeak.h"
#include "CCallback.h"


#ifdef BOOST_REGEX_MAX_CACHE_BLOCKS
#undef BOOST_REGEX_MAX_CACHE_BLOCKS
#endif
#define BOOST_REGEX_MAX_CACHE_BLOCKS 32


#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/karma.hpp>

using boost::lexical_cast;
using namespace boost::spirit;




void CTeamspeak::NetThreadFunc()
{
	m_NetThreadRunning = true;
	while (m_NetThreadRunning) 
	{
		string empty_cmd(" ");
		if (m_Connected && Write(empty_cmd))
		{
			CommandList *commands = NULL;
			while(m_CmdListQueue.pop(commands)) 
			{
				string stored_val[9];
				while (!commands->empty()) 
				{
					CCommand *cmd = commands->front();
					commands->pop();
				
					//replace <X> with real parameters
					size_t param_pos = 0; 
					while ((param_pos = cmd->Command.find_first_of("<", param_pos)) != string::npos) 
					{
						int template_param_len = cmd->Command.find_first_of(">", param_pos) - param_pos;
						string substr = cmd->Command.substr(param_pos+1, template_param_len-1);

						unsigned int param_idx = 0;
						if(qi::parse(substr.begin(), substr.end(), qi::uint_, param_idx))
						{
							param_idx--;
							if (param_idx < (sizeof(stored_val) / sizeof(stored_val[0])) && stored_val[param_idx].length() > 0)
								cmd->Command.replace(param_pos, template_param_len + 1, stored_val[param_idx]);
						}
						else
							param_pos++;
					}


					//send and recieve data
					string result_str;

					Write(cmd->Command);
					Read();
					result_str = m_ReadBuf.c_array();


					//process parameter
					if(cmd->MFind.length() > 0) 
					{
						size_t search_pos = 0;
						if ((search_pos = result_str.find(cmd->MFind)) != string::npos) 
						{
							size_t last_delim_pos = result_str.find_first_of("|", search_pos);
							if (last_delim_pos == string::npos)
								last_delim_pos = result_str.length();

							string tmp = result_str.substr(0, last_delim_pos);
							size_t first_delim_pos = tmp.find_last_of("|");
							if (first_delim_pos == string::npos)
								first_delim_pos = 0;

							result_str = result_str.substr(first_delim_pos, last_delim_pos - first_delim_pos);
						}
					}

					if(cmd->ValName.length() > 0) 
					{
						boost::regex rx(cmd->ValName + "=([^ \n\r]+)");
						boost::match_results<std::string::const_iterator> rx_res;
						if (boost::regex_search(result_str, rx_res, rx)) 
						{
							for (unsigned int i = 0, max = sizeof(stored_val) / sizeof(stored_val[0]); i < max; ++i) 
							{
								if (stored_val[i].length() == 0) 
								{
									stored_val[i] = rx_res[1];
									break;
								}
							}
						}
					}

					if(cmd->Command.find("login") == 0)
						m_LoggedIn = true;

					delete cmd;
				}
				
				delete commands;
				commands = NULL;
			}

			if(m_LoggedIn)
			{
				RegisterNotifyEvents();
				Read();
				if(m_ReadBuf.at(0) != 0)
				{
					string data = m_ReadBuf.c_array();
					if(data.find("notify") != string::npos)
						CheckCallbacks(data);
				}
			}
				

		} //check if alive
		else
		{
			//reconnect
			m_Socket.close();
			m_Socket.connect(tcp::endpoint(asio::ip::address::from_string(m_Ip), 10011), m_Error);
			string port_cmd;
			karma::generate(std::back_insert_iterator<string>(port_cmd), 
				lit("use port=") << karma::string(m_Port)
			);
			Write(port_cmd);
			Read();
		}
		
		this_thread::sleep_for(chrono::milliseconds(20));
	}
}

void CTeamspeak::RegisterNotifyEvents()
{
	static bool run_once = false;
	if(run_once == false)
	{
		string cmd_str;
		karma::generate(std::back_inserter(cmd_str),
			lit("servernotifyregister event=channel id=0\n") <<
			lit("servernotifyregister event=textchannel\n") <<
			lit("servernotifyregister event=textserver")
		);
		Write(cmd_str);
		Read();

		UpdateInternalData();
		run_once = true;
	}
}

void CTeamspeak::UpdateInternalData()
{
	if(!m_ClientInfo.empty())
	{
		for(unordered_map<int, SClientInfo *>::iterator i = m_ClientInfo.begin(), end = m_ClientInfo.end(); i != end; ++i)
			delete i->second;
		m_ClientInfo.clear();
	}
	
	string cmd_str;
	unordered_map<int, string> channel_list;

	cmd_str = "channellist";
	Write(cmd_str);
	Read();

	boost::regex chlist_rx("cid=([^ ]+) pid=[^ ]+ channel_order=[^ ]+ channel_name=([^ ]+)");
	boost::regex_iterator<const char *> chlist_rx_iter(m_ReadBuf.c_array(), m_ReadBuf.c_array()+strlen(m_ReadBuf.c_array()), chlist_rx);
	boost::regex_iterator<const char *> chlist_rx_iter_end;
	
	for( ; chlist_rx_iter != chlist_rx_iter_end; ++chlist_rx_iter)
	{
		string channelname = (*chlist_rx_iter)[2];
		UnEscapeString(channelname);
		int channelid = boost::lexical_cast<int>((*chlist_rx_iter)[1]);

		channel_list.insert( unordered_map<int, string>::value_type(channelid, channelname) );
	}


	cmd_str = "clientlist -uid";
	Write(cmd_str);
	Read();

	boost::regex cllist_rx("clid=([^ ]+) cid=([^ ]+) client_database_id=[^ ]+ client_nickname=[^ ]+ client_type=0 client_unique_identifier=([^=]+)");
	boost::regex_iterator<const char *> cllist_rx_iter(m_ReadBuf.c_array(), m_ReadBuf.c_array()+strlen(m_ReadBuf.c_array()), cllist_rx);
	boost::regex_iterator<const char *> cllist_rx_iter_end;
				
	for( ; cllist_rx_iter != cllist_rx_iter_end; ++cllist_rx_iter) 
	{
		string uid = (*cllist_rx_iter)[3];
		UnEscapeString(uid);
		uid.push_back('=');
		int 
			channelid = boost::lexical_cast<int>((*cllist_rx_iter)[2]), 
			clientid = boost::lexical_cast<int>((*cllist_rx_iter)[1]);

		SClientInfo *Client = new SClientInfo;
		Client->UID = uid;
		if(channel_list.find(channelid) != channel_list.end())
			Client->LastChannelName = channel_list.at(channelid);
		m_ClientInfo.insert( unordered_map<int, SClientInfo*>::value_type(clientid, Client) );
	}
}


void CTeamspeak::OnClientConnect(int channelid, int clientid, string uid, string nickname)
{
	UnEscapeString(uid);
	UnEscapeString(nickname);


	string cmd_str;
	karma::generate(std::back_inserter(cmd_str),
		lit("channelinfo cid=") << karma::int_(channelid)
	);
	Write(cmd_str);
	Read();

	boost::regex chname_rx("channel_name=([^ ]+)");
	boost::match_results<const char *> chname_rx_res;
	if(boost::regex_search(m_ReadBuf.c_array(), chname_rx_res, chname_rx)) 
	{
		SClientInfo *Client = new SClientInfo;
		Client->LastChannelName = chname_rx_res[1];
		UnEscapeString(Client->LastChannelName);
		Client->UID = uid;
		m_ClientInfo.insert( unordered_map<int, SClientInfo*>::value_type(clientid, Client) );
	}

	
	CCallback *Callback = new CCallback;
	Callback->Name = "TSC_OnClientConnect";
	Callback->Params.push(nickname);
	Callback->Params.push(uid);
	CCallback::Queue(Callback);
}

void CTeamspeak::OnClientDisconnect(int clientid)
{
	if(m_ClientInfo.find(clientid) != m_ClientInfo.end()) 
	{
		SClientInfo *client = m_ClientInfo.at(clientid);
		m_ClientInfo.erase(clientid);


		string cmd_str;
		string nickname;
		karma::generate(std::back_inserter(cmd_str),
			lit("clientgetnamefromuid cluid=") << karma::string(client->UID)
		);
		Write(cmd_str);
		Read();


		boost::regex subrx("name=([^ \n\r]+)");
		boost::match_results<const char *> subrx_res;
		if(boost::regex_search(m_ReadBuf.c_array(), subrx_res, subrx)) 
		{
			string nickname = subrx_res[1];
			UnEscapeString(nickname);

			CCallback *callback = new CCallback;
			callback->Name = "TSC_OnClientDisconnect";
			callback->Params.push(nickname);
			callback->Params.push(client->UID);
			CCallback::Queue(callback);
		}

		delete client;
	}
}

void CTeamspeak::OnClientServerText(string nickname, string uid, string msg)
{
	if(!nickname.empty() && !uid.empty() && !msg.empty())
	{
		UnEscapeString(nickname);
		UnEscapeString(uid);
		UnEscapeString(msg);
		
		CCallback *callback = new CCallback;
		callback->Name = "TSC_OnClientServerText";
		callback->Params.push(nickname);
		callback->Params.push(uid);
		callback->Params.push(msg);
		CCallback::Queue(callback);
	}
}

void CTeamspeak::OnClientChannelText(int clientid, string nickname, string uid, string msg)
{
	if(m_ClientInfo.find(clientid) != m_ClientInfo.end() && !nickname.empty() && !uid.empty() && !msg.empty()) 
	{
		UnEscapeString(nickname);
		UnEscapeString(uid);
		UnEscapeString(msg);
		
		SClientInfo *client = m_ClientInfo.at(clientid);
		CCallback *callback = new CCallback;
		callback->Name = "TSC_OnClientChannelText";
		callback->Params.push(nickname);
		callback->Params.push(uid);
		callback->Params.push(client->LastChannelName);
		callback->Params.push(msg);
		CCallback::Queue(callback);
	}
}

void CTeamspeak::OnClientChannelMove(int clientid, int channelid)
{
	string cmd_str;

	karma::generate(std::back_inserter(cmd_str),
		lit("channelinfo cid=") << karma::int_(channelid) << lit('\n') <<
		lit("clientinfo clid=") << karma::int_(clientid)
	);
	Write(cmd_str);
	Read();

	
	boost::regex chname_rx("channel_name=([^ ]+)");
	boost::match_results<const char *> chname_rx_res;
	if(boost::regex_search(m_ReadBuf.c_array(), chname_rx_res, chname_rx)) 
	{
		string channelname = chname_rx_res[1];
		UnEscapeString(channelname);

		boost::regex nickname_rx("client_nickname=([^ ]+)");
		boost::match_results<const char *> nickname_rx_res;
		if(boost::regex_search(m_ReadBuf.c_array(), nickname_rx_res, nickname_rx)) 
		{
			string nickname = nickname_rx_res[1];
			UnEscapeString(nickname);
			

			if(m_ClientInfo.find(clientid) != m_ClientInfo.end() && !channelname.empty() && !nickname.empty()) 
			{
				SClientInfo *Client = m_ClientInfo.at(clientid);
				Client->LastChannelName = channelname;

				CCallback *Callback = new CCallback;
				Callback->Name = "TSC_OnClientChannelMove";
				Callback->Params.push(nickname);
				Callback->Params.push(Client->UID);
				Callback->Params.push(channelname);
				CCallback::Queue(Callback);
			}
		}
	}
}

void CTeamspeak::CheckCallbacks(string &str)
{
	boost::match_results<std::string::const_iterator> rx_res;
	if(str.find("notifycliententerview") != string::npos) 
	{
		boost::regex rx("notifycliententerview cfid=[^ ]+ ctid=([^ ]+) reasonid=[^ ]+ clid=([^ ]+) client_unique_identifier=([^ ]+) client_nickname=([^ ]+)");
		if(boost::regex_search(str, rx_res, rx)) 
			OnClientConnect(lexical_cast<int>(rx_res[1]), lexical_cast<int>(rx_res[2]), rx_res[3], rx_res[4]);
	}
	else if(str.find("notifyclientleftview") != string::npos) 
	{
		boost::regex rx("notifyclientleftview.*clid=([^ \n\r]+)");
		if(boost::regex_search(str, rx_res, rx)) 
			OnClientDisconnect(lexical_cast<int>(rx_res[1]));
	}
	else if(str.find("notifytextmessage") != string::npos) 
	{
		boost::regex rx("notifytextmessage targetmode=([^ ]+) msg=([^ ]+) invokerid=([^ ]+) invokername=([^ ]+) invokeruid=([^ \n\r]+)");
		if(boost::regex_search(str, rx_res, rx)) 
		{
			switch(lexical_cast<int>(rx_res[1]))
			{
			case 3:
				OnClientServerText(rx_res[4], rx_res[5], rx_res[2]);
				break;

			case 2:
				OnClientChannelText(lexical_cast<int>(rx_res[3]), rx_res[4], rx_res[5], rx_res[2]);
				break;
			}
		}
	}
	else if(str.find("notifyclientmoved") != string::npos) 
	{
		boost::regex rx("notifyclientmoved ctid=([^ ]+).*clid=([^ \n\r]+)");
		if(boost::regex_search(str, rx_res, rx)) 
			OnClientChannelMove(lexical_cast<int>(rx_res[2]), lexical_cast<int>(rx_res[1]));
	}
}