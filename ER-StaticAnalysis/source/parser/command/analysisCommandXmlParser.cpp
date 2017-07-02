/**
* @file       analysisCommandXmlParser.cpp
* @date       2016/01/31
* @author     Akihiro Sugeno
* @par        Revision
* $Id$
* @par        Copyright
* Copyright 2015 Akihiro Sugeno
* @par        History
*     - 2015/09/22 Akihiro Sugeno
*       -# Initial Version
*/
#include "analysisCommandXmlParser.h"
#include "../../common/commonXerces.h"

/**
* @fn class AnalysisProjects * AnalysisCommandXmlParser::parse(void)
* @brief 解析命令ファイルのパース関数
* @return 解析命令プロジェクトデータ
* @details 解析命令ファイルのパース処理を行う関数
*/
class AnalysisProjects * AnalysisCommandXmlParser::parse(void)
{
	// プロジェクトの初期化
	AnalysisProjects *analysisProjects = new AnalysisProjects();

	// Xerces-C++を初期化する
	try {
		XMLPlatformUtils::Initialize();		
	}
	catch (...) {
		logger::error() << logger::logPrefix("ERROR") << "Xerces-C++の初期化に失敗しました。" << endl;
		return NULL;
	}

	try {
		//Xerces DOMパーサーの取得
		XercesDOMParser* parser = new XercesDOMParser();
		//Xercesパース開始
		parser->parse(this->commandFilePath.c_str());

		//Documentオブジェクトの取得
		DOMDocument* dom = parser->getDocument();
		//全体のエレメントを取得
		DOMElement *root = dom->getDocumentElement();
		//Projectsの子要素、ProjectをDOMNodeリストとして取得
		DOMNodeList* domNodeList = getElementByCharTagName(root, PROJECT_TAG.c_str());
		//ノード数の取得
		int objectNum = (int)(domNodeList->getLength());
		//Projectの数だけループ
		for (int i = 0; i < objectNum; i++)
		{
			this->setProjects(analysisProjects, domNodeList->item(i));
		}

		//スペルチェック機能の設定情報読み込み
		this->spellCheckConfig(analysisProjects, root);

		//Xerces DOMパーサー終了処理;
		XMLPlatformUtils::Terminate();
	}
	catch (...) {
		logger::error() << logger::logPrefix("ERROR") << "コマンドファイル解析に失敗しました。" << endl;
	}

	return analysisProjects;
}

/**
* @fn void AnalysisCommandXmlParser::spellCheckConfig(AnalysisProjects *analysisProjects, DOMElement *root)
* @brief XML SpellCheckConfig要素の処理
* @param analysisProjects 解析命令情報を格納するプロジェクトインスタンス
* @param root DOMエレメント
* @details  SpellCheckConfigの設定を読み込み
*/
void AnalysisCommandXmlParser::spellCheckConfig(AnalysisProjects *analysisProjects, DOMElement *root)
{
	//Spell Checker設定
	DOMNodeList* domNodeList = getElementByCharTagName(root, "DicPath");
	int objectNum = (int)(domNodeList->getLength());
	if (objectNum != 1)
	{
		//辞書データの設定が無いためエラー
		throw "error";
	}

	//辞書ファイルデータを取得
	domNodeList->item(0);
	NodeParams* dicPathParam = getNodeDataString(domNodeList->item(0));
	analysisProjects->getSpellCheckConfig()->setDicPath(dicPathParam->param);

	domNodeList = getElementByCharTagName(root, "DicFileName");
	objectNum = (int)(domNodeList->getLength());

	//DicFileNameオブジェクトの数だけファイル名を取得
	for (int i = 0; i < objectNum; i++)
	{
		NodeParams* dicFileNameParam = getNodeDataString(domNodeList->item(i));
		analysisProjects->getSpellCheckConfig()->setDicFileName(dicFileNameParam->param);
	}

	//affixファイルデータを取得
	domNodeList = getElementByCharTagName(root, "AffFileName");
	objectNum = (int)(domNodeList->getLength());

	//affixファイルの数だけループ
	for (int i = 0; i < objectNum; i++)
	{
		NodeParams* dicFileNameParam = getNodeDataString(domNodeList->item(i));
		analysisProjects->getSpellCheckConfig()->setAffFileName(dicFileNameParam->param);
	}

	//スペルチェック除外対象単語設定の読み込み
	domNodeList = getElementByCharTagName(root, "SpellExclude");
	objectNum = (int)(domNodeList->getLength());
	DOMNodeList* excludeParams = domNodeList->item(0)->getChildNodes();
	objectNum = (int)(excludeParams->getLength());

	//スペルチェック除外対象単語の数だけループ処理
	for (int i = 0; i < objectNum; i++)
	{
		NodeParams* excludeParam = getNodeDataString(excludeParams->item(i));
		if (excludeParam->param.length() == 0)
		{
			continue;
		}
		analysisProjects->getSpellCheckConfig()->setExcludeValue(excludeParam->param);
	}
}

/**
* @fn void AnalysisCommandXmlParser::setComandParams(AnalysisCommand *analysisCommand, DOMNode *commandParam)
* @brief AnalysisCommandインスタンス用パラメータセット関数
* @param analysisCommand 解析命令インスタンス
* @param commandParam 解析命令情報を含んだDOMノード
* @details AnalysisCommandインスタンス用パラメータの設定を行う関数
*/
void AnalysisCommandXmlParser::setComandParams(AnalysisCommand *analysisCommand, DOMNode *commandParam)
{

	// パラメータ格納変数
	NodeParams *  nodeData = getNodeDataString(commandParam);

	//ノード名から、Class AnalysisCommandの格納用関数を取得
	SetAnalysisCommandFuc::iterator it = this->setAnalysisCommandFuc.find(nodeData->name);
	if (it != this->setAnalysisCommandFuc.end())
	{
		//AnalysisCommandインスタンスへパラメータセット
		it->second(analysisCommand, nodeData->param, nodeData->attr);
	}

	//一時的に確保したメモリ解放
	if (nodeData != NULL)free(nodeData);
}

/**
* @fn void AnalysisCommandXmlParser::setCommand(AnalysisProject * analysisProject, DOMNode *command)
* @brief XML Command要素の処理
* @param analysisProject 解析命令情報を格納するプロジェクトインスタンス
* @param command 解析命令DOMノード
* @details 解析命令情報を含んだDOMノードから命令数分analysisCommandのインスタンスを生成する
*/
void AnalysisCommandXmlParser::setCommand(AnalysisProject * analysisProject, DOMNode *command)
{
	//Command要素の取得
	DOMNodeList* commandParams = command->getChildNodes();
	int commandParamCount = (int)(commandParams->getLength());
	AnalysisCommand *analysisCommand = NULL;
	if (commandParamCount != 0)
	{
		//コマンドパラメータが0でなければ、コマンドインスタンスを生成

		//0は#textであるため、決め打ち
		NodeParams * commandType = getNodeDataString(commandParams->item(1));
		
		//必要なインスタンスを取得する
		analysisCommand = this->createAnalysisCommand(commandType->param);

		//コマンドIDの取得
		char *commandIdChar = getAttributeValueByName(commandParams->item(1), COMMAND_ID_ATTRIBUTE.c_str());
		if (commandIdChar != NULL)
		{
			int commandId = atoi(commandIdChar);
			//コマンドIDのセット
			analysisCommand->setId(commandId);
			//一時確保のメモリ領域を開放
			delete commandIdChar;
		}
		
		//プロジェクトに登録する
		analysisProject->addAnalisysCommand(analysisCommand);

		//一時的に確保したメモリ解放
		if (commandType != NULL)free(commandType);
	}

	//コマンドの子要素の分だけループ
	for (int l = 0; l < commandParamCount; l++)
	{
		this->setComandParams(analysisCommand, commandParams->item(l));
	}
}

/**
* @fn void AnalysisCommandXmlParser::setProject(AnalysisProjects *analysisProjects, DOMNode *item, int projectId)
* @brief Projectインスタンスの生成処理
* @param analysisProjects 解析命令プロジェクト情報を格納するオブジェクト
* @param item 解析命令プロジェクト情報
* @param projectId プロジェクトID
* @details 解析命令情報を含んだDOMノードからanalysisCommandインスタンスに命令情報を登録する
*/
void AnalysisCommandXmlParser::setProject(AnalysisProjects *analysisProjects, DOMNode *item, int projectId)
{
	AnalysisProject *analysisProject = NULL;

	// NULLの時エラー
	if (analysisProjects == NULL || item == NULL)
	{
		throw "error";
	}

	//XML Projectの子要素の名前を取得
	const XMLCh *nodeName_xml = item->getNodeName();
	char* nodeName = XMLString::transcode(nodeName_xml);

	if (strcmp(nodeName, PROJECT_NAME_TAG.c_str()) == 0)
	{
		//プロジェクト名を取得
		const XMLCh* projectName_xml = (item->getFirstChild())->getNodeValue();
		char* projectName = XMLString::transcode(projectName_xml);
		//ProjectインスタンスをProjectsインスタンスへ登録
		analysisProject = analysisProjects->addProject(projectId, std::string(projectName));
	}
	else if (strcmp(nodeName, PLAN_TAG.c_str()) == 0)
	{
		//最後に追加されたProjectインスタンスを取得
		analysisProject = analysisProjects->getLastAddProject();
		if (analysisProject == NULL)
		{
			//想定外エラー
			logger::error() << logger::logPrefix("ERROR") << "解析中にプロジェクトデータの生成・取得時に問題が発生しました。" << endl;
			throw std::logic_error("解析中にプロジェクトデータの生成・取得時に問題が発生しました。");
			return;
		}
		//XML Planの子要素の取得
		DOMNodeList* commands = item->getChildNodes();
		int commandCount = (int)(commands->getLength());
		//コマンドの分だけループ
		for (int k = 0; k < commandCount; k++)
		{
			this->setCommand(analysisProject, commands->item(k));
		}
	}

	//一時確保用のメモリの開放
	XMLString::release(&nodeName);
}

/**
* @fn void AnalysisCommandXmlParser::setProjects(AnalysisProjects *analysisProjects, DOMNode *project)
* @brief Projectsインスタンスの成形処理
* @param analysisProjects 解析命令インスタンス
* @param project 解析命令プロジェクト情報を含んだDOM情報
* @details Projectインスタンスを格納するオブジェクトの生成
*/
void AnalysisCommandXmlParser::setProjects(AnalysisProjects *analysisProjects, DOMNode *project)
{
	//XML Project子要素の取得
	DOMNodeList* projectChild = project->getChildNodes();
	int itemCount = (int)(projectChild->getLength());

	//XML Project Attribute(id)値の取得
	char *projectIdChar = getAttributeValueByName(project, PROJECT_ID_ATTRIBUTE.c_str());
	int projectId = projectIdChar ? atoi(projectIdChar) : 0;//ProjectのAttribute(id)値が取得できなかった場合は、ひとまず0を代入
	XMLString::release(&projectIdChar);
	//Projectの子要素の分だけループ
	for (int j = 0; j < itemCount; j++)
	{
		//プロジェクトの設定処理
		this->setProject(analysisProjects, projectChild->item(j), projectId);
	}
}



/**
* @fn AnalysisCommand * AnalysisCommandXmlParser::createAnalysisCommand(std::string commandType)
* @brief AnalysisCommandインスタンスの生成
* @param commandType 命令タイプ
* @return AnalysisCommandインスタンス
* @details 命令タイプ情報から、必要なAnalysisCommandインスタンスを生成しを呼び出し元に返す
*/
AnalysisCommand * AnalysisCommandXmlParser::createAnalysisCommand(std::string commandType)
{
	AnalysisCommand * analysisCommand;
	switch (CommandTypeLabelList[commandType])
	{
	case CommandTypeList::E_Comparison:
		analysisCommand = new AnalysisCommand();
		break;
	case CommandTypeList::E_TypeCheck:
		analysisCommand = new AnalysisCommandTypeCheck();
		break;
	default:
		analysisCommand = NULL;
			break;
	}
	return analysisCommand;
}
