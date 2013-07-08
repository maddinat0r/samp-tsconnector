#include "main.h"
#include "thread.h"

#include "CTeamspeak.h"

CMutex CTeamspeak::SocketMutex;
queue<CommandList*> CTeamspeak::SendQueue;
CTeamspeak TSServer;

CTeamspeak::CTeamspeak() : SocketID(-1), LoggedIn(false), Connected(false)
{ }

CTeamspeak::~CTeamspeak() {
	if(SocketID != -1)
		Disconnect();
}

bool CTeamspeak::Connect(char *ip, char *vport) {
	SocketMutex.Lock();

	if(SocketID != -1)
		Disconnect();

	if(!IP.length()) {
		IP.assign(ip);
		Port.assign(vport);
	}
	
	SocketMutex.Unlock();
	return true;
}

bool CTeamspeak::ConnectT() {
	if(!IP.length())
		return false;
	
	SocketMutex.Lock();
	struct addrinfo sHints, *sRes;
	
	memset(&sHints, 0, sizeof sHints);
	sHints.ai_family = AF_UNSPEC;
	sHints.ai_socktype = SOCK_STREAM;

	if(getaddrinfo(IP.c_str(), "10011", &sHints, &sRes) != 0)
		return SocketMutex.Unlock(), false;

	SocketID = socket(sRes->ai_family, sRes->ai_socktype, sRes->ai_protocol);
	if(SocketID == -1)
		return SocketMutex.Unlock(), false;
	if(connect(SocketID, sRes->ai_addr, sRes->ai_addrlen) == SOCKET_ERROR)
		return SocketMutex.Unlock(), false;


	SetTimeoutTime(10);
	SLEEP(50);

	string GreetingMsg, TmpRecvBuf;
	while(Recv(&TmpRecvBuf) != SOCKET_ERROR) {
		GreetingMsg.append(TmpRecvBuf);
		TmpRecvBuf.clear();
	}

	char SendCmd[32];
	sprintf(SendCmd, "use port=%s", Port.c_str());
	Send(SendCmd);

	string SendRes;
	if(Recv(&SendRes) == SOCKET_ERROR)
		return SocketMutex.Unlock(), false;

	Connected = true;
	SocketMutex.Unlock();

	return true;
}

int CTeamspeak::Login(char *login, char *pass, char *nickname) {
	SocketMutex.Lock();
	if(!LoginName.length()) {
		LoginName.assign(login);
		LoginPass.assign(pass);
		LoginNick.assign(nickname);
	}
	SocketMutex.Unlock();
	return 0;
}

int CTeamspeak::LoginT() {
	if(!LoginName.length())
		return -1;

	char SendCmd[128];
	sprintf(SendCmd, "login client_login_name=%s client_login_password=%s", LoginName.c_str(), LoginPass.c_str());
	Send(SendCmd);
	
	string SendResult;
	if(Recv(&SendResult) == SOCKET_ERROR)
		return -1;
	
	sprintf(SendCmd, "clientupdate client_nickname=%s", LoginNick.c_str());
	Send(SendCmd);
	if(Recv(&SendResult) == SOCKET_ERROR)
		return -1;

	LoggedIn = true;
	return 0;
}



bool CTeamspeak::Disconnect() {
	SocketMutex.Lock();
#ifdef _WIN32
	closesocket(CTeamspeak::SocketID);
#else
	close(CTeamspeak::SocketID);
#endif

	SocketID = -1;
	LoggedIn = false;
	Connected = false;
	SocketMutex.Unlock();
	return true;
}

bool CTeamspeak::SetTimeoutTime(unsigned int millisecs) {
	SocketMutex.Lock();
	if(SocketID != -1) {
	#ifdef _WIN32
		DWORD Timeout = millisecs;
		if(setsockopt(SocketID, SOL_SOCKET, SO_RCVTIMEO, (char *)&Timeout, sizeof(Timeout)) < 0) {
			return logprintf("Warning! An error occured in TSConnector while setting the timeout time."), false;
		}
	#else
		struct timeval sTimeout;      
		sTimeout.tv_sec = 0;
		sTimeout.tv_usec = millisecs * 1000;
		if (setsockopt (SocketID, SOL_SOCKET, SO_RCVTIMEO, (char *)&sTimeout, sizeof(sTimeout))) {
			return logprintf("Warning! An error occured in TSConnector while setting the timeout time."), false;
		}
	#endif
	}
	SocketMutex.Unlock();
	return true;
}


int CTeamspeak::Send(string cmd) {
	int errorid = 0;
	SocketMutex.Lock();
	if(cmd.at(cmd.length()-1) != '\n')
		cmd.append("\n");

	if(send(SocketID, cmd.c_str(), cmd.length(), 0) == SOCKET_ERROR) {
#ifdef _WIN32
		errorid = WSAGetLastError();
#else
		errorid = errno;
#endif
		logprintf("[ERROR] TSConnector encountered an error at \"Send\": %d", errorid);

		//attempt reconnect
		if(ConnectT() == false || LoginT() != 0)
			logprintf("[ERROR] TSConnector could not connect to Teamspeak server.");
		

	}
	SocketMutex.Unlock();
	return errorid;
}

int CTeamspeak::Recv(string *dest) {
	SocketMutex.Lock();
	if(SocketID == -1)
		return SocketMutex.Unlock(), -1;

	int ErrorID = 0;
	char buf[2048];
	memset(buf, 0, sizeof(buf));
	
	if( (ErrorID = recv(SocketID, buf, sizeof(buf)-1, 0)) == SOCKET_ERROR) {
		
	#ifdef _WIN32
		int SocketErrorID = WSAGetLastError();
		bool WouldBlock = (SocketErrorID == WSAEWOULDBLOCK);
		bool TimedOut = (SocketErrorID == WSAETIMEDOUT);
	#else
		int SocketErrorID = errno;
		bool WouldBlock = (SocketErrorID == EWOULDBLOCK);
		bool TimedOut = (SocketErrorID == ETIMEDOUT);
	#endif
		if(!TimedOut && !WouldBlock) {
			logprintf("[ERROR] TSConnector encountered an error at \"Recv\": %d", SocketErrorID);

			ErrorID = -2;
		}


	}
	else {
		if(dest != NULL)
			(*dest) = buf;
	}
	SocketMutex.Unlock();
	return ErrorID;
}


bool CTeamspeak::AddCommandListToQueue(CommandList *cmdlist) {
	SocketMutex.Lock();
	SendQueue.push(cmdlist);
	SocketMutex.Unlock();
	return true;
}

CommandList *CTeamspeak::GetNextCommandList() {
	CommandList *NextVal = NULL;
	SocketMutex.Lock();
	if(!SendQueue.empty()) {
		NextVal = SendQueue.front();
		SendQueue.pop();
	}
	SocketMutex.Unlock();
	return NextVal;
}

bool CTeamspeak::EscapeString(string &str) {
	for(size_t s = 0; s < str.length(); ++s) {
		char Char = str.at(s);
		if(Char == ' ')
			str.replace(s, 1, "\\s"), s++;
		else if(Char == '\\')
			str.replace(s, 1, "\\\\"), s++;
		else if(Char == '/')
			str.replace(s, 1, "\\/"), s++;
		else if(Char == '|')
			str.replace(s, 1, "\\p"), s++;
		else if(Char == '\a')
			str.replace(s, 1, "\\a"), s++;
		else if(Char == '\b')
			str.replace(s, 1, "\\b"), s++;
		else if(Char == '\f')
			str.replace(s, 1, "\\f"), s++;
		else if(Char == '\n')
			str.replace(s, 1, "\\n"), s++;
		else if(Char == '\r')
			str.replace(s, 1, "\\r"), s++;
		else if(Char == '\t')
			str.replace(s, 1, "\\t"), s++;
		else if(Char == '\v')
			str.replace(s, 1, "\\v"), s++;
		
		else if(Char == 'ä')
			str.replace(s, 1, "\xc3\xa4"), s++;
		else if(Char == 'ö')
			str.replace(s, 1, "\xc3\xb6"), s++;
		else if(Char == 'ü')
			str.replace(s, 1, "\xc3\xbc"), s++;
		else if(Char == 'Ä')
			str.replace(s, 1, "\xc3\x84"), s++;
		else if(Char == 'Ö')
			str.replace(s, 1, "\xc3\x96"), s++;
		else if(Char == 'Ü')
			str.replace(s, 1, "\xc3\x9c"), s++;
	}
	return true;
}

bool CTeamspeak::UnEscapeString(string &str) {
	int FoundPos;

	while( (FoundPos = str.find("\\s")) != -1)
		str.replace(FoundPos, 2, " ");

	while( (FoundPos = str.find("\\\\")) != -1)
		str.replace(FoundPos, 2, "\\");

	while( (FoundPos = str.find("\\/")) != -1)
		str.replace(FoundPos, 2, "/");

	while( (FoundPos = str.find("\\p")) != -1)
		str.replace(FoundPos, 2, "|");

	while( (FoundPos = str.find("\\a")) != -1)
		str.replace(FoundPos, 2, "\a");

	while( (FoundPos = str.find("\\b")) != -1)
		str.replace(FoundPos, 2, "\b");

	while( (FoundPos = str.find("\\f")) != -1)
		str.replace(FoundPos, 2, "\f");

	while( (FoundPos = str.find("\\n")) != -1)
		str.replace(FoundPos, 2, "\n");

	while( (FoundPos = str.find("\\r")) != -1)
		str.replace(FoundPos, 2, "\r");

	while( (FoundPos = str.find("\\t")) != -1)
		str.replace(FoundPos, 2, "\t");

	while( (FoundPos = str.find("\\v")) != -1)
		str.replace(FoundPos, 2, "\v");


	while( (FoundPos = str.find("\xc3\xa4")) != -1)
		str.replace(FoundPos, 2, "ä");
	while( (FoundPos = str.find("\xc3\xb6")) != -1)
		str.replace(FoundPos, 2, "ö");
	while( (FoundPos = str.find("\xc3\xbc")) != -1)
		str.replace(FoundPos, 2, "ü");
	while( (FoundPos = str.find("\xc3\x84")) != -1)
		str.replace(FoundPos, 2, "Ä");
	while( (FoundPos = str.find("\xc3\x96")) != -1)
		str.replace(FoundPos, 2, "Ö");
	while( (FoundPos = str.find("\xc3\x9c")) != -1)
		str.replace(FoundPos, 2, "Ü");
	return true;
}
