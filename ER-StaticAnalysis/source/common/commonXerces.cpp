/**
* @file       commonXerces.cpp
* @date       2016/01/31
* @author     Akihiro Sugeno
* @par        Revision
* $Id$
* @par        Copyright
* Copyright 2015 Akihiro Sugeno
* @par        History
*     - 2015/10/29 Akihiro Sugeno
*       -# Initial Version
*/

#include "commonXerces.h"

#include <map>
#include <string>

/**
* @fn DOMNodeList* getElementByCharTagName(DOMElement* elem, const char *tagName)
* @brief タグネームを用いたエレメント取得関数
* @param elem DOMオブジェクト
* @param tagName タグ名
* @return Domノードリスト
* @details XMLオブジェクトから、入力したタグネームに該当するオブジェクトを取得する関数
*/
DOMNodeList* getElementByCharTagName(DOMElement* elem, const char *tagName)
{
	XMLCh* attributeName = XMLString::transcode(tagName);
	DOMNodeList* nlist = elem->getElementsByTagName(attributeName);
	XMLString::release(&attributeName);

	return nlist;
}

/**
* @fn char* getAttributeValueByName(DOMNode * elem, const char *name)
* @brief DOMノードから指定したattribute属性値取得する関数
* @param elem DOMノード
* @param name attribute名
* @return attribute属性値
* @details DOMノードから指定したattribute属性値を取得する関数
*/
char* getAttributeValueByName(DOMNode * elem, const char *name)
{
	XMLCh* attributeName = XMLString::transcode(name);
	DOMNamedNodeMap* nodeMap = elem->getAttributes();
	if (!nodeMap)
	{
		//Attribute値が無いと思われる場合の処理
		return NULL;
	}

	DOMAttr* attr = static_cast<DOMAttr*>(nodeMap->getNamedItem(attributeName));
	char* attrValue = NULL;
	if (attr != NULL)
	{
		attrValue = XMLString::transcode(attr->getValue());
	}
	XMLString::release(&attributeName);
	return attrValue;
}

/**
* @fn AttributeMap getAttributeValueByName(DOMNode * elem)
* @brief DOMノードに付属するattribute属性に関する情報を全て取得する関数
* @param elem DOMノード
* @return 全てのattributeデータ
* @details DOMノードに付属するattribute属性に関する情報を全て取得する関数
*/
AttributeMap getAttributeValueByName(DOMNode * elem)
{
	AttributeMap attrs;
	DOMNamedNodeMap* nodeMap = elem->getAttributes();
	if (!nodeMap)
	{
		//Attribute値が無いと思われる場合の処理
		return attrs;
	}

	XMLSize_t len = nodeMap->getLength();
	for (int i = 0; i < static_cast<int>(len); i++)
	{
		DOMAttr* attr = static_cast<DOMAttr*>(nodeMap->item(i));
		char* attrValue = NULL;
		char* attrName = NULL;
		if (attr != NULL)
		{
			attrValue = XMLString::transcode(attr->getValue());
			attrName = XMLString::transcode(attr->getName());
			attrs.insert(std::make_pair(attrName, attrValue));
			delete attrName;
			delete attrValue;
		}
	}

	return attrs;
}

/**
* @fn NodeParams * AnalysisCommandXmlParser::getNodeDataString(DOMNode *_domNode)
* @brief DomNodeのvalueを取得
* @param _domNode 解析命令インスタンス
* @return ノードパラメータ
* @details DomNodeからvalueを含んだノードパラメータを呼び出し元に返す
*/
NodeParams * getNodeDataString(DOMNode *_domNode)
{
	// ノードネーム取得
	const XMLCh *Param_xml = _domNode->getNodeName();

	// ノードネーム取得(char型へ変更)
	char *param = XMLString::transcode(Param_xml);

	if (param == NULL)return NULL;//パラメータがなければ何も無いので、処理中断

	//変数宣言
	NodeParams * nodeParams = new NodeParams();

	//NodeNameを代入
	nodeParams->name = param;
	//一時確保用メモリ解放
	XMLString::release(&param);

	DOMNode *valueNode = _domNode->getFirstChild();

	if (valueNode){
		// ノードのvalueの取得
		const XMLCh *value_xml = valueNode->getNodeValue();
		char *tmp = XMLString::transcode(value_xml);
		if (tmp != NULL)
		{
			//valueがあれば、ノードvalueとしてデータを格納する
			nodeParams->param = tmp;
		}
		//一時確保用メモリの開放
		XMLString::release(&tmp);
	}

	//属性値をセット
	AttributeMap nodeAttr = getAttributeValueByName(_domNode);
	nodeParams->attr = nodeAttr;

	return nodeParams;
}