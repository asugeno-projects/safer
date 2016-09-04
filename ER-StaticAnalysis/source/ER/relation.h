/**
* @file       relation.h
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

#ifndef RDS_ER_STATICANALYSIS_SOURCE_ER_RELATION_H_
#define RDS_ER_STATICANALYSIS_SOURCE_ER_RELATION_H_

#include <string>
#include <list>
#include "../ER/entity.h"

/**
* @class Relation
* リレーションクラス
* @brief リレーション情報を格納するクラス
*/
class Relation
{
public:
	typedef std::list <class Entity *> ENTITY_LIST;
	typedef std::list <std::wstring> DEPEND_KEY_LIST;
private:
	ENTITY_LIST entityList;/*!< テーブル情報 */
	DEPEND_KEY_LIST entity1DependKeyList;/*!< テーブル1の依存するカラムリスト */
	DEPEND_KEY_LIST entity2DependKeyList;/*!< テーブル2の依存するカラムリスト */
	int cardinalityType1;/*!< テーブル1の依存するカラムリスト */
	int cardinalityType2;/*!< テーブル2の依存するカラムリスト */
	bool dependenceFlg;/*!< 依存フラグ ? */

public:
	enum CardinalityTypeList{
		E_ZERO_ONE = 1, //0又は1
		E_ONE, //1,
		E_ZERO_OR_MORE,//多数(0以上)
		E_MORE,//多数(1以上),
		E_VALUE_FIXED//N固定(値が必ず決まっている)
	};

	/*! エンティティオブジェクトのセット関数 */
	void setEntity(Entity *entity)
	{
		this->entityList.push_back(entity);
	}

	/*! エンティティオブジェクトのゲット関数 */
	const ENTITY_LIST & getEntityList() const
	{
		return this->entityList;
	}

	/*! エンティティ1番依存カラム セット関数 */
	void setEntity1DependKey(std::wstring key)
	{
		this->entity1DependKeyList.push_back(key);
	}

	/*! エンティティ1番依存カラム ゲット関数 */
	const DEPEND_KEY_LIST & getEntity1DependKeyList() const
	{
		return this->entity1DependKeyList;
	}

	/*! エンティティ2番依存カラム セット関数 */
	void setEntity2DependKey(std::wstring key)
	{
		this->entity2DependKeyList.push_back(key);
	}

	/*! エンティティ2番依存カラム ゲット関数 */
	const DEPEND_KEY_LIST &getEntity2DependKeyList() const
	{
		return this->entity2DependKeyList;
	}

	/*! リレーションタイプ1 セット関数 */
	void setCardinalityType1(int type)
	{
		this->cardinalityType1 = type;
	}

	/*! リレーションタイプ1 ゲット関数 */
	const int getCardinalityType1() const
	{
		return this->cardinalityType1;
	}

	/*! リレーションタイプ2 セット関数 */
	void setCardinalityType2(int type)
	{
		this->cardinalityType2 = type;
	}

	/*! リレーションタイプ2 ゲット関数 */
	const int getCardinalityType2() const
	{
		return this->cardinalityType2;
	}

	/*! 依存フラグ セット関数 */
	void setDependenceFlg(bool flg)
	{
		this->dependenceFlg = flg;
	}

	/*! 依存フラグ ゲット関数 */
	const bool getDependenceFlg() const
	{
		return this->dependenceFlg;
	}

	/*! Relationクラスデストラクタ */
	~Relation()
	{
		//entityListはEntityオブジェクトのポインタを格納していますが、実体は別のオブジェクトで管理されるため、
		//ここではdelete命令を行わない
	}
};

#endif //RDS_ER_STATICANALYSIS_SOURCE_ER_RELATION_H_