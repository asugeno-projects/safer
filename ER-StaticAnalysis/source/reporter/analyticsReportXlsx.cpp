/**
* @file       analyticsReportXlsx.cpp
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

#include <Xlsx/Workbook.h>
using namespace SimpleXlsx;

#include "analyticsReportXlsx.h"
#include <stdlib.h>
#include <codecvt>

/**
* @fn AnalyticsReportXlsx::AnalyticsReportXlsx()
* @brief AnalyticsReportXlsxのコンストラクタ
* @details AnalyticsReportXlsxのコンストラクタ
*/
AnalyticsReportXlsx::AnalyticsReportXlsx()
{
	this->m_book = NULL;
}

/**
* @fn class CWorkbook * AnalyticsReportXlsx::getBookInstance()
* @brief CWorkbookのインスタンスを取得する関数
* @return CWorkbookインスタンス
* @details CWorkbookのインスタンスを取得する関数
*/
class CWorkbook * AnalyticsReportXlsx::getBookInstance()
{
	if (this->m_book == NULL)
	{
		this->m_book = new CWorkbook();
	}
	return this->m_book;
}

/**
* @fn void AnalyticsReportXlsx::save(std::string filename)
* @brief xlsxファイルの保存関数
* @param filename 保存パスとファイル名(拡張子なし)
* @details CWorkbookのインスタンス内部に保存された表計算データを指定のファイルに保存する関数
* @note save関数実行前にwrite関数を実行する必要があります。
*/
bool AnalyticsReportXlsx::save(std::string filename)
{
	if (this->getBookInstance() == NULL)
	{
		//ここに来る場合は、write関数が呼び込まれていないか、
		//表計算インスタンスが消失してしまっている
		return false;
	}
	//ファイルへ保存
	filename += ".xlsx";
#ifdef _WIN32
	wchar_t * filenameWchar = stringToWchar(filename);
	bool bRes = this->getBookInstance()->Save(filenameWchar);
	delete filenameWchar;
#else
	bool bRes = this->getBookInstance()->Save((_tstring &)filename);
#endif
	if (bRes)
	{
		cout << "The book has been saved successfully";
		return true;
	}
	else
	{
		cout << "The book saving has been failed";
		return false;
	}
	return false;
}

/**
* @fn void AnalyticsReportXlsx::AddSummaryRow(class CWorksheet *sheet, const _tstring *str, int num, int style = 0)
* @brief Summaryレポート用レコード登録関数
* @param sheet 表計算シートインスタンス
* @param *str 数値データのカテゴリ名
* @param num 数値データ
* @param style1 Columnに適用するスタイルID
* @param style2 Columnに適用するスタイルID
* @details Summaryレポートに記録するレコードを表計算シートインスタンスに登録する
*/
template<typename T>
void AnalyticsReportXlsx::AddSummaryRow(class CWorksheet *sheet, const _tstring str, T num, int style1, int style2)
{
	if (typeid(T) == typeid(int) ||
		typeid(T) == typeid(float) ||
		typeid(T) == typeid(double))
	{
		CellDataStr cellStr;
		CellDataDbl cellDbl;
		cellStr.value = str;
		cellStr.style_id = style1;
		cellDbl.value = num;
		cellDbl.style_id = style2;
		sheet->BeginRow();
		sheet->AddCell(cellStr);
		sheet->AddCell(cellDbl);
		sheet->EndRow();
	}
}

/**
* @fn void AnalyticsReportXlsx::AddStringCell(class CWorksheet *sheet, std::string str, int style = 0)
* @brief Cellへ文字列を登録する関数
* @param sheet 表計算シートインスタンス
* @param str 文字列
* @param style Columnに適用するスタイルID
* @details レポートに記録するCellを表計算シートインスタンスに登録する<br>
登録時に使用する文字列情報はstring型を想定
*/
void AnalyticsReportXlsx::AddStringCell(class CWorksheet *sheet, std::string str, int style = 0)
{
	CellDataStr data;
#ifdef _WIN32
	wchar_t *wc;
	data.style_id = style;
	wc = stringToWchar(str);
	data.value = wc;
	delete wc;
#else
	data.value = str;
#endif
	sheet->AddCell(data);
}

/**
* @fn void AnalyticsReportXlsx::AddWstringCell(class CWorksheet *sheet, const wstring str, int style = 0)
* @brief Cellへワイド文字型の文字列を登録する関数
* @param sheet 表計算シートインスタンス
* @param str 文字列
* @param style Columnに適用するスタイルID
* @details レポートに記録するCellを表計算シートインスタンスに登録する<br>
登録時に使用する文字列情報はワイド文字型を想定
*/
void AnalyticsReportXlsx::AddWstringCell(class CWorksheet *sheet, const wstring str, int style = 0)
{
	CellDataStr cellStr;
#ifdef _WIN32
	cellStr.value = str;
#else
	std::wstring_convert<std::codecvt_utf8<wchar_t>,wchar_t> cv;
	cellStr.value = cv.to_bytes(str);
#endif
	cellStr.style_id = style;
	sheet->AddCell(cellStr);
}

/**
* @fn void AnalyticsReportXlsx::AddTstringCell(class CWorksheet *sheet, const _tstring str, int style = 0)
* @brief Cellへワイド文字型の文字列を登録する関数
* @param sheet 表計算シートインスタンス
* @param str 文字列
* @param style Columnに適用するスタイルID
* @details レポートに記録するCellを表計算シートインスタンスに登録する<br>
登録時に使用する文字列情報はワイド文字型を想定
*/
void AnalyticsReportXlsx::AddTstringCell(class CWorksheet *sheet, const _tstring str, int style = 0)
{
	CellDataStr cellStr;
	cellStr.value = str;
	cellStr.style_id = style;
	sheet->AddCell(cellStr);
}

/**
* @fn AnalyticsReportXlsx * AnalyticsReportXlsx::write(list <AnalyticsRowData> rowData, ER * er)
* @brief   レポート情報生成関数
* @param   rowData 検出情報
* @param   er ER情報
* @return  処理の成否
* @details レポート用のデータを生成します。<br>
CWorksBook インスタンスにレポート情報を記録していきます。
* @note    write関数実行後にレポートファイルとして保存を行う場合は、save関数を呼び出す必要があります。
*/
AnalyticsReportXlsx * AnalyticsReportXlsx::write(list <AnalyticsRowData> rowData, ER * er)
{
	//この定義が無いとxlsxファイルの日本語が文字化けします。
	std::locale::global(std::locale(""));
	std::ios_base::sync_with_stdio(false);

	//サマリーレポートの処理の定義
	{
		int styleId = this->getPercentStyle();
		int normalStyle = this->getNormalColumnStyle();
		//Columnフォーマットの定義
		std::vector<ColumnWidth> colWidths;
		ColumnWidth colWidth;
		colWidth.colFrom = colWidth.colTo = 0;
		colWidth.width = 30;
		colWidths.push_back(colWidth);
		colWidth.colFrom = colWidth.colTo = 1;
		colWidth.width = 30;
		colWidths.push_back(colWidth);

		CWorksheet &sheet = this->getBookInstance()->AddSheet(_T("SummaryReport"), colWidths);

		int entityCount = 0;
		//Entityマップ生成(エラー判定用)
		map<std::wstring, bool> entityMapForError;
		map<std::wstring, bool> entityMapForWarning;
		map<std::wstring, bool> entityMapForIllegal;
		for (auto entityIt = er->entitys.begin(); entityIt != er->entitys.end(); entityIt++, entityCount++)
		{
			entityMapForError[(*entityIt)->getPhysicalTableName()] = false;
			entityMapForWarning[(*entityIt)->getPhysicalTableName()] = false;
			entityMapForIllegal[(*entityIt)->getPhysicalTableName()] = false;
		}

		int errorEntityCount = 0;//エラーが出たEntityの数
		int warningEntityCount = 0;//ワーニングEntity数
		int illegalEntityCount = 0;
		int errorCount = 0;//エラー数
		int warningCount = 0;
		int noteCount = 0;//notice数
		vector<int> errorTypeCount;//項目ごとのエラー数
		errorTypeCount.resize(CommandTypeList::E_CommandTypeMax);
		//エラーの内訳を算出
		for (auto dataIt = rowData.begin(); dataIt != rowData.end(); dataIt++)
		{
			if (((*dataIt).alertLevel == AlertLevelList::E_critical))
			{
				errorCount++;
				if (entityMapForError[(*dataIt).tableName] == false)
				{
					errorEntityCount++;
					entityMapForError[(*dataIt).tableName] = true;
				}
				if (entityMapForIllegal[(*dataIt).tableName] == false)
				{
					illegalEntityCount++;
					entityMapForIllegal[(*dataIt).tableName] = true;
				}
			}
			else if ((*dataIt).alertLevel == AlertLevelList::E_warning)
			{
				warningCount++;
				if (entityMapForWarning[(*dataIt).tableName] == false)
				{
					warningEntityCount++;
					entityMapForWarning[(*dataIt).tableName] = true;
				}
				if (entityMapForIllegal[(*dataIt).tableName] == false)
				{
					illegalEntityCount++;
					entityMapForIllegal[(*dataIt).tableName] = true;
				}
			}
			else
			{
				noteCount++;
			}
			errorTypeCount[(*dataIt).commandType]++;
		}

		//エラーレート算出
		double errorRate = 0;
		if (errorEntityCount)
		{
			errorRate = (double)errorEntityCount / (double)entityCount;
		}

		//値のセット
		this->AddSummaryRow(&sheet, _T("総テーブル数"), entityCount, normalStyle, normalStyle);
		this->AddSummaryRow(&sheet, _T("正常テーブル数"), entityCount - illegalEntityCount, normalStyle, normalStyle);
		this->AddSummaryRow(&sheet, _T("エラーテーブル数"), errorEntityCount, normalStyle, normalStyle);
		this->AddSummaryRow(&sheet, _T("ワーニングテーブル数(エラー無しテーブル対象)"), illegalEntityCount - errorEntityCount, normalStyle, normalStyle);
		this->AddSummaryRow(&sheet, _T("エラー数"), errorCount, normalStyle,  normalStyle);
		this->AddSummaryRow(&sheet, _T("notice数"), noteCount, normalStyle, normalStyle);
		this->AddSummaryRow(&sheet, _T("エラーテーブル率"), errorRate, normalStyle, styleId);
		for (int commandType = CommandTypeList::E_Comparison; commandType < CommandTypeList::E_CommandTypeMax; commandType++)
		{
			_tstring title = CommandTypeLogicalNameList[(CommandTypeList)commandType] + _T("による検出数");
			this->AddSummaryRow(&sheet, title, errorTypeCount[commandType], normalStyle, normalStyle);
		}

		// グラフ1
		CChartsheet::Series ser;

		//データ名があるシートの指定
		ser.catSheet = &sheet;
		// データ名の位置指定
		ser.catAxisFrom = CellCoord(1, 0);
		ser.catAxisTo = CellCoord(3, 0);

		//データがあるシートの指定
		ser.valSheet = &sheet;
		// データの位置指定
		ser.valAxisFrom = CellCoord(1, 1);
		ser.valAxisTo = CellCoord(3, 1);
		//タイトルのセット
		ser.title = _T("全体評価");
		
		//グラフを表示するシートの生成
		CChartsheet &Errorchart = this->getBookInstance()->AddChart(_T("Pie chart"), CHART_PIE);
		Errorchart.SetDiagrammName(_T("全体評価"));
		Errorchart.SetLegendPos(CChartsheet::EPosition::POS_RIGHT);
		
		//グラフをチャートシートにセット
		Errorchart.AddSeries(ser);
	}

	//等値比較で検出したデータシートの生成
	{
		//Cell毎のスタイルの取得
		int topStyle = this->getStyleTop1();
		int alertLevelStyle[3];
		alertLevelStyle[AlertLevelList::E_note] = getAlertLevelColumnStyle(AlertLevelList::E_note);
		alertLevelStyle[AlertLevelList::E_warning] = getAlertLevelColumnStyle(AlertLevelList::E_warning);
		alertLevelStyle[AlertLevelList::E_critical] = getAlertLevelColumnStyle(AlertLevelList::E_critical);
		int normalStyle = this->getNormalColumnStyle();

		//Columnフォーマットの定義
		std::vector<ColumnWidth> colWidths;
		ColumnWidth colWidth;
		colWidth.colFrom = colWidth.colTo = 0;
		colWidth.width = 23;
		colWidths.push_back(colWidth);
		colWidth.colFrom = 1;
		colWidth.colTo = 2;
		colWidth.width = 30;
		colWidths.push_back(colWidth);
		colWidth.colFrom = colWidth.colTo = 3;
		colWidth.width = 50;
		colWidths.push_back(colWidth);

		map<int, CWorksheet *> sheets;
		for (int commandType = CommandTypeList::E_Comparison; commandType < CommandTypeList::E_CommandTypeMax; commandType++)
		{
			sheets.insert(make_pair(commandType, this->createCommonSheet(topStyle, colWidths, CommandTypeLogicalNameList[(CommandTypeList)commandType])));
		}

		//実データ出力
		for (auto dataIt = rowData.begin(); dataIt != rowData.end(); dataIt++) {
			CWorksheet *sheet = sheets[(*dataIt).commandType];
			sheet->BeginRow();

			// アラートレベル
			this->AddStringCell(sheet, AlertLevelToLabel[(*dataIt).alertLevel], alertLevelStyle[(*dataIt).alertLevel]);
			// テーブルネーム
			this->AddWstringCell(sheet, (*dataIt).tableName, normalStyle);
			// 検出対象名
			this->AddWstringCell(sheet, (*dataIt).targetName, normalStyle);
			//<! @note メッセージ
			this->AddWstringCell(sheet, (*dataIt).message, normalStyle);

			sheet->EndRow();
		}
		//Filter追加
		for (int commandType = CommandTypeList::E_Comparison; commandType < CommandTypeList::E_CommandTypeMax; commandType++)
		{
			CWorksheet *sheet = sheets[commandType];
			sheet->filterCells(CellCoord(1, 0), CellCoord(1, 3));
		}
		
	}

	return this;
}

/**
* @fn CWorksheet * AnalyticsReportXlsx::createCommonSheet(int styleId, std::vector<ColumnWidth> colWidths, _tstring sheetName)
* @brief   汎用検出リスト用シート生成関数
* @param   styleId スタイルID
* @param   colWidths カラム幅
* @param   sheetName シート名
* @return  シートインスタンス
* @details 汎用化した検出リスト用シートを生成し呼び出し元に返します。
*/
CWorksheet * AnalyticsReportXlsx::createCommonSheet(int styleId, std::vector<ColumnWidth> colWidths, _tstring sheetName)
{
	CWorksheet &sheet = this->getBookInstance()->AddSheet(sheetName, colWidths);

	//ヘッダー部分の定義
	sheet.BeginRow();
	this->AddTstringCell(&sheet, _T("アラートレベル"), styleId);
	this->AddTstringCell(&sheet, _T("検出対象テーブル"), styleId);
	this->AddTstringCell(&sheet, _T("検出対象名"), styleId);
	this->AddTstringCell(&sheet, _T("コメント"), styleId);
	sheet.EndRow();

	return &sheet;
}


/**
* @fn int AnalyticsReportXlsx::getPercentStyle()
* @brief 数値(%)用スタイル生成関数
* @return スタイルID
* @details %用のスタイル設定をシートに登録し、登録されたIDを呼び出し元に返す
*/
int AnalyticsReportXlsx::getPercentStyle()
{
	Style style;
	style.fill.patternType = PATTERN_NONE;
	style.font.size = 11;
	style.font.theme = true;
	style.font.attributes = FONT_ITALIC;
	style.horizAlign = ALIGN_H_RIGHT;
	style.vertAlign = ALIGN_V_CENTER;
	style.numFormat.numberStyle = NUMSTYLE_PERCENTAGE;
	Border border;
	border.top.style = BORDER_THIN;
	border.bottom.style = BORDER_THIN;
	border.left.style = BORDER_THIN;
	border.right.style = BORDER_THIN;
	style.border = border;
	return this->getBookInstance()->m_styleList.Add(style);
}

/**
* @fn int AnalyticsReportXlsx::getStyleTop1()
* @brief head用スタイル生成関数
* @return スタイルID
* @details 表上にて、強調されたスタイルを登録し、登録されたIDを呼び出し元に返す
*/
int AnalyticsReportXlsx::getStyleTop1()
{
	Style style;
	style.fill.patternType = PATTERN_NONE;
	style.font.size = 14;
	style.font.theme = true;
	style.font.attributes = FONT_ITALIC;
	style.horizAlign = ALIGN_H_LEFT;
	style.vertAlign = ALIGN_V_CENTER;
	style.fill.patternType = PATTERN_SOLID;
	style.fill.fgColor = _T("8FD3D3D3");
	Border border;
	border.top.style = BORDER_THIN;
	border.bottom.style = BORDER_MEDIUM;
	border.left.style = BORDER_THIN;
	border.right.style = BORDER_THIN;
	style.border = border;
	return this->getBookInstance()->m_styleList.Add(style);
}

/**
* @fn int AnalyticsReportXlsx::getAlertLevelColumnStyle(AlertLevelList alertLevel)
* @brief アラートレベル表記用スタイル生成関数
* @param alertLevel アラートレベル
* @return スタイルID
* @details 入力されたアラートレベルに応じてスタイルを生成し登録、登録されたIDを呼び出し元に返す
*/
int AnalyticsReportXlsx::getAlertLevelColumnStyle(AlertLevelList alertLevel)
{
	Style style;
	style.fill.patternType = PATTERN_SOLID;
	style.font.size = 14;
	style.font.theme = true;
	style.font.attributes = FONT_ITALIC ^ FONT_BOLD ^ FONT_EXTEND;
	style.font.color = _T("FF000000");
	style.horizAlign = ALIGN_H_LEFT;
	style.vertAlign = ALIGN_V_CENTER;
	Border border;
	border.top.style = BORDER_THIN;
	border.bottom.style = BORDER_THIN;
	border.left.style = BORDER_THIN;
	border.right.style = BORDER_THIN;
	style.border = border;

	switch (alertLevel)
	{
	case AlertLevelList::E_note:
		style.fill.fgColor = _T("FF66CCFF");//safe color
		break;
	case AlertLevelList::E_warning:
		style.fill.fgColor = _T("FFFFAC3F");//safe color
		break;
	case AlertLevelList::E_critical:
		style.fill.fgColor = _T("FFFF4C4C");//safe color
		break;
	}
	return this->getBookInstance()->m_styleList.Add(style);
}

/**
* @fn int AnalyticsReportXlsx::getNormalColumnStyle()
* @brief 標準用スタイル生成関数
* @return スタイルID
* @details 標準的なスタイルを生成し登録、登録されたIDを呼び出し元に返す
*/
int AnalyticsReportXlsx::getNormalColumnStyle()
{
	Style style;
	style.fill.patternType = PATTERN_NONE;
	style.font.size = 10;
	style.font.theme = true;
	//style.font.attributes = FONT_ITALIC;
	style.horizAlign = ALIGN_H_LEFT;
	style.vertAlign = ALIGN_V_CENTER;
	style.wrapText = 1;
	Border border;
	border.top.style = BORDER_THIN;
	border.bottom.style = BORDER_THIN;
	border.left.style = BORDER_THIN;
	border.right.style = BORDER_THIN;
	style.border = border;
	return this->getBookInstance()->m_styleList.Add(style);
}
