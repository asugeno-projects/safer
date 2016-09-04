/**
* @file       consoleLog.h
* @date       2016/01/31
* @author     Akihiro Sugeno
* @par        Revision
* $Id$
* @par        Copyright
* Copyright 2015 Akihiro Sugeno
* @par        History
*     - 2015/07/03 Akihiro Sugeno
*       -# Initial Version
*/

#ifndef RDS_ER_STATICANALYSIS_SOURCE_LOG_CONSOLELOG_H_
#define RDS_ER_STATICANALYSIS_SOURCE_LOG_CONSOLELOG_H_

#include <iostream>
#include <string>


class ConsoleLog
{
public:
	template<typename T>
	const ConsoleLog& operator<<(const T& t)const
	{
#ifdef _DEBUG
		std::cout << t;
#endif
		return *this;
	}

	const ConsoleLog& operator<<(const std::wstring& t)const
	{
#ifdef _DEBUG
		std::wcout << t;
#endif
		return *this;
	}

	const ConsoleLog& operator<<(std::ostream& (__cdecl *_func_ptr)(std::ostream& os))const
	{
#ifdef _DEBUG
		std::cout << _func_ptr;
#endif
		return *this;
	}

private:
};

#endif //RDS_ER_STATICANALYSIS_SOURCE_LOG_CONSOLELOG_H_