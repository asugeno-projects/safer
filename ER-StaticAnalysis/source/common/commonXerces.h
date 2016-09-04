/**
* @file       commonXerces.h
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

#ifndef RDS_ER_STATICANALYSIS_SOURCE_COMMON_COMMONXERCES_H_
#define RDS_ER_STATICANALYSIS_SOURCE_COMMON_COMMONXERCES_H_

#include <string>
#include "../common/commonXml.h"
#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMText.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>

using namespace xercesc;

//! ノードパラメータ
struct NodeParams{
	std::string name;
	std::string param;
	AttributeMap attr;
};

//! タグネームを用いたエレメント取得関数
DOMNodeList* getElementByCharTagName(DOMElement* elem, const char *tagName);

//! DOMノードから指定したattribute属性値取得する関数
char* getAttributeValueByName(DOMNode * elem, const char *name);

//! DOMノードに付属するattribute属性に関する情報を全て取得する関数
AttributeMap getAttributeValueByName(DOMNode * elem);

//! DomNodeのvalueを取得
NodeParams * getNodeDataString(DOMNode *_domNode);

#endif //RDS_ER_STATICANALYSIS_SOURCE_COMMON_COMMONXERCES_H_