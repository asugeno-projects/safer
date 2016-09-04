/**
* @file       columnDataTypeChecker.h
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

#ifndef RDS_ER_STATICANALYSIS_SOURCE_STATIC_ANALYTICS_COLUMNDATATYPECHECKER_H_
#define RDS_ER_STATICANALYSIS_SOURCE_STATIC_ANALYTICS_COLUMNDATATYPECHECKER_H_

#include <vector>
#include <string>
#include <list>
#include "../reporter/reporter.h"

/*! @class ColumnDataTypeChecker
* @brief Columnデータ型チェッカークラス
* @details 正規表現を用いてER情報の分析を行います。
*/

class ColumnDataTypeChecker
{
private:
public:
	//! 解析処理実行関数(Columnのデータ型判定)
	void run(AnalysisCommandTypeCheck *analysisCommand, ER *er);

	//! コマンド実行回数とヒット回数を比較し、検出対象か判定する関数
	bool checkForJudgment(int *commandCount, int *hitCount);

	//! Columnのデータ内容を正規表現で比較と論理演算子情報から成否を判定する関数
	void execMatchForColumnType(int *commandCount, int *hitCount, std::string patternString, LogicalOperationTypeList logicalOperationType, std::wstring value);

	//! 対象のデータ型の検出対象かチェックを行う関数
	bool chackForDataTypeList(std::list<DataType> dataTypeList, std::wstring targetString);

	//! 対象のColumnのテーブルオプションが検査リストと一致するかチェックを行う関数
	bool chackFoTableOptionList(std::list<TableOption> tableOptionList, std::list<std::wstring> DdlOptions);
};

#endif //RDS_ER_STATICANALYSIS_SOURCE_STATIC_ANALYTICS_COLUMNDATATYPECHECKER_H_