/**
* @file       relationshipChecker.h
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

#ifndef RDS_ER_STATICANALYSIS_SOURCE_STATIC_ANALYTICS_RELATIONSHIPCHECKER_H_
#define RDS_ER_STATICANALYSIS_SOURCE_STATIC_ANALYTICS_RELATIONSHIPCHECKER_H_

#include <string>
#include <map>
#include <vector>
#include <list>

class RelationshipChecker
{
private:
	const std::wstring TAG_COLUMNNAME = L"カラム名：";
	const std::wstring TABLE_COLUMNNAME = L"テーブル名：";
	const std::wstring COMMENT_FOR_MISSNG_RELATIONSHIP = L"リレーションシップが設定ミス、又は違う用途であるカラムが他のカラムと同じ名称になっている可能性があります。\n\nリレーションシップ修正候補\n";
	const std::wstring COMMENT_FOR_MISSNG_RELATIONSHIP_AND_FIND_INDEX = L"リレーションシップが設定不足が推定されました。\nまた、Secondary Indexの設定も検知したため、リレーションシップの設定不足の可能性が高いと思われます。\n\nリレーションシップ修正候補\n";
	const std::wstring COMMENT_FOR_MISSING_INDEX_AND_RELATIONSHIP = L"インデックス情報の設定不足が推定されました。\nこの検知は、リレーションシップの設定ミスが推定され、かつ、Indexの設定がなかった場合に表示されます。\n誤検知率は高いと思いますので確認を行う程度で問題ありません。";
	const std::wstring COMMENT_FOR_RELATIONSHIP_SELF_LOOP = L"リレーションシップのループを検知しました。テーブルに紐付いているリレーションシップが自テーブルを向いています。\n\nループした経路\n";
	const std::wstring COMMENT_FOR_RELATIONSHIP_RECURSIVE_SEARCH_STOP = L"リレーションシップの探索が多いため処理を中断しました。リレーションシップの多重度(カーディナリティ)の設定、又は依存関係の設定に不備がある可能性があります。また、リレーションシップ構造が多岐にわたって設計されている場合において、このメッセージが表示されます。依存設定の不備は修正してください。\n\nループした経路\n";
	const std::wstring COMMENT_FOR_RELATIONSHIP_LOOP = L"リレーションシップのループを検知しました。設計に問題が無いか確認してください\n\nループした経路\n";
public:
	//! リレーションシップ情報チェック実行関数
	void run(class AnalysisProjects *analysisProjects, class ER *er);

	//! リレーションシップ情報に対象のカラムが設定されているかチェックする関数
	bool isSetRelationshipForColumn(class Entity *entity, class Relationship *relationship, std::wstring tableName, std::wstring columnName);

	//! 該当のテーブル・カラムに紐づくリレーションシップ情報が設定されているかチェックする関数
	bool isMissingRelationship(class ER *er, std::wstring tableName, std::wstring columnName);

	//! リレーションシップ設定不足チェック関数
	void checkRelationship(class ER *er, const std::vector<std::wstring> &tableNames, const std::wstring &columnName);

	//! フィールド(カラム)マップ用リレーションシップチェック関数
	void checkForFieldMap(class ER *er, std::map <std::wstring, std::vector<std::wstring>>&fieldMap);

	//! リレーションシップ情報再帰検索関数
	void recursiveSearchForRelationship(std::map<std::wstring, std::list<Relationship *>> entityToRelationshipMap,class Relationship *prevRelaion, std::wstring tableName, std::wstring startTableName, std::list<std::wstring> tableRoute);

	//! リレーションシップ情報再帰検索プログラム用report生成関数
	void reportForRecursiveSearch(std::list<std::wstring> tableRoute, std::wstring startTableName, std::wstring endTableName, std::wstring message);
};

#endif //RDS_ER_STATICANALYSIS_SOURCE_STATIC_ANALYTICS_RELATIONSHIPCHECKER_H_