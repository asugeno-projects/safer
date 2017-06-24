/**
* @file       analyticsReportXlsx.h
* @date       2016/01/31
* @author     Akihiro Sugeno
* @par        Revision
* $Id$
* @par        Copyright
* Copyright 2015 Akihiro Sugeno
* @par        History
*     - 2015/10/12 Akihiro Sugeno
*       -# Initial Version
*/

#ifndef RDS_ER_STATICANALYSIS_SOURCE_REPORTER_ANALYTICSREPORTXLSX_H_
#define RDS_ER_STATICANALYSIS_SOURCE_REPORTER_ANALYTICSREPORTXLSX_H_

#include <iostream>
#include <string>
#include <list>
#include <vector>
//<! @include analyticsReport.h
#include "analyticsReport.h"
//<! @include Er.h
#include "../ER/er.h"

/*! @class AnalyticsReportXlsx
@brief 解析レポートを生成するクラス
*/
class AnalyticsReportXlsx : private AnalyticsReport
{
private:
	//! CWorkbookの格納用ポインタ変数
	class CWorkbook *m_book;
	//! CWorkbookのインスタンスを取得する関数
	class CWorkbook *getBookInstance();
public:
	//! コンストラクタ
	AnalyticsReportXlsx();
	//! head用スタイル生成関数
	int getStyleTop1();
	//! アラートレベル表記用スタイル生成関数
	int getAlertLevelColumnStyle(AlertLevelList alertLevel);
	//! 標準用スタイル生成関数
	int getNormalColumnStyle();
	//! 数値(%)用スタイル生成関数
	int getPercentStyle();
	//! Cellへ文字列を登録する関数
	void AddStringCell(class CWorksheet *sheet, std::string str, int style);
	//! Cellへワイド文字型の文字列を登録する関数
	void AddWstringCell(class CWorksheet *sheet, const wstring str, int style);
	//! Cellへ_tstring型の文字列を登録する関数
	void AddTstringCell(class CWorksheet *sheet, const _tstring str, int style);
	template<typename T>
	//! Summaryレポート用レコード登録関数
	void AddSummaryRow(class CWorksheet *sheet, const _tstring str, T num, int style1, int style2);
	//! レポート情報生成関数
	AnalyticsReportXlsx * write(list <AnalyticsRowData> rowData, ER * er);
	//! xlsxファイルの保存関数
	bool save(std::string file);

	class CWorksheet * createCommonSheet(int topStyle, std::vector<class ColumnWidth> colWidths, _tstring sheetName);
};

#endif //RDS_ER_STATICANALYSIS_SOURCE_REPORTER_ANALYTICSREPORTXLSX_H_
