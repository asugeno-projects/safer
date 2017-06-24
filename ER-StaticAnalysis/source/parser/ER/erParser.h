/**
* @file       erParser.h
* @date       2016/01/31
* @author     Akihiro Sugeno
* @par        Revision
* $Id$
* @par        Copyright
* Copyright 2015 Akihiro Sugeno
* @par        History
*     - 2015/07/05 Akihiro Sugeno
*       -# Initial Version
*/

#ifndef RDS_ER_STATICANALYSIS_SOURCE_PARSER_ER_ERPARSER_H_
#define RDS_ER_STATICANALYSIS_SOURCE_PARSER_ER_ERPARSER_H_

#include <boost/algorithm/string.hpp>

#include <iostream>
#include <string>

#include "../../ER/er.h"

#ifndef _WIN32
typedef std::basic_ifstream<wchar_t, std::char_traits<wchar_t> > wifstream;
typedef std::basic_string<wchar_t> wstring;
#endif

/**
* @class ErParser
* ER図パーサークラス
* @brief ER図ファイルの構文解析を行うクラス
*/
class ErParser
{
private:
	virtual void setParams(){};
public:
	ErParser(){};
	~ErParser(){};

	/*!< ER図ファイルのパース関数 */
	virtual class ER * parse(void)
	{
		return NULL;
	}


};

#endif //RDS_ER_STATICANALYSIS_SOURCE_PARSER_ER_ERPARSER_H_
