/**
* @file       main.cpp
* @date       2016/01/31
* @author     Akihiro Sugeno
* @par        Revision
* $Id$
* @par        Copyright
* Copyright 2015 Akihiro Sugeno
* @par        History
*     - 2015/06/30 Akihiro Sugeno
*       -# Initial Version
*/

#ifndef _TEST

#include<boost/program_options.hpp>
#include <string>
#include "./main.h"
#include "./parser/ER/A5erParser.h"
#include "./parser/ER/erParserFactory.h"
#include "./parser/command/analysisCommandParserFactory.h"
#include "./reporter/reporter.h"


/**
* @fn main
* メイン処理
* @breaf mein処理
* @return OSへのリターンコード
* @detail ER図をパースして静的解析する処理を行うmain関数
*/
int main(int argc, char *argv[])
{
	ErParser *erParser;
	ER *er;

	// コマンドライン引数の表示(Debug用)
#if _DEBUG
	logger::debug() << logger::logPrefix("debug") << "コマンドライン引数" << endl;
	for (int i = 1; i < argc; i++) {
		logger::debug() << argv[i] << endl;
	}
	logger::debug() << endl;
#endif

	//コマンドオプションの設定
	namespace bpo = boost::program_options;
	bpo::options_description desc("Allowed options");
	desc.add_options()
		("help,h", "コマンド説明")
		("erfile,e", bpo::value<std::string>(), "ER図ファイルを指定してください(対応フォーマットA5Mk2)")
		("cofile,c", bpo::value<std::string>(), "ER図解析設定ファイルを指定してください")
		("outputDir,o", bpo::value<std::string>(), "レポートファイルの出力先を指定してください");

	//コマンドオプションのパース
	bpo::variables_map argmap;
	try {
		bpo::store(bpo::parse_command_line(argc, argv, desc), argmap);
		bpo::notify(argmap);
	}
	catch (std::exception& ex) {
		std::cerr << "コマンドライン引数の指定に誤りがあります: " << ex.what() <<
			std::endl << desc << std::endl;
	}

	//ヘルプ表示
	if (argmap.count("help")) {
		cout << desc << "\n";
		return 1;
	}

	//ER図ファイルの指定がない場合はエラー
	if (argmap.count("erfile") == 0) {
		cout << "ER図ファイルを指定してください" << endl;
		cout << desc << "\n";
		return errno;
	}

	//ER図ファイルの指定がない場合はエラー
	if (argmap.count("cofile") == 0) {
		cout << "ER図解析設定ファイルを指定してください" << endl;
		cout << desc << "\n";
		return errno;
	}

	//Parserクラスのインスタンスを取得
	erParser = ErParserFactory::create(argmap["erfile"].as<std::string>());

	AnalysisCommandParser *acParser = AnalysisCommandParserFactory::create(argmap["cofile"].as<std::string>());
	AnalysisProjects *analysisProjects =  acParser->parse();

	//ER図ファイルをパース
	er = erParser->parse();

	//解析開始
	analysisProjects->run(er);

	Reporter * reporter = Reporter::getInstance();
	//結果を画面に表示
	reporter->outputConsole();
	if (argmap.count("outputDir") != 0) {
		//出力先が指定されていれば、レポートファイルを出力する
		reporter->writeReport(er, argmap["outputDir"].as<std::string>());
	}
	reporter->finalize();

	delete erParser;
	delete er;
	delete acParser;
	delete analysisProjects;

	cout << "finish!" << endl;
	getchar();

	return 0;
}

#endif