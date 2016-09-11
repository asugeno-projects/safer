/**
* @file       A5erParser.h
* @date       2016/01/31
* @author     Akihiro Sugeno
* @par        Revision
* $Id$
* @par        Copyright
* Copyright 2015 Akihiro Sugeno
* @par        History
*     - 2015/07/05 Akihiro Sugeno
*       -# Initial Version
*/

#ifndef RDS_ER_STATICANALYSIS_SOURCE_PARSER_ER_A5ERPARSER_H_ 
#define RDS_ER_STATICANALYSIS_SOURCE_PARSER_ER_A5ERPARSER_H_ 

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "erParser.h"
#include "../../log/logger.h"

/**
* @class A5erParser
* A5erパーサークラス
* @brief A5 Mk2のER図ファイルをパース処理するクラス
*/
class A5erParser : public ErParser
{
private:
	//! ERファイルのPath
	std::string erFilePath; //ER図ファイルのパス
	//! Entityに関する処理を文字列と紐付かせるmap変数の定義
	typedef std::map<std::string, void(*)(Entity *, std::wstring)> funcs_type;
	//! Entityに関する処理を文字列と紐付かせるmap情報を格納する変数
	funcs_type setSectionParamFunc;

	//! Relationshipに関する処理を文字列と紐付かせるmap変数の定義
	typedef std::map<std::string, void(*)(Relationship *, ER *, std::wstring)> relationship_funcs;
	//! Relationshipに関する処理を文字列と紐付かせるmap情報を格納する変数
	relationship_funcs setRelationshipFunc;
	

	/* セクションの子要素タグ名リスト */
	//! Entity物理名関連タグ
	const std::string PHYSICAL_TABLE_NAME_TAG = "Entity.PName";
	//! Entity論理名関連タグ
	const std::string LOGICAL_TABLE_NAME_TAG = "Entity.LName";
	//! Entityコメント情報タグ
	const std::string LOGICAL_TABLE_COMMENT = "Entity.Commet";
	//! Column情報タグ
	const std::string LOGICAL_FIELD = "Entity.Field";
	//! Entityタグ
	const std::string TAG = "Entity.Tag";
	//! Indexタグ
	const std::string INDEX = "Entity.Index";

	//! Relationship情報に紐づくEntity1のタグ
	const std::string RELATIONSHIP_ENTITY1_TAG = "Relation.Entity1";
	//! Relationship情報に紐づくEntity2のタグ
	const std::string RELATIONSHIP_ENTITY2_TAG = "Relation.Entity2";
	//! Relationshipフィールドのタグ1
	const std::string RELATIONSHIP_FIELDS1_TAG = "Relation.Fields1";
	//! Relationshipフィールドのタグ2
	const std::string RELATIONSHIP_FIELDS2_TAG = "Relation.Fields2";
	//! 多重度(カーディナリティ)タグ1
	const std::string CARDINALITY_TYPE1_TAG = "Relation.RelationType1";//リレーションシップタイプとなっているが、実際はカーディナリティ(多重度)を表しているため、定数名を実態とは違う名前にしています。
	//! 多重度(カーディナリティ)タグ2
	const std::string CARDINALITY_TYPE2_TAG = "Relation.RelationType2";//リレーションシップタイプとなっているが、実際はカーディナリティ(多重度)を表しているため、定数名を実態とは違う名前にしています。
	//! Relationship依存タグ
	const std::string RELATIONSHIP_DEPENDENCE_TAG = "Relation.Dependence";


	/* セクション判定の正規表現リスト */
	//! Entityセクション抽出用正規表現パターン
	const std::string ENTITY_REGEX = "^Entity.*";
	//! Relationshipセクション抽出用正規表現パターン
	const std::string RELATIONSHIP_REGEX = "^Relation.*";

	//! Entityインスタンスに物理テーブル名情報を格納する処理
	static void setPhysicalTableNameForEntity(Entity *entity, std::wstring name);
	//! Entityインスタンスに論理テーブル名情報を格納する処理
	static void setLogicalTableNameForEntity(Entity *entity, std::wstring name);
	//! Entityインスタンスにテーブルコメント情報を格納する処理
	static void setTableCommentForEntity(Entity *entity, std::wstring name);
	//! EntityインスタンスにColumn情報を格納する処理
	static void setFieldForEntity(Entity *entity, std::wstring name);
	//! EntityインスタンスにTAG情報を格納する処理
	static void setTagForEntity(Entity *entity, std::wstring tag);
	//! EntityインスタンスにIndex情報を格納する処理
	static void setIndexForEntity(Entity *entity, std::wstring index);

	//! RelationshipにEntity情報を紐付ける処理
	static void setRelationshipEntity(Relationship *relationship, ER *er, std::wstring name);
	//! Relationshipに関係依存Column名を登録する
	static void setEntity1DependKey(Relationship *relationship, ER *er, std::wstring key);
	//! Relationshipに関係依存Column名を登録する
	static void setEntity2DependKey(Relationship *relationship, ER *er, std::wstring key);
	//! Relationshipに多重度(カーディナリティ)を登録する
	static void setCardinalityType1(Relationship *relationship, ER *er, std::wstring type);
	//! Relationshipに多重度(カーディナリティ)を登録する
	static void setCardinalityType2(Relationship *relationship, ER *er, std::wstring type);
	//! Relationshipに依存フラグを登録する
	static void setDependenceFlg(Relationship *relationship, ER *er, std::wstring type);

public:
	//! コンストラクタ
	A5erParser(std::string filePath)
	{
		erFilePath = filePath;// ER図ファイルパスをセット
		this->setParamFuc();
	};
	//! A5Mk2形式のER図ファイルのパース関数
	class ER * parse(void);
	//! パース処理の初期化関数
	void setParamFuc();
	//! Entity情報のセット
	void setEntity(class ER * er, const boost::program_options::woption & option);
	//! Relationship情報のセット
	void setRelationship(class ER * er, const boost::program_options::woption & option);
	//! BOM情報の除去
	void removeBom(std::wifstream *file);
};

#endif //RDS_ER_STATICANALYSIS_SOURCE_PARSER_ER_A5ERPARSER_H_ 