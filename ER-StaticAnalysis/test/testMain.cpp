#ifdef _TEST
#include <limits.h>
#include<boost/program_options.hpp>
#include "gtest/gtest.h"

// gtest.lib をリンク
#pragma comment (lib, "gtest.lib")

#include "testCompRegularExpression.h"

namespace bpo = boost::program_options;
bpo::variables_map argmap;

int main(int argc, char **argv)
{
	//コマンドオプションの設定
	bpo::options_description desc("Allowed options");
	desc.add_options()
		("help,h", "コマンド説明")
		("erfile,e", bpo::value<std::string>(), "ER図ファイルを指定してください(対応フォーマットA5Mk2)")
		("cofile,c", bpo::value<std::string>(), "ER図解析設定ファイルを指定してください")
		("outputDir,o", bpo::value<std::string>(), "レポートファイルの出力先を指定してください");;

	//コマンドオプションのパース
	
	try {
		bpo::store(bpo::parse_command_line(argc, argv, desc), argmap);
		bpo::notify(argmap);
	}
	catch (std::exception& ex) {
		std::cerr << "コマンドライン引数の指定に誤りがあります: " << ex.what() <<
			std::endl << desc << std::endl;
	}

	// gtest の初期化
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();    // gtest の実行
}

#endif