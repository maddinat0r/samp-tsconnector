#include "main.h"

#include "CTeamspeak.h"

int CTeamspeak::SocketID = 0;

bool CTeamspeak::Connect(const char *ip, const char *port) {
	if(SocketID != 0)
		Close();

	struct addrinfo sHints, *sRes;
	
	memset(&sHints, 0, sizeof sHints);
	sHints.ai_family = AF_UNSPEC;
	sHints.ai_socktype = SOCK_STREAM;

	getaddrinfo(ip, port, &sHints, &sRes);
	CTeamspeak::SocketID = socket(sRes->ai_family, sRes->ai_socktype, sRes->ai_protocol);
	connect(CTeamspeak::SocketID, sRes->ai_addr, sRes->ai_addrlen);

	SetTimeoutTime(100);
	
	if(Recv(NULL) == 5) //Only "TS3"-greeting recieved
		Recv(NULL);
	return true;
}

bool CTeamspeak::Close() {
#ifdef _WIN32
	closesocket(CTeamspeak::SocketID);
#else
	close(sockfd);
#endif

	SocketID = 0;

	return true;
}

bool CTeamspeak::SetTimeoutTime(unsigned int millisecs) {
	if(!SocketID)
		return false;
#ifdef _WIN32
	DWORD Timeout = millisecs;
	if(setsockopt(SocketID, SOL_SOCKET, SO_RCVTIMEO, (char *)&Timeout, sizeof(Timeout)) < 0) {
		return logprintf("Warning! An error occured in TSConnector while setting timeout time."), false;
	}
#else
	struct timeval sTimeout;      
    sTimeout.tv_sec = 0;
    sTimeout.tv_usec = millisecs * 1000;
    if (setsockopt (SocketID, SOL_SOCKET, SO_RCVTIMEO, (char *)&sTimeout, sizeof(sTimeout)) {
		return logprintf("Warning! An error occured in TSConnector while setting timeout time."), false;
	}
#endif
	return true;
}


bool CTeamspeak::Send(string cmd) {
	if(cmd.at(cmd.length()-1) != '\n')
		cmd.append("\n");
	send(SocketID, cmd.c_str(), cmd.length(), 0);
	return true;
}

int CTeamspeak::Recv(string *dest) {
	char buf[2048];
	memset(buf, 0, sizeof(buf));
	int bytes = recv(SocketID, buf, sizeof(buf)-1, 0);
	if(dest != NULL)
		(*dest) = buf;
	return bytes;
}



bool CTeamspeak::ExpectIntVal(string valname, int *val, int *error) {
	(*val) = -1;
	(*error) = -1;
	string str;
	while( ((*val) == -1 && (*error) == 0) || (*error) == -1) {
		if(CTeamspeak::Recv(&str) == SOCKET_ERROR)
			return false;


		if((*error) == -1)
			(*error) = CTeamspeak::ParseError(str);
		if((*val) == -1)
			(*val) = CTeamspeak::ParseInteger(str, valname);
	}
	return true;
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
		if( (*str).at(s) == ' ') {
			(*str).replace(s, 1, "\\s");
		}
	}
	return true;
}