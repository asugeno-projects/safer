/**
* @file       ermErParser.cpp
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

#include "ermErParser.h"
#include "../../common/commonString.h"
#include "../../log/logger.h"
#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMText.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <boost/lexical_cast.hpp>

using namespace xercesc;

#include "../../common/commonXerces.h"

/**
* @fn class ER * ErmErParser::parse(void)
* @brief ER Master形式のER図ファイルをパースし、ERデータを生成する関数
* @return er ER情報格納インスタンスのポインタ情報
* @details ER Master形式のER図ファイルをパースし、ERデータを生成し呼び出し元に返す
*/
class ER * ErmErParser::parse(void)
{
	//この定義が無いとxlsxファイルの日本語が文字化けします。
	std::locale::global(std::locale(""));
	std::ios_base::sync_with_stdio(false);

	ER *er = new ER();
	logger::info() << logger::logPrefix("INFO") << "File Open > " << this->erFilePath << std::endl;

	// Xerces-C++を初期化する
	try {
		XMLPlatformUtils::Initialize();
	}
	catch (...) {
		logger::error() << logger::logPrefix("ERROR") << "Xerces-C++の初期化に失敗しました。" << std::endl;
		return NULL;
	}

	try {
		//Xerces DOMパーサーの取得
		XercesDOMParser* parser = new XercesDOMParser();
		//Xercesパース開始
		parser->parse(this->erFilePath.c_str());

		//Documentオブジェクトの取得
		DOMDocument* dom = parser->getDocument();
		//全体のエレメントを取得
		DOMElement *root = dom->getDocumentElement();
		
		//wordMap作成
		this->setWordMap(root);

		//fieldsMap作成
		this->setFieldsMap(root);

		//参照カラム情報を正しいカラム情報と紐付ける
		this->mergeFieldsMapAndReferenceField(root);

		//Entityにカラム情報をセットする
		this->setEntity(root, er);

		//relationship情報の設定
		this->setRelationship(root, er);

		//カテゴリ(TAG)情報設定
		this->setTag(root);

		//Xerces DOMパーサー終了処理;
		XMLPlatformUtils::Terminate();

		//パーサー終了処理
		this->finalize();
	}
	catch (...) {
		logger::error() << logger::logPrefix("ERROR") << "コマンドファイル解析に失敗しました。" << std::endl;
	}

	//ER図情報を格納したインスタンスのポインタ情報を返す
	return er;
}

/**
* @fn void ErmErParser::setWordMap(DOMElement *domElement)
* @brief DOM Elementから辞書データを抽出し、辞書データとIDをひも付けたマップを作成する
* @param domElement ER MasterのER情報を格納したDOMオブジェクト
* @details DOM Elementから辞書データを抽出し、辞書データとIDをひも付けたマップを作成する
*/
void ErmErParser::setWordMap(DOMElement *domElement)
{
	//wordの要素、Word情報をDOMNodeリストとして取得
	DOMNodeList* wordNodeList = getElementByCharTagName(domElement, ErmErParser::WORD.c_str());

	//ノード数(単語数)の取得
	int wordNum = (int)(wordNodeList->getLength());

	//ワード(辞書データ)に登録されている単語の数だけループ処理を行う
	for (int i = 0; i < wordNum; i++)
	{
		//単語データを一つ取得する
		DOMNodeList* wordObject = wordNodeList->item(i)->getChildNodes();

		//カラムクラスインスタンスの生成
		fieldParam *field = new fieldParam();

		//カラムIDを保持する変数の定義
		std::string id;

		//単語パラメータの数を取得
		int wordParamNum = wordObject->getLength();

		//単語パラメータの数だけループ処理を行う
		for (int j = 0; j < wordParamNum; j++)
		{
			//単語パラメータのオブジェクトを取得
			DOMNode* node = wordObject->item(j);
			//単語パラメータの要素情報を取得
			NodeParams* param = getNodeDataString(node);

			//パラメータ名からデータをセットする関数(イテレータ)を取得する
			SetErmColumnFunc::iterator it = setErmColumnFunc.find(param->name);

			if (it != setErmColumnFunc.end())
			{
				//イテレータの末端でなければ関数を実行する
				it->second(field, param);
			}
			if (param->name.compare(ErmErParser::ID) == 0)
			{
				//IDパラムで有った場合、id格納変数にデータをセットする
				id = param->param;
			}

			//パラメータ情報の持った変数のメモリを解放
			delete param;
		}

		//単語マップにidとカラム情報を登録する
		wordsMap.insert(std::make_pair(id, *field));
	}
}

/**
* @fn void ErmErParser::setFieldsMap(DOMElement *domElement)
* @brief DOM Elementからカラムデータを取得し、カラムデータマップを作成する関数
* @param domElement ER MasterのER情報を格納したDOMオブジェクト
* @details DOM Elementからカラムデータを取得、ワードマップから適用するカラム情報を取得し、カラムデータを生成する。<br>その後、生成したカラムをカラムデータマップを作成する
*/
void ErmErParser::setFieldsMap(DOMElement *domElement)
{
	//normal_column(カラム)の要素を取得
	DOMNodeList* columnNodeList = getElementByCharTagName(domElement, ErmErParser::NORMAL_COLUMN.c_str());

	//カラム数の取得
	int columnNum = (int)(columnNodeList->getLength());

	//カラム数に応じてループ処理を行う
	for (int k = 0; k < columnNodeList->getLength(); k++)
	{
		//ワードID(単語ID)を保持する変数
		std::string id;
		//カラム情報を取得
		DOMNode* columnNode = columnNodeList->item(k);
		//カラムパラメータ一覧を取得
		DOMNodeList* columnParams = columnNode->getChildNodes();
		//パラメータの数を取得
		int colomnParamNum = columnParams->getLength();
		//パラメータの数だけループ処理を行う
		for (int l = 0; l < colomnParamNum; l++)
		{
			//カラムパラメータ要素の取得
			DOMNode* columnParam = columnParams->item(l);
			//取得したカラムパラメータ要素からパラメータ情報を取得
			NodeParams* param = getNodeDataString(columnParam);

			if (param->name.compare(ErmErParser::WORD_ID) == 0)
			{
				//ワードID(単語ID)が登録されていた場合、単語IDを保持する
				id = param->param;
			}
			else if (param->name.compare(ErmErParser::ID) == 0 && param->param.compare(ErmErParser::NULL_STRING) != 0 && wordsMap.find(id) != wordsMap.end())
			{
				//ワードID(単語ID)が登録されているカラムだった場合、
				//ワードマップからカラム情報を取得しカラムデータマップにセットする。
				this->fieldsMap[param->param] = new fieldParam((wordsMap[id]));

				//ワードマップからカラムデータをセットすれば良いので、
				//このタイミングでループ処理を終了する。
				//他のカラムパラメータを参照する場合は、下記のbreakは消す必要がある。
				break;
			}

			//パラメータ情報の持った変数のメモリを解放
			delete param;
		}
	}
}

/**
* @fn void ErmErParser::mergeFieldsMapAndReferenceField(DOMElement *domElement)
* @brief DOM Elementからカラムデータを取得し、カラム参照情報を元にカラム情報を合成する関数
* @param domElement ER MasterのER情報を格納したDOMオブジェクト
* @details カラム参照情報とカラムマップから適用するカラム情報を取得しカラムデータを合成する。
*/
void ErmErParser::mergeFieldsMapAndReferenceField(DOMElement *domElement)
{
	//tableの要素、Entity情報をDOMNodeリストとして取得
	DOMNodeList* columnNodeList = getElementByCharTagName(domElement, ErmErParser::NORMAL_COLUMN.c_str());

	//IDに紐づく参照関係にあるIDをリストとして保持する変数
	//参照関係を確認した際に、必ず参照先のカラムデータが生成完了しているとは限らないため、
	//参照先のカラムのデータが生成されたタイミングで処理が終わってない参照カラムIDを保持させます。
	std::map<std::string, std::list<std::string>> referenceFielsMap;

	//カラムの数だけループ処理を行う
	for (int k = 0; k < columnNodeList->getLength(); k++)
	{
		//ID情報を保持する変数
		std::string id;
		//カラム要素を取得する
		DOMNode* columnNode = columnNodeList->item(k);
		//カラムパラメータの取得
		DOMNodeList* columnParams = columnNode->getChildNodes();
		//パラメータ数を取得
		int columnParamNum = columnParams->getLength();

		//カラムパラメータの数だけループ処理を行う
		for (int l = 0; l < columnParamNum; l++)
		{
			//パラメータ要素を取得する
			DOMNode* columnParam = columnParams->item(l);
			//パラメータ要素からパラメータのname、value、アトリビューを取得
			NodeParams* param = getNodeDataString(columnParam);

			if (param->name.compare(ErmErParser::ID) == 0 && param->param.compare(ErmErParser::NULL_STRING) != 0)
			{
				//カラムIDが"null"(文字列)でない場合IDを保持する
				id = param->param;
			}
			else if (param->name.compare(ErmErParser::REFERENCED_COLUMN) == 0 && param->param.compare(ErmErParser::NULL_STRING) != 0)
			{
				//カラム参照情報があった場合、下記の処理を行う
				if (this->fieldsMap.find(param->param) != this->fieldsMap.end())
				{
					//カラムマップに対象のデータが有った場合、カラム情報をセットする関数を呼び出す
					this->recursiveSetFieldMap(param->param, id, &referenceFielsMap);
				}
				else
				{
					//こちらに来た場合、まだ参照先のデータの生成が終わっていないため、参照先のIDに紐付いたリストに登録する
					referenceFielsMap[param->param].push_back(id);
				}
			}

			//パラメータ情報の持った変数のメモリを解放
			delete param;
		}
	}
}

/**
* @fn void ErmErParser::setEntity(DOMElement *domElement, ER *er)
* @brief Entity情報を生成関数
* @param domElement ER MasterのER情報を格納したDOMオブジェクト
* @param er ER図情報を格納するクラスのインスタンス
* @details DOM ElementからEntity情報を抽出し、Entityの一覧を生成する関数
*/
void ErmErParser::setEntity(DOMElement *domElement, ER *er)
{
	//tableの要素、Entity情報をDOMNodeリストとして取得
	DOMNodeList* domNodeList = getElementByCharTagName(domElement, ErmErParser::TABLE.c_str());
	//ノード数の取得
	int entityNum = (int)(domNodeList->getLength());

	//Entityの数だけループ処理を行う
	for (int i = 0; i < entityNum; i++)
	{
		//Entityインスタンスの生成
		Entity *entity = new Entity();
		//Entity情報をERインスタンスリストにpushする
		er->entitys.push_back(entity);
		//テーブルオブジェクトを取得します。
		DOMNodeList* tableObject = domNodeList->item(i)->getChildNodes();
		int tableObjectNum = tableObject->getLength();
		//テーブル要素に登録されている子要素の数だけループ処理を行う
		for (int j = 0; j < tableObjectNum; j++)
		{
			//テーブルの子要素を取得
			DOMNode* node = tableObject->item(j);
			//テーブルの子要素のパラメータを取得する
			NodeParams* param = getNodeDataString(node);
			//テーブルの子要素の処理を行う関数を取得する
			SetErmErFunc::iterator it = setErmErFunc.find(param->name);
			if (it != this->setErmErFunc.end())
			{
				//関数イテレータが末端を示すイテレータでない場合、関数を実行する
				it->second(entity, param, node, this);
			}
			if (param->name.compare("id") == 0)
			{
				//パラメータ名がidで有った場合、
				//EntityとIDを紐付けるマップ変数にIDとそれに対応するEntity情報をセットする
				entitysIdMap.insert(make_pair(param->param, entity));
			}

			//パラメータ情報の持った変数のメモリを解放
			delete param;
		}
		//ER図情報のentityマップ変数に値をセットする
		er->entityMap.insert(make_pair(entity->getPhysicalTableName(), entity));
	}
}

/**
* @fn void ErmErParser::setRelationship(DOMElement *domElement, ER *er)
* @brief Relationship情報を生成関数
* @param domElement ER MasterのER情報を格納したDOMオブジェクト
* @param er ER図情報を格納するクラスのインスタンス
* @details DOM ElementからRelationship情報を抽出し、Relationshipの一覧を生成する関数
*/
void ErmErParser::setRelationship(DOMElement *domElement, ER *er)
{
	//Relationship情報をDOMNodeリストとして取得
	DOMNodeList* relationshipNodeList = getElementByCharTagName(domElement, ErmErParser::RELATIONSHIP.c_str());

	//Relationshipの数を取得する
	int relationshipNum = (int)(relationshipNodeList->getLength());

	//Relationshipの数だけループ処理を行う
	for (int i = 0; i < relationshipNum; i++)
	{
		//Relationshipの要素とノードパラメータを取得する
		NodeParams* param = getNodeDataString(relationshipNodeList->item(i));
		if (!std::all_of(param->param.cbegin(), param->param.cend(), isdigit))
		{
			//文字列情報が数字でなかった場合、
			//下記のRelationship情報インスタンス生成関数を実行する
			this->createRelationship(er, relationshipNodeList->item(i)->getChildNodes());
		}

		//パラメータ情報の持った変数のメモリを解放
		delete param;
	}
}

/**
* @fn void ErmErParser::setTag(DOMElement *domElement)
* @brief カテゴリ(TAG)情報を登録関数
* @param domElement ER MasterのER情報を格納したDOMオブジェクト
* @details DOM ElementからEntityのカテゴリ(TAG)情報を抽出し、対象のEntityにセットする関数
*/
void ErmErParser::setTag(DOMElement *domElement)
{
	//カテゴリ(TAG)情報設定
	DOMNodeList* categoryNodeList = getElementByCharTagName(domElement, ErmErParser::CATEGORY.c_str());

	//カテゴリ数を取得する
	int categoryNum = (int)(categoryNodeList->getLength());

	//カテゴリの数だけループ処理を行う
	for (int i = 0; i < categoryNum; i++)
	{
		//カテゴリ情報を格納したDOM Listを取得
		DOMNodeList* categoryObject = categoryNodeList->item(i)->getChildNodes();
		//カテゴリ名を一時的に保持する変数
		std::wstring categoryName;
		//カテゴリパラメータの数を取得
		int categoryParamNum = categoryObject->getLength();
		//カテゴリパラメータの数だけループ処理
		for (int j = 0; j < categoryParamNum; j++)
		{
			//カテゴリパラメータを取得
			DOMNode* categoryParam = categoryObject->item(j);
			//カテゴリパラメータから文字列情報を取得
			NodeParams* param = getNodeDataString(categoryParam);
			if (param->name.compare(ErmErParser::NAME) == 0)
			{
				//ノード名がnameだった場合の処理
				//カテゴリ名を取得
				stringToWstringForUtf8(&categoryName, param->param);
			}
			else if (param->name.compare(ErmErParser::NODE_ELEMENT) == 0 && entitysIdMap.find(param->param) != entitysIdMap.end())
			{
				//ノード名がnode_elementかつ、valueがEntityマップに登録されていた場合、
				//対象Entityにカテゴリ名をセットする
				entitysIdMap[param->param]->setTagName(categoryName);
			}

			//パラメータ情報の持った変数のメモリを解放
			delete param;
		}
	}
}

/**
* @fn void ErmErParser::setPhysicalTableName(Entity *entity, struct NodeParams* param, DOMNode* node, ErmErParser* instance)
* @brief Entityインスタンスにテーブル物理名をセットする関数
* @param entity Entityインスタンス
* @param param  ノードパラメータ
* @param node   DOMノードインスタンス
* @param instance ErmErParserインスタンス
* @details Entityインスタンスにテーブル物理名をセットする関数
*/
void ErmErParser::setPhysicalTableName(Entity *entity, struct NodeParams* param, DOMNode* node, ErmErParser* instance)
{
	//指定されたパラメータから物理テーブル名を取得しセットする
	wchar_t *tableName = stringToWchar(param->param);
	entity->setPhysicalTableName(tableName);
	delete tableName;
}

/**
* @fn void ErmErParser::setLogicalTableName(Entity *entity, struct NodeParams* param, DOMNode* node, ErmErParser* instance)
* @brief Entityインスタンスにテーブル論理名をセットする関数
* @param entity Entityインスタンス
* @param param  ノードパラメータ
* @param node   DOMノードインスタンス
* @param instance ErmErParserインスタンス
* @details Entityインスタンスにテーブル論理名をセットする関数
*/
void ErmErParser::setLogicalTableName(Entity *entity, struct NodeParams* param, DOMNode* node, ErmErParser* instance)
{
	//指定されたパラメータから論理テーブル名を取得しセットする
	wchar_t *tableName = stringToWchar(param->param);
	entity->setLogicalTableName(tableName);
	delete tableName;
}

/**
* @fn void ErmErParser::setIndexes(Entity *entity, struct NodeParams* param, DOMNode* node, ErmErParser* instance)
* @brief Entityインスタンスにインデックス情報をセットする関数
* @param entity Entityインスタンス
* @param param  ノードパラメータ
* @param node   DOMノードインスタンス
* @param instance ErmErParserインスタンス
* @details Entityインスタンスにインデックス情報をセットする関数
*/
void ErmErParser::setIndexes(Entity *entity, struct NodeParams* param, DOMNode* node, ErmErParser* instance)
{
	//インデックス情報を格納したDOMを取得する
	DOMNodeList* indexList = node->getChildNodes();
	//インデックスの数を取得
	int indexNum = indexList->getLength();
	//インデックスの数だけループ処理を行う
	for (int i = 0; i < indexNum; i++)
	{
		//インデックスのDOM情報を取得
		DOMNode* indexNode = indexList->item(i);
		//インデックスのパラメータが格納されたDOMノードリストを取得
		DOMNodeList* indexParams = indexNode->getChildNodes();
		//インデックスのパラメータ情報数を取得
		int indexParamNum = indexParams->getLength();
		//インデックスのパラメータ情報数の数だけループ処理
		for (int j = 0; j < indexParamNum; j++)
		{
			//インデックスパラメータの取得
			DOMNode* indexParamNode = indexParams->item(j);
			//インデックスパラメータ情報の取得
			NodeParams* indexParam = getNodeDataString(indexParamNode);

			//パラメータ名に紐づくインデックス関連の処理を行う関数を取得
			ErmErParser::SetErmIndexFunc::iterator it = instance->setErmIndexFunc.find(indexParam->name);
			if (it != instance->setErmIndexFunc.end())
			{
				//各パラメータの処理を行う
				it->second(entity, indexParamNode, instance);
			}

			//パラメータ情報の持った変数のメモリを解放
			delete indexParam;
		}
	}
}

/**
* @fn void ErmErParser::setColumns(Entity *entity, struct NodeParams* param, DOMNode* node, ErmErParser* instance)
* @brief EntityインスタンスにColumn情報をセットする関数
* @param entity Entityインスタンス
* @param param  ノードパラメータ
* @param node   DOMノードインスタンス
* @param instance ErmErParserインスタンス
* @details EntityインスタンスにColumn情報をセットする関数
*/
void ErmErParser::setColumns(Entity *entity, struct NodeParams* param, DOMNode* node, ErmErParser* instance)
{
	//Columnの情報を持ったDOMノードリストを取得
	DOMNodeList* columnList = node->getChildNodes();
	//カラム数を取得
	int columnNum = columnList->getLength();
	//カラム数分、ループ処理を行う
	for (int i = 0; i < columnNum; i++)
	{
		//Column情報が入ったDOMノードを取得
		DOMNode* columnNode = columnList->item(i);
		//Columnパラメータを含んだDOMノードリストを取得
		DOMNodeList* columnParams = columnNode->getChildNodes();
		//Columnパラメータのポインタ変数の定義
		fieldParam * field = NULL;
		//Primary KeyIDを保持する変数
		int pkId = 0;
		//リレーションシップ関係にあるカラム情報であるかを保持する変数
		bool relationshipFlg = false;
		//ColumnID情報を保持する変数
		std::string id;
		//Columnパラメータの数を取得
		int columnParamNum = columnParams->getLength();
		//Columnパラメータの数だけループ処理
		for (int j = 0; j < columnParamNum; j++)
		{
			//Columnパラメータを保持するDOMノード取得
			DOMNode* columnParam = columnParams->item(j);
			//Columnパラメータ情報の取得
			NodeParams* param = getNodeDataString(columnParam);
			//Columnパラメータのnameに紐づく関数を取得する
			SetErmFieldFunc::iterator it = instance->setErmFieldFunc.find(param->name);
			if (it != instance->setErmFieldFunc.end())
			{
				//Columnのパラメータをセットする
				field = it->second(field, param, &relationshipFlg, &id, entity, instance);
			}
			//パラメータ情報のために確保したメモリを解放する
			delete param;
		}
		if (field != NULL)
		{
			//Column情報が生成された場合、EntityインスタンスのColumnリストに登録する
			entity->setfields(*field);
		}
		if (relationshipFlg == true)
		{
			//リレーションシップ関係にあるColumnだった場合、リレーションシップマップ変数に物理カラム名をセットする
			//リレーションシップ定義の数だけループ処理を行う
			for (auto relationshipIdIt = instance->relationshipId.begin(); relationshipIdIt != instance->relationshipId.end(); relationshipIdIt++)
			{
				//リレーションシップIDとColumn物理名を紐付ける
				instance->relationshipMap[(*relationshipIdIt)].push_back(field->name.physicalName);
				if (instance->entitysMap.find((*relationshipIdIt)) == instance->entitysMap.end())
				{
					//EntityインスタンスをEntityMapにセットする
					instance->entitysMap.insert(make_pair((*relationshipIdIt), entity));
				}
			}
		}
		//リレーションシップIDリストをクリアする
		instance->relationshipId.clear();

	}
}

/**
* @fn fieldParam * ErmErParser::getFieldForFieldsMap(fieldParam * field, NodeParams* nodeParam, bool *relationshipFlg, std::string *fieldId, Entity * entity, ErmErParser* instance)
* @brief フィールドマップからフィールド情報を取得する
* @param field フィールドパラメータ
* @param nodeParam ノードパラメータ
* @param relationshipFlg リレーションシップ判定フラグ
* @param fieldId フィールドID
* @param entity Entityインスタンス
* @param instance ErmErParserインスタンス
* @return field Columnインスタンスのポインタ情報
* @details フィールドマップからフィールド情報を取得する
*/
fieldParam * ErmErParser::getFieldForFieldsMap(fieldParam * field, NodeParams* nodeParam, bool *relationshipFlg, std::string *fieldId, Entity * entity, ErmErParser* instance)
{
	if (instance->fieldsMap.find(nodeParam->param) != instance->fieldsMap.end())
	{
		//フィールドマップにすでに登録されていた場合、
		//フィールドマップからポインタを取得し、ポインタ変数にセットする
		field = instance->fieldsMap[nodeParam->param];
	}
	//ID情報をセットする
	*fieldId = nodeParam->param;
	return field;
}

/**
* @fn fieldParam * ErmErParser::addRelationshipId(fieldParam * field, NodeParams* nodeParam, bool *relationshipFlg, std::string *fieldId, Entity * entity, ErmErParser* instance)
* @brief リレーションシップIDをセットする関数
* @param field フィールドパラメータ
* @param nodeParam ノードパラメータ
* @param relationshipFlg リレーションシップ判定フラグ
* @param fieldId フィールドID
* @param entity Entityインスタンス
* @param instance ErmErParserインスタンス
* @return field Columnインスタンスのポインタ情報
* @details リレーションシップIDをセットする関数
*/
fieldParam * ErmErParser::addRelationshipId(fieldParam * field, NodeParams* nodeParam, bool *relationshipFlg, std::string *fieldId, Entity * entity, ErmErParser* instance)
{
	//リレーションシップフラグをtrueにする
	*relationshipFlg = true;
	//リレーションシップ関係候補リストにID情報をセットする
	instance->relationshipId.push_back(nodeParam->param);
	return field;
}

/**
* @fn fieldParam * ErmErParser::setColomnParam(fieldParam * field, NodeParams* nodeParam, bool *relationshipFlg, std::string *fieldId, Entity * entity, ErmErParser* instance)
* @brief Column情報をセットする関数
* @param field フィールドパラメータ
* @param nodeParam ノードパラメータ
* @param relationshipFlg リレーションシップ判定フラグ
* @param fieldId フィールドID
* @param entity Entityインスタンス
* @param instance ErmErParserインスタンス
* @return field Columnインスタンスのポインタ情報
* @details Column情報をセットする関数
*/
fieldParam * ErmErParser::setColomnParam(fieldParam * field, NodeParams* nodeParam, bool *relationshipFlg, std::string *fieldId, Entity * entity, ErmErParser* instance)
{
	//Column情報セット関数を取得する
	SetErmColumnFunc::iterator it = setErmColumnFunc.find(nodeParam->name);
	if (it != setErmColumnFunc.end() && field != NULL)
	{
		//該当のColumn情報セット関数を実行する
		it->second(field, nodeParam);
	}
	return field;
}

/**
* @fn fieldParam * ErmErParser::setNotNullFlg(fieldParam * field, NodeParams* nodeParam, bool *relationshipFlg, std::string *fieldId, Entity * entity, ErmErParser* instance)
* @brief Not Null制約のフラグをColumn情報にセットする
* @param field フィールドパラメータ
* @param nodeParam ノードパラメータ
* @param relationshipFlg リレーションシップ判定フラグ
* @param fieldId フィールドID
* @param entity Entityインスタンス
* @param instance ErmErParserインスタンス
* @return field Columnインスタンスのポインタ情報
* @details Not Null制約のフラグをColumn情報にセットする
*/
fieldParam * ErmErParser::setNotNullFlg(fieldParam * field, NodeParams* nodeParam, bool *relationshipFlg, std::string *fieldId, Entity * entity, ErmErParser* instance)
{
	//Not Null制約
	if (field != NULL)
	{
		//フィールドポインタ変数がNULL出なかった場合、Not Null制約フラグをtrueにする
		field->notNullFlg = boost::iequals(instance->BOOL_TRUE, nodeParam->param);
	}
	return field;
}

/**
* @fn fieldParam * ErmErParser::setDefaultParam(fieldParam * field, NodeParams* nodeParam, bool *relationshipFlg, std::string *fieldId, Entity * entity, ErmErParser* instance)
* @brief デフォルトパラメータのフラグをColumn情報にセットする
* @param field フィールドパラメータ
* @param nodeParam ノードパラメータ
* @param relationshipFlg リレーションシップ判定フラグ
* @param fieldId フィールドID
* @param entity Entityインスタンス
* @param instance ErmErParserインスタンス
* @return field Columnインスタンスのポインタ情報
* @details デフォルトパラメータのフラグをColumn情報にセットする
*/
fieldParam * ErmErParser::setDefaultParam(fieldParam * field, NodeParams* nodeParam, bool *relationshipFlg, std::string *fieldId, Entity * entity, ErmErParser* instance)
{
	if (field != NULL)
	{
		//デフォルト値をセットする
		stringToWstringForUtf8(&field->defaultParam, nodeParam->param);
	}
	return field;
}

/**
* @fn fieldParam * ErmErParser::setAutoIncrement(fieldParam * field, NodeParams* nodeParam, bool *relationshipFlg, std::string *fieldId, Entity * entity, ErmErParser* instance)
* @brief AUTO INCREMENT情報をColumn情報にセットする
* @param field フィールドパラメータ
* @param nodeParam ノードパラメータ
* @param relationshipFlg リレーションシップ判定フラグ
* @param fieldId フィールドID
* @param entity Entityインスタンス
* @param instance ErmErParserインスタンス
* @return field Columnインスタンスのポインタ情報
* @details AUTO INCREMENT情報をColumn情報にセットする
*/
fieldParam * ErmErParser::setAutoIncrement(fieldParam * field, NodeParams* nodeParam, bool *relationshipFlg, std::string *fieldId, Entity * entity, ErmErParser* instance)
{
	if (field != NULL && boost::iequals(instance->BOOL_TRUE, nodeParam->param))
	{
		//DDLオプションをセットする
		field->DDLOptions.push_back(instance->AUTO_INCREMENT_W);
	}
	return field;
}

/**
* @fn fieldParam * ErmErParser::setPrimaryKey(fieldParam * field, NodeParams* nodeParam, bool *relationshipFlg, std::string *fieldId, Entity * entity, ErmErParser* instance)
* @brief Primary Key情報をEntityインスタンスにセットする
* @param field フィールドパラメータ
* @param nodeParam ノードパラメータ
* @param relationshipFlg リレーションシップ判定フラグ
* @param fieldId フィールドID
* @param entity Entityインスタンス
* @param instance ErmErParserインスタンス
* @return field Columnインスタンスのポインタ情報
* @details Primary Key情報をEntityインスタンスにセットする
*/
fieldParam * ErmErParser::setPrimaryKey(fieldParam * field, NodeParams* nodeParam, bool *relationshipFlg, std::string *fieldId, Entity * entity, ErmErParser* instance)
{
	if (field != NULL && boost::iequals(instance->BOOL_TRUE, nodeParam->param))
	{
		//Primary Keyリストにセットする
		entity->insertPk(entity->getPks().size(), field->name.physicalName);
	}
	return field;
}

/**
* @fn void ErmErParser::createRelationship(ER *er, DOMNodeList* relationshipObject)
* @brief リレーションシップ情報生成関数
* @param er フィールドパラメータ
* @param relationshipObject ノードパラメータ
* @details リレーションシップ情報生成関数
*/
void ErmErParser::createRelationship(ER *er, DOMNodeList* relationshipObject)
{
	//リレーションシップインスタンスを生成する
	Relationship *relationship = new Relationship();
	//リレーションシップリストにリレーションシップインスタンスをセットする
	er->relationships.push_back(relationship);
	//リレーションシップパラメータ数を取得
	int relationshipNum = relationshipObject->getLength();
	//リレーションシップのパラメータの数だけループ処理
	for (int j = 0; j < relationshipNum; j++)
	{
		//リレーションシップのパラメータ情報を保持するDOMノードを取得する
		DOMNode* node = relationshipObject->item(j);
		//リレーションシップのパラメータ情報を文字列情報に変換する
		NodeParams* param = getNodeDataString(node);
		//リレーションシップパラメータを処理する関数を取得する
		SetErmRelationshipFunc::iterator it = this->setErmRelationshipFunc.find(param->name);
		if (it != this->setErmRelationshipFunc.end())
		{
			//リレーションシップパラメータ名に従って処理を行う
			it->second(relationship, param, this);
		}
		//一時的に確保されたパラメータのメモリを解放する
		delete param;
	}
}

/**
* @fn void ErmErParser::setPhysicalColumnName(class fieldParam *field, struct NodeParams* param)
* @brief 物理Column名をColumnインスタンスににセットする
* @param field フィールドパラメータ
* @param param ノードパラメータ
* @details 物理Column名をColumnインスタンスににセットする
*/
void ErmErParser::setPhysicalColumnName(class fieldParam *field, struct NodeParams* param)
{
	if (param->param.size() > 0)
	{
		//Column物理名をセットする
		stringToWstringForUtf8(&field->name.physicalName, param->param);
	}
}

/**
* @fn void ErmErParser::setLogicalColumnName(class fieldParam *field, struct NodeParams* param)
* @brief 論理Column名をColumnインスタンスににセットする
* @param field フィールドパラメータ
* @param param ノードパラメータ
* @details 論理Column名をColumnインスタンスににセットする
*/
void ErmErParser::setLogicalColumnName(class fieldParam *field, struct NodeParams* param)
{
	if (param->param.size() > 0)
	{
		//Column論理名をセットする
		stringToWstringForUtf8(&field->name.logicalName, param->param);
	}
}

/**
* @fn void ErmErParser::setColumnType(class fieldParam *field, struct NodeParams* param)
* @brief Columnデータ型情報をColumnインスタンスににセットする
* @param field フィールドパラメータ
* @param param ノードパラメータ
* @details Columnデータ型情報をColumnインスタンスににセットする
*/
void ErmErParser::setColumnType(class fieldParam *field, struct NodeParams* param)
{
	if (param->param.size() > 0)
	{
		//Columnデータ型をセットする
		stringToWstringForUtf8(&field->type, param->param);
	}
}

/**
* @fn void ErmErParser::setColumnComment(class fieldParam *field, struct NodeParams* param)
* @brief Columnコメント情報をColumnインスタンスににセットする
* @param field フィールドパラメータ
* @param param ノードパラメータ
* @details Columnコメント情報をColumnインスタンスににセットする
*/
void ErmErParser::setColumnComment(class fieldParam *field, struct NodeParams* param)
{
	if (param->param.size() > 0)
	{
		//Columnのコメント情報をセットする
		stringToWstringForUtf8(&field->name.comment, param->param);
	}
}

/**
* @fn void ErmErParser::setDependKeyList(Relationship * relationship, NodeParams* nodeParam, ErmErParser* instance)
* @brief リレーションシップ関係にあるカラム名情報をリレーションシップインスタンスにセットする
* @param relationship リレーションシップインスタンス
* @param nodeParam ノードパラメータ
* @param instance ErmErParserインスタンス
* @details リレーションシップ関係にあるカラム名情報をリレーションシップインスタンスにセットする
*/
void ErmErParser::setDependKeyList(Relationship * relationship, NodeParams* nodeParam, ErmErParser* instance)
{
	//リレーションシップの依存Columnをセットする
	//ER Masterの場合、依存Columnは同一Columnとして保持させるため、Column名が変わらない
	//そのため、下記のように同じ名前がセットされている前提で処理を実装できる
	std::list<std::wstring>dependKeyList = instance->getRelationshipMap(nodeParam->param);
	for (auto keyIt = dependKeyList.begin(); keyIt != dependKeyList.end(); keyIt++)
	{
		relationship->setEntity1DependKey(*keyIt);
		relationship->setEntity2DependKey(*keyIt);
	}
}

/**
* @fn void ErmErParser::setEntityForRelationship(Relationship * relationship, NodeParams* nodeParam, ErmErParser* instance)
* @brief リレーションシップ関係にあるEntityインスタンスのポインタ情報をリレーションシップインスタンスにセットする
* @param relationship リレーションシップインスタンス
* @param nodeParam ノードパラメータ
* @param instance ErmErParserインスタンス
* @details リレーションシップ関係にあるEntityインスタンスのポインタ情報をリレーションシップインスタンスにセットする
*/
void ErmErParser::setEntityForRelationship(Relationship * relationship, NodeParams* nodeParam, ErmErParser* instance)
{
	//リレーションシップ関係にありEntityのポインタをセットする
	relationship->setEntity(instance->entitysIdMap[nodeParam->param]);
}

/**
* @fn void ErmErParser::setCardinality1(Relationship * relationship, NodeParams* nodeParam, ErmErParser* instance)
* @brief Entity1のカーディナリティ(多重度)情報をリレーションシップインスタンスにセットする
* @param relationship リレーションシップインスタンス
* @param nodeParam ノードパラメータ
* @param instance ErmErParserインスタンス
* @details Entity1のカーディナリティ(多重度)情報をリレーションシップインスタンスにセットする
*/
void ErmErParser::setCardinality1(Relationship * relationship, NodeParams* nodeParam, ErmErParser* instance)
{
	//カーディナリティ(多重度)によってセットするパラメータを取得する
	ErmErParser::CardinalityConverter::iterator it = instance->cardinalityConverter.find(nodeParam->param);
	if (it != instance->cardinalityConverter.end())
	{
		//指定されたカーディナリティ(多重度)に沿ったパラメータをセットする
		relationship->setCardinalityType1(it->second);
	}
}

/**
* @fn void ErmErParser::setCardinality2(Relationship * relationship, NodeParams* nodeParam, ErmErParser* instance)
* @brief Entity2のカーディナリティ(多重度)情報をリレーションシップインスタンスにセットする
* @param relationship リレーションシップインスタンス
* @param nodeParam ノードパラメータ
* @param instance ErmErParserインスタンス
* @details Entity2のカーディナリティ(多重度)情報をリレーションシップインスタンスにセットする
*/
void ErmErParser::setCardinality2(Relationship * relationship, NodeParams* nodeParam, ErmErParser* instance)
{
	//カーディナリティ(多重度)によってセットするパラメータを取得する
	ErmErParser::CardinalityConverter::iterator it = instance->cardinalityConverter.find(nodeParam->param);
	if (it != instance->cardinalityConverter.end())
	{
		//指定されたカーディナリティ(多重度)に沿ったパラメータをセットする
		relationship->setCardinalityType2(it->second);
	}
}

/**
* @fn void ErmErParser::recursiveSetFieldMap(std::string originalId, std::string id, map<std::string, std::list<std::string>> * referenceFielsMap)
* @brief 参照関係にあるColumnをコピー作成し、新たにフィールドマップに登録し直す
* @param originalId 参照元のフィールドID
* @param id 新しく作成するフィールドID
* @param referenceFielsMap 新しく作成するフィールドIDを参照しているフィールドIDリスト
* @details 参照関係にあるColumnをコピー作成し、新たにフィールドマップに登録し直す
*/
void ErmErParser::recursiveSetFieldMap(std::string originalId, std::string id, std::map<std::string, std::list<std::string>> * referenceFielsMap)
{
	if (this->fieldsMap.find(originalId) != this->fieldsMap.end())
	{
		//指定したoriginalIdがフィールドマップに登録されていれば、
		//Columnインスタンスをコピーする
		this->fieldsMap[id] = new fieldParam(*(this->fieldsMap[originalId]));
	}
	else
	{
		//指定したoriginalIdと適合するColumn情報が見つからなかった場合、
		//処理をスキップする
		return;
	}
	if ((*referenceFielsMap).find(id) != (*referenceFielsMap).end() && (*referenceFielsMap)[id].size() > 0)
	{
		//コピーを行ったColumnIDが複数のColumnの参照関係に有った場合、
		//登録された参照関係にあるColumnID分、recursiveSetFieldMap関数を呼び出し、Columnデータをコピーさせる
		for (auto it = (*referenceFielsMap)[id].begin(); it != (*referenceFielsMap)[id].end(); it++)
		{
			//recursiveSetFieldMap関数を実行する
			this->recursiveSetFieldMap(id, (*it), referenceFielsMap);
		}
		//参照関係にあるIDリストをクリアする
		(*referenceFielsMap)[id].clear();
	}
}

/**
* @fn void ErmErParser::setIndexColumns(Entity * entity, DOMNode* columnsNode, ErmErParser* instance)
* @brief インデックス情報をセットする関数
* @param entity Entityインスタンス
* @param columnsNode Domノードインスタンス
* @param instance ErmErParserインスタンス
* @details インデックス情報をセットする関数
*/
void ErmErParser::setIndexColumns(Entity * entity, DOMNode* columnsNode, ErmErParser* instance)
{
	//Columnリスト情報を持つDomノードリストを取得
	DOMNodeList* columns = columnsNode->getChildNodes();
	//Columnポインタ変数を定義
	fieldParam * field = NULL;
	//インデックスを格納する変数の定義
	std::list<std::wstring> indexs;
	//カラム数を取得
	int columnNum = columns->getLength();
	//カラムの数だけループ処理
	for (int k = 0; k < columnNum; k++)
	{
		//Column情報を持つDOMノードを取得
		DOMNode* columnNode = columns->item(k);
		//DOMノードから文字列情報を取得
		NodeParams* param = getNodeDataString(columnNode);
		if (param->name.compare("column") == 0)
		{
			//パラメータ名がcolumnだった場合、
			//Columnパラメータを取得する
			DOMNodeList* columnParams = columnNode->getChildNodes();
			//Columnパラメータの数を取得
			int columnParamNum = columnParams->getLength();
			//Columnパラメータの数だけループ処理
			for (int j = 0; j < columnParamNum; j++)
			{
				//Columnパラメータを持つDOMノードを取得
				DOMNode* columnParam = columnParams->item(j);
				//Columnパラメータを持つDOMノードから文字列情報へ変換する
				NodeParams* columnNodeParam = getNodeDataString(columnParam);
				if (columnNodeParam->name.compare("id") == 0)
				{
					//Columnパラメータ名がidだった場合、
					//フィールドマップからColumnインスタンスのポインタを取得
					field = instance->fieldsMap[columnNodeParam->param];
					//インデックスリストにColumn物理名をセットする
					indexs.push_back(field->name.physicalName);
				}
				//一時的に確保されたメモリを解放
				delete columnNodeParam;
			}
		}
		//一時的に確保されたメモリを解放
		delete param;
	}
	//Entityインスタンスにインデックス情報をセットする
	entity->addIndex(indexs);
}


/**
* @fn void ErmErParser::setParams()
* @brief パラメータセット関数
* @details パラメータセット関数
*/
void ErmErParser::setParams()
{
	//テーブル関係のセット関数群
	//テーブル物理名をセットする関数をセット
	setErmErFunc.insert(make_pair(ErmErParser::PHYSICAL_NAME, ErmErParser::setPhysicalTableName));
	//テーブル論理名をセットする関数をセット
	setErmErFunc.insert(make_pair(ErmErParser::LOGICAL_NAME, ErmErParser::setLogicalTableName));
	//Columnをセットする関数をセット
	setErmErFunc.insert(make_pair(ErmErParser::COLUMNS, ErmErParser::setColumns));
	//インデックスをセットする関数をセット
	setErmErFunc.insert(make_pair(ErmErParser::INDEXES, ErmErParser::setIndexes));

	//Column関係のセット関数群
	//Column物理名をセットする関数をセット
	setErmColumnFunc.insert(make_pair(ErmErParser::PHYSICAL_NAME, ErmErParser::setPhysicalColumnName));
	//Column論理名をセットする関数をセット
	setErmColumnFunc.insert(make_pair(ErmErParser::LOGICAL_NAME, ErmErParser::setLogicalColumnName));
	//Columnデータ型をセットする関数をセット
	setErmColumnFunc.insert(make_pair(ErmErParser::TYPE, ErmErParser::setColumnType));
	//Columnコメントをセットする関数をセット
	setErmColumnFunc.insert(make_pair(ErmErParser::COMMENT, ErmErParser::setColumnComment));

	//リレーションシップ関係のセット関数群
	//依存Columnセット関数をセット
	setErmRelationshipFunc.insert(make_pair(ErmErParser::ID, ErmErParser::setDependKeyList));
	//リレーションシップ親情報のセット関数をセット
	setErmRelationshipFunc.insert(make_pair(ErmErParser::RELATIONSHIP_SOURCE, ErmErParser::setEntityForRelationship));
	//リレーションシップ子情報のセット関数をセット
	setErmRelationshipFunc.insert(make_pair(ErmErParser::RELATIONSHIP_TARGET, ErmErParser::setEntityForRelationship));
	//カーディナリティ(多重度)をセットする関数をセット(親)
	setErmRelationshipFunc.insert(make_pair(ErmErParser::PARENT_CARDINALITY, ErmErParser::setCardinality1));
	//カーディナリティ(多重度)をセットする関数をセット(子)
	setErmRelationshipFunc.insert(make_pair(ErmErParser::CHILD_CARDINALITY, ErmErParser::setCardinality2));

	//カーディナリティ(多重度)の文字列とIDを紐付ける関数群
	//多関係のカーディナリティ(多重度)をセット
	cardinalityConverter.insert(make_pair(ErmErParser::CARDINALITY_MORE, Relationship::E_MORE));
	//0、多のカーディナリティ(多重度)をセット
	cardinalityConverter.insert(make_pair(ErmErParser::CARDINALITY_ZERO_OR_MORE, Relationship::E_ZERO_OR_MORE));
	//1のカーディナリティ(多重度)をセット
	cardinalityConverter.insert(make_pair(ErmErParser::CARDINALITY_ONE, Relationship::E_ONE));
	//0、1のカーディナリティ(多重度)をセット
	cardinalityConverter.insert(make_pair(ErmErParser::CARDINALITY_ZERO_ONE, Relationship::E_ZERO_ONE));

	//Columnパラメータをセットする関数群
	//ColumnIDをセットする関数
	setErmFieldFunc.insert(make_pair(ErmErParser::ID, ErmErParser::getFieldForFieldsMap));
	//参照関係にあるColumn情報を処理する関数のセット
	setErmFieldFunc.insert(make_pair(ErmErParser::RELATIONSHIP, ErmErParser::addRelationshipId));
	//Column物理名をセットする関数をセット
	setErmFieldFunc.insert(make_pair(ErmErParser::PHYSICAL_NAME, ErmErParser::setColomnParam));
	//Column論理名をセットする関数をセット
	setErmFieldFunc.insert(make_pair(ErmErParser::LOGICAL_NAME, ErmErParser::setColomnParam));
	//Columnデータ型をセットする関数をセット
	setErmFieldFunc.insert(make_pair(ErmErParser::TYPE, ErmErParser::setColomnParam));
	//Columnコメントをセットする関数をセット
	setErmFieldFunc.insert(make_pair(ErmErParser::COMMENT, ErmErParser::setColomnParam));
	//Not NULL制約をセットする関数をセット
	setErmFieldFunc.insert(make_pair(ErmErParser::NOT_NULL, ErmErParser::setNotNullFlg));
	//デフォルトパラメータをセットする関数をセット
	setErmFieldFunc.insert(make_pair(ErmErParser::DEFAULT_VALUE, ErmErParser::setDefaultParam));
	//AUTO INCREMENTをセットする関数をセット
	setErmFieldFunc.insert(make_pair(ErmErParser::AUTO_INCREMENT, ErmErParser::setAutoIncrement));
	//Primary Keyをセットする関数をセット
	setErmFieldFunc.insert(make_pair(ErmErParser::PRIMARY_KRY, ErmErParser::setPrimaryKey));

	//インデックス情報をセットする関数をセット
	setErmIndexFunc.insert(make_pair(ErmErParser::COLUMNS, ErmErParser::setIndexColumns));
}

/**
* @fn void ErmErParser::finalize()
* @brief 終了処理用関数
* @details 終了処理用関数
*/
void ErmErParser::finalize()
{
	//フィールドマップ情報を削除する
	for (auto it = this->fieldsMap.begin(); it != this->fieldsMap.end(); it++)
	{
		delete (*it).second;
	}
	this->fieldsMap.clear();
}