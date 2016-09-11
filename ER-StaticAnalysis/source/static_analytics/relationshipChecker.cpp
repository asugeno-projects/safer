/**
* @file       relationshipChecker.cpp
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

#include "relationshipChecker.h"

#include "../ER/er.h"
#include "../analysis_command/AnalysisProjects.h"
#include <map>
#include <vector>
#include "../reporter/reporter.h"
#include "indexChecker.h"

/**
* @fn void RelationshipChecker::run(AnalysisProjects *analysisProjects, ER *er)
* @brief リレーションシップ情報チェック実行関数
* @param analysisProjects 解析設定を格納した情報インスタンス
* @param er ER情報格納インスタンスのポインタ変数
* @details リレーションシップ情報チェック実行関数
*/
void RelationshipChecker::run(AnalysisProjects *analysisProjects, ER *er)
{
	//特定のカラム名を持つテーブルリスト情報を管理する変数
	map <std::wstring, std::vector<std::wstring>> fieldMap;
	//Entityの数だけループ処理
	for (auto entityIt = er->entitys.begin(); entityIt != er->entitys.end(); entityIt++)
	{
		//カラム数の数だけループ処理
		auto fields = (*entityIt)->getfields();
		for (auto fieldIt = fields.begin(); fieldIt != fields.end(); fieldIt++)
		{
			//カラム->テーブル名の関係のmapを作成する
			fieldMap[(*fieldIt).name.physicalName].push_back((*entityIt)->getPhysicalTableName());
		}
	}

	//フィールドマップの中身をチェックしリレーションシップが正しいか判定する
	this->checkForFieldMap(er, fieldMap);

	//Entityに紐づくリレーションシップ情報をMap化し保持する
	map<std::wstring, std::list<Relationship *>> entityToRelationshipMap;
	//Relationshipの数だけループ処理を行う
	for (auto relationshipIt = er->relationships.begin(); relationshipIt != er->relationships.end(); relationshipIt++)
	{
		//Relationship情報に所属するEntity情報を取得する
		Relationship::ENTITY_LIST entityList = (*relationshipIt)->getEntityList();
		//Relationship情報に所属するEntityの数だけループ処理を行う(2回しかループしない想定)
		for (auto entityIt = entityList.begin(); entityIt != entityList.end(); entityIt++)
		{
			//Entity名->Relationship情報のポインタ情報をMap化する
			entityToRelationshipMap[(*entityIt)->getPhysicalTableName()].push_back((*relationshipIt));
		}
	}

	//Entityの数だけループ処理
	for (auto entityIt = er->entitys.begin(); entityIt != er->entitys.end(); entityIt++)
	{
		//Entity物理名を取得する
		wstring tableName = (*entityIt)->getPhysicalTableName();
		//Entityの親子関係情報を保持する変数
		std::list<std::wstring> tableRoute;
		//Entityの親子関係情報を保持する変数に自信のテーブル名を最初に登録する
		tableRoute.push_back(tableName);
		//Relationship情報から子のEntityの探索を再帰的に行う
		this->recursiveSearchForRelationship(entityToRelationshipMap, NULL, tableName, tableName, tableRoute);
	}
}

/**
* @fn void RelationshipChecker::recursiveSearchForRelationship(map<std::wstring, std::list<Relationship *>> entityToRelationshipMap, class Relationship *prevRelaion, std::wstring tableName, std::wstring startTableName, std::list<std::wstring> tableRoute)
* @brief リレーションシップ情報再帰検索関数
* @param entityToRelationshipMap Entityに紐づくリレーションシップ情報Map
* @param prevRelaion 再帰前のRelationship情報
* @param tableName 現在の探索テーブル名
* @param startTableName 開始テーブル名
* @param tableRoute 経由したテーブル情報
* @details リレーションシップ情報再帰検索関数
*/
void RelationshipChecker::recursiveSearchForRelationship(std::map<std::wstring, std::list<Relationship *>> entityToRelationshipMap, class Relationship *prevRelaion, std::wstring tableName, std::wstring startTableName, std::list<std::wstring> tableRoute)
{
	//Entity名に属するRelationship情報を取得
	auto relationshipList = entityToRelationshipMap[tableName];
	//Relationshipの数だけループ処理を行う
	for (auto relationshipIt = relationshipList.begin(); relationshipIt != relationshipList.end(); relationshipIt++)
	{
		//Relationship情報に属するEntity情報を取得する
		const Relationship::ENTITY_LIST entityList = (*relationshipIt)->getEntityList();
		if (entityList.front() == entityList.back())
		{
			//Relationship情報に登録されているEntityリストの最初(1つめのEntity)と最後(2つめのEntity)が、
			//同一で有った場合、自己参照を行っていると判定し、report関数を呼び出す
			this->reportForRecursiveSearch(tableRoute, startTableName, startTableName, RelationshipChecker::COMMENT_FOR_RELATIONSHIP_SELF_LOOP);
			//自己参照関係がある場合、何度もループするため、探索を打ち切る
			return;
		}

		//Entityを取得
		auto entityIt = entityList.begin();
		if (((*relationshipIt)->getDependenceFlg() &&
			(*entityIt)->getPhysicalTableName().compare(tableName) != 0) ||
			((*relationshipIt) == prevRelaion))
		{
			//Relationshipの依存フラグがあり、テーブル名が違う場合(依存フラグがある場合一つ目のEntityは親EntityになるためRelationshipの流れが、そこで止まるため)、
			//又は、探索しているRelationshipが呼び出し元で呼ばれたRelationshipと同じ情報である場合(再帰的にRelationship情報を追う際に来た道を行き来しないようにするため)、
			//探索をスキップする
			continue;
		}
		
		//カーディナリティ(多重度)情報を保持する変数を定義
		std::list<int>cardinalityList;
		//ひとつ目のEntityのカーディナリティ(多重度)情報を登録
		cardinalityList.push_back((*relationshipIt)->getCardinalityType1());
		//ふたつ目のEntityのカーディナリティ(多重度)情報を登録
		cardinalityList.push_back((*relationshipIt)->getCardinalityType2());
		//カーディナリティ(多重度)リストから、ひとつ目のEntityのカーディナリティ(多重度)を取得
		std::list<int>::iterator cardinalityIt = cardinalityList.begin();

		if ((*relationshipIt)->getDependenceFlg())
		{
			//依存フラグがtrueの場合、ひとつ目のEntityは親Entityになるため、
			//カウントアップすることで処理をスキップする
			entityIt++;
			cardinalityIt++;
		}

		//Entityの数だけループ処理を行う(ただし、上記の処理で親Entityをスキップする処理が入っているため、1～2回のループ処理になります。)
		for (; entityIt != entityList.end() && cardinalityIt != cardinalityList.end(); entityIt++, cardinalityIt++)
		{
			if ((*cardinalityIt) == Relationship::CardinalityTypeList::E_ONE ||
				(*cardinalityIt) == Relationship::CardinalityTypeList::E_ZERO_ONE)
			{
				//1,又は0,1の多重度(多重度)の場合のEntityは親側とみなし再帰処理をスキップ
				continue;
			}
			if (tableRoute.size() > 20)
			{
				//リレーションシップの再帰探索回数が規定値に超えたため、再帰処理を中断
				this->reportForRecursiveSearch(tableRoute, startTableName, (*entityIt)->getPhysicalTableName(), RelationshipChecker::COMMENT_FOR_RELATIONSHIP_RECURSIVE_SEARCH_STOP);
				return;
			}
			if ((*entityIt)->getPhysicalTableName().compare(tableName) != 0)
			{
				//ここに辿ってくるEntityが、この関数を呼び出す前のEntityと同一名出ない場合は、
				//子のEntityを参照していることになる。そこから子のEntityを探索を行う。

				//テーブルのルート情報をコピー
				std::list<std::wstring> newTableRoute(tableRoute);
				if ((find(newTableRoute.begin(), newTableRoute.end(), (*entityIt)->getPhysicalTableName())) != newTableRoute.end())
				{
					//今まで探索してきた経路にて、今回の関数で処理を行っているEntityと同一名があれば、
					//Relationshipがループしていることになるため、再帰探索を中断し、reportに記録する
					this->reportForRecursiveSearch(tableRoute, startTableName, (*entityIt)->getPhysicalTableName(), RelationshipChecker::COMMENT_FOR_RELATIONSHIP_LOOP);
					return;
				}
				//この関数で処理したEntity名をルート情報に登録する
				newTableRoute.push_back((*entityIt)->getPhysicalTableName());
				//子Entity情報を元に更に探索処理を行う
				this->recursiveSearchForRelationship(entityToRelationshipMap, (*relationshipIt), (*entityIt)->getPhysicalTableName(), startTableName, newTableRoute);
			}
		}
	}
}

/**
* @fn void RelationshipChecker::reportForRecursiveSearch(std::list<std::wstring> tableRoute, std::wstring startTableName, std::wstring endTableName, std::wstring message)
* @brief リレーションシップ情報再帰検索プログラム用report生成関数
* @param tableRoute 経由したテーブル情報
* @param startTableName 開始テーブル名
* @param endTableName 終了テーブル名
* @param message reportメッセージ
* @details リレーションシップ情報再帰検索プログラム用report生成関数
*/
void RelationshipChecker::reportForRecursiveSearch(std::list<std::wstring> tableRoute, std::wstring startTableName, std::wstring endTableName, std::wstring message)
{
	//Relationship探索履歴に、最後に辿ったテーブルの名前を登録する
	tableRoute.push_back(endTableName);
	//テーブル遷移履歴をメッセージ文として生成する際に一時的に使う変数
	wstring transactionHistoryMessage;
	//遷移回数
	int count = 0;
	//遷移してきたテーブルの数だけループ処理を行う
	for (auto targetTableNameIt = tableRoute.begin(); targetTableNameIt != tableRoute.end(); targetTableNameIt++)
	{
		if (count != 0)
		{
			//テーブル一つ目以外はカンマを設置しない
			transactionHistoryMessage += L" -> ";
		}
		//修正候補テーブル名を追加
		transactionHistoryMessage += (*targetTableNameIt);
		count++;
	}
	//メッセージを登録
	Reporter::getInstance()->addRecord(
		AlertLevelList::E_critical,
		CommandTypeList::E_ErDesignCheck,
		startTableName,
		RelationshipChecker::TABLE_COLUMNNAME + startTableName,
		message + transactionHistoryMessage
		);
}

/**
* @fn bool RelationshipChecker::isSetRelationshipForColumn(Entity *entity, Relationship *relationship, std::wstring tableName, std::wstring columnName)
* @brief リレーションシップ情報に対象のカラムが設定されているかチェックする関数
* @param entity Entity情報格納インスタンスのポインタ変数
* @param relationship Relationship情報格納インスタンスのポインタ変数
* @param tableName 比較対象のテーブル名
* @param columnName 比較対象のカラム名
* @details リレーションシップ情報に対象のカラムが設定されているかチェックする関数
*/
bool RelationshipChecker::isSetRelationshipForColumn(Entity *entity, Relationship *relationship, std::wstring tableName, std::wstring columnName)
{
	//対象のテーブル名が比較対象予定のテーブル名であるか確認
	if (entity->getPhysicalTableName().compare(tableName) != 0)
	{
		//対象テーブルの依存Columnを参照させないために処理をスキップ
		return false;
	}

	//リレーションシップ情報に登録された依存Columnの数だけループ
	for (auto dependKey = relationship->getEntity1DependKeyList().begin(); dependKey != relationship->getEntity1DependKeyList().end(); dependKey++)
	{
		//依存Columnと同名のカラムか確認
		if ((*dependKey).compare(columnName) == 0)
		{
			//同名のColumnがリレーションシップの依存Columnに登録されていることが判定されたため、
			//対象Columnのリレーションシップが登録されていると見なし呼び出し元にtrueを返す
			return true;
		}
	}
	//リレーションシップ情報に対象のColumnが見つからなかったため、呼び出し元にfalseを返す
	return false;
}

/**
* @fn bool RelationshipChecker::isMissingRelationship(ER *er, std::wstring tableName, std::wstring columnName)
* @brief 該当のテーブル・カラムに紐づくリレーションシップ情報が設定されているかチェックする関数
* @param er ER情報格納インスタンスのポインタ変数
* @param tableName 比較対象のテーブル名
* @param columnName 比較対象のカラム
* @details 該当のテーブル・カラムに紐づくリレーションシップ情報が設定されているかチェックし、判定結果を呼び出し元に返す。
*/
bool RelationshipChecker::isMissingRelationship(ER *er, std::wstring tableName, std::wstring columnName)
{
	//リレーションシップ情報の数だけループ処理
	for (auto relationshipIt = er->relationships.begin(); relationshipIt != er->relationships.end(); relationshipIt++)
	{
		//リレーションシップ情報に登録されているEntity情報の数だけループ処理(二回だけループ)
		for (auto rEntityIt = (*relationshipIt)->getEntityList().begin(); rEntityIt != (*relationshipIt)->getEntityList().end(); rEntityIt++)
		{
			//リレーションシップが設定されているかチェック
			if (this->isSetRelationshipForColumn(*rEntityIt, *relationshipIt, tableName, columnName))
			{
				//リレーションシップが設定されていればfalseを返す
				return false;
			}
		}
	}
	//ここに来た場合Columnに対するリレーションシップを検知できなかったことを意味するのでtrueを返す
	return true;
}

/**
* @fn void RelationshipChecker::checkRelationship(ER *er, const std::vector<std::wstring> &tableNames, const std::wstring &columnName)
* @brief リレーションシップ設定不足チェック関数
* @param er ER情報格納インスタンスのポインタ変数
* @param tableNames 比較対象のテーブル名一覧
* @param columnName 比較対象のカラム
* @details フィールド(カラム)名が同一であるEntity情報を取得し、リレーションシップ用結線が施されているかチェックを行う<br>リレーションシップの設定が不足していると推定された場合、Indexが設定されていなければ、Indexの設定の不足を警告する
*/
void RelationshipChecker::checkRelationship(ER *er, const std::vector<std::wstring> &tableNames, const std::wstring &columnName)
{
	//同名のColumnを持つテーブルの数だけループ処理
	for (auto tableNameIt = tableNames.begin(); tableNameIt != tableNames.end(); tableNameIt++)
	{
		//リレーションシップ設定ミスがあるか判定
		if (this->isMissingRelationship(er, *tableNameIt, columnName))
		{
			//リレーションシップ設定にミスが検知された場合、リレーションシップの修正指摘メッセージを生成する。

			wstring targetCsvList = L"";//修正候補テーブルリスト(カンマ区切り)
			int count = 0;//修正候補テーブル数

			//同名のColumnを持つEntityの数だけループ処理
			for (auto targetTableNameIt = tableNames.begin(); targetTableNameIt != tableNames.end(); targetTableNameIt++)
			{
				if ((*tableNameIt).compare((*targetTableNameIt)) == 0)
				{
					//リレーションシップ修正候補に登録する予定のテーブル名が、
					//現在のテーブルと同一であれば、登録処理をスキップ
					continue;
				}
				if (count != 0)
				{
					//テーブル一つ目以外はカンマを設置しない
					targetCsvList += L",";
				}
				//修正候補テーブル名を追加
				targetCsvList += (*targetTableNameIt);
				count++;
			}

			//アラートレベルの決定
			//Primary Keyに登録されているかつ、リレーションシップが無い場合は、警告度を通常より高くする。
			AlertLevelList alertLevel = AlertLevelList::E_note;
			Entity *entity = er->entityMap[(*tableNameIt)];
			bool isPkFlg = false;
			wstring message = RelationshipChecker::COMMENT_FOR_MISSNG_RELATIONSHIP;

			if (IndexChecker::isSetPrimaryKey(entity, columnName))
			{
				//Primary Keyリストにありながらリレーションシップがない場合は、警告度上昇
				isPkFlg = true;
				alertLevel = AlertLevelList::E_warning;
			}

			//該当カラムがSecondary Indexに登録されていないかチェック
			if (IndexChecker::isMissingIndexForIndexList(entity->getIndexs(), columnName))
			{
				//リレーションシップ設定ミスの疑いがあり、かつIndex設定がなければ、
				//弱めの警告度でレポーターに報告
				Reporter::getInstance()->addRecord(
					AlertLevelList::E_note,
					CommandTypeList::E_IndexCheck,
					(*tableNameIt),
					RelationshipChecker::TAG_COLUMNNAME + columnName,
					RelationshipChecker::COMMENT_FOR_MISSING_INDEX_AND_RELATIONSHIP
					);
			}
			else if (isPkFlg != false)
			{
				//該当ColumnにてPKの設定がなく、Indexの設定があった場合、
				//リレーションシップの設定ミスが高いと判断する
				alertLevel = AlertLevelList::E_critical;
				message = RelationshipChecker::COMMENT_FOR_MISSNG_RELATIONSHIP_AND_FIND_INDEX;
			}

			//レポーターに報告
			Reporter::getInstance()->addRecord(
				alertLevel,
				CommandTypeList::E_RelationshipCheck,
				(*tableNameIt),
				RelationshipChecker::TAG_COLUMNNAME + columnName,
				message + targetCsvList
				);
		}
	}
}

/**
* @fn void RelationshipChecker::checkForFieldMap(ER *er, map <std::wstring, std::vector<std::wstring>>&fieldMap)
* @brief フィールド(カラム)マップ用リレーションシップチェック関数
* @param er ER情報格納インスタンスのポインタ変数
* @param fieldMap フィールド(カラム)と紐付いているEntity情報格納情報
* @details フィールド(カラム)名が同一であるEntity情報を取得し、リレーションシップ用結線が施されているかチェックを行う
*/
void RelationshipChecker::checkForFieldMap(ER *er, map <std::wstring, std::vector<std::wstring>>&fieldMap)
{
	//フィールド(Column)名の数だけループ
	for (auto fieldIt = fieldMap.begin(); fieldIt != fieldMap.end(); fieldIt++)
	{
		//Entity情報を取得
		auto entityIt = (*fieldIt).second.begin();
		if ((*fieldIt).second.size() < 2)
		{
			//同名のフィールドがない場合、処理をスキップ
			continue;
		}
		//リレーションシップのチェック処理
		this->checkRelationship(er, (*fieldIt).second, (*fieldIt).first);
	}
}