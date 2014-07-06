#ifndef INC_CUTILS_H
#define INC_CUTILS_H


#include <string>

#include "CSingleton.h"

using std::string;


class CUtils : public CSingleton <CUtils>
{
	friend class CSingleton <CUtils>;
private: //constructor / deconstructor
	CUtils() {}
	~CUtils() {}


public: //functions
	bool ParseField(const string &row, const string &field, string &dest);
	bool ParseField(const string &row, const string &field, int &dest);
	bool ParseField(const string &row, const string &field, unsigned int &dest);

	bool ConvertStringToInt(const string &input, int &output);
	bool ConvertStringToInt(const string &input, unsigned int &output);

	void EscapeString(string &str);
	void UnEscapeString(string &str);

};


#endif // INC_CUTILS_H
