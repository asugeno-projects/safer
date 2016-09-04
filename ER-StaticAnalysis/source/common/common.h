/**
* @file       common.h
* @date       2016/01/31
* @author     Akihiro Sugeno
* @par        Revision
* $Id$
* @par        Copyright
* Copyright 2015 Akihiro Sugeno
* @par        History
*     - 2015/07/30 Akihiro Sugeno
*       -# Initial Version
*/

#ifndef RDS_ER_STATICANALYSIS_SOURCE_COMMON_COMMON_H_
#define RDS_ER_STATICANALYSIS_SOURCE_COMMON_COMMON_H_

#include <iostream>
#include <string>

class debugMessage
{
public:
	template<typename T>
	debugMessage& operator<<(const T& t)
	{
#ifdef _DEBUG
		cout << t;
#endif
		return *this;
	}

	debugMessage& operator<<(std::ostream& (__cdecl *_func_ptr)(std::ostream& os))
	{
#ifdef _DEBUG
		std::cout << _func_ptr;
#endif
		return *this;
	}

private:
};

#endif //RDS_ER_STATICANALYSIS_SOURCE_COMMON_COMMON_H_