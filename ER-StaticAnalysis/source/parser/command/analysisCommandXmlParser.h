/**
* @file       analysisCommandXmlParser.h
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

#ifndef RDS_ER_STATICANALYSIS_SOURCE_PARSER_COMMAND_ANALYSISCOMMANDXMLPARSER_H_
#define RDS_ER_STATICANALYSIS_SOURCE_PARSER_COMMAND_ANALYSISCOMMANDXMLPARSER_H_

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMText.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>

#include <iostream>
#include <string>

#include "../../analysis_command/analysisCommand.h"
#include "./analysisCommandParser.h"
#include "../../log/logger.h"
#include "../../analysis_command/analysisProjects.h"
#include "../../common/commonXml.h"
using namespace xercesc;

/*! @class AnalysisCommandXmlParser
@brief 解析命令に関する情報を格納するクラス
*/
class AnalysisCommandXmlParser : public AnalysisCommandParser
{
private:
	//! コマンドファイル名
	string commandFilePath;
public:
	//! コンストラクタ
	AnalysisCommandXmlParser(std::string filePath)
	{
		commandFilePath = filePath;
	}

	/*!< 解析命令ファイルのパース関数 */
	class AnalysisProjects * parse(void);

	/*!< AnalysisCommandインスタンス用パラメータセット関数 */
	void setComandParams(AnalysisCommand * analysisCommand, DOMNode *commandParam);

	/*!< XML Command要素の処理 */
	void setCommand(AnalysisProject * analysisProject, DOMNode *commandParam);

	/*!< Projectインスタンスの生成処理 */
	void setProject(AnalysisProjects *analysisProjects, DOMNode *item, int projectId);

	/*!< Projectsインスタンスの成形処理 */
	void setProjects(AnalysisProjects *analysisProjects, DOMNode *project);

	//! AnalysisCommandインスタンスの生成
	AnalysisCommand * createAnalysisCommand(std::string commandType);

	//! XML SpellCheckConfig要素の処理
	void spellCheckConfig(AnalysisProjects *analysisProjects, DOMElement *root);
};

#endif