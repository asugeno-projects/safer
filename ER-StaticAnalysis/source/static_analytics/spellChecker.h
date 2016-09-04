/**
* @file       spellChecker.h
* @date       2016/01/31
* @author     Akihiro Sugeno
* @par        Revision
* $Id$
* @par        Copyright
* Copyright 2015 Akihiro Sugeno
* @par        History
*     - 2015/10/19 Akihiro Sugeno
*       -# Initial Version
*/

#ifndef RDS_ER_STATICANALYSIS_SOURCE_STATIC_ANALYTICS_SPELLCHECKER_H_
#define RDS_ER_STATICANALYSIS_SOURCE_STATIC_ANALYTICS_SPELLCHECKER_H_

#include <boost/regex.hpp>
#include <regex>
#include <string>


/*! @class SpellChecker
* @brief スペルチェッククラス
* @details Entityのスペルチェックを行うクラス
*/
class SpellChecker
{
private:
	const std::wstring unknownMessage = L"辞書に無い単語\n";//! 対象の単語がなかった場合のメッセージ
	const std::wstring hitMessage = L"修正候補\n";//! 修正候補を表示する直前の文書
	const boost::wregex camelString = boost::wregex(L"([A-Z])");//! キャメルケース判定用正規表現
	const std::wstring camelConvertString = L"_$1";//! キャメルケース用スネークケース変換表現
	const std::string snakeSplitWord = "_";//! スネークケースセパレータ用文字列
	const std::wstring prefixTableName = L"テーブル名：";//! データ表記用タグ(テーブル名)
	const std::wstring prefixColumnName = L"カラム名：";//! データ表記用タグ(カラム名)

public:
	//! スペルチェックの実行関数
	void run(class SpellCheckConfig *spellCheckConfig, class ER *er);

	//! スペルチェック用関数
	void spellCheck(class Hunhandle *dic, std::wstring table, std::wstring target, std::wstring sectionTag, std::wregex excludePattern);
};

#endif //RDS_ER_STATICANALYSIS_SOURCE_STATIC_ANALYTICS_SPELLCHECKER_H_