/**
* @file       analysisCommandParserFactory.h
* @date       2016/01/31
* @author     Akihiro Sugeno
* @par        Revision
* $Id$
* @par        Copyright
* Copyright 2015 Akihiro Sugeno
* @par        History
*     - 2015/09/21 Akihiro Sugeno
*       -# Initial Version
*/
#ifndef RDS_ER_STATICANALYSIS_SOURCE_PARSER_COMMAND_ANALYSISCOMMANDPARSERFACTORY_H_
#define RDS_ER_STATICANALYSIS_SOURCE_PARSER_COMMAND_ANALYSISCOMMANDPARSERFACTORY_H_

#include <boost/filesystem.hpp>
#include <map>
#include <string>
#include "analysisCommandParser.h"
#include "analysisCommandXmlParser.h"

/*! @class AnalysisCommandParserFactory
@brief 解析命令ファイルをパースするクラスを生成するファクトリークラス
*/
class AnalysisCommandParserFactory
{
	/*! パーサーの生成メソッドをハッシュで管理構造の定義 */
	typedef std::map<std::string, class AnalysisCommandParser *(*)(std::string filePath)> get_perser_method;
	/*! パーサーの生成メソッドをハッシュで管理する変数 */
	get_perser_method getParserMethod;

	/*! XMLで定義された解析命令ファイルをパースするクラスを返す関数 */
	static AnalysisCommandParser * getXMLFileParser(std::string filePath)
	{
		//XMLパーサークラスを返却
		return new class AnalysisCommandXmlParser(filePath);
	}

public:
	/*! CommandParserFactoryのコンストラクタ */
	AnalysisCommandParserFactory()
	{
		//ファイル拡張子に合わせてパーサーインスタンス生成関数をセットする
		getParserMethod.insert(make_pair(".xml", AnalysisCommandParserFactory::getXMLFileParser));
	}

	/*! AnalysisCommandParserインスタンス生成関数 */
	static class AnalysisCommandParser * create(std::string filePath)
	{
		AnalysisCommandParserFactory analysisCommandParserFactory;

		//拡張子ごとに必要なパーサークラスを生成する関数を探索する
		string extension = boost::filesystem::extension(filePath);
		get_perser_method::iterator it = analysisCommandParserFactory.getParserMethod.find(extension);
		return it->second(filePath);
	}
};

#endif //RDS_ER_STATICANALYSIS_SOURCE_PARSER_COMMAND_ANALYSISCOMMANDPARSERFACTORY_H_