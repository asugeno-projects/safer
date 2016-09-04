/**
* @file       commonString.h
* @date       2016/01/31
* @author     Akihiro Sugeno
* @par        Revision
* $Id$
* @par        Copyright
* Copyright 2015 Akihiro Sugeno
* @par        History
*     - 2015/10/12 Akihiro Sugeno
*       -# Initial Version
*/

#ifndef RDS_ER_STATICANALYSIS_SOURCE_COMMON_COMMONSTRING_H_
#define RDS_ER_STATICANALYSIS_SOURCE_COMMON_COMMONSTRING_H_

#include <string>


static int stringToWString(std::wstring &ws, const std::string &s)
{
	std::wstring wsTmp(s.begin(), s.end());

	ws = wsTmp;

	return 0;
}

static int wstringToString(const std::wstring &ws, std::string &s)
{
	std::string sTmp(ws.begin(), ws.end());

	s = sTmp;

	return 0;
}

static wchar_t * stringToWchar(std::string s)
{
	int len = strlen(s.c_str()) + 1;
	wchar_t *dst = new wchar_t[len];
	mbstowcs(dst, s.c_str(), len);
	return dst;
}

static void stringToWstringForUtf8(std::wstring *ws, std::string s)
{
	int len = strlen(s.c_str()) + 1;
	wchar_t *dst = new wchar_t[len];
	mbstowcs(dst, s.c_str(), len);

	*ws = dst;
	delete dst;
}

#endif //RDS_ER_STATICANALYSIS_SOURCE_COMMON_COMMONSTRING_H_