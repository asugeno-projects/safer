/**
* @file       erParserFactory.h
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

#ifndef RDS_ER_STATICANALYSIS_SOURCE_PARSER_ER_ERPARSERFACTORY_H_
#define RDS_ER_STATICANALYSIS_SOURCE_PARSER_ER_ERPARSERFACTORY_H_

#include <map>
#include <string>

#include "erParser.h"
#include "A5erParser.h"
#include "ermErParser.h"

/*! @class ErParserFactory
@brief ER図ファイルをパースするクラスを生成するファクトリークラス
*/
class ErParserFactory
{
	/*! パーサーの生成メソッドをハッシュで管理構造の定義 */
	typedef std::map<std::string, class ErParser *(*)(std::string filePath)> get_perser_method;
	/*! パーサーの生成メソッドをハッシュで管理する変数 */
	get_perser_method getParserMethod;

	/*! A5 Mk2用のER図ファイルフォーマットをパースするクラスを返す関数 */
	static ErParser * getA5erFileParser(std::string filePath)
	{
		//A5erパーサークラスを返却
		return new A5erParser(filePath);
	}

	static ErParser * getErmFileParser(std::string filePath)
	{
		//ErmErパーサークラスを返却
		return new ErmErParser(filePath);
	}

public:
	/*! ErParserFactoryのコンストラクタ */
	ErParserFactory()
	{
		//ファイル拡張子に合わせてパーサーインスタンス生成関数をセットする
		getParserMethod.insert(make_pair(".a5er", ErParserFactory::getA5erFileParser));
		getParserMethod.insert(make_pair(".erm", ErParserFactory::getErmFileParser));
	}

	/*! パーサーインスタンス生成関数 */
	static class ErParser * create(std::string filePath)
	{
		ErParserFactory erParserFactory;

		//拡張子ごとに必要なパーサークラスを生成する関数を探索する
		std::string extension = boost::filesystem::extension(filePath);
		get_perser_method::iterator it = erParserFactory.getParserMethod.find(extension);
		return it->second(filePath);
	}
};

#endif //RDS_ER_STATICANALYSIS_SOURCE_PARSER_ER_ERPARSERFACTORY_H_