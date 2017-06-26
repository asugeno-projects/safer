/**
* @file       analysisCommandPaser.cpp
* @date       2016/01/31
* @author     Akihiro Sugeno
* @par        Revision
* $Id$
* @par        Copyright
* Copyright 2015 Akihiro Sugeno
* @par        History
*     - 2015/10/07 Akihiro Sugeno
*       -# Initial Version
*/

#include "../../parser/command/analysisCommandParser.h"

/**
* @fn void AnalysisCommandParser::setCommandType(AnalysisCommand *analysisCommand, std::string value, AttributeMap attr)
* @brief コマンドタイプのセット関数
* @param analysisCommand 解析命令コマンドインスタンス
* @param value コマンドタイプ識別子
* @param attr 属性値
* @details コマンドタイプをanalysisCommandに登録します。
*/
void AnalysisCommandParser::setCommandType(AnalysisCommand *analysisCommand, std::string value, AttributeMap attr)
{
	analysisCommand->setCommandType(CommandTypeLabelList[value]);
}

/**
* @fn void AnalysisCommandParser::setCommandName(AnalysisCommand *analysisCommand, std::string value, AttributeMap attr)
* @brief コマンド名のセット関数
* @param analysisCommand 解析命令コマンドインスタンス
* @param value コマンド名
* @param attr 属性値
* @details コマンド名をanalysisCommandに登録します。
*/
void AnalysisCommandParser::setCommandName(AnalysisCommand *analysisCommand, std::string value, AttributeMap attr)
{
	analysisCommand->setCommandName(value);
}

/**
* @fn void AnalysisCommandParser::addTarget(AnalysisCommand *analysisCommand, std::string value, AttributeMap attr)
* @brief 検査対象のセット関数
* @param analysisCommand 解析命令コマンドインスタンス
* @param value 検査対象識別子
* @param attr 属性値
* @details 検査対象をanalysisCommandに登録します。
*/
void AnalysisCommandParser::addTarget(AnalysisCommand *analysisCommand, std::string value, AttributeMap attr)
{
	analysisCommand->addTarget(targetList[value]);
}

/**
* @fn void AnalysisCommandParser::addValue(AnalysisCommand *analysisCommand, std::string value, AttributeMap attr)
* @brief 検査に使用する文字列のセット関数
* @param analysisCommand 解析命令コマンドインスタンス
* @param value 検査に使用する文字列
* @param attr 属性値
* @details 検査に使用する文字列をanalysisCommandに登録します。
*/
void AnalysisCommandParser::addValue(AnalysisCommand *analysisCommand, std::string value, AttributeMap attr)
{
	if (value.length() != 0)
	{
		analysisCommand->addValue(value);
	}
}

/**
* @fn void AnalysisCommandParser::setOutputMessage(AnalysisCommand *analysisCommand, std::string value, AttributeMap attr)
* @brief 検出時に表示するメッセージのセット関数
* @param analysisCommand 解析命令コマンドインスタンス
* @param value 表示メッセージ
* @param attr 属性値
* @details 検出時に表示するメッセージをanalysisCommandに登録します。
*/
void AnalysisCommandParser::setOutputMessage(AnalysisCommand *analysisCommand, std::string value, AttributeMap attr)
{
	analysisCommand->setOutputMessage(value);
}

/**
* @fn void AnalysisCommandParser::setAlertLevel(AnalysisCommand *analysisCommand, std::string value, AttributeMap attr)
* @brief 検出時のアラート情報のセット関数
* @param analysisCommand 解析命令コマンドインスタンス
* @param value 検出時のアラート情報識別子
* @param attr 属性値
* @details 検出時のアラート情報をanalysisCommandに登録します。
*/
void AnalysisCommandParser::setAlertLevel(AnalysisCommand *analysisCommand, std::string value, AttributeMap attr)
{
	analysisCommand->setMessageAlertLevel(AlertLevelLabelList[value]);
}

/**
* @fn void AnalysisCommandParser::setEnableFlg(AnalysisCommand *analysisCommand, std::string value, AttributeMap attr)
* @brief 解析命令有効フラグセット関数
* @param analysisCommand 解析命令コマンドインスタンス
* @param value 解析命令有効フラグ
* @param attr 属性値
* @details 解析命令有効をanalysisCommandに登録します。
*/
void AnalysisCommandParser::setEnableFlg(AnalysisCommand *analysisCommand, std::string value, AttributeMap attr)
{
	analysisCommand->setEnableFlg(boost::lexical_cast<bool>(value));
}

/**
* @fn void AnalysisCommandParser::addTargetExtractionParam(AnalysisCommand *analysisCommand, std::string value, AttributeMap attr)
* @brief 検査テーブル抽出用正規表現パターンセット関数
* @param analysisCommand 解析命令コマンドインスタンス
* @param value 検査テーブル抽出用正規表現パターン文字列
* @param attr 属性値
* @details 検査テーブル抽出用正規表現パターンをanalysisCommandに登録します。
*/
void AnalysisCommandParser::addTargetExtractionParam(AnalysisCommand *analysisCommand, std::string value, AttributeMap attr)
{
	if (value.length() != 0)
	{
		ExtractionParam extractionParam;
		extractionParam.targetName = value;
		extractionParam.section = attr[SECTION_ATTRIBUTE];
		analysisCommand->addTargetExtractionParam(extractionParam);
	}
}

/**
* @fn void AnalysisCommandParser::addTableOptionParam(AnalysisCommand *analysisCommand, std::string value, AttributeMap attr)
* @brief テーブルオプション判定正規表現パターンセット関数
* @param analysisCommand 解析命令コマンドインスタンス
* @param value テーブルオプション判定文字列
* @param attr 属性値
* @details テーブルオプション判定用正規表現パターンをanalysisCommandに登録します。
*/
void AnalysisCommandParser::addTableOptionParam(AnalysisCommand *analysisCommand, std::string value, AttributeMap attr)
{
	TableOption tableOption;
	tableOption.tableOption = TableOptionTypeItemList[value];
	tableOption.logicalOperation = StringToLogicalOperationType[attr[LOGICAL_OPERATION_ATTRIBUTE]];
	((AnalysisCommandTypeCheck *)analysisCommand)->addTableOption(tableOption);
}

/**
* @fn void AnalysisCommandParser::addDataTypeParam(AnalysisCommand *analysisCommand, std::string value, AttributeMap attr)
* @brief データ型判定用正規表現パターンセット関数
* @param analysisCommand 解析命令コマンドインスタンス
* @param value データ型判定用文字列
* @param attr 属性値
* @details データ型判定用正規表現パターンをanalysisCommandに登録します。
*/
void AnalysisCommandParser::addDataTypeParam(AnalysisCommand *analysisCommand, std::string value, AttributeMap attr)
{
	DataType dataType;
	dataType.dataType = value;
	dataType.logicalOperation = StringToLogicalOperationType[attr[LOGICAL_OPERATION_ATTRIBUTE]];
	((AnalysisCommandTypeCheck *)analysisCommand)->addDateType(dataType);
}

/**
* @fn void AnalysisCommandParser::setParams()
* @brief 解析命令インスタンスパラメータセット関数のmap情報生成関数
* @details 解析命令インスタンスのパラメータセット関数をmap変数に登録します。
*/
void AnalysisCommandParser::setParams()
{
	setAnalysisCommandFuc.insert(make_pair(AnalysisCommandParser::COMMAND_TYPE, AnalysisCommandParser::setCommandType));
	setAnalysisCommandFuc.insert(make_pair(AnalysisCommandParser::COMMAND_NAME, AnalysisCommandParser::setCommandName));
	setAnalysisCommandFuc.insert(make_pair(AnalysisCommandParser::COMMAND_TARGET, AnalysisCommandParser::addTarget));
	setAnalysisCommandFuc.insert(make_pair(AnalysisCommandParser::COMMAND_VALUE, AnalysisCommandParser::addValue));
	setAnalysisCommandFuc.insert(make_pair(AnalysisCommandParser::COMMAND_OUTPUT_MESSAGE, AnalysisCommandParser::setOutputMessage));
	setAnalysisCommandFuc.insert(make_pair(AnalysisCommandParser::COMMAND_ALERT_LEVEL, AnalysisCommandParser::setAlertLevel));
	setAnalysisCommandFuc.insert(make_pair(AnalysisCommandParser::COMMAND_ENABLE_FLG, AnalysisCommandParser::setEnableFlg));
	setAnalysisCommandFuc.insert(make_pair(AnalysisCommandParser::COMMAND_TARGET_EXTRACTION_PARAM, AnalysisCommandParser::addTargetExtractionParam));
	setAnalysisCommandFuc.insert(make_pair(AnalysisCommandParser::COMMAND_TABLE_OPTION, AnalysisCommandParser::addTableOptionParam));
	setAnalysisCommandFuc.insert(make_pair(AnalysisCommandParser::COMMAND_DATA_TYPE, AnalysisCommandParser::addDataTypeParam));
}