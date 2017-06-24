/**
* @file       analysisCommand.h
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

#ifndef RDS_ER_STATICANALYSIS_SOURCE_ANALYSIS_COMMAND_ANALYSISCOMMAND_H_
#define RDS_ER_STATICANALYSIS_SOURCE_ANALYSIS_COMMAND_ANALYSISCOMMAND_H_

#include <iostream>
#include <string>
#include <list>
#include <map>
#include "../common/commonString.h"
#include "../common/commonXml.h"
#include "Xlsx/SimpleXlsxDef.h"

using namespace std;

/**
 * @enum TargetList
 * 比較対象リスト
 */
enum TargetList{ 
	//! 物理カラム名
	E_PhysicalColumnName, 

	//! 論理カラム名
	E_LogicalColumnName,

	//! 物理テーブル名
	E_PhysicalTableName,

	//! 論理テーブル名
	E_LogicalTableName,

	//! テーブルコメント
	E_TableComment,

	//! カラムコメント
	E_ColumnComment,

	//! インデックス
	E_Index,

	//! リレーションシップ
	E_Relationship,

	//! ターゲットタイプ最大数
	E_TargetMax
};

/**
 * @enum CommandTypeList
 * コマンドタイプリスト
 */
enum CommandTypeList{ 
	//! 比較
	E_Comparison,

	//! 型チェック
	E_TypeCheck,

	//! スペルチェック
	E_SpellCheck,

	//! リレーションシップチェック
	E_RelationshipCheck,

	//! インデックスチェック
	E_IndexCheck,

	//! ER図設計チェック
	E_ErDesignCheck,

	//! コマンドタイプ最大数
	E_CommandTypeMax
};

/**
* @enum AlertLevelList
* アラートレベルリスト
*/
enum AlertLevelList{ 
	//! ノート 情報
	E_note,

	//! ワーニング 注意
	E_warning,

	//! クリティカル 警告
	E_critical,

	//! アラート設定最大数
	E_AlertMax
};

/**
* @enum DataTypeList
* データタイプリスト
*/
enum DataTypeList{
	//! CHAR型
	E_CHAR,

	//! VARCHAR型
	E_VARCHAR,

	//! DECIMAL型
	E_DECIMAL,

	//! INT型
	E_INT,

	//! TINYINT型
	E_TINYINT,

	//! SMALLINT型
	E_SMALLINT,

	//! MEDIUMINT型
	E_MEDIUMINT,

	//! BIGINT型
	E_BIGINT,

	//! FLOAT型
	E_FLOAT,

	//! DOUBLE型
	E_DOUBLE,

	//! DATE型
	E_DATE,

	//! DATETIME型
	E_DATETIME,

	//! TIMESTAMP型
	E_TIMESTAMP,

	//! TIME型
	E_TIME,

	//! YEAR型
	E_YEAR,

	//! BINARY型
	E_BINARY,

	//! VARBINARY型
	E_VARBINARY,

	//! BLOB型
	E_BLOB,

	//! TINYBLOB型
	E_TINYBLOB,

	//! MEDIUMBLOB型
	E_MEDIUMBLOB,

	//! LONGBLOB型
	E_LONGBLOB,

	//! TINYTEXT型
	E_TINYTEXT,

	//! TEXT型
	E_TEXT,

	//! MEDIUMTEXT型
	E_MEDIUMTEXT,

	//! LONGTEXT型
	E_LONGTEXT,

	//! ENUM型
	E_ENUM,

	//! SET型
	E_SET,

	//! DataType最大数
	E_DataTypeMax
};

/**
* @enum TableOptionTypeList
* Table Optionタイプリスト
*/
enum TableOptionTypeList{
	//! Auto Increment
	E_AutoIncrement,

	//! on update CURRENT_TIMESTAMP
	E_OnUpdateCurrentTimestamp,

	//! NOT NULL成約
	E_NotNull,

	//! Table Option最大数
	E_TableOptionTypeMax,
};

/**
* @enum LogicalOperationTypeList
* Table 論理演算タイプリスト
*/
enum LogicalOperationTypeList{
	//! AND
	E_And,

	//! OR
	E_Or,

	//! NOT
	E_Not,

	//! NAND
	E_NotAnd,

	//! NOR
	E_NotOr,

	//! LogicalOperationTypeListアイテム最大数
	E_LogicalOperationTypeMax,
};


typedef pair<std::string, CommandTypeList> CommandTypeListItem;
/*! コマンドタイプ名と定数を紐付ける連想配列 */
static map<std::string, CommandTypeList> CommandTypeLabelList = {
	CommandTypeListItem("Comparison", CommandTypeList::E_Comparison),
	CommandTypeListItem("TypeCheck", CommandTypeList::E_TypeCheck)
};

typedef pair<CommandTypeList, std::string> CommandTypeName;
/*! コマンドタイプ名と定数を紐付ける連想配列 */
static map<CommandTypeList, std::string> CommandTypeNameList = {
	CommandTypeName(CommandTypeList::E_Comparison, "Comparison"),
	CommandTypeName(CommandTypeList::E_TypeCheck, "TypeCheck")
};

typedef pair<CommandTypeList, _tstring> CommandTypeLogicalName;
/*! コマンドタイプ名と定数を紐付ける連想配列 */
static map<CommandTypeList, _tstring> CommandTypeLogicalNameList = {
	CommandTypeLogicalName(CommandTypeList::E_Comparison, _T("Regex")),
	CommandTypeLogicalName(CommandTypeList::E_TypeCheck, _T("Data Type")),
	CommandTypeLogicalName(CommandTypeList::E_SpellCheck, _T("Spell")),
	CommandTypeLogicalName(CommandTypeList::E_RelationshipCheck, _T("Relationship")),
	CommandTypeLogicalName(CommandTypeList::E_IndexCheck, _T("Index")),
	CommandTypeLogicalName(CommandTypeList::E_ErDesignCheck, _T("ER design"))
};
/* Mac版にて日本語がどうしても化けてしまうため、シート名を英語表現に変更 */

typedef pair<std::string, AlertLevelList> AlertLevelListItem;
/*! コマンドタイプ名と定数を紐付ける連想配列 */
static map<std::string, AlertLevelList> AlertLevelLabelList = {
	AlertLevelListItem("note", AlertLevelList::E_note),
	AlertLevelListItem("warning", AlertLevelList::E_warning),
	AlertLevelListItem("critical", AlertLevelList::E_critical)
};

typedef pair<AlertLevelList, std::string> AlertLevelToMessage;
/*! コマンドタイプ名と定数を紐付ける連想配列 */
static map<AlertLevelList, std::string> AlertLevelToLabel = {
	AlertLevelToMessage(AlertLevelList::E_note, "note"),
	AlertLevelToMessage(AlertLevelList::E_warning, "warning"),
	AlertLevelToMessage(AlertLevelList::E_critical, "critical")
};


typedef pair<std::string, TargetList> TargetListItem;
/*! 検査対象と定数を紐付ける連想配列 */
static map<std::string, TargetList> targetList = {
	TargetListItem("PhysicalTableName", TargetList::E_PhysicalTableName),
	TargetListItem("LogicalTableName", TargetList::E_LogicalTableName),
	TargetListItem("PhysicalColumnName", TargetList::E_PhysicalColumnName),
	TargetListItem("LogicalColumnName", TargetList::E_LogicalColumnName),
	TargetListItem("TableComment", TargetList::E_TableComment),
	TargetListItem("ColumnComment", TargetList::E_ColumnComment),
	TargetListItem("Index", TargetList::E_Index),
	TargetListItem("Relationship", TargetList::E_Relationship)
};

const std::string PROJECT_TAG = "Project";
const std::string PROJECT_ID_ATTRIBUTE = "id";
const std::string PROJECT_NAME_TAG = "ProjectName";
const std::string PLAN_TAG = "Plan";
const std::string SPELLCHECK_TAG = "SpellCheckConfig";
const std::string COMMAND_ID_ATTRIBUTE = "id";
const std::string LOGICAL_OPERATION_ATTRIBUTE = "logicalOperation";
const std::string SECTION_ATTRIBUTE = "section";

/* 現在未使用
typedef pair<DataTypeList, std::string> DataTypeToMessage;
//! コマンドタイプ名と定数を紐付ける連想配列 
static map<DataTypeList, std::string> DataTypeToLabel = {
	DataTypeToMessage(DataTypeList::E_CHAR,       "CHAR"),
	DataTypeToMessage(DataTypeList::E_VARCHAR,    "VARCHAR"),
	DataTypeToMessage(DataTypeList::E_DECIMAL,    "DECIMAL"),
	DataTypeToMessage(DataTypeList::E_FLOAT,      "FLOAT"),
	DataTypeToMessage(DataTypeList::E_DOUBLE,     "DOUBLE"),
	DataTypeToMessage(DataTypeList::E_INT,        "INT"),
	DataTypeToMessage(DataTypeList::E_TINYINT,    "TINYINT"),
	DataTypeToMessage(DataTypeList::E_SMALLINT,   "SMALLINT"),
	DataTypeToMessage(DataTypeList::E_MEDIUMINT,  "MEDIUMINT"),
	DataTypeToMessage(DataTypeList::E_BIGINT,     "BIGINT"),
	DataTypeToMessage(DataTypeList::E_TEXT,       "TEXT"),
	DataTypeToMessage(DataTypeList::E_TINYTEXT,   "TINYTEXT"),
	DataTypeToMessage(DataTypeList::E_LONGTEXT,   "LONGTEXT"),
	DataTypeToMessage(DataTypeList::E_BLOB,       "BLOB"),
	DataTypeToMessage(DataTypeList::E_TINYBLOB,   "TINYBLOB"),
	DataTypeToMessage(DataTypeList::E_MEDIUMBLOB, "MEDIUMBLOB"),
	DataTypeToMessage(DataTypeList::E_LONGBLOB,   "LONGBLOB"),
	DataTypeToMessage(DataTypeList::E_TIME,       "TIME"),
	DataTypeToMessage(DataTypeList::E_TIMESTAMP,  "TIMESTAMP"),
	DataTypeToMessage(DataTypeList::E_DATE,       "DATE"),
	DataTypeToMessage(DataTypeList::E_DATETIME,   "DATETIME"),
	DataTypeToMessage(DataTypeList::E_YEAR,       "YEAR"),
	DataTypeToMessage(DataTypeList::E_BINARY,     "BINARY"),
	DataTypeToMessage(DataTypeList::E_VARBINARY,  "VARBINARY"),
	DataTypeToMessage(DataTypeList::E_SET,        "SET"),
	DataTypeToMessage(DataTypeList::E_ENUM,       "ENUM"),
};

typedef pair<std::string, DataTypeList> DataTypeItem;
//! コマンドタイプ名と定数を紐付ける連想配列 
static map<std::string, DataTypeList> DataTypeItemList = {
	DataTypeItem("CHAR",       DataTypeList::E_CHAR),
	DataTypeItem("VARCHAR",    DataTypeList::E_VARCHAR),
	DataTypeItem("DECIMAL",    DataTypeList::E_DECIMAL),
	DataTypeItem("FLOAT",      DataTypeList::E_FLOAT),
	DataTypeItem("DOUBLE",     DataTypeList::E_DOUBLE),
	DataTypeItem("INT",        DataTypeList::E_INT),
	DataTypeItem("TINYINT",    DataTypeList::E_TINYINT),
	DataTypeItem("SMALLINT",   DataTypeList::E_SMALLINT),
	DataTypeItem("MEDIUMINT",  DataTypeList::E_MEDIUMINT),
	DataTypeItem("BIGINT",     DataTypeList::E_BIGINT),
	DataTypeItem("TEXT",       DataTypeList::E_TEXT),
	DataTypeItem("TINYTEXT",   DataTypeList::E_TINYTEXT),
	DataTypeItem("LONGTEXT",   DataTypeList::E_LONGTEXT),
	DataTypeItem("BLOB",       DataTypeList::E_BLOB),
	DataTypeItem("TINYBLOB",   DataTypeList::E_TINYBLOB),
	DataTypeItem("MEDIUMBLOB", DataTypeList::E_MEDIUMBLOB),
	DataTypeItem("LONGBLOB",   DataTypeList::E_LONGBLOB),
	DataTypeItem("TIME",       DataTypeList::E_TIME),
	DataTypeItem("TIMESTAMP",  DataTypeList::E_TIMESTAMP),
	DataTypeItem("DATE",       DataTypeList::E_DATE),
	DataTypeItem("DATETIME",   DataTypeList::E_DATETIME),
	DataTypeItem("YEAR",       DataTypeList::E_YEAR),
	DataTypeItem("BINARY",     DataTypeList::E_BINARY),
	DataTypeItem("VARBINARY",  DataTypeList::E_VARBINARY),
	DataTypeItem("SET",        DataTypeList::E_SET),
	DataTypeItem("ENUM",       DataTypeList::E_ENUM),
};
*/

typedef pair<TableOptionTypeList, std::string> TableOptionToMessage;
/*! コマンドタイプ名と定数を紐付ける連想配列 */
static map<TableOptionTypeList, std::string> TableOptionToLabel = {
	TableOptionToMessage(TableOptionTypeList::E_AutoIncrement, "AUTO_INCREMENT"),
	TableOptionToMessage(TableOptionTypeList::E_NotNull, "NOT NULL"),
	TableOptionToMessage(TableOptionTypeList::E_OnUpdateCurrentTimestamp, "ON UPDATE CURRENT TIMESTAMP"),
};

typedef pair<std::string, TableOptionTypeList> TableOptionItem;
static map<std::string, TableOptionTypeList> TableOptionTypeItemList = {
	TableOptionItem("AUTO_INCREMENT", TableOptionTypeList::E_AutoIncrement),
	TableOptionItem("NOT NULL", TableOptionTypeList::E_NotNull),
	TableOptionItem("ON UPDATE CURRENT TIMESTAMP", TableOptionTypeList::E_OnUpdateCurrentTimestamp),
};

typedef pair<std::string, LogicalOperationTypeList> LogicalOperationTypeItem;
/*! コマンドタイプ名と定数を紐付ける連想配列 */
static map<std::string, LogicalOperationTypeList> StringToLogicalOperationType = {
	LogicalOperationTypeItem("AND", LogicalOperationTypeList::E_And),
	LogicalOperationTypeItem("OR", LogicalOperationTypeList::E_Or),
	LogicalOperationTypeItem("NOT",LogicalOperationTypeList::E_Not),
	LogicalOperationTypeItem("NAND", LogicalOperationTypeList::E_NotAnd),
	LogicalOperationTypeItem("NOR", LogicalOperationTypeList::E_NotOr),
};

struct DataType{
	//! Data識別子
	string dataType;

	//! 論理演算子Type
	LogicalOperationTypeList logicalOperation;
};

struct TableOption{
	//! Table Option
	TableOptionTypeList tableOption;

	//! 論理演算子Type
	LogicalOperationTypeList logicalOperation;
};

struct ExtractionParam{
	string targetName;
	string section;
};

const std::string SECTION_TABLE = "TABLE";
const std::string SECTION_TAG = "TAG";

/*! @class AnalysisCommand
@brief 解析命令に関する情報を格納するクラス
*/
class AnalysisCommand
{
protected:
	//! コマンドID
	int id;

	//! コマンド名
	string commandName;

	//! コマンドタイプ
	CommandTypeList commandType;

	//! ターゲットリスト
	std::list<TargetList> targets;

	//! 値リスト
	std::list<std::string> values;

	//! 対象抽出値リスト
	map<std::string, std::list<std::string>> targetExtractionParams;

	//! 検出時出力メッセージ
	string outputMessage;

	//! ログレベル
	AlertLevelList outputMessageAlertLevel;

	//! 有効フラグ
	bool enable;

public:
	//! ID setter
	void setId(const int id){ this->id = id; };

	//! Id getter
	int getId(){ return this->id; }; 

	//! CommandName setter
	void setCommandName(const std::string commandName){ this->commandName = commandName; };

	//! CommandName getter
	string getCommandName(){ return this->commandName; };

	//! CommandType setter
	void setCommandType(const CommandTypeList commandType){ this->commandType = commandType; };

	//! CommandType getter
	CommandTypeList getCommandType(){ return this->commandType; };

	//! Target Add
	void addTarget(const TargetList target){ this->targets.push_back(target); };

	//! Targets getter
	std::list<TargetList> getTargets(){ return this->targets; };

	//! Value Add
	void addValue(const std::string value){ this->values.push_back(value); };

	//! Values getter
	std::list<std::string> getValues(){ return this->values; };

	//! Output Message setter
	void setOutputMessage(const std::string outputMessage){ this->outputMessage = outputMessage; };

	//! Output Message getter
	string getOutputMessage(){ return this->outputMessage; };

	//! Message Alert Level setter
	void setMessageAlertLevel(const AlertLevelList outputMessageAlertLevel){ this->outputMessageAlertLevel = outputMessageAlertLevel; };

	//! Message Alert Level getter
	AlertLevelList getMessageAlertLevel(){ return this->outputMessageAlertLevel; };

	//! Enable Flg setter
	void setEnableFlg(bool flg){ this->enable = flg; };

	//! Enable Flg getter
	bool getEnableFlg(){ return this->enable; };

	//! Target Extraction Param setter
	void addTargetExtractionParam(ExtractionParam param){ this->targetExtractionParams[param.targetName].push_back(param.section); };

	//! Target Extraction Param getter
	std::list<std::string> getTargetExtractionParams(std::string section){ return this->targetExtractionParams[section]; };
};

/*! @class AnalysisCommand
@brief 解析命令に関する情報を格納するクラス
*/
class AnalysisCommandTypeCheck : public AnalysisCommand
{
private:
	//! データ型リスト
	std::list<DataType> dataTypes;// 正規表現パターンも許可するためにstringへ

	//テーブルオプションリスト
	std::list<TableOption> tableOptions;

public:
	//! DataType setter
	void addDateType(DataType _dataType){ this->dataTypes.push_back(_dataType); };

	//! DataType getter
	std::list<DataType> getDataTypes(){ return this->dataTypes; };

	//! Table Option setter
	void addTableOption(TableOption _tableOption){ this->tableOptions.push_back(_tableOption); };


	//! Table Option getter
	std::list<TableOption> getTableOptions(){ return this->tableOptions; };

};

#endif //RDS_ER_STATICANALYSIS_SOURCE_ANALYSIS_COMMAND_ANALYSISCOMMAND_H_