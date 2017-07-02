/**
* @file       analysisProjects.cpp
* @date       2016/01/31
* @author     Akihiro Sugeno
* @par        Revision
* $Id$
* @par        Copyright
* Copyright 2015 Akihiro Sugeno
* @par        History
*     - 2015/10/07 Akihiro Sugeno
*       -# Initial Version
*/
#include <string>
#include <list>
#include "../analysis_command/analysisProjects.h"
#include "../static_analytics/dBStaticAnalytics.h"

/**
* @fn AnalysisProjects::AnalysisProjects()
* @brief AnalysisProjectsのコンストラクタ
* @details コンストラクタ
*/
AnalysisProjects::AnalysisProjects()
{
};

/**
* @fn AnalysisProjects::~AnalysisProjects()
* @brief AnalysisProjectsのデストラクタ
* @details デストラクタ
*/
AnalysisProjects::~AnalysisProjects()
{
	for_each(this->analysisProjects.begin(), this->analysisProjects.end(), [](AnalysisProject *p){delete p; });
};

/**
* @fn void AnalysisProject * AnalysisProjects::addProject(int id, std::string name)
* @brief projectの追加関数
* @param id プロジェクトID
* @param name プロジェクト名
* @return プロジェクトインスタンス
* @details projectを追加し、追加されたインスタンス情報を呼び出し元に返す関数
*/
AnalysisProject * AnalysisProjects::addProject(int id, std::string name)
{
	AnalysisProject *analysisProject = new AnalysisProject();
	analysisProject->setProjectId(id);
	analysisProject->setProjectName(name);
	this->analysisProjects.push_back(analysisProject);
	return analysisProject;
};

/**
* @fn AnalysisProject * AnalysisProjects::getLastAddProject()
* @brief 最後に登録したprojectを取得する関数
* @return プロジェクトインスタンス
* @details 最後に登録したprojectを取得する関数<br>
プロジェクトリストの末端からインスタンスを取得し呼び出し元に返します。
*/
AnalysisProject * AnalysisProjects::getLastAddProject()
{
	if (this->analysisProjects.empty())return NULL;
	return this->analysisProjects.back();
};

/**
* @fn std::list<AnalysisProject *> AnalysisProjects::getProjects()
* @brief 登録されたプロジェクトリスト情報を取得する関数
* @details 登録されたプロジェクトリストを呼び出し元に返します。
*/
std::list<AnalysisProject *> AnalysisProjects::getProjects()
{
	return this->analysisProjects;
};

/**
* @fn void AnalysisProjects::run(ER *er)
* @brief 解析処理実行関数
* @param er ER情報格納インスタンスのポインタ変数
* @details 解析処理の実行を行う。<br>DBStaticAnalyticsインスタンスを生成し、DBStaticAnalyticsのrun関数を実行します。
*/
void AnalysisProjects::run(class ER *er)
{
	DBStaticAnalytics dbStaticAnalytics;
	dbStaticAnalytics.run(this, er);
}

/**
* @fn void AnalysisProjects::getSpellCheckConfig()
* @brief スペルチェックインスタンスを取得する関数
* @details スペルチェックインスタンスを呼び出し元に返します。
*/
SpellCheckConfig * AnalysisProjects::getSpellCheckConfig()
{
	return &(this->spellCheckConfig);
}