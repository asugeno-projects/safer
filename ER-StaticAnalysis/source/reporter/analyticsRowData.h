/**
* @file       analyticsRowData.h
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

#ifndef RDS_ER_STATICANALYSIS_SOURCE_REPORTER_ANALYTICSROWDATA_H_
#define RDS_ER_STATICANALYSIS_SOURCE_REPORTER_ANALYTICSROWDATA_H_

#include "../analysis_command/analysisCommand.h"

/*! @struct AnalyticsRowData
* @brief 解析レポート情報(1レコード分)を格納する構造体
* @details 解析レポート情報(レコード分)を格納する構造体です。
*/
struct AnalyticsRowData{
	//! projectID
	int projectId;
	//! コマンドID
	int commandId;
	//! コマンドタイプ
	CommandTypeList commandType;
	//! アラートレベル
	AlertLevelList alertLevel;
	//! コマンド名
	wstring commandName;
	//! テーブル名
	wstring tableName;
	//! 検出対象名
	wstring targetName;
	//! メッセージ
	wstring message;
};

#endif //RDS_ER_STATICANALYSIS_SOURCE_REPORTER_ANALYTICSROWDATA_H_