/**
* @file       er.h
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

#ifndef RDS_ER_STATICANALYSIS_SOURCE_ER_ER_H_
#define RDS_ER_STATICANALYSIS_SOURCE_ER_ER_H_

#include <string>
#include <list>
#include <map>

#include "./entity.h"
#include "./relation.h"

/*! @class ER
ERクラス
@brief ER図に関する情報を格納するクラス
*/
class ER
{
private:
public:
	/*! @brief Entityマップ定義 */
	typedef std::map<std::wstring, class Entity *> entity_map;
	std::list <class Entity *> entitys;/*!< エンティティ(テーブル)リスト */
	std::list <class Relation *> relations;/*!< リレーションリスト */
	entity_map entityMap;/*!< テーブルマップ(Keyがエンティティ(テーブル)名) */
	/*! ERクラスデストラクタ */
	~ER();

};

#endif // RDS_ER_STATICANALYSIS_SOURCE_ER_ER_H_