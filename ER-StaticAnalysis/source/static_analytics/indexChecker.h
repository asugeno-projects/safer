/**
* @file       indexChecker.h
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

#ifndef RDS_ER_STATICANALYSIS_SOURCE_STATIC_ANALYTICS_INDEXCHECKER_H_
#define RDS_ER_STATICANALYSIS_SOURCE_STATIC_ANALYTICS_INDEXCHECKER_H_

#include <string>
#include "../ER/entity.h"
#include "../ER/relationship.h"

/**
* @class IndexChecker
* @brief インデックス情報チェック処理クラス
* @details インデックス情報を解析し、問題点をreporterクラスに登録する処理を定義したクラス
*/
class IndexChecker
{
private:
	//! レポート出力用文字列 テーブル名プレフィックス
	const std::wstring TAG_TABLENAME = L"テーブル名：";
	//! レポート出力用文字列 カラム名プレフィックス
	const std::wstring TAG_COLUMNNAME = L"カラム名：";
	//! レポート出力用文字列 Primary Key不足指摘コメント
	const std::wstring COMMENT_FOR_NO_PRIMARYKEY = L"Primary Keyの設定がありません。\nPrimary Keyの設定を推奨します。";
	//! レポート出力用文字列 Index不足指摘コメント
	const std::wstring COMMENT_FOR_NO_INDEX = L"リレーションシップ情報には登録されていますが、Index情報がありません。\nリレーションシップが設定されている場合、SELECTなどで参照される可能性があるためIndexの設定をしたほうが良い可能性があります。";
	//! レポート出力用文字列 Indexの過剰設定の指摘コメント
	const std::wstring COMMENT_FOR_TOO_MUCH_INDEX = L"Secondary Indexの数が規定値を超えています。\nSecondary Indexが多すぎる場合は、オプティマイザが正常に動かなくなる可能性があります。";
	//! インデックス過剰判定用の定数 To Do:設定ファイルに移したい
	const int THRESHOLD_INDEX = 4;
public:
	//! 指定したColumn名がIndexリストに登録されているか確認する関数
	static bool isMissingIndexForIndexList(const Entity::INDEXS_LIST_TYPE indexList, const std::wstring columnName);

	//! 指定したColumn名がPrimary Keyリストに登録されているか確認する関数
	static bool isSetPrimaryKey(const Entity * const entity, const std::wstring columnName);

	//! Indexの問題をチェックする関数
	void run(const class AnalysisProjects * const analysisProjects,const class ER * const er);

	//! 対象のColumnがPrimary Key又はSecondary Indexに登録されているかチェックする関数
	bool isSetColumnInIndex(const std::wstring columnName, const Entity::PK_MAP_TYPE pks, const Entity::INDEXS_LIST_TYPE indexs);
	
	//! 外部キー制約に設定されているカラムがIndexに登録されているかチェック
	void checkForeignKeyIsInIndex(const Relationship::DEPEND_KEY_LIST & dependKeyList, const Entity * const entity);
};

#endif //RDS_ER_STATICANALYSIS_SOURCE_STATIC_ANALYTICS_INDEXCHECKER_H_