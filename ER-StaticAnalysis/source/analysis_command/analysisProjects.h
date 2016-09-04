/**
* @file       analysisProjects.h
* @date       2016/01/31
* @author     Akihiro Sugeno
* @par        Revision
* $Id$
* @par        Copyright
* Copyright 2015 Akihiro Sugeno
* @par        History
*     - 2015/10/06 Akihiro Sugeno
*       -# Initial Version
*/

#ifndef RDS_ER_STATICANALYSIS_SOURCE_ANALYSIS_COMMAND_ANALYSISPROJECTS_H_
#define RDS_ER_STATICANALYSIS_SOURCE_ANALYSIS_COMMAND_ANALYSISPROJECTS_H_

#include <algorithm>
#include <string>
#include <list>
#include "analysisProject.h"
#include "../ER/er.h"
#include "../config/spellCheckConfigh.h"

/*! @class AnalysisProjects
@brief 解析命令に関する情報を全て格納するクラス
*/
class AnalysisProjects
{
private:
	//! 解析プロジェクトリスト
	std::list<AnalysisProject *> analysisProjects;

	SpellCheckConfig spellCheckConfig;
public:
	//! コンストラクタ
	AnalysisProjects();
	//! デストラクタ
	~AnalysisProjects();

	//! projectの追加関数
	AnalysisProject * addProject(int id, std::string name);

	//! 最後に登録したprojectを取得する関数
	AnalysisProject * getLastAddProject();

	//! 登録されたプロジェクトリスト情報を取得する関数
	std::list<AnalysisProject *> getProjects();

	//! スペルチェックインスタンスを取得する関数
	SpellCheckConfig * getSpellCheckConfig();

	//! 解析処理実行関数
	void run(ER *er);
};

#endif //RDS_ER_STATICANALYSIS_SOURCE_ANALYSIS_COMMAND_ANALYSISPROJECTS_H_