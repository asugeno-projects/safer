#include "../source/static_analytics/compRegularExpression.h"
#include "gtest/gtest.h"
#include<boost/program_options.hpp>

#include "../source/parser/ER/a5erParser.h"
#include "../source/parser/ER/erParserFactory.h"
#include "../source/parser/command/analysisCommandParserFactory.h"
#include "../source/reporter/reporter.h"

namespace bpo = boost::program_options;
extern bpo::variables_map argmap;

TEST(CompRegularExpression, DefaultConstructor) {
	CompRegularExpression compRegularExpression;

	//Parserクラスのインスタンスを取得
	ErParser *erParser = ErParserFactory::create(argmap["erfile"].as<std::string>());
	//ER図ファイルをパース
	ER *er = erParser->parse();

	ExtractionParam extractionParam;
	extractionParam.section = ".*";
	extractionParam.targetName = ".*";

	AnalysisCommand analysisCommand;
	analysisCommand.setCommandName("test");
	analysisCommand.setCommandType(CommandTypeList::E_Comparison);
	analysisCommand.setEnableFlg(0);
	analysisCommand.addTargetExtractionParam(extractionParam);
	analysisCommand.addTarget(TargetList::E_PhysicalTableName);
	analysisCommand.addValue(".*_tbl");
	analysisCommand.addValue(".*_mst");

	compRegularExpression.run(&analysisCommand, er);

	Reporter *reporter = Reporter::getInstance();
	auto rowDatas = reporter->getRowData();

	EXPECT_NE(0, rowDatas.size());
}
