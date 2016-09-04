/**
* @file       dBStaticAnalytics.h
* @date       2016/01/31
* @author     Akihiro Sugeno
* @par        Revision
* $Id$
* @par        Copyright
* Copyright 2015 Akihiro Sugeno
* @par        History
*     - 2015/08/16 Akihiro Sugeno
*       -# Initial Version
*/

#ifndef RDS_ER_STATICANALYSIS_SOURCE_STATIC_ANALYTICS_DBSTATICANALYTICS_H_
#define RDS_ER_STATICANALYSIS_SOURCE_STATIC_ANALYTICS_DBSTATICANALYTICS_H_

/*! @class DBStaticAnalytics
* @brief ER分析クラス
* @details ER情報を分析するクラスです。
*/
class DBStaticAnalytics
{
private:
public:
	//! コンストラクタ
	DBStaticAnalytics()
	{

	};

	//! 解析処理実行関数
	void run(class AnalysisProjects *analysisProjects, class ER *er);
};

#endif //RDS_ER_STATICANALYSIS_SOURCE_STATIC_ANALYTICS_DBSTATICANALYTICS_H_