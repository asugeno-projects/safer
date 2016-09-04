/**
* @file       relationChecker.h
* @date       2016/01/31
* @author     Akihiro Sugeno
* @par        Revision
* $Id$
* @par        Copyright
* Copyright 2015 Akihiro Sugeno
* @par        History
*     - 2015/10/26 Akihiro Sugeno
*       -# Initial Version
*/

#ifndef RDS_ER_STATICANALYSIS_SOURCE_STATIC_ANALYTICS_RELATIONCHECKER_H_
#define RDS_ER_STATICANALYSIS_SOURCE_STATIC_ANALYTICS_RELATIONCHECKER_H_

#include <string>
#include <map>
#include <vector>
#include <list>

class RelationChecker
{
private:
	const std::wstring TAG_COLUMNNAME = L"カラム名：";
	const std::wstring TABLE_COLUMNNAME = L"テーブル名：";
	const std::wstring COMMENT_FOR_MISSNG_RELATIONSHIP = L"リレーションが設定ミス、又は違う用途であるカラムが他のカラムと同じ名称になっている可能性があります。\n\nリレーション修正候補\n";
	const std::wstring COMMENT_FOR_MISSNG_RELATIONSHIP_AND_FIND_INDEX = L"リレーションが設定不足が推定されました。\nまた、Secondary Indexの設定も検知したため、リレーションの設定不足の可能性が高いと思われます。\n\nリレーション修正候補\n";
	const std::wstring COMMENT_FOR_MISSING_INDEX_AND_RELATIONSHIP = L"インデックス情報の設定不足が推定されました。\nこの検知は、リレーションの設定ミスが推定され、かつ、Indexの設定がなかった場合に表示されます。\n誤検知率は高いと思いますので確認を行う程度で問題ありません。";
	const std::wstring COMMENT_FOR_RELATIONSHIP_SELF_LOOP = L"リレーションのループを検知しました。テーブルに紐付いているリレーションが自テーブルを向いています。\n\nループした経路\n";
	const std::wstring COMMENT_FOR_RELATIONSHIP_RECURSIVE_SEARCH_STOP = L"リレーションの探索が多いため処理を中断しました。リレーションの多重度(カーディナリティ)の設定、又は依存関係の設定に不備がある可能性があります。また、リレーション構造が多岐にわたって設計されている場合において、このメッセージが表示されます。依存設定の不備は修正してください。\n\nループした経路\n";
	const std::wstring COMMENT_FOR_RELATIONSHIP_LOOP = L"リレーションのループを検知しました。設計に問題が無いか確認してください\n\nループした経路\n";
public:
	//! リレーション情報チェック実行関数
	void run(class AnalysisProjects *analysisProjects, class ER *er);

	//! リレーション情報に対象のカラムが設定されているかチェックする関数
	bool isSetRelationForColumn(class Entity *entity, class Relation *relation, std::wstring tableName, std::wstring columnName);

	//! 該当のテーブル・カラムに紐づくリレーション情報が設定されているかチェックする関数
	bool isMissingRelationship(class ER *er, std::wstring tableName, std::wstring columnName);

	//! リレーション設定不足チェック関数
	void checkRelationship(class ER *er, const std::vector<std::wstring> &tableNames, const std::wstring &columnName);

	//! フィールド(カラム)マップ用リレーションチェック関数
	void checkForFieldMap(class ER *er, std::map <std::wstring, std::vector<std::wstring>>&fieldMap);

	//! リレーション情報再帰検索関数
	void recursiveSearchForRelation(std::map<std::wstring, std::list<Relation *>> entityToRelationMap,class Relation *prevRelaion, std::wstring tableName, std::wstring startTableName, std::list<std::wstring> tableRoute);

	//! リレーション情報再帰検索プログラム用report生成関数
	void reportForRecursiveSearch(std::list<std::wstring> tableRoute, std::wstring startTableName, std::wstring endTableName, std::wstring message);
};

#endif //RDS_ER_STATICANALYSIS_SOURCE_STATIC_ANALYTICS_RELATIONCHECKER_H_