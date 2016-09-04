/**
* @file       analysisProject.h
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

#ifndef RDS_ER_STATICANALYSIS_SOURCE_ANALYSIS_COMMAND_ANALYSISPROJECT_H_
#define RDS_ER_STATICANALYSIS_SOURCE_ANALYSIS_COMMAND_ANALYSISPROJECT_H_

#include <algorithm>
#include <string>
#include <list>
#include "../analysis_command/analysisCommand.h"

/*! @class AnalysisProject
@brief 解析命令に関する情報をひとまとめに格納するクラス
*/
class AnalysisProject
{
private:
	//! プロジェクトID
	int projectId;

	//! プロジェクト名
	string projectName;

	//! 分析コマンドリスト
	std::list<AnalysisCommand *> analysisCommands;

public:
	//! コンストラクタ
	AnalysisProject(){};
	//! デストラクタ
	~AnalysisProject()
	{
		this->analysisCommands.clear();
	};
	//! 解析処理実行関数
	void run();
	//! プロジェクトIDセット関数
	void setProjectId(int id){ this->projectId = id; };
	//! プロジェクトIDゲット関数
	int getProjectId(){ return this->projectId; };
	//! プロジェクト名セット関数
	void setProjectName(std::string name){ this->projectName = name; };
	//! プロジェクト名ゲット関数
	string getProjectName(){ return this->projectName; };
	//! 解析命令の追加関数
	void addAnalisysCommand(AnalysisCommand *analysisCommand){ this->analysisCommands.push_back(analysisCommand); };
	//! 解析命令リストの取得関数
	std::list<AnalysisCommand *> getAnalysisCommand(){ return this->analysisCommands; };
};

#endif //RDS_ER_STATICANALYSIS_SOURCE_ANALYSIS_COMMAND_ANALYSISPROJECT_H_