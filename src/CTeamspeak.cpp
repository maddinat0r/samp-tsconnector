#include "main.h"
#include "thread.h"

#include "CTeamspeak.h"

CMutex CTeamspeak::SocketMutex;
queue<CommandList*> CTeamspeak::SendQueue;
CTeamspeak TSServer;

CTeamspeak::CTeamspeak() : SocketID(-1)
{ }

CTeamspeak::~CTeamspeak() {
	if(SocketID != -1)
		Disconnect();
}

bool CTeamspeak::Connect(string ip, string vport) {
	SocketMutex.Lock();

	if(SocketID != -1)
		Disconnect();

	if(!IP.length()) {
		IP = ip;
		Port = vport;
	}

	struct addrinfo sHints, *sRes;
	
	memset(&sHints, 0, sizeof sHints);
	sHints.ai_family = AF_UNSPEC;
	sHints.ai_socktype = SOCK_STREAM;

	if(getaddrinfo(ip.c_str(), "10011", &sHints, &sRes) != 0)
		return SocketMutex.Unlock(), false;
	SocketID = socket(sRes->ai_family, sRes->ai_socktype, sRes->ai_protocol);
	if(SocketID == -1)
		return SocketMutex.Unlock(), false;
	if(connect(CTeamspeak::SocketID, sRes->ai_addr, sRes->ai_addrlen) == SOCKET_ERROR)
		return SocketMutex.Unlock(), false;

	/*
#ifdef _WIN32
	u_long iMode=1;
	ioctlsocket(SocketID, FIONBIO, &iMode);
#else
	int flags = fcntl(fd, F_GETFL);
	int result = fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);
#endif
	*/

	SetTimeoutTime(5);
	
	
	//if(Recv(NULL) == 5) //Only "TS3"-greeting recieved
		//Recv(NULL);
	while(Recv(NULL) != SOCKET_ERROR) {}
	
	stringstream StrBuf;
	StrBuf << "use port=" << vport;
	Send(StrBuf.str());
	StrBuf.str("");
	string SendRes;
	if(Recv(&SendRes) == SOCKET_ERROR)
		return false;

	SocketMutex.Unlock();
	return true;
}

int CTeamspeak::Login(string login, string pass, string nickname) {
	SocketMutex.Lock();
	if(!LoginName.length()) {
		LoginName = login;
		LoginPass = pass;
		LoginNick = nickname;
	}
	SocketMutex.Unlock();

	stringstream StrBuf;
	StrBuf << "login client_login_name=" << login << " client_login_password=" << pass <<"\n";
	string SendResult;
	Send(StrBuf.str());
	StrBuf.clear();
	StrBuf.str(string());
	if(Recv(&SendResult) == SOCKET_ERROR)
		return -1;
	
	StrBuf << "clientupdate client_nickname=" << nickname;
	Send(StrBuf.str());
	if(Recv(&SendResult) == SOCKET_ERROR)
		return -1;

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


bool CTeamspeak::Send(string cmd) {
	SocketMutex.Lock();
	if(cmd.at(cmd.length()-1) != '\n')
		cmd.append("\n");

	if(send(SocketID, cmd.c_str(), cmd.length(), 0) == SOCKET_ERROR) {
#ifdef _WIN32
		int errorid = WSAGetLastError();
#else
		int errorid = errno;
#endif
		logprintf("[ERROR] TSConnector encountered an error at \"Send\": %d", errorid);

		//attempt reconnect
		if(Connect(IP, Port) == false || Login(LoginName, LoginPass, LoginNick) != 0)
			logprintf("[ERROR] Teamspeak Connector could not connect to Teamspeak server.");
		

	}
	SocketMutex.Unlock();
	return true;
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

			//attempt reconnect
			if(Connect(IP, Port) == false || Login(LoginName, LoginPass, LoginNick) != 0)
				logprintf("[ERROR] Teamspeak Connector could not connect to Teamspeak server.");
		}

	}
	else {
		if(dest != NULL)
			(*dest) = buf;
		//if(strlen(buf) > 0)
			//logprintf("[RECV] %s", buf);
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
	SocketMutex.Lock();
	CommandList *NextVal = SendQueue.front();
	SendQueue.pop();
	SocketMutex.Unlock();
	return NextVal;
}

bool CTeamspeak::IsQueueEmpty() {
	SocketMutex.Lock();
	bool IsEmpty = SendQueue.empty();
	SocketMutex.Unlock();
	return IsEmpty;
}

bool CTeamspeak::EscapeString(string *str) {
	for(size_t s = 0; s < (*str).length(); ++s) {
		char Char = (*str).at(s);
		if(Char == ' ')
			(*str).replace(s, 1, "\\s"), s++;
		else if(Char == '\\')
			(*str).replace(s, 1, "\\\\"), s++;
		else if(Char == '/')
			(*str).replace(s, 1, "\\/"), s++;
		else if(Char == '|')
			(*str).replace(s, 1, "\\p"), s++;
		else if(Char == '\a')
			(*str).replace(s, 1, "\\a"), s++;
		else if(Char == '\b')
			(*str).replace(s, 1, "\\b"), s++;
		else if(Char == '\f')
			(*str).replace(s, 1, "\\f"), s++;
		else if(Char == '\n')
			(*str).replace(s, 1, "\\n"), s++;
		else if(Char == '\r')
			(*str).replace(s, 1, "\\r"), s++;
		else if(Char == '\t')
			(*str).replace(s, 1, "\\t"), s++;
		else if(Char == '\v')
			(*str).replace(s, 1, "\\v"), s++;
		
	}
	return true;
}

bool CTeamspeak::UnEscapeString(string *str) {
	int FoundPos;

	while( (FoundPos = (*str).find("\\s")) != -1)
		(*str).replace(FoundPos, 2, " ");

	while( (FoundPos = (*str).find("\\\\")) != -1)
		(*str).replace(FoundPos, 2, "\\");

	while( (FoundPos = (*str).find("\\/")) != -1)
		(*str).replace(FoundPos, 2, "/");

	while( (FoundPos = (*str).find("\\p")) != -1)
		(*str).replace(FoundPos, 2, "|");

	while( (FoundPos = (*str).find("\\a")) != -1)
		(*str).replace(FoundPos, 2, "\a");

	while( (FoundPos = (*str).find("\\b")) != -1)
		(*str).replace(FoundPos, 2, "\b");

	while( (FoundPos = (*str).find("\\f")) != -1)
		(*str).replace(FoundPos, 2, "\f");

	while( (FoundPos = (*str).find("\\n")) != -1)
		(*str).replace(FoundPos, 2, "\n");

	while( (FoundPos = (*str).find("\\r")) != -1)
		(*str).replace(FoundPos, 2, "\r");

	while( (FoundPos = (*str).find("\\t")) != -1)
		(*str).replace(FoundPos, 2, "\t");

	while( (FoundPos = (*str).find("\\v")) != -1)
		(*str).replace(FoundPos, 2, "\v");
	
	return true;
}
