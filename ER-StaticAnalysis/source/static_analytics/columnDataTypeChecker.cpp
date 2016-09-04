/**
* @file       columnDataTypeChecker.cpp
* @date       2016/01/31
* @author     Akihiro Sugeno
* @par        Revision
* $Id$
* @par        Copyright
* Copyright 2015 Akihiro Sugeno
* @par        History
*     - 2015/10/15 Akihiro Sugeno
*       -# Initial Version
*/

#include "columnDataTypeChecker.h"
#include "../static_analytics/compRegularExpression.h"
#include "../analysis_command/analysisCommand.h"
#include <list>
#include <map>
#include <regex>

/**
* @fn void ColumnDataTypeChecker::run(AnalysisCommandTypeCheck *analysisCommand, ER *er)
* @brief 解析処理実行関数(Columnのデータ型判定)
* @param analysisCommand 解析命令インスタンス
* @param er ER情報格納インスタンスのポインタ変数
* @details 解析処理の実行を行う。<br>データ型が検出対象のものかを判定し、検出対象であればrepoterに報告情報を追加する
*/
void ColumnDataTypeChecker::run(AnalysisCommandTypeCheck *analysisCommand, ER *er)
{
	//判定用データ型リストを取得
	std::list<DataType> dataTypes = analysisCommand->getDataTypes();
	//判定用テーブルオプションリストを取得
	std::list<TableOption> tableOptions = analysisCommand->getTableOptions();
	
	//対象テーブル判定用正規表現パターンインスタンスの生成
	std::wregex extractPatternForTable = CompRegularExpression::getExtractPattern(analysisCommand->getTargetExtractionParams(SECTION_TABLE));
	std::wregex extractPatternForTab = CompRegularExpression::getExtractPattern(analysisCommand->getTargetExtractionParams(SECTION_TAG));
	//Column検出正規表現パターンインスタンスの生成
	std::wregex columnPattern = CompRegularExpression::getStringsToWOrPattern(analysisCommand->getValues());

	//Entityの数だけループ処理
	for (std::list<Entity *>::iterator entityIt = er->entitys.begin(); entityIt != er->entitys.end(); entityIt++)
	{
		//検出対象のテーブルか判定
		if (!regex_match((*entityIt)->getPhysicalTableName().c_str(), extractPatternForTable) &&
			!regex_match((*entityIt)->getTagName().c_str(), extractPatternForTab))
		{
			//対象テーブルでない場合、処理をスキップ
			continue;
		}

		//EntityからColumn情報を取得
		auto fields = (*entityIt)->getfields();

		//Columnの数だけループ処理
		for (auto fieldIt = fields.begin(); fieldIt != fields.end(); fieldIt++)
		{
			//検出対象カラム判定
			if (!regex_match((*fieldIt).name.physicalName.c_str(), columnPattern))
			{
				//対象Columnじゃない場合は飛ばす
				continue;
			}

			//データカラムの判定処理
			if (!this->chackForDataTypeList(dataTypes, (*fieldIt).type))
			{
				//検出対象でなければ処理終了
				continue;
			}

			//Table Option判定
			if (!this->chackFoTableOptionList(tableOptions, (*fieldIt).DDLOptions))
			{
				//検出対象でなければ処理終了
				continue;
			}

			//ここに到達した時点で、検出対象とみなし、レポート情報へ蓄積する
			Reporter::getInstance()->addRecord(analysisCommand, CommandTypeList::E_TypeCheck, (*entityIt)->getPhysicalTableName(), (*fieldIt).name.physicalName);
		}
	}
}

/**
* @fn bool ColumnDataTypeChecker::checkForJudgment(int *commandCount, int *hitCount)
* @brief コマンド実行回数とヒット回数を比較し、検出対象か判定する関数
* @param commandCount 命令実行回数のカウンター用配列
* @param hitCount 判定回数のカウンター用配列
* @return 命令実行回数カウンターと判定回数カウンターを比較し、<br>処理されたデータが検出対象だったものか判定し結果をbool型で返す
* @details 
*/
bool ColumnDataTypeChecker::checkForJudgment(int *commandCount, int *hitCount)
{
	//判定確認
	if (commandCount[LogicalOperationTypeList::E_And] != hitCount[LogicalOperationTypeList::E_And])
	{
		//AND条件判定試行回数と判定数が一致しない場合、条件未達(false)
		return false;
	}
	if (commandCount[LogicalOperationTypeList::E_NotAnd] != hitCount[LogicalOperationTypeList::E_NotAnd])
	{
		//NOT AND条件判定試行回数と判定数が一致しない場合、条件未達(false)
		return false;
	}
	if (commandCount[LogicalOperationTypeList::E_Or] > 0 &&
		hitCount[LogicalOperationTypeList::E_Or] == 0)
	{
		//OR判定条件にて、判定試行回数が1以上かつ判定回数が1以上では無いとき、
		//条件未達(false)
		return false;
	}
	if (commandCount[LogicalOperationTypeList::E_NotOr] > 0 &&
		hitCount[LogicalOperationTypeList::E_NotOr] != 0)
	{
		//OR判定条件にて、判定試行回数が1以上かつ判定回数が1以上であるとき、
		//条件未達にて、次のループ処理を開始(false)
		return false;
	}
	return true;
}

/**
* @fn void ColumnDataTypeChecker::execMatchForColumnType(int *commandCount, int *hitCount, std::string patternString, LogicalOperationTypeList logicalOperationType, std::wstring value)
* @brief Columnのデータ内容を正規表現で比較と論理演算子情報から成否を判定する関数
* @param commandCount 命令実行回数のカウンター用配列
* @param hitCount 判定回数のカウンター用配列
* @param patternString 判定用正規表現を定義した文字列
* @param logicalOperationType 判定用論理演算パラメータ
* @param value 検出対象文字列
* @details Columnのデータ内容を正規表現で比較と論理演算子情報から成否を判定しカウンター変数をカウントアップさせます
*/
void ColumnDataTypeChecker::execMatchForColumnType(int *commandCount, int *hitCount, std::string patternString, LogicalOperationTypeList logicalOperationType, std::wstring value)
{
	//受け取った正規表現用文字列を正規表現インスタンスへ変換
	wstring ws;
	stringToWString(ws, patternString);
	std::wregex pattern(ws);

	//対象のデータが条件にマッチするか判定
	bool flg = regex_match(value.c_str(), pattern);

	//正規表現でマッチするかどうかを判定したフラグと
	//論理演算の判定を組み合わせ両条件に一致したものを
	//検出と判定されたものとし回数をカウントする
	switch (logicalOperationType)
	{
	case LogicalOperationTypeList::E_And://AND条件処理
	case LogicalOperationTypeList::E_Or://OR条件処理
		commandCount[logicalOperationType]++;
		if (flg)
		{
			//条件でtrueが出た場合判定加算
			hitCount[logicalOperationType]++;
		}
		break;
	case LogicalOperationTypeList::E_NotAnd://NOT AND処理
	case LogicalOperationTypeList::E_NotOr://NOT OR処理
		commandCount[logicalOperationType]++;
		if (!flg)
		{
			//NOT条件でfalseが出た場合判定加算
			hitCount[logicalOperationType]++;
		}
		break;
	}
}

/**
* @fn bool ColumnDataTypeChecker::chackForDataTypeList(std::list<DataType> dataTypeList, std::wstring targetString)
* @brief 対象のデータ型の検出対象かチェックを行う関数
* @param dataTypeList 判定用データ型リスト
* @param targetString 検査対象データ型
* @return 検出対象か判定した結果をboolで返す
* @details 対象のデータ型の検出対象かチェックを行う関数
*/
bool ColumnDataTypeChecker::chackForDataTypeList(std::list<DataType> dataTypeList, std::wstring targetString)
{
	//フラグ初期化
	int hitCount[LogicalOperationTypeList::E_LogicalOperationTypeMax] = { 0 };
	int exeCount[LogicalOperationTypeList::E_LogicalOperationTypeMax] = { 0 };


	//データ型リストの数だけループ処理
	for (auto dataType = dataTypeList.begin(); dataType != dataTypeList.end(); dataType++)
	{
		//対象のデータ型の検出対象かチェックを行う
		this->execMatchForColumnType(exeCount,
			hitCount,
			(*dataType).dataType,
			(*dataType).logicalOperation,
			targetString);
	}

	//判定確認
	if (!this->checkForJudgment(exeCount, hitCount))
	{
		//検出対象出なかった場合呼び出し元にfalseを返す
		return false;
	}
	return true;
}

/**
* @fn bool ColumnDataTypeChecker::chackFoTableOptionList(std::list<TableOption> tableOptions, std::list<std::wstring> DdlOptions)
* @brief 対象のColumnのテーブルオプションが検査リストと一致するかチェックを行う関数
* @param tableOptions 判定用テーブルオプションリスト
* @param DdlOptions DDLオプション
* @return 検出対象か判定した結果をboolで返す
* @details 対象のColumnのテーブルオプションが検査リストと一致するかチェックを行う関数
*/
bool ColumnDataTypeChecker::chackFoTableOptionList(std::list<TableOption> tableOptions, std::list<std::wstring> DdlOptions)
{
	//フラグ初期化
	int tableOptionHitCount[LogicalOperationTypeList::E_LogicalOperationTypeMax] = { 0 };
	int tableOptionExeCount[LogicalOperationTypeList::E_LogicalOperationTypeMax] = { 0 };

	//DDLオプションの数だけループ処理
	for (auto ddlOptionIt = DdlOptions.begin(); ddlOptionIt != DdlOptions.end(); ddlOptionIt++)
	{
		//テーブルにセットされたオプションの数だけループ処理
		for (auto tableOptionIt = tableOptions.begin(); tableOptionIt != tableOptions.end(); tableOptionIt++)
		{
			//対象のテーブルオプションが検出対象のものかチェックを行う
			this->execMatchForColumnType(tableOptionExeCount,
				tableOptionHitCount,
				TableOptionToLabel[(*tableOptionIt).tableOption],
				(*tableOptionIt).logicalOperation,
				(*ddlOptionIt));
		}
	}

	//判定確認
	if (!this->checkForJudgment(tableOptionExeCount, tableOptionHitCount))
	{
		//検出対象出なかった場合処理、呼び出し元にfalseを返す
		return false;
	}
	return true;
}