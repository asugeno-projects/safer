/**
* @file       spellChecker.cpp
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

#include "spellChecker.h"
#include <hunspell.h>
#include <hunspelldll.h>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <string>
#include <vector>
#include "../ER/er.h"
#include "../static_analytics/compRegularExpression.h"
#include "../common/commonString.h"
#include "../reporter/reporter.h"

/**
* @fn void SpellChecker::run(SpellCheckConfig *spellCheckConfig, ER *er)
* @brief スペルチェックの実行関数
* @param spellCheckConfig スペルチェック用設定情報
* @param er ER情報インスタンス
* @details スペルチェックのに関する処理を行う関数
*/
void SpellChecker::run(class SpellCheckConfig *spellCheckConfig, class ER *er)
{
	std::wregex excludePattern = CompRegularExpression::getStringsToWOrPattern(spellCheckConfig->getExcludeValues());
	Hunhandle *dic;
	dic = Hunspell_create((*(spellCheckConfig->getAffFileNames().begin())).c_str(), (*(spellCheckConfig->getDicFileNames().begin())).c_str());

	//Entityの数だけループ処理
	for (std::list<Entity *>::iterator entityIt = er->entitys.begin(); entityIt != er->entitys.end(); entityIt++)
	{
		// テーブル物理名チェック
		this->spellCheck(dic, (*entityIt)->getPhysicalTableName(), (*entityIt)->getPhysicalTableName(), SpellChecker::prefixTableName, excludePattern);

		//カラム数の数だけループ処理
		auto fields = (*entityIt)->getfields();
		for (auto fieldIt = fields.begin(); fieldIt != fields.end(); fieldIt++)
		{
			// カラム物理名チェック
			this->spellCheck(dic, (*entityIt)->getPhysicalTableName(), (*fieldIt).name.physicalName, SpellChecker::prefixColumnName, excludePattern);
		}
	}
}

/**
* @fn void SpellChecker::spellCheck(Hunhandle *dic, std::wstring table, std::wstring target, std::wstring sectionTag, std::wregex excludePattern)
* @brief スペルチェック用関数
* @param dic 辞書情報
* @param table 検出対象テーブル名
* @param target 検出対象名
* @param sectionTag コメントにセクション名を挿入するための文字列
* @param excludePattern 除外判定用正規表現を定義した文字列
* @details 入力された情報からスペルチェックを行う関数
*/
void SpellChecker::spellCheck(Hunhandle *dic, std::wstring table, std::wstring target, std::wstring sectionTag, std::wregex excludePattern)
{
	//CamelケースをSnakeケースへ変更
	std::wstring convert;
	convert = boost::regex_replace(target, SpellChecker::camelString, SpellChecker::camelConvertString);

	//_区切りをスプリット
	vector<std::string> v;
	boost::algorithm::split(v, convert, boost::is_any_of(SpellChecker::snakeSplitWord));

	//分割されたワードの数だけループ処理
	for (auto wordIt = v.begin(); wordIt != v.end(); wordIt++)
	{
		//対象ワードが検出除外対象かどうかチェック
		std::wstring word;
		stringToWString(word, (*wordIt));
		if (regex_match(word.c_str(), excludePattern))
		{
			//検出対象外であるため、処理をスキップ
			continue;
		}

		//対象の文字列が辞書データに登録されているかチェック
		int result = Hunspell_spell(dic, (*wordIt).c_str());
		if (result == false)
		{
			//対象の文字列が辞書データになかったため、修正候補の文字列を取得する処理を行う。
			int list_size = 0;//修正候補数
			char **slist;//修正候補の文字列

			//修正候補の文字列を取得
			list_size = Hunspell_suggest(dic, &slist, (*wordIt).c_str());

			//エラー検出した文字列をワイド文字化
			std::wstring errorWord;
			stringToWString(errorWord, (*wordIt));

			//エラーメッセージ生成
			std::wstring message = SpellChecker::unknownMessage;
			message += errorWord + L"\n\n";
			message += SpellChecker::hitMessage;
			//修正候補の文字列だけループ処理
			for (int i = 0; i < list_size; i++)
			{
				//修正候補の文字列をカンマ区切りでエラーメッセージに結合
				std::wstring hitSpell;
				stringToWString(hitSpell, (std::string)slist[i]);
				message += hitSpell;
				if (i + 1 < list_size)
				{
					message += L",";
				}
			}

			//レポーターに報告
			Reporter::getInstance()->addRecord(
				AlertLevelList::E_note,
				CommandTypeList::E_SpellCheck,
				table,
				sectionTag + target,
				message
				);
		}
	}
}