/**
* @file       spellCheckConfigh.h
* @date       2016/01/31
* @author     Akihiro Sugeno
* @par        Revision
* $Id$
* @par        Copyright
* Copyright 2015 Akihiro Sugeno
* @par        History
*     - 2015/10/22 Akihiro Sugeno
*       -# Initial Version
*/

#ifndef RDS_ER_STATICANALYSIS_SOURCE_CONFIG_SPELLCHECKCONFIGH_H_
#define RDS_ER_STATICANALYSIS_SOURCE_CONFIG_SPELLCHECKCONFIGH_H_

#include <string>
#include <list>

class SpellCheckConfig
{
public:
	std::string dicPath;
	std::list<std::string> dicFileName;
	std::list<std::string> affFileName;
	std::list<std::string> excludeValue;

	void setDicPath(std::string _dicPath){ this->dicPath = _dicPath; };

	void setDicFileName(std::string _dicFileName){ this->dicFileName.push_back(this->dicPath + _dicFileName); };

	std::list<std::string> getDicFileNames(){ return this->dicFileName; };

	void setAffFileName(std::string _affFileName){ this->affFileName.push_back(this->dicPath + _affFileName); };

	std::list<std::string> getAffFileNames(){ return this->affFileName; };

	void setExcludeValue(std::string _excludeValue){ this->excludeValue.push_back(_excludeValue); };

	std::list<std::string> getExcludeValues(){ return this->excludeValue; };
};

#endif //RDS_ER_STATICANALYSIS_SOURCE_CONFIG_SPELLCHECKCONFIGH_H_