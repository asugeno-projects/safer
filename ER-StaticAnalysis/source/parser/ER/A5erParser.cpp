/**
* @file       A5erParser.cpp
* @date       2016/01/31
* @author     Akihiro Sugeno
* @par        Revision
* $Id$
* @par        Copyright
* Copyright 2015 Akihiro Sugeno
* @par        History
*     - 2015/07/08 Akihiro Sugeno
*       -# Initial Version
*/

#include "A5erParser.h"
#include "../../common/commonString.h"

/**
* @fn class ER * A5erParser::parse()
* @brief A5Mk2形式のER図ファイルのパース関数
* @return er ER情報格納インスタンス
* @details A5Mk2形式のER図ファイルの構文解析を行う処理
*/
class ER * A5erParser::parse()
{
	//この定義が無いとxlsxファイルの日本語が文字化けします。
	std::locale::global(std::locale(""));
	std::ios_base::sync_with_stdio(false);

	ER *er = new ER();
	logger::info() << logger::logPrefix("INFO") << "File Open > " << this->erFilePath << std::endl;

	FILE* filePt = fopen(this->erFilePath.c_str(), "r, ccs=UNICODE");
	std::wifstream file(filePt);
	try{
		//ファイルオープン
		if (file.good())
		{
			//BOMの除去
			this->removeBom(&file);

			//parse rest streamの取得
			boost::program_options::wparsed_options parsedOptions = boost::program_options::parse_config_file(
				file,
				boost::program_options::options_description(),
				true);

			logger::info() << logger::logPrefix("INFO") << "parse start" << std::endl;

			//行単位で回して、セクションごとにデータを格納する
			//ER図情報を確認したところRelationshipがEntityより先に記述されていることと、
			//Entityオブジェクトマップを作成してから、Relationshipに格納するEntityデータはポインタで紐付けたかったためEntity→Relationshipの順で処理している
			for (const auto& option : parsedOptions.options) {

				//Debugメッセージ Section表示
				logger::debug() << logger::logPrefix("DEBUG") << option.string_key << ";";

				//Sectionを取得するための正規表現の設定
				boost::smatch m;
				boost::regex  rEntity(A5erParser::ENTITY_REGEX);

				//キーがEntityを含んでいた場合
				if (boost::regex_search(option.string_key, m, rEntity))
				{
					//Entity関連の処理
					this->setEntity(er, option);
				}

				logger::debug() << std::endl;

			}

			//Relationshipの解析
			//行単位で回して、セクションごとにデータを格納する
			for (const auto& option : parsedOptions.options) {

				//Debugメッセージ Section表示
				logger::debug() << logger::logPrefix("DEBUG") << option.string_key << ";";

				//Sectionを取得するための正規表現の設定
				boost::smatch m;
				boost::regex  rRelationship(A5erParser::RELATIONSHIP_REGEX);

				if (boost::regex_search(option.string_key, m, rRelationship))
				{
					//Relationship関連の処理
					this->setRelationship(er, option);
				}

				logger::debug() << std::endl;

			}
			logger::info() << logger::logPrefix("INFO") << "parse end" << std::endl;
			file.close();
			logger::info() << logger::logPrefix("INFO") << "file close" << std::endl;
		}
		else
		{
			logger::error() << logger::logPrefix("ERROR") << "file not found > " << this->erFilePath << std::endl;
		}
	}
	catch (std::ifstream::failure e){
		//IO系でエラーが出たらメッセージを残しておく
		std::cout << "IO error##" << e.what() << std::endl;
	}
	catch (std::exception e) {
		std::cout << "error## 想定外エラー(std::exception)" << e.what() << std::endl;
	}
	catch (...){
		std::cout << "error## 想定外エラー" << std::endl;
	}
	return er;
}

/**
* @fn void A5erParser::removeBom(ifstream *file)
* @brief BOM情報の除去
* @param file ifstreamインスタンス
* @details fileに格納されたBOMを削除する
*/
void A5erParser::removeBom(std::wifstream *file)
{
	//skip BOM
	wchar_t buffer[8];
	buffer[0] = 255;
	while (file->good() && buffer[0] > 127)
		file->read((wchar_t *)buffer, 1);

	std::fpos_t pos = file->tellg();
	if (pos > 0)
		file->seekg(pos - 1);
}

/**
* @fn void A5erParser::setEntity(class ER * er, const boost::program_options::option & option)
* @brief Entity情報のセット
* @param er ER情報格納インスタンス
* @param option boostオプションインスタンス
* @details ER図情報格納インスタンスにEntity情報を格納する処理を行う
*/
void A5erParser::setEntity(class ER * er, const boost::program_options::woption & option)
{
	std::list<class Entity *>::reverse_iterator entityRIt;

	if (boost::iequals(option.string_key, A5erParser::PHYSICAL_TABLE_NAME_TAG)) {
		//物理テーブル名が来た時に新しいEntityインスタンスを生成し、Entity配列に登録する
		Entity *entity = new Entity();
		er->entitys.push_back(entity);

		//EntityMapに登録、MapのFirstキーの登録は決め打ち
		er->entityMap.insert(make_pair(option.value[0], entity));
	}

	//Entity配列の末端のオブジェクトを使用する。かなりの決め打ちであるが、いい感じに効率よくデータをオブジェクト化する方法も思いつかないので、コレで。
	entityRIt = er->entitys.rbegin();


	// Option value is a vector of std::strings.
	for (const auto& value : option.value)
	{
		//セクション名を用いて処理すべき関数情報を持ってくる
		funcs_type::iterator it = this->setSectionParamFunc.find(option.string_key);
		if (it != this->setSectionParamFunc.end())
		{
			//要素を登録する。
			it->second((*entityRIt), value);
			logger::debug() << " " << value;
		}
		else
		{
			//要素に対して処理がない場合は、こちらに流れます。
			//静的解析では、全ての要素を見る必要が無いので、かなりの数がこっちに流れることが予想されます。
			logger::debug() << option.string_key << " not found func" << std::endl;
		}
		//logger::debug() << ' ' << value;
	}
}

/**
* @fn void A5erParser::setPhysicalTableNameForEntity(Entity *entity, std::string name)
* @brief Entityインスタンスに物理テーブル名情報を格納する処理
* @param entity Entityインスタンス
* @param name 物理テーブル名
* @details Entityインスタンスに物理テーブル名情報を格納する処理を行う
*/
void A5erParser::setPhysicalTableNameForEntity(Entity *entity, std::wstring name)
{
	entity->setPhysicalTableName(name);
}

/**
* @fn void A5erParser::setLogicalTableNameForEntity(Entity *entity, std::string name)
* @brief Entityインスタンスに論理テーブル名情報を格納する処理
* @param entity Entityインスタンス
* @param name 論理テーブル名
* @details Entityインスタンスに論理テーブル名情報を格納する処理を行う
*/
void A5erParser::setLogicalTableNameForEntity(Entity *entity, std::wstring name)
{
	entity->setLogicalTableName(name);
}

/**
* @fn void A5erParser::setTableCommentForEntity(Entity *entity, std::string comment)
* @brief Entityインスタンスにテーブルコメント情報を格納する処理
* @param entity Entityインスタンス
* @param comment テーブルコメント
* @details Entityインスタンスにテーブルコメント情報を格納する処理を行う
*/
void A5erParser::setTableCommentForEntity(Entity *entity, std::wstring comment)
{
	entity->setTableComment(comment);
}

/**
* @fn void A5erParser::setFieldForEntity(Entity *entity, std::string name)
* @brief EntityインスタンスにColumn情報を格納する処理
* @param entity Entityインスタンス
* @param fields Column情報
* @details EntityインスタンスにColumn情報を格納する処理を行う
*/
void A5erParser::setFieldForEntity(Entity *entity, std::wstring fields)
{
	std::vector<std::wstring> v;
	// /
	for (size_t c = fields.find_first_of(L"\\"); c != std::wstring::npos; c = c = fields.find_first_of(L"\\")){
		fields.erase(c, 1);
	}
	//"の除去
	for (size_t c = fields.find_first_of(L"\""); c != std::wstring::npos; c = c = fields.find_first_of(L"\"")){
		fields.erase(c, 1);
	}
	boost::algorithm::split(v, fields, std::bind2nd(std::equal_to<char>(), ','));
	int a = 0;
	fieldParam field;
	field.name.logicalName = v[0];//カラム論理名
	field.name.physicalName = v[1];//カラム物理名
	field.name.comment = v[6];//カラムコメント
	field.type = v[2];//カラムの型
	field.notNullFlg = boost::iequals(L"Not Null", v[3]);//Not Null制約
	if (v[4].size() >= 1)
	{
		//Primary Key関連の設定処理
		int pkNum = boost::lexical_cast<int>(v[4]);
		entity->insertPk(pkNum, field.name.physicalName);
	}
	//DDLオプション
	std::list<std::wstring> ddlOptions;
	boost::algorithm::split(ddlOptions, v[8], std::bind2nd(std::equal_to<char>(), ' '));
	for (auto ddlOption = ddlOptions.begin(); ddlOption != ddlOptions.end(); ddlOption++)
	{
		field.DDLOptions.push_back((*ddlOption));
	}

	field.defaultParam = v[5];//カラムのデフォルト値設定
	entity->setfields(field);//カラム情報をセット
}

/**
* @fn void A5erParser::setTagForEntity(Entity *entity, std::wstring comment)
* @brief EntityインスタンスにTAG情報を格納する処理
* @param entity Entityインスタンス
* @param tag TAG情報
* @details EntityインスタンスにTAG情報を格納する処理を行う
*/
void A5erParser::setTagForEntity(Entity *entity, std::wstring tag)
{
	entity->setTagName(tag);
}

/**
* @fn void A5erParser::setIndexForEntity(Entity *entity, std::wstring index)
* @brief EntityインスタンスにIndex情報を格納する処理
* @param entity Entityインスタンス
* @param index Index情報
* @details EntityインスタンスにIndex情報を格納する処理を行う
*/
void A5erParser::setIndexForEntity(Entity *entity, std::wstring index)
{
	//Index格納用リスト
	std::list<std::wstring> indexs;
	//カンマ区切りにIndex情報が登録されているのため、「,」でスプリット
	boost::algorithm::split(indexs, index, std::bind2nd(std::equal_to<char>(), ','));
	//先頭はIndex名の情報なので除去
	indexs.pop_front();
	//Indexを登録
	entity->addIndex(indexs);
}

/**
* @fn void A5erParser::setParamFuc()
* @brief パース処理の初期化関数
* @details パース処理に使用する各インスタンスに情報をセット関数をmap化を行う
*/
void A5erParser::setParamFuc()
{
	//テーブル物理名セット関数
	this->setSectionParamFunc.insert(make_pair(A5erParser::PHYSICAL_TABLE_NAME_TAG, A5erParser::setPhysicalTableNameForEntity));
	//テーブル論理名セット関数
	this->setSectionParamFunc.insert(make_pair(A5erParser::LOGICAL_TABLE_NAME_TAG, A5erParser::setLogicalTableNameForEntity));
	//テーブルコメントセット関数
	this->setSectionParamFunc.insert(make_pair(A5erParser::LOGICAL_TABLE_COMMENT, A5erParser::setTableCommentForEntity));
	//エンティティのフィールド情報をセットする関数
	this->setSectionParamFunc.insert(make_pair(A5erParser::LOGICAL_FIELD, A5erParser::setFieldForEntity));
	//エンティティのTAG情報をセットする関数
	this->setSectionParamFunc.insert(make_pair(A5erParser::TAG, A5erParser::setTagForEntity));
	//エンティティのIndex情報をセットする関数
	this->setSectionParamFunc.insert(make_pair(A5erParser::INDEX, A5erParser::setIndexForEntity));

	/* リレーションシップ情報 */
	//リレーションシップ情報に記録されているエンティティ情報を処理する関数
	this->setRelationshipFunc.insert(make_pair(A5erParser::RELATIONSHIP_ENTITY1_TAG, A5erParser::setRelationshipEntity));
	this->setRelationshipFunc.insert(make_pair(A5erParser::RELATIONSHIP_ENTITY2_TAG, A5erParser::setRelationshipEntity));
	//リレーションシップ情報に記載されている依存カラムを処理する関数
	this->setRelationshipFunc.insert(make_pair(A5erParser::RELATIONSHIP_FIELDS1_TAG, A5erParser::setEntity1DependKey));
	this->setRelationshipFunc.insert(make_pair(A5erParser::RELATIONSHIP_FIELDS2_TAG, A5erParser::setEntity2DependKey));
	//リレーションシップ情報に記載されているカーディナリティ(多重度)を処理する関数
	this->setRelationshipFunc.insert(make_pair(A5erParser::CARDINALITY_TYPE1_TAG, A5erParser::setCardinalityType1));
	this->setRelationshipFunc.insert(make_pair(A5erParser::CARDINALITY_TYPE2_TAG, A5erParser::setCardinalityType2));
	//依存フラグを処理する関数
	this->setRelationshipFunc.insert(make_pair(A5erParser::RELATIONSHIP_DEPENDENCE_TAG, A5erParser::setDependenceFlg));
}

/**
* @fn void A5erParser::setRelationship(ER * er, const boost::program_options::option & option)
* @brief Relationship情報のセット
* @param er ER情報格納インスタンス
* @param option boostオプションインスタンス
* @details ER図情報格納インスタンスにRelationship情報を格納する処理を行う
*/
void A5erParser::setRelationship(ER * er, const boost::program_options::woption & option)
{
	std::list<Relationship *>::reverse_iterator relationshipRIT;

	if (boost::iequals(option.string_key, A5erParser::RELATIONSHIP_ENTITY1_TAG)) {
		//物理テーブル名が来た時に新しいEntityインスタンスを生成し、Entity配列に登録する
		Relationship *relationship = new Relationship();
		er->relationships.push_back(relationship);
	}

	//Entity配列の末端のオブジェクトを使用する。かなりの決め打ちであるが、いい感じに効率よくデータをオブジェクト化する方法も思いつかないので、コレで。
	relationshipRIT = er->relationships.rbegin();


	// Option value is a vector of std::strings.
	for (const auto& value : option.value)
	{
		//セクション名を用いて処理すべき関数情報を持ってくる
		relationship_funcs::iterator it = this->setRelationshipFunc.find(option.string_key);
		if (it != this->setRelationshipFunc.end())
		{

			//要素を登録する。
			it->second((*relationshipRIT), er, value);
			logger::debug() << " " << value;
		}
		else
		{
			//要素に対して処理がない場合は、こちらに流れます。
			//静的解析では、全ての要素を見る必要が無いので、かなりの数がこっちに流れることが予想されます。
			logger::debug() << option.string_key << " not found func" << std::endl;
		}
		//logger::debug() << ' ' << value;
	}
}

/**
* @fn void A5erParser::setRelationshipEntity(Relationship *relationship, ER *er, std::string name)
* @brief RelationshipにEntity情報を紐付ける処理
* @param relationship Relationshipインスタンス
* @param er ER情報インスタンス
* @param name Entity名
* @details RelationshipにEntity情報を紐付ける処理を行う
*/
void A5erParser::setRelationshipEntity(Relationship *relationship, ER *er, std::wstring name)
{
	//該当するEntityオブジェクトのポインタを取得する
	ER::entity_map::iterator entityData = er->entityMap.find(name);
	relationship->setEntity(entityData->second);
}

/**
* @fn void A5erParser::setEntity1DependKey(Relationship *relationship, ER *er, std::string key)
* @brief Relationshipに関係依存Column名を登録する
* @param relationship Relationshipインスタンス
* @param er ER情報インスタンス
* @param key 関係依存Key名(Column名)
* @details Relationshipに関係依存Column名を登録する<br>Entity1のColumn情報を格納しています。
*/
void A5erParser::setEntity1DependKey(Relationship *relationship, ER *er, std::wstring key)
{
	relationship->setEntity1DependKey(key);
}

/**
* @fn void A5erParser::setEntity2DependKey(Relationship *relationship, ER *er, std::string key)
* @brief Relationshipに関係依存Column名を登録する
* @param relationship Relationshipインスタンス
* @param er ER情報インスタンス
* @param key 関係依存Key名(Column名)
* @details Relationshipに関係依存Column名を登録する<br>Entity2のColumn情報を格納しています。
*/
void A5erParser::setEntity2DependKey(Relationship *relationship, ER *er, std::wstring key)
{
	relationship->setEntity2DependKey(key);
}

/**
* @fn void A5erParser::setCardinalityType1(Relationship *relationship, ER *er, std::string type)
* @brief Relationshipに多重度(カーディナリティ)を登録する
* @param relationship Relationshipインスタンス
* @param er ER情報インスタンス
* @param type 多重度(カーディナリティ)
* @details Relationshipに多重度(カーディナリティ)を登録する<br>Entity1側の多重度(カーディナリティ)を格納しています。
*/
void A5erParser::setCardinalityType1(Relationship *relationship, ER *er, std::wstring type)
{
	relationship->setCardinalityType1(boost::lexical_cast<int>(type));
}

/**
* @fn void A5erParser::setCardinalityType2(Relationship *relationship, ER *er, std::string type)
* @brief Relationshipに多重度(カーディナリティ)を登録する
* @param relationship Relationshipインスタンス
* @param er ER情報インスタンス
* @param type 多重度(カーディナリティ)
* @details Relationshipに多重度(カーディナリティ)を登録する<br>Entity2側の多重度(カーディナリティ)を格納しています。
*/
void A5erParser::setCardinalityType2(Relationship *relationship, ER *er, std::wstring type)
{
	relationship->setCardinalityType2(boost::lexical_cast<int>(type));
}

/**
* @fn void A5erParser::setDependenceFlg(Relationship *relationship, ER *er, std::string type)
* @brief Relationshipに依存フラグを登録する
* @param relationship Relationshipインスタンス
* @param er ER情報インスタンス
* @param type 依存フラグ
* @details Relationshipに依存フラグを登録する<br>
依存フラグとは、対象のEntityは独立Entityか依存Entityかを判別するものです。<br>
依存が有効であれば、多重度(カーディナリティ)が1または0,1となるEntityが親になります。
*/
void A5erParser::setDependenceFlg(Relationship *relationship, ER *er, std::wstring type)
{
	relationship->setDependenceFlg(boost::lexical_cast<bool>(type));
}