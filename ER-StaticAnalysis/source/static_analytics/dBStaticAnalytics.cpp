/**
* @file       dBStaticAnalytics.cpp
* @date       2016/01/31
* @author     Akihiro Sugeno
* @par        Revision
* $Id$
* @par        Copyright
* Copyright 2015 Akihiro Sugeno
* @par        History
*     - 2015/08/16 Akihiro Sugeno
*       -# Initial Version
*/

#include "../ER/er.h"
#include "../analysis_command/AnalysisProjects.h"
#include "dBStaticAnalytics.h"
#include "../static_analytics/compRegularExpression.h"
#include "columnDataTypeChecker.h"
#include "spellChecker.h"
#include "relationChecker.h"
#include "indexChecker.h"
#include "../log/logger.h"


using namespace std;

/**
* @fn void DBStaticAnalytics::run(AnalysisProjects *analysisProjects, ER *er)
* @brief 解析処理実行関数
* @param analysisProjects 解析命令情報格納しとりまとめているインスタンスの情報
* @param er ER情報格納インスタンスのポインタ変数
* @details 解析処理の実行を行う。登録されている命令の種類に応じて呼び出す処理を切り替えます。
*/
void DBStaticAnalytics::run(AnalysisProjects *analysisProjects, ER *er)
{
	//分析に使うクラスのインスタンスを生成
	CompRegularExpression compRegularExpression;
	ColumnDataTypeChecker columnDataTypeChecker;
	SpellChecker spellChecker;
	RelationChecker relationChecker;
	IndexChecker indexChecker;

	// Relation Check
	logger::info() << logger::logPrefix("INFO") << "Relation check start" << endl;
	relationChecker.run(analysisProjects, er);

	// Index Check
	logger::info() << logger::logPrefix("INFO") << "Index check start" << endl;
	indexChecker.run(analysisProjects, er);

	//Spell Check
	logger::info() << logger::logPrefix("INFO") << "Spell check start" << endl;
	spellChecker.run(analysisProjects->getSpellCheckConfig(), er);


	logger::info() << logger::logPrefix("INFO") << "name check & data type check start" << endl;

	//プロジェクトリストからプロジェクト情報を取得
	auto projects = analysisProjects->getProjects();
	//プロジェクトの数だけループ処理
	for (auto projectIt = projects.begin(); projectIt != projects.end(); ++projectIt)
	{
		//プロジェクトからコマンドリストを取得
		auto commands = (*projectIt)->getAnalysisCommand();
		//コマンド数だけループ処理
		for (auto commandIt = commands.begin(); commandIt != commands.end(); commandIt++)
		{
			//コマンドの有効状態を確認
			if (!(*commandIt)->getEnableFlg())
			{
				//コマンドが無効なら処理スキップ
				continue;
			}
			//コマンドの種類に応じて処理を切り替える
			switch ((*commandIt)->getCommandType())
			{
			case CommandTypeList::E_Comparison:
				//命名規則チェック
				compRegularExpression.run((*commandIt), er);
				break;
			case CommandTypeList::E_TypeCheck:
				//データ型チェック
				columnDataTypeChecker.run((AnalysisCommandTypeCheck *)(*commandIt), er);
				break;
			}
		}
	}
	
}