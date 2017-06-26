/**
* @file       analysisCommandParser.h
* @date       2016/01/31
* @author     Akihiro Sugeno
* @par        Revision
* $Id$
* @par        Copyright
* Copyright 2015 Akihiro Sugeno
* @par        History
*     - 2015/09/21 Akihiro Sugeno
*       -# Initial Version
*/
#ifndef RDS_ER_STATICANALYSIS_SOURCE_PARSER_COMMAND_ANALYSISCOMMANDPARSER_H_
#define RDS_ER_STATICANALYSIS_SOURCE_PARSER_COMMAND_ANALYSISCOMMANDPARSER_H_

#include <stdlib.h>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <map>
#include <string>

#include "../../analysis_command/analysisProjects.h"

/*! AnalysisCommandクラスへのデータ投入関数群 */
typedef map <std::string, void(*)(AnalysisCommand *analysisCommand, std::string value, AttributeMap attr) > SetAnalysisCommandFuc;
/**
* @class AnalysisCommandParser
* 解析命令パーサークラス
* @brief 解析命令ファイルの構文解析を行うクラス
*/
class AnalysisCommandParser
{
private:
	//! コマンドタイプのセット関数
	static void setCommandType(AnalysisCommand *analysisCommand, std::string value, AttributeMap attr);
	//! コマンド名のセット関数
	static void setCommandName(AnalysisCommand *analysisCommand, std::string value, AttributeMap attr);
	//! 検査対象のセット関数
	static void addTarget(AnalysisCommand *analysisCommand, std::string value, AttributeMap attr);
	//! 検査に使用する文字列のセット関数
	static void addValue(AnalysisCommand *analysisCommand, std::string value, AttributeMap attr);
	//! 検出時に表示するメッセージのセット関数
	static void setOutputMessage(AnalysisCommand *analysisCommand, std::string value, AttributeMap attr);
	//! 検出時のアラート情報のセット関数
	static void setAlertLevel(AnalysisCommand *analysisCommand, std::string value, AttributeMap attr);
	//! 解析命令有効フラグセット関数
	static void setEnableFlg(AnalysisCommand *analysisCommand, std::string value, AttributeMap attr);
	//! 検査テーブル抽出用正規表現パターンセット関数
	static void addTargetExtractionParam(AnalysisCommand *analysisCommand, std::string value, AttributeMap attr);
	//! テーブルオプション抽出用正規表現パターンセット関数
	static void addTableOptionParam(AnalysisCommand *analysisCommand, std::string value, AttributeMap attr);
	//! データ型抽出用正規表現パターンセット関数
	static void addDataTypeParam(AnalysisCommand *analysisCommand, std::string value, AttributeMap attr);
	
protected:
	//! コマンドタイプ識別子
	const std::string COMMAND_TYPE = "CommandType";
	//! コマンド名識別子
	const std::string COMMAND_NAME = "CommandName";
	//! 検出項目識別子
	const std::string COMMAND_TARGET = "Target";
	//! 検出に使用する文字列が格納されているタグの識別子
	const std::string COMMAND_VALUE = "Value";
	//! 出力メッセージタグの識別子
	const std::string COMMAND_OUTPUT_MESSAGE = "OutputMessage";
	//! アラートレベルタグの識別子
	const std::string COMMAND_ALERT_LEVEL = "OutputMessageAlertLevel";
	//! 命令実行有効フラグタグの識別子
	const std::string COMMAND_ENABLE_FLG = "Enable";
	//! 検査対象テーブル抽出正規表現情報格納タグの識別子
	const std::string COMMAND_TARGET_EXTRACTION_PARAM = "TargetExtractionParam";
	//! テーブルオプション報格納用タグの識別子
	const std::string COMMAND_TABLE_OPTION = "TableOption";
	//! データ型情報格納用タグの識別子
	const std::string COMMAND_DATA_TYPE = "DataType";
	//! 解析命令インスタンスにパラメータを登録する関数群を格納する変数
	SetAnalysisCommandFuc setAnalysisCommandFuc;

public:
	//! コンストラクタ
	AnalysisCommandParser()
	{
		this->setParams();
	}
	/*!< 解析命令ファイルのパース関数 */
	virtual class AnalysisProjects * parse(void){
		return NULL;
	};

	//! 解析命令インスタンスパラメータセット関数のmap情報生成関数
	void setParams();

};

#endif //RDS_ER_STATICANALYSIS_SOURCE_PARSER_COMMAND_ANALYSISCOMMANDPARSER_H_