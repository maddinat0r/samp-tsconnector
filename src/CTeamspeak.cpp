#include "main.h"

#include "CTeamspeak.h"

int CTeamspeak::SocketID = 0;
string 
	CTeamspeak::IP, CTeamspeak::Port,
	CTeamspeak::LoginName, CTeamspeak::LoginPass;

bool CTeamspeak::Connect(string ip/*, const char *port*/) {
	if(SocketID != 0)
		Close();

	IP = ip;

	struct addrinfo sHints, *sRes;
	
	memset(&sHints, 0, sizeof sHints);
	sHints.ai_family = AF_UNSPEC;
	sHints.ai_socktype = SOCK_STREAM;

	if(getaddrinfo(ip.c_str(), "10011", &sHints, &sRes) != 0)
		return false;
	SocketID = socket(sRes->ai_family, sRes->ai_socktype, sRes->ai_protocol);
	if(SocketID == -1)
		return false;
	if(connect(CTeamspeak::SocketID, sRes->ai_addr, sRes->ai_addrlen) == SOCKET_ERROR)
		return false;

	SetTimeoutTime(200);
	
	if(Recv(NULL) == 5) //Only "TS3"-greeting recieved
		Recv(NULL);
	return true;
}

int CTeamspeak::Login(string login, string pass) {
	LoginName = login;
	LoginPass = pass;

	stringstream StrBuf;
	StrBuf << "login client_login_name=" << login << " client_login_password=" << pass <<"\n";
	string SendResult;
	Send(StrBuf.str());
	if(Recv(&SendResult) == SOCKET_ERROR)
		return -1;
	return ParseError(SendResult);
}

int CTeamspeak::SetActiveVServer(string port) {
	Port = port;

	stringstream StrBuf;
	StrBuf << "use port=" << port;
	Send(StrBuf.str());
	StrBuf.str("");
	string SendRes;
	if(Recv(&SendRes) == SOCKET_ERROR)
		return -1;
	return ParseError(SendRes);
}


bool CTeamspeak::Close() {
#ifdef _WIN32
	closesocket(CTeamspeak::SocketID);
#else
	close(CTeamspeak::SocketID);
#endif

	SocketID = 0;

	return true;
}

bool CTeamspeak::SetTimeoutTime(unsigned int millisecs) {
	if(SocketID <= 0)
		return false;
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
	return true;
}


bool CTeamspeak::Send(string cmd) {
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
		if(Connect(IP.c_str()) == false || Login(LoginName, LoginPass) != 0 ||SetActiveVServer(Port) != 0) {
			logprintf("[ERROR] Teamspeak Connector could not connect to Teamspeak server.");
		}

	}
	return true;
}

int CTeamspeak::Recv(string *dest) {
	char buf[2048];
	memset(buf, 0, sizeof(buf));
	int bytes = recv(SocketID, buf, sizeof(buf)-1, 0);
	if(dest != NULL)
		(*dest) = buf;
	
	if(bytes == SOCKET_ERROR) {
#ifdef _WIN32
		int errorid = WSAGetLastError();
#else
		int errorid = errno;
#endif
		logprintf("[ERROR] TSConnector encountered an error at \"Recv\": %d", errorid);
		//if(errorid == WSAENOTCONN)
		//{
		//attempt reconnect
		if(Connect(IP.c_str()) == false || Login(LoginName, LoginPass) != 0 ||SetActiveVServer(Port) != 0) {
			logprintf("[ERROR] Teamspeak Connector could not connect to Teamspeak server.");
		}
		//}

	}

	return bytes;
}



bool CTeamspeak::ExpectIntVal(string valname, int *val, int *error) {
	(*val) = -1;
	(*error) = -1;
	string str;
	while( ((*val) == -1 && (*error) == 0) || (*error) == -1) {
		if(CTeamspeak::Recv(&str) == SOCKET_ERROR)
			return false;

		if(str.find('|') != -1) //more than one result
			return false;
		
		if(str.find("error") != -1)
			(*error) = CTeamspeak::ParseError(str);
		if((*error) <= 0 && (*val) == -1)
			(*val) = CTeamspeak::ParseInteger(str, valname);
	}
	return true;
}

bool CTeamspeak::ExpectStringVal(string valname, string *val, int *error) {
	(*val).clear();
	(*error) = -1;
	string str;
	while( ((*val).length() == 0 && (*error) == 0) || (*error) == -1) {
		if(CTeamspeak::Recv(&str) == SOCKET_ERROR)
			return false;
		
		if(str.find('|') != -1) //more than one result
			return false;

		if(str.find("error") != -1)
			(*error) = CTeamspeak::ParseError(str);
		if((*error) <= 0 && (*val).length() == 0)
			CTeamspeak::ParseString(str, valname, val);
	}
	return true;
}

bool CTeamspeak::ParseString(string str, string valname, string *dest) {
	stringstream StrBuf;
	StrBuf << valname << "=([^ ]+)";
	boost::regex rx(StrBuf.str());
	StrBuf.str("");
	boost::match_results<std::string::const_iterator> rxRes;
	if(boost::regex_search(str, rxRes, rx)) {
		(*dest).assign(rxRes[1]);
		return true;
	}
	return false;
}

int CTeamspeak::ParseInteger(string str, string valname) {
	stringstream StrBuf;
	int ReturnVal = -1;
	StrBuf << valname << "=([0-9]+).*";
	boost::regex rx(StrBuf.str());
	StrBuf.str("");
	boost::match_results<std::string::const_iterator> rxRes;
	if(boost::regex_search(str, rxRes, rx)) {
		StrBuf.str(rxRes[1]);
		StrBuf >> ReturnVal;
	}
	return ReturnVal;
}

int CTeamspeak::ParseError(string str) {
	return ParseInteger(str, "error id");
}


bool CTeamspeak::EscapeString(string *str) {
	for(size_t s = 0; s < (*str).length(); ++s) {
		char Char = (*str).at(s);
		if(Char == ' ')
			(*str).replace(s, 1, "\\s");
		else if(Char == '\\')
			(*str).replace(s, 1, "\\\\");
		else if(Char == '/')
			(*str).replace(s, 1, "\\/");
		else if(Char == '|')
			(*str).replace(s, 1, "\\p");
		else if(Char == '\a')
			(*str).replace(s, 1, "\\a");
		else if(Char == '\b')
			(*str).replace(s, 1, "\\b");
		else if(Char == '\f')
			(*str).replace(s, 1, "\\f");
		else if(Char == '\n')
			(*str).replace(s, 1, "\\n");
		else if(Char == '\r')
			(*str).replace(s, 1, "\\r");
		else if(Char == '\t')
			(*str).replace(s, 1, "\\t");
		else if(Char == '\v')
			(*str).replace(s, 1, "\\v");
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



int CTeamspeak::GetClientDBIDByID(int clientid) {
	if(clientid <= 0)
		return -1;
	stringstream StrBuf; 
	StrBuf << "clientinfo clid=" << clientid;
	Send(StrBuf.str());
	StrBuf.str("");

	int ClientDBID = -1, ErrorID = -1;
	if(ExpectIntVal("client_database_id", &ClientDBID, &ErrorID) == false)
		return -1;
	return ClientDBID;
}