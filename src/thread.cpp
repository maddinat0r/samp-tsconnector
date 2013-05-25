#include "main.h"
#include "thread.h"

#include "CTeamspeak.h"

#include <map>

using std::map;

bool ThreadAlive = true;
bool NotifyEventSended = false;

struct SClientInfo {
	string UID;
	string LastChannelName;
};

#ifdef _WIN32
DWORD __stdcall SocketThread(LPVOID lpParam)
#else
void * SocketThread(void *lpParam)
#endif

{
	map<int, SClientInfo*> ClientInfo;

	while(ThreadAlive) {
		
		while(!TSServer.IsQueueEmpty()) {
			CommandList *Commands = TSServer.GetNextCommandList();
			
			string MyVal[9];
			while(!Commands->empty()) {
				string RecvStr, RecvStrTmp;
				CCommand *Cmd = Commands->front();
				Commands->pop();
				
				//replace <X> with real parameters
				size_t ParamPos = 0;
				while((ParamPos = Cmd->Command.find_first_of("<", ParamPos)) != -1) {
					int TemplateParamLen = Cmd->Command.find_first_of(">", ParamPos)-ParamPos;
					unsigned int ParamIndex = 0;
					string substr = Cmd->Command.substr(ParamPos+1, TemplateParamLen-1);
					stringstream ConvBuf (substr);
					ConvBuf >> ParamIndex;
					ParamIndex--;
					if(ParamIndex < (sizeof(MyVal)/sizeof(MyVal[0])) && MyVal[ParamIndex].length() > 0)
						Cmd->Command.replace(ParamPos, TemplateParamLen+1, MyVal[ParamIndex]);
					else
						printf("[ERROR] An error occured in Teamspeak Connector while processing a parameter.\n");
				}

				//send and recieve data
				TSServer.Send(Cmd->Command);
				while(TSServer.Recv(&RecvStrTmp) != SOCKET_ERROR) {
					RecvStr.append(RecvStrTmp);
					RecvStrTmp.clear();
				}

				//process parameter
				if(Cmd->MFind.length() > 0) {
					size_t SearchPos = 0;
					if( (SearchPos = RecvStr.find(Cmd->MFind)) != -1) {
						size_t LastDelim = RecvStr.find_first_of("|", SearchPos);
						if(LastDelim == -1)
							LastDelim = RecvStr.length();

						string tmp = RecvStr.substr(0, LastDelim);
						size_t FirstDelim = tmp.find_last_of("|");
						if(FirstDelim == -1)
							FirstDelim = 0;
						string TargetStr = RecvStr.substr(FirstDelim, LastDelim-FirstDelim);

						RecvStr = TargetStr;
					}
				}
				if(Cmd->ValName.length() > 0) {
					stringstream RxValBuf;
					RxValBuf << Cmd->ValName << "=([^ ]+)";
					boost::regex rx(RxValBuf.str());
					boost::match_results<std::string::const_iterator> rxRes;
					if(boost::regex_search(RecvStr, rxRes, rx)) {
						for(unsigned int Idx=0, Max = sizeof(MyVal)/sizeof(MyVal[0]); Idx < Max; ++Idx) {
							if(MyVal[Idx].length() == 0) {
								MyVal[Idx] = rxRes[1];
								break;
							}
						}
					}
				}
				delete Cmd;
				Cmd = NULL;
			}

			delete Commands;
			Commands = NULL;
		}
		SLEEP(5);
	}
	return NULL;
}
