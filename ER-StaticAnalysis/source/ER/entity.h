/**
* @file       entity.h
* @date       2016/01/31
* @author     Akihiro Sugeno
* @par        Revision
* $Id$
* @par        Copyright
* Copyright 2015 Akihiro Sugeno
* @par        History
*     - 2015/07/03 Akihiro Sugeno
*       -# Initial Version
*/

#ifndef RDS_ER_STATICANALYSIS_SOURCE_ER_ENTITY_H_
#define RDS_ER_STATICANALYSIS_SOURCE_ER_ENTITY_H_

#include <string>
#include <list>
#include <map>

/*! @struct entityName
@brief エンティティ名情報を格納する構造体
*/
struct entityName
{
	std::wstring physicalName;/*!< テーブル物理名 */
	std::wstring logicalName;/*!< テーブル論理名 */
	std::wstring comment;/*!< テーブルコメント */
};

/*! @class fieldParam
@brief カラムに関する情報を格納する構造体
*/
class fieldParam
{
public:
	entityName name;/*!< カラム名 */
	std::wstring type;/*!< カラムタイプ */
	bool notNullFlg;/*!< Not Null制約フラグ */
	std::wstring defaultParam;/*!< デフォルト値 */
	std::list<std::wstring> DDLOptions;/*! Column DDLオプション */

	fieldParam()
	{
	}

	fieldParam(const fieldParam &originalFieldParam)
	{
		name.physicalName = originalFieldParam.name.physicalName;
		name.logicalName = originalFieldParam.name.logicalName;
		name.comment = originalFieldParam.name.comment;
		type = originalFieldParam.type;
		notNullFlg = originalFieldParam.notNullFlg;
		defaultParam = originalFieldParam.defaultParam;
		DDLOptions = originalFieldParam.DDLOptions;
	}
};

/*! @class Entity
@brief エンティティに関する情報を格納するクラス
*/
class Entity
{
public:
	typedef std::list<std::list<std::wstring>> INDEXS_LIST_TYPE;
	typedef std::list <class fieldParam> FIELD_LIST_TYPE;
	typedef std::map <int, std::wstring> PK_MAP_TYPE;
private:
	struct entityName tableName;/*!< テーブル名 */
	FIELD_LIST_TYPE fields;/*!< カラム情報リスト */
	PK_MAP_TYPE pkList;/*!< Primary Keyリスト(map) */
	std::wstring tagName;/*!< Entityタグ */
	INDEXS_LIST_TYPE indexList;/*!< Indexリスト */
public:
	/*! エンティティクラスデストラクタ */
	~Entity()
	{
	}

	/*! テーブル物理名のセット関数 */
	void setPhysicalTableName(std::wstring name)
	{
		this->tableName.physicalName = name;
	}

	/*! テーブル物理名のゲット関数 */
	const std::wstring getPhysicalTableName() const
	{
		return this->tableName.physicalName;
	}

	/*! テーブル論理名のセット関数 */
	void setLogicalTableName(std::wstring name)
	{
		this->tableName.logicalName = name;
	}

	/*! テーブル論理名のゲット関数 */
	const std::wstring getLogicalTableName() const
	{
		return this->tableName.logicalName;
	}

	/*! テーブルコメントのセット関数 */
	void setTableComment(std::wstring comment)
	{
		this->tableName.comment = comment;
	}

	/*! テーブルコメントのゲット関数 */
	const std::wstring getTableComment() const
	{
		return this->tableName.comment;
	}

	/*! カラム情報のセット関数 */
	void setfields(fieldParam field)
	{
		this->fields.push_back(field);
	}

	/*! カラム情報のゲット関数 */
	const FIELD_LIST_TYPE & getfields() const
	{
		return this->fields;
	}

	/*! Primary Keyのセット関数 */
	void insertPk(int pkNum, std::wstring pk)
	{
		this->pkList.insert(make_pair(pkNum, pk));
	}

	/*! Primary Keyのゲット関数 */
	const PK_MAP_TYPE & getPks() const
	{
		return this->pkList;
	}

	/*! TAG情報のセット関数 */
	void setTagName(std::wstring _tagName)
	{
		this->tagName = _tagName;
	}

	/*! タグ情報のゲット関数 */
	const std::wstring getTagName() const
	{
		return this->tagName;
	}

	/*! Index情報の追加関数 */
	void addIndex(std::list<std::wstring> _index)
	{
		this->indexList.push_back(_index);
	}

	/*! Index情報のゲット関数 */
	const INDEXS_LIST_TYPE & getIndexs() const
	{
		return this->indexList;
	}
};

#endif //RDS_ER_STATICANALYSIS_SOURCE_ER_ENTITY_H_