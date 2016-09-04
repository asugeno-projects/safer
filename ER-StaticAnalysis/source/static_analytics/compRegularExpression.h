/**
* @file       compRegularExpression.h
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

#ifndef RDS_ER_STATICANALYSIS_SOURCE_STATIC_ANALYTICS_COMPREGULAREXPRESSION_H_
#define RDS_ER_STATICANALYSIS_SOURCE_STATIC_ANALYTICS_COMPREGULAREXPRESSION_H_

#include <regex>
#include <list>
#include <string>

#include "../ER/er.h"
#include "../analysis_command/AnalysisProjects.h"
#include "../reporter/reporter.h"

/*! @class CompRegularExpression
* @brief ER情報用正規表現比較クラス
* @details 正規表現を用いてER情報の分析を行います。
*/
class CompRegularExpression
{
private:
public:
	//! 正規表現を用いた比較処理関数
	void run(AnalysisCommand *analysisCommand, ER *er);
	//! 物理テーブル名を対象とした比較関数
	void physicalTableNameCheck(AnalysisCommand *analysisCommand, Entity *entity, std::wregex pattern);
	//! 論理テーブル名を対象とした比較関数
	void logicalTableNameCheck(AnalysisCommand *analysisCommand, Entity *entity, std::wregex pattern);
	//! 物理Column名を対象とした比較関数
	void physicalColumnNameCheck(AnalysisCommand *analysisCommand, Entity *entity, std::wregex pattern);
	//! 論理Column名を対象とした比較関数
	void logicalColumnNameCheck(AnalysisCommand *analysisCommand, Entity *entity, std::wregex pattern);
	//! テーブルコメントを対象とした比較関数
	void tableCommentCheck(AnalysisCommand *analysisCommand, Entity *entity, std::wregex pattern);
	//! カラムコメントを対象とした比較関数
	void columnCommentCheck(AnalysisCommand *analysisCommand, Entity *entity, std::wregex pattern);
	//! 正規表現を用いた比較関数
	void compString(AnalysisCommand *analysisCommand, std::wstring tableName, std::wstring compString, std::wstring targetName, std::wregex pattern);
	//! 比較用正規表現変換関数
	static std::wregex CompRegularExpression::getStringsToWOrPattern(std::list<std::string> values);
	//! テーブル比較用正規表現変換関数
	static std::wregex CompRegularExpression::getExtractPattern(std::list<std::string> targetExtractionParams);
};

#endif //RDS_ER_STATICANALYSIS_SOURCE_STATIC_ANALYTICS_COMPREGULAREXPRESSION_H_