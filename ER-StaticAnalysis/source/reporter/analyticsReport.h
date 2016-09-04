/**
* @file       analyticsReport.h
* @date       2016/01/31
* @author     Akihiro Sugeno
* @par        Revision
* $Id$
* @par        Copyright
* Copyright 2015 Akihiro Sugeno
* @par        History
*     - 2015/10/08 Akihiro Sugeno
*       -# Initial Version
*/

#ifndef RDS_ER_STATICANALYSIS_SOURCE_REPORTER_ANALYTICSREPORT_H_
#define RDS_ER_STATICANALYSIS_SOURCE_REPORTER_ANALYTICSREPORT_H_

#include <iostream>
#include <string>
#include "analyticsRowData.h"
#include "../analysis_command/analysisCommand.h"

using namespace std;

/*! @class AnalyticsReport
@brief 解析レポートを生成するクラス
*/
class AnalyticsReport
{
private:
public:
	//! ファイルの保存関数
	virtual bool save(std::string file) = 0;
};

#endif //RDS_ER_STATICANALYSIS_SOURCE_REPORTER_ANALYTICSREPORT_H_