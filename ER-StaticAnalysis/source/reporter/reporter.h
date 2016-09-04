/**
* @file       reporter.h
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

#ifndef RDS_ER_STATICANALYSIS_SOURCE_REPORTER_REPORTER_H_
#define RDS_ER_STATICANALYSIS_SOURCE_REPORTER_REPORTER_H_

#include <iostream>
#include <string>
#include <list>
#include "analyticsRowData.h"
#include "analyticsReportXml.h"
#include "analyticsReportXlsx.h"

/*! @class Reporter
* @brief レポータークラス
* @details レポートの書き出しと情報収集を行うクラス
*/
class Reporter
{
private:
	//! レポーターインスタンス格納ポインタ変数
	static Reporter* instance;
	//! レポート情報格納変数
	list <AnalyticsRowData> rowData;
public:
	//! コンストラクタ
	Reporter(){};
	//! デストラクタ
	~Reporter(){};
	//! レポーターインスタンス取得関数
	static Reporter * getInstance();
	//! レポータークラス終了処理
	static void finalize();
	//! 報告情報コンソール書き出し関数
	void outputConsole();
	//! レポート生成関数
	void writeReport(ER *er, std::string filepath);
	//! 報告情報格納関数
	void addRecord(AnalyticsRowData data);
	//! 報告情報格納関数
	void addRecord(AnalysisCommand *analysisCommand, CommandTypeList commandType, std::wstring tableName, std::wstring targetName);
	//! 報告情報格納関数
	void addRecord(AlertLevelList alertLevel, CommandTypeList commandType, std::wstring tableName, std::wstring targetName, std::wstring message);
	//! 報告情報取得関数
	list <AnalyticsRowData> getRowData(){ return rowData; };
};

#endif //RDS_ER_STATICANALYSIS_SOURCE_REPORTER_REPORTER_H_