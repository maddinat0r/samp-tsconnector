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

	//int flag = 1;
    //int result = setsockopt(SocketID, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int)); 
	
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


bool CTeamspeak::Send(string cmd) {
	if(cmd.at(cmd.length()-1) != '\n')
		cmd.append("\n");
	send(SocketID, cmd.c_str(), cmd.length(), 0);
	return true;
}

int CTeamspeak::Recv(string *dest) {
	char buf[1024];
	memset(buf, 0, 1024);
	int bytes = recv(SocketID, buf, 1023, 0);
	if(dest != NULL)
		(*dest) = buf;
	return bytes;
}



bool CTeamspeak::ExpectIntVal(string valname, int *val, int *error) {
	(*val) = -1;
	(*error) = -1;
	string str;
	while( ((*val) == -1 && (*error) >= 0) || (*error) == -1) {
		CTeamspeak::Recv(&str);

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