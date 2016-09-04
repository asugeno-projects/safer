/**
* @file       analyticsReportXml.h
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

#ifndef RDS_ER_STATICANALYSIS_SOURCE_REPORTER_ANALYTICSREPORTXML_H_
#define RDS_ER_STATICANALYSIS_SOURCE_REPORTER_ANALYTICSREPORTXML_H_

#include <iostream>
#include <string>
#include "analyticsReport.h"

/**
* @class AnalyticsReportXml
* @brief 解析レポート生成クラスXML版
* @details 解析レポート生成クラス<br/>XML形式のレポート情報を出力します。
*/
class AnalyticsReportXml : private AnalyticsReport
{
private:
public:
	//! save関数 
	bool save(std::string filename);
};

#endif //RDS_ER_STATICANALYSIS_SOURCE_REPORTER_ANALYTICSREPORTXML_H_