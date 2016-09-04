/**
* @file       er.cpp
* @date       2016/01/31
* @author     Akihiro Sugeno
* @par        Revision
* $Id$
* @par        Copyright
* Copyright 2015 Akihiro Sugeno
* @par        History
*     - 2016/01/31 Akihiro Sugeno
*       -# Initial Version
*/

#include <algorithm>
#include "./er.h"

ER::~ER()
{
	std::for_each(this->entitys.begin(), this->entitys.end(), [](class Entity *p) { delete p; });
	this->entitys.clear();

	std::for_each(this->relations.begin(), this->relations.end(), [](class Relation *p) { delete p; });
	this->relations.clear();

	//entityMapのEntityポインタが指す実体は、entitysで管理しているため、entityMapの内部に格納しているポインタに対してはdelete命令を行わない。
}