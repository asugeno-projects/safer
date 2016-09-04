/**
* @file       indexChecker.cpp
* @date       2016/01/31
* @author     Akihiro Sugeno
* @par        Revision
* $Id$
* @par        Copyright
* Copyright 2015 Akihiro Sugeno
* @par        History
*     - 2015/10/28 Akihiro Sugeno
*       -# Initial Version
*/


#include "indexChecker.h"
#include "../ER/entity.h"
#include "../reporter/reporter.h"

/**
* @fn bool IndexChecker::isMissingIndexForIndexList(Entity::INDEXS_LIST_TYPE indexList, std::wstring columnName)
* @brief 指定したColumn名がIndexリストに登録されているか確認する関数
* @param indexList Indexリスト
* @param columnName 比較対象のColumn名
* @details 指定したColumn名がIndexリストに登録されているか確認する関数
*/
bool IndexChecker::isMissingIndexForIndexList(const Entity::INDEXS_LIST_TYPE indexList, const std::wstring columnName)
{
	bool indexSerchFlg = false;
	for (auto indexIt = indexList.begin(); indexIt != indexList.end(); indexIt++)
	{
		for (auto columnIt = (*indexIt).begin(); columnIt != (*indexIt).end(); columnIt++)
		{
			if ((*columnIt).compare(columnName) == 0)
			{
				return false;
			}
		}
	}
	return true;
}

/**
* @fn bool IndexChecker::isSetPrimaryKey(Entity *entity, std::wstring columnName)
* @brief 指定したColumn名がPrimary Keyリストに登録されているか確認する関数
* @param entity Entityリスト
* @param columnName 比較対象のColumn名
* @details 指定したColumn名がPrimary Keyリストに登録されているか確認する関数
*/
bool IndexChecker::isSetPrimaryKey(const Entity * const entity, const std::wstring columnName)
{
	auto pks = entity->getPks();
	for (auto pkIt = pks.begin(); pkIt != pks.end(); pkIt++)
	{
		if ((*pkIt).second.compare(columnName) == 0)
		{
			return true;
		}
	}
	return false;
}

/**
* @fn void IndexChecker::run(class AnalysisProjects *analysisProjects, class ER *er)
* @brief Indexの問題をチェックする関数
* @param analysisProjects 解析命令用情報格納インスタンス
* @param er ER図情報を格納するインスタンス
* @details Indexの問題をチェックする関数
*/
void IndexChecker::run(const class AnalysisProjects * const analysisProjects, const class ER * const er)
{
	//Entityの数だけループ処理
	for (auto entityIt = er->entitys.begin(); entityIt != er->entitys.end(); entityIt++)
	{
		//Primary Key情報の取得
		auto pks = (*entityIt)->getPks();
		if (pks.size() == 0)
		{
			//Primary Keyの設定が無い
			Reporter::getInstance()->addRecord(
				AlertLevelList::E_warning,
				CommandTypeList::E_IndexCheck,
				(*entityIt)->getPhysicalTableName(),
				IndexChecker::TAG_TABLENAME + (*entityIt)->getPhysicalTableName(),
				IndexChecker::COMMENT_FOR_NO_PRIMARYKEY
				);
		}

		//Index情報の取得
		auto indexs = (*entityIt)->getIndexs();
		if (indexs.size() > IndexChecker::THRESHOLD_INDEX)
		{
			//インデックスが多すぎる場合のメッセージ
			Reporter::getInstance()->addRecord(
				AlertLevelList::E_warning,
				CommandTypeList::E_IndexCheck,
				(*entityIt)->getPhysicalTableName(),
				IndexChecker::TAG_TABLENAME + (*entityIt)->getPhysicalTableName(),
				IndexChecker::COMMENT_FOR_TOO_MUCH_INDEX
				);
		}
	}

	//リレーションの数だけループ処理
	for (auto relationIt = er->relations.begin(); relationIt != er->relations.end(); relationIt++)
	{
		//リレーション情報に登録されたEntity情報の取得
		auto entityIt = (*relationIt)->getEntityList().begin();
		//外部キー制約のカラムがIndexに登録されているかチェック
		this->checkForeignKeyIsInIndex((*relationIt)->getEntity1DependKeyList(), *entityIt);

		//リレーション情報に登録された二個目のEntity情報に切り替え
		entityIt++;
		//外部キー制約のカラムがIndexに登録されているかチェック
		this->checkForeignKeyIsInIndex((*relationIt)->getEntity2DependKeyList(), *entityIt);
	}
}

/**
* @fn bool IndexChecker::isSetColumnInIndex(std::wstring columnName, Entity::PK_MAP_TYPE pks, Entity::INDEXS_LIST_TYPE indexs)
* @brief 対象のColumnがPrimary Key又はSecondary Indexに登録されているかチェックする関数
* @param columnName Column名
* @param pks Primary Keyリスト
* @param indexs Secondary Indexリスト
* @details 対象のColumnがPrimary Key又はSecondary Indexに登録されているかチェックする関数
*/
bool IndexChecker::isSetColumnInIndex(const std::wstring columnName, const Entity::PK_MAP_TYPE pks, const Entity::INDEXS_LIST_TYPE indexs)
{
	//Primary Keyの数だけループ処理
	for (auto pkIt = pks.begin(); pkIt != pks.end(); pkIt++)
	{
		//Primary Keyに登録されているKey名とカラム名を比較
		if ((*pkIt).second.compare(columnName) == 0)
		{
			//対象カラム名がPrimary Keyにセットされているため、
			//trueを呼び出し元に返す
			return true;
		}
	}
	//Secondary Indexの数だけループ処理
	for (auto indexIt = indexs.begin(); indexIt != indexs.end(); indexIt++)
	{
		//Secondary Indexに登録されているKey名の数だけループ処理
		for (auto columnNameIt = (*indexIt).begin(); columnNameIt != (*indexIt).end(); columnNameIt++)
		{
			//Indexに登録されているKey名と比較対象のカラム名を比較する
			if ((*columnNameIt).compare(columnName) == 0)
			{
				//対象カラム名がSecondary Indexにセットされているため、
				//呼び出し元にtrueを返す
				return true;
			}
		}
	}
	//ここに来た場合、Primary KeyにもSecondary Indexにも登録されていないカラムであるため、
	//falseを返す。
	return false;
}

/**
* @fn bool IndexChecker::checkForeignKeyIsInIndex(const Relation::DEPEND_KEY_LIST & dependKeyList, const Entity * const entity)
* @brief 外部キー制約に設定されているカラムがIndexに登録されているかチェック
* @param dependKeyList 外部キー制約のカラムリスト
* @param entity Entity情報を格納したインスタンス
* @details 外部キー制約に設定されているカラムがIndexに登録されているかチェックする関数
*/
void IndexChecker::checkForeignKeyIsInIndex(const Relation::DEPEND_KEY_LIST & dependKeyList, const Entity * const entity)
{
	//リレーションの依存カラムの数だけループ処理
	for (auto dependKey = dependKeyList.begin(); dependKey != dependKeyList.end(); dependKey++)
	{
		if (!this->isSetColumnInIndex(*dependKey, entity->getPks(), entity->getIndexs()))
		{
			//対象の外部キー制約に設定されているカラムがIndexに登録されていなかった場合、
			//インデックスの設定不足に関する指摘をする
			Reporter::getInstance()->addRecord(
				AlertLevelList::E_warning,
				CommandTypeList::E_IndexCheck,
				entity->getPhysicalTableName(),
				IndexChecker::TAG_COLUMNNAME + *dependKey,
				IndexChecker::COMMENT_FOR_NO_INDEX
				);
		}
	}
}