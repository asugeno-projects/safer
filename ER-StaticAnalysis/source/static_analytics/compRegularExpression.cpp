/**
* @file       compRegularExpression.cpp
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

#include "../static_analytics/compRegularExpression.h"

/**
* @fn void CompRegularExpression::run(AnalysisCommand *analysisCommand, ER *er)
* @brief 正規表現を用いた比較処理関数
* @param analysisCommand 解析命令情報格納インスタンスのポインタ変数
* @param er ER情報格納インスタンスのポインタ変数
* @details ER情報から正規表現を用いてパラメータの文字列比較を行い、検出情報をreporterインスタンスへ格納する
*/
void CompRegularExpression::run(AnalysisCommand *analysisCommand, ER *er)
{
	//検出用正規表現パターンを生成処理
	std::wregex pattern = this->getStringsToWOrPattern(analysisCommand->getValues());

	//検出先を抽出する正規表現パターンの生成処理
	std::wregex extractPatternForTable = this->getExtractPattern(analysisCommand->getTargetExtractionParams(SECTION_TABLE));
	std::wregex extractPatternForTab = this->getExtractPattern(analysisCommand->getTargetExtractionParams(SECTION_TAG));

	//analysisCommandに登録された情報を基にER図情報にヒットする文字列が無いか探索する
	auto targets = analysisCommand->getTargets();
	for (std::list<Entity *>::iterator entityIt = er->entitys.begin(); entityIt != er->entitys.end(); entityIt++)
	{
		//処理対象のテーブルか判定する
		if (!regex_match((*entityIt)->getPhysicalTableName().c_str(), extractPatternForTable) &&
			!regex_match((*entityIt)->getTagName().c_str(), extractPatternForTab))
		{
			//検出対象のテーブルではない場合、処理をスキップ
			continue;
		}

		//正規表現比較対象(テーブル名、Columnなど)の分だけ処理をループさせる
		for (auto targetIt = targets.begin(); targetIt != targets.end(); targetIt++)
		{
			TargetList target = (*targetIt);
			//比較対象ごとに処理を切り替えます。
			switch (target)
			{
			case TargetList::E_PhysicalTableName:
				//比較対象が物理テーブル名の場合の処理
				physicalTableNameCheck(analysisCommand, (*entityIt), pattern);
				break;
			case TargetList::E_LogicalTableName:
				//比較対象が論理テーブル名の場合の処理
				logicalTableNameCheck(analysisCommand, (*entityIt), pattern);
				break;
			case TargetList::E_PhysicalColumnName:
				//比較対象が物理Column名の場合の処理
				physicalColumnNameCheck(analysisCommand, (*entityIt), pattern);
				break;
			case TargetList::E_LogicalColumnName:
				//比較対象が論理カラム名の場合の処理
				logicalColumnNameCheck(analysisCommand, (*entityIt), pattern);
				break;
			case TargetList::E_TableComment:
				//比較対象がテーブルコメントの場合の処理
				tableCommentCheck(analysisCommand, (*entityIt), pattern);
				break;
			case TargetList::E_ColumnComment:
				//比較対象がテーブルコメントの場合の処理
				columnCommentCheck(analysisCommand, (*entityIt), pattern);
				break;
			}
		}
	}
};

/**
* @fn void CompRegularExpression::physicalTableNameCheck(AnalysisCommand *analysisCommand, Entity *entity, std::wregex pattern)
* @brief 物理テーブル名を対象とした比較関数
* @param analysisCommand 解析命令情報格納インスタンスのポインタ変数
* @param entity Entityインスタンスのポインタ変数
* @param pattern 正規表現パターンインスタンスのポインタ変数
* @details 物理テーブル名を対象とした比較処理のため、物理テーブル名をワイド文字に変更します。最終的にはCompRegularExpression::compString関数へ比較に必要なパラメータを渡します。
*/
void CompRegularExpression::physicalTableNameCheck(AnalysisCommand *analysisCommand, Entity *entity, std::wregex pattern)
{
	//正規表現を用いた文字列比較関数処理の実行
	CompRegularExpression::compString(analysisCommand, entity->getPhysicalTableName(), entity->getPhysicalTableName(), entity->getPhysicalTableName(), pattern);
}

/**
* @fn void CompRegularExpression::logicalTableNameCheck(AnalysisCommand *analysisCommand, Entity *entity, std::wregex pattern)
* @brief 論理テーブル名を対象とした比較関数
* @param analysisCommand 解析命令情報格納インスタンスのポインタ変数
* @param entity Entityインスタンスのポインタ変数
* @param pattern 正規表現パターンインスタンスのポインタ変数
* @details 論理テーブル名を対象とした比較処理のため、論理テーブル名をワイド文字に変更します。最終的にはCompRegularExpression::compString関数へ比較に必要なパラメータを渡します。
*/
void CompRegularExpression::logicalTableNameCheck(AnalysisCommand *analysisCommand, Entity *entity, std::wregex pattern)
{
	//正規表現を用いた文字列比較関数処理の実行
	CompRegularExpression::compString(analysisCommand, entity->getPhysicalTableName(), entity->getLogicalTableName(), entity->getLogicalTableName(), pattern);
}

/**
* @fn void CompRegularExpression::physicalColumnNameCheck(AnalysisCommand *analysisCommand, Entity *entity, std::wregex pattern)
* @brief 物理Column名を対象とした比較関数
* @param analysisCommand 解析命令情報格納インスタンスのポインタ変数
* @param entity Entityインスタンスのポインタ変数
* @param pattern 正規表現パターンインスタンスのポインタ変数
* @details 物理Column名を対象とした比較処理のため、物理Column名をワイド文字に変更します。最終的にはCompRegularExpression::compString関数へ比較に必要なパラメータを渡します。
*/
void CompRegularExpression::physicalColumnNameCheck(AnalysisCommand *analysisCommand, Entity *entity, std::wregex pattern)
{
	auto fields = entity->getfields();
	for (auto field = fields.begin(); field != fields.end(); field++)
	{
		//正規表現を用いた文字列比較関数処理の実行
		CompRegularExpression::compString(analysisCommand, entity->getPhysicalTableName(), (*field).name.physicalName, (*field).name.physicalName, pattern);
	}
}

/**
* @fn void CompRegularExpression::logicalColumnNameCheck(AnalysisCommand *analysisCommand, Entity *entity, std::wregex pattern)
* @brief 論理Column名を対象とした比較関数
* @param analysisCommand 解析命令情報格納インスタンスのポインタ変数
* @param entity Entityインスタンスのポインタ変数
* @param pattern 正規表現パターンインスタンスのポインタ変数
* @details 論理Column名を対象とした比較処理のため、論理Column名をワイド文字に変更します。最終的にはCompRegularExpression::compString関数へ比較に必要なパラメータを渡します。
*/
void CompRegularExpression::logicalColumnNameCheck(AnalysisCommand *analysisCommand, Entity *entity, std::wregex pattern)
{
	//Columnの数だけループ処理
	auto fields = entity->getfields();
	for (auto field = fields.begin(); field != fields.end(); field++)
	{
		//正規表現を用いた文字列比較関数処理の実行
		CompRegularExpression::compString(analysisCommand, entity->getPhysicalTableName(), (*field).name.logicalName, (*field).name.physicalName, pattern);
	}
}

/**
* @fn void CompRegularExpression::tableCommentCheck(AnalysisCommand *analysisCommand, Entity *entity, std::wregex pattern)
* @brief テーブルコメントを対象とした比較関数
* @param analysisCommand 解析命令情報格納インスタンスのポインタ変数
* @param entity Entityインスタンスのポインタ変数
* @param pattern 正規表現パターンインスタンスのポインタ変数
* @details テーブルコメントを対象とした比較処理のため、テーブルコメントをワイド文字に変更します。最終的にはCompRegularExpression::compString関数へ比較に必要なパラメータを渡します。
*/
void CompRegularExpression::tableCommentCheck(AnalysisCommand *analysisCommand, Entity *entity, std::wregex pattern)
{
	//正規表現を用いた文字列比較関数処理の実行
	CompRegularExpression::compString(analysisCommand, entity->getPhysicalTableName(), entity->getTableComment(), entity->getPhysicalTableName(), pattern);
}

/**
* @fn void CompRegularExpression::columnCommentCheck(AnalysisCommand *analysisCommand, Entity *entity, std::wregex pattern)
* @brief カラムコメントを対象とした比較関数
* @param analysisCommand 解析命令情報格納インスタンスのポインタ変数
* @param entity Entityインスタンスのポインタ変数
* @param pattern 正規表現パターンインスタンスのポインタ変数
* @details カラムコメントを対象とした比較処理のため、カラムコメントをワイド文字に変更します。最終的にはCompRegularExpression::compString関数へ比較に必要なパラメータを渡します。
*/
void CompRegularExpression::columnCommentCheck(AnalysisCommand *analysisCommand, Entity *entity, std::wregex pattern)
{
	//Columnの数だけループ処理
	auto fields = entity->getfields();
	for (auto field = fields.begin(); field != fields.end(); field++)
	{
		//正規表現を用いた文字列比較関数処理の実行
		CompRegularExpression::compString(analysisCommand, entity->getPhysicalTableName(), (*field).name.comment, (*field).name.physicalName, pattern);
	}
}

/**
* @fn void CompRegularExpression::compString(AnalysisCommand *analysisCommand, std::string tableName, std::wstring compString, std::string targetName, std::wregex pattern)
* @brief 正規表現を用いた比較関数
* @param analysisCommand 解析命令情報格納インスタンスのポインタ変数
* @param tableName 比較対象のテーブル名
* @param compString 比較文字列
* @param targetName 比較対象(テーブルやColumnなど)
* @param pattern 正規表現パターンインスタンスのポインタ変数
* @details テーブルコメントを対象とした比較処理のため、テーブルコメントをワイド文字に変更します。最終的にはCompRegularExpression::compString関数へ比較に必要なパラメータを渡します。
*/
void CompRegularExpression::compString(AnalysisCommand *analysisCommand, std::wstring tableName, std::wstring compString, std::wstring targetName, std::wregex pattern)
{

	if (regex_match(compString.c_str(), pattern)) {
		//正規表現を用いて比較した文字列パターンが一致した場合、
		//reporterインスタンスにレポート情報を登録する
		Reporter::getInstance()->addRecord(analysisCommand, CommandTypeList::E_Comparison, tableName, targetName);
	}
}


/**
* @fn std::wregex CompRegularExpression::getStringsToWOrPattern(std::list<std::string> values)
* @brief 比較用正規表現変換関数
* @param values 文字列リスト
* @return pattern 正規表現パターン変数
* @details 複数の文字列を正規表現のORで結合し、正規表現パターンを変数に格納し呼び出し元に返します。
*/
std::wregex CompRegularExpression::getStringsToWOrPattern(std::list<std::string> values)
{
	//検出用正規表現パターンを生成処理
	wstring patternstring;
	const std::wstring orPatternstring = L"|";
	for (auto value = values.begin(); value != values.end(); value++)
	{
		//valuesに格納されている検索文字列情報をorで結合する
		if (value != values.begin())patternstring = patternstring + orPatternstring;
		wchar_t * ws = stringToWchar((*value));
		patternstring += ws;
		delete ws;
	}
	//正規表現パターンインスタンス生成
	std::wregex pattern(patternstring.c_str());

	return pattern;
}

/**
* @fn std::wregex CompRegularExpression::getExtractPattern(std::list<std::string> targetExtractionParams)
* @brief テーブル比較用正規表現変換関数
* @param targetExtractionParams 文字列リスト
* @return pattern 正規表現パターン変数
* @details 複数の文字列を正規表現のORで結合し、正規表現パターンを変数に格納し呼び出し元に返します。
*/
std::wregex CompRegularExpression::getExtractPattern(std::list<std::string> targetExtractionParams)
{
	const std::wstring orPatternstring = L"|";
	wstring extractionPatternString;

	for (auto extractParam = targetExtractionParams.begin(); extractParam != targetExtractionParams.end(); extractParam++)
	{
		//targetExtractionParamsに格納されている検索用文字列をorで結合する
		if (extractParam != targetExtractionParams.begin())extractionPatternString = extractionPatternString + orPatternstring;
		wchar_t * ws = stringToWchar((*extractParam));
		extractionPatternString += ws;
		delete ws;
	}
	if (extractionPatternString.empty())
	{
		//抽出用生成処理パターンが空の場合、全対象とする正規表現パターンを代入
		extractionPatternString = L".*";
	}
	//正規表現パターンインスタンス生成
	std::wregex extractPattern(extractionPatternString.c_str());

	return extractPattern;
}