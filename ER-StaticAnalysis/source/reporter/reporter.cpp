/**
* @file       reporter.cpp
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

#include "../reporter/reporter.h"

//! レポーターインスタンスの生成
Reporter * Reporter::instance = new Reporter();

/**
* @fn Reporter * Reporter::getInstance()
* @brief レポーターインスタンス取得関数
* @return レポーターインスタンス
* @details レポーターインスタンスを生成し、呼び出し元に返すクラス<br/>シングルトン関数です。
*/
Reporter * Reporter::getInstance()
{
	if (!instance)
	{
		instance = new Reporter();
	}
	return instance;
}

/**
* @fn void Reporter::finalize()
* @brief レポータークラス終了処理
* @details レポータークラス終了処理を行います。<br/>レポーターインスタンスを破棄します。
*/
void Reporter::finalize()
{
	if (instance)
	{
		delete instance;
	}
}

/**
* @fn void Reporter::writeReport(ER * _er, std::string _savePath)
* @brief レポート生成関数
* @param _er ER図情報格納インスタンス
* @param _savePath 保存Path
* @details レポート情報を生成します。<br/>
レポート情報生成のために必要なレポートインスタンスを生成し、指定のPathに吐き出すよう適宜出力します。
* @note レポート出力関連の処理は実装次第、この関数に羅列していきます。
*/
void Reporter::writeReport(ER * _er, std::string _savePath)
{
	AnalyticsReportXlsx analyticsReportXlsx;
	string filename = _savePath + "解析結果";
	analyticsReportXlsx.write(this->rowData, _er)->save(filename);
}

/**
* @fn void Reporter::addRecord(AnalyticsRowData _data)
* @brief 報告情報格納関数
* @param _data 報告情報
* @details 報告情報を受け取り、rowDataに蓄積していきます。
*/
void Reporter::addRecord(AnalyticsRowData _data)
{
	this->rowData.push_back(_data);
}

/**
* @fn void Reporter::outputConsole()
* @brief 報告情報コンソール書き出し関数
* @details 報告情報をコンソールへ出力します。
*/
void Reporter::outputConsole()
{
	for (auto dataIt = this->rowData.begin(); dataIt != this->rowData.end(); dataIt++)
	{
		auto data = (*dataIt);
		cout << "[" << AlertLevelToLabel[data.alertLevel] << "]";
		wcout << " " << data.tableName << " ";
		wcout << " " << data.targetName << " " << data.message << endl;
	}
}

/**
* @fn void Reporter::addRecord(AnalysisCommand *analysisCommand, CommandTypeList commandType, std::wstring tableName, std::wstring targetName)
* @brief 報告情報格納関数
* @param analysisCommand 解析命令コマンド
* @param commandType 解析命令タイプ
* @param tableName 検出テーブル名
* @param targetName 検出対象名
* @details 報告情報を受け取り、rowDataに蓄積していきます。
*/
void Reporter::addRecord(AnalysisCommand *analysisCommand, CommandTypeList commandType, std::wstring tableName, std::wstring targetName)
{
	AnalyticsRowData reportData;
	reportData.commandId = analysisCommand->getId();
	reportData.alertLevel = analysisCommand->getMessageAlertLevel();
	reportData.tableName = tableName;
	reportData.commandType = commandType;
	reportData.targetName = targetName;
	wchar_t * ws = stringToWchar(analysisCommand->getOutputMessage());
	reportData.message = ws;
	delete ws;
	this->addRecord(reportData);
}

/**
* @fn void Reporter::addRecord(AlertLevelList alertLevel, CommandTypeList commandType, std::wstring tableName, std::wstring targetName, std::wstring message)
* @brief 報告情報格納関数
* @param alertLevel アラートレベル
* @param commandType 解析命令タイプ
* @param tableName 検出テーブル名
* @param targetName 検出対象名
* @param message 出力メッセージ
* @details 報告情報を受け取り、rowDataに蓄積していきます。
*/
void Reporter::addRecord(AlertLevelList alertLevel, CommandTypeList commandType, std::wstring tableName, std::wstring targetName, std::wstring message)
{
	AnalyticsRowData reportData;
	reportData.alertLevel = alertLevel;
	reportData.tableName = tableName;
	reportData.commandType = commandType;
	reportData.targetName = targetName;
	reportData.message = message;
	this->addRecord(reportData);
}