#include "CUtils.h"

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/karma.hpp>


bool CUtils::ParseField(const string &row, const string &field, string &dest)
{
	size_t field_pos = row.find(field);
	if (field_pos != string::npos)
	{
		//field can exist in row without a result
		//that's why we have to check if there is an equal sign
		size_t equal_sign_pos = field_pos + field.length();
		if (equal_sign_pos < row.length() && row.at(equal_sign_pos) == '=')
		{
			const size_t data_pos = field_pos + field.length() + 1;
			dest = row.substr(data_pos, row.find(' ', field_pos) - data_pos);
			return true;
		}
	}
	return false;
}

bool CUtils::ParseField(const string &row, const string &field, int &dest)
{
	string buffer;
	return  ParseField(row, field, buffer)
			&& boost::spirit::qi::parse(buffer.begin(), buffer.end(), boost::spirit::qi::int_, dest);
}

bool CUtils::ParseField(const string &row, const string &field, unsigned int &dest)
{
	string buffer;
	return  ParseField(row, field, buffer)
		&& boost::spirit::qi::parse(buffer.begin(), buffer.end(), boost::spirit::qi::uint_, dest);
}


bool CUtils::ConvertStringToInt(const string &input, int &output)
{
	return boost::spirit::qi::parse(input.begin(), input.end(), boost::spirit::qi::int_, output);
}

bool CUtils::ConvertStringToInt(const string &input, unsigned int &output)
{
	return boost::spirit::qi::parse(input.begin(), input.end(), boost::spirit::qi::uint_, output);
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
	{ "\v", "\\v" }

	/*{ "ä", "\xc3\xa4" },
	{ "ö", "\xc3\xb6" },
	{ "ü", "\xc3\xbc" },
	{ "Ä", "\xc3\x84" },
	{ "Ö", "\xc3\x96" },
	{ "Ü", "\xc3\x9c" }*/
};

void CUtils::EscapeString(string &str)
{
	if (str.length())
	{
		for (size_t i = 0; i < str.length(); ++i)
		{
			const char char_val = str.at(i);
			for (size_t ci = 0; ci < (sizeof(CharEscapeTable) / sizeof(CharEscapeTable[0])); ++ci)
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

void CUtils::UnEscapeString(string &str)
{
	if (str.length() >= 2)
	{
		for (size_t ci = 0; ci < (sizeof(CharEscapeTable) / sizeof(CharEscapeTable[0])); ++ci)
		{
			size_t find_pos = 0;
			while ((find_pos = str.find(CharEscapeTable[ci][1])) != string::npos)
				str.replace(find_pos, 2, CharEscapeTable[ci][0]);
		}
	}
}
