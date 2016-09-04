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

/**
* @class ErParser
* ER図パーサークラス
* @brief ER図ファイルの構文解析を行うクラス
*/
class ErParser
{
private:

public:
	/*!< ER図ファイルのパース関数 */
	virtual class ER * parse(void)
	{
		return NULL;
	}


};

#endif //RDS_ER_STATICANALYSIS_SOURCE_PARSER_ER_ERPARSER_H_
