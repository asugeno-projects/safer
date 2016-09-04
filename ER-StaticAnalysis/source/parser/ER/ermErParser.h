/**
* @file       ermErParser.h
* @date       2016/01/31
* @author     Akihiro Sugeno
* @par        Revision
* $Id$
* @par        Copyright
* Copyright 2015 Akihiro Sugeno
* @par        History
*     - 2015/10/29 Akihiro Sugeno
*       -# Initial Version
*/

#ifndef RDS_ER_STATICANALYSIS_SOURCE_PARSER_ER_ERMERPARSER_H_
#define RDS_ER_STATICANALYSIS_SOURCE_PARSER_ER_ERMERPARSER_H_

#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMElement.hpp>

#include <string>
#include <map>
#include <list>

#include "erParser.h"

#include "../../ER/entity.h"
using namespace xercesc;

typedef std::map <std::string, void(*)(class fieldParam *, struct NodeParams*)> SetErmColumnFunc;

static SetErmColumnFunc setErmColumnFunc;

class ErmErParser : public ErParser
{
private:
	//! ERファイルのPath
	std::string erFilePath; //ER図ファイルのパス
	
	/*! Entityクラスへのデータ投入関数群 */
	typedef std::map <std::string, void(*)(class Entity *, struct NodeParams*, DOMNode*, ErmErParser*)> SetErmErFunc;

	typedef std::map <std::string, void(*)(Relation *, NodeParams*, ErmErParser*)> SetErmRelationFunc;

	typedef std::map <std::string, fieldParam *(*)(fieldParam *, NodeParams*, bool *, std::string *, Entity *, ErmErParser*)> SetErmFieldFunc;

	typedef std::map <std::string, void(*)(Entity *, DOMNode*, ErmErParser*) > SetErmIndexFunc;

	typedef std::map <std::string, Relation::CardinalityTypeList> CardinalityConverter;

	std::map<std::string, fieldParam> wordsMap;
	std::map<std::string, fieldParam*> fieldsMap;
	std::map<std::string, std::list<std::wstring>> relationMap;
	std::map<std::string, Entity*> entitysMap;
	std::map<std::string, Entity*> entitysIdMap;

	std::list<std::string> relationId;

	const std::string TABLE = "table";
	const std::string NORMAL_COLUMN = "normal_column";
	const std::string PHYSICAL_NAME = "physical_name";
	const std::string LOGICAL_NAME = "logical_name";
	const std::string COLUMNS = "columns";
	const std::string TYPE = "type";
	const std::string COMMENT = "description";
	const std::string INDEXES = "indexes";

	const std::string ID = "id";
	const std::string RELATION_SOURCE = "source";
	const std::string RELATION_TARGET = "target";

	const std::string PARENT_CARDINALITY = "parent_cardinality";
	const std::string CHILD_CARDINALITY = "child_cardinality";

	const std::string CARDINALITY_MORE = "1..n";
	const std::string CARDINALITY_ZERO_OR_MORE = "0..n";
	const std::string CARDINALITY_ONE = "1";
	const std::string CARDINALITY_ZERO_ONE = "0..1";

	const std::string WORD = "word";
	const std::string WORD_ID = "word_id";
	const std::string REFERENCED_COLUMN = "referenced_column";
	const std::string RELATION = "relation";
	const std::string NOT_NULL = "not_null";
	const std::string NULL_STRING = "null";
	const std::string DEFAULT_VALUE = "default_value";
	const std::string AUTO_INCREMENT = "auto_increment";
	const std::string PRIMARY_KRY = "primary_key";

	const std::string BOOL_TRUE = "true";

	const std::wstring AUTO_INCREMENT_W = L"AUTO_INCREMENT";

	const std::string CATEGORY = "category";

	const std::string NAME = "name";
	const std::string NODE_ELEMENT = "node_element";

public:
	ErmErParser(std::string filePath)
	{
		erFilePath = filePath;// ER図ファイルパスをセット
		this->setParams();
	}

	//! Entityインスタンスにテーブル物理名をセットする関数
	static void setPhysicalTableName(class Entity *entity, struct NodeParams* param, DOMNode* node, ErmErParser* instance);
	//! Entityインスタンスにテーブル論理名をセットする関数
	static void setLogicalTableName(Entity *entity, struct NodeParams* param, DOMNode* node, ErmErParser* instance);
	//! EntityインスタンスにColumn情報をセットする関数
	static void setColumns(Entity *entity, struct NodeParams* param, DOMNode* node, ErmErParser* instance);
	//! Entityインスタンスにインデックス情報をセットする関数
	static void setIndexes(Entity *entity, struct NodeParams* param, DOMNode* node, ErmErParser* instance);

	//! 物理Column名をColumnインスタンスににセットする
	static void setPhysicalColumnName(class fieldParam *entity, struct NodeParams* param);
	//! 論理Column名をColumnインスタンスににセットする
	static void setLogicalColumnName(class fieldParam *field, struct NodeParams* param);
	//! Columnデータ型情報をColumnインスタンスににセットする
	static void setColumnType(class fieldParam *field, struct NodeParams* param);
	//! Columnコメント情報をColumnインスタンスににセットする
	static void setColumnComment(class fieldParam *field, struct NodeParams* param);
	//! リレーション情報生成関数
	void createRelation(ER *er, DOMNodeList* relationObject);

	//! リレーション関係にあるカラム名情報をリレーションインスタンスにセットする
	static void setDependKeyList(Relation * relation, NodeParams* nodeParam, ErmErParser* instance);
	//! リレーション関係にあるEntityインスタンスのポインタ情報をリレーションインスタンスにセットする
	static void setEntityForRelation(Relation * relation, NodeParams* nodeParam, ErmErParser* instance);
	//! Entity1のカーディナリティ(多重度)情報をリレーションインスタンスにセットする
	static void setCardinality1(Relation * relation, NodeParams* nodeParam, ErmErParser* instance);
	//! Entity2のカーディナリティ(多重度)情報をリレーションインスタンスにセットする
	static void setCardinality2(Relation * relation, NodeParams* nodeParam, ErmErParser* instance);

	//! フィールドマップからフィールド情報を取得する
	static fieldParam * getFieldForFieldsMap(fieldParam * field, NodeParams* nodeParam, bool *relationFlg, std::string *fieldId, Entity * entity, ErmErParser* instance);
	//! リレーションIDをセットする関数
	static fieldParam * addRelationId(fieldParam * field, NodeParams* nodeParam, bool *relationFlg, std::string *fieldId, Entity * entity, ErmErParser* instance);
	//! Column情報をセットする関数
	static fieldParam * setColomnParam(fieldParam * field, NodeParams* nodeParam, bool *relationFlg, std::string *fieldId, Entity * entity, ErmErParser* instance);
	//! Not Null制約のフラグをColumn情報にセットする
	static fieldParam * setNotNullFlg(fieldParam * field, NodeParams* nodeParam, bool *relationFlg, std::string *fieldId, Entity * entity, ErmErParser* instance);
	//! デフォルトパラメータのフラグをColumn情報にセットする
	static fieldParam * setDefaultParam(fieldParam * field, NodeParams* nodeParam, bool *relationFlg, std::string *fieldId, Entity * entity, ErmErParser* instance);
	//! AUTO INCREMENT情報をColumn情報にセットする
	static fieldParam * setAutoIncrement(fieldParam * field, NodeParams* nodeParam, bool *relationFlg, std::string *fieldId, Entity * entity, ErmErParser* instance);
	//! Primary Key情報をEntityインスタンスにセットする
	static fieldParam * setPrimaryKey(fieldParam * field, NodeParams* nodeParam, bool *relationFlg, std::string *fieldId, Entity * entity, ErmErParser* instance);

	//! 参照関係にあるColumnをコピー作成し、新たにフィールドマップに登録し直す
	void recursiveSetFieldMap(std::string originalId, std::string id, std::map<std::string, std::list<std::string>> * referenceFielsMap);
	//! インデックス情報をセットする関数
	static void setIndexColumns(Entity * entity, DOMNode* columnsNode, ErmErParser* instance);

	std::list<std::wstring> getRelationMap(std::string name){ return relationMap[name]; };
	fieldParam getWordMap(std::string name){ return wordsMap[name]; };
	//! ER Master形式のER図ファイルをパースし、ERデータを生成する関数
	class ER * parse(void);
	SetErmErFunc setErmErFunc;
	SetErmRelationFunc setErmRelationFunc;
	CardinalityConverter cardinalityConverter;
	SetErmFieldFunc setErmFieldFunc;
	SetErmIndexFunc setErmIndexFunc;
	//! パラメータセット関数
	void setParams();
	//! 終了処理用関数
	void finalize();
	//! DOM Elementから辞書データを抽出し、辞書データとIDをひも付けたマップを作成する
	void ErmErParser::setWordMap(DOMElement *domElement);
	//! DOM Elementからカラムデータを取得、カラムデータマップを作成する関数
	void ErmErParser::setFieldsMap(DOMElement *domElement);
	//! DOM Elementからカラムデータを取得し、カラム参照情報を元にカラム情報を合成する関数
	void ErmErParser::mergeFieldsMapAndReferenceField(DOMElement *domElement);
	//! Relation情報を生成関数
	void ErmErParser::setRelation(DOMElement *domElement, class ER *er);
	//! カテゴリ(TAG)情報を登録関数
	void ErmErParser::setTag(DOMElement *domElement);
	//! Entity情報を生成関数
	void ErmErParser::setEntity(DOMElement *domElement, ER *er);
};

#endif //RDS_ER_STATICANALYSIS_SOURCE_PARSER_ER_ERMERPARSER_H_