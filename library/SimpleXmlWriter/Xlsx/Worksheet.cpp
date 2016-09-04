/*

Copyright (c) 2012-2013, Pavel Akimov
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include <stdlib.h>
#include <limits>
#include <iomanip>

#include "SimpleXlsxDef.h"
#include "Worksheet.h"
#include "XlsxHeaders.h"

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

namespace SimpleXlsx {

using namespace std;
using namespace xmlw;

extern bool MakeDirectory(const _tstring& dirName);

// ****************************************************************************
/// @brief      The class default constructor
/// @return     no
/// @note	    The constructor creates an instance with sheet1.xml filename
///             and without any frozen panes
// ****************************************************************************
CWorksheet::CWorksheet()
{
    m_temp_path = _T("");
	std::vector<ColumnWidth> colWidths;
    Init(1, 0, 0, colWidths);
}

// ****************************************************************************
/// @brief      The class constructor
/// @param      index index of a sheet to be created (for example, sheet1.xml)
/// @param		temp_path path to the temporary directory where sheet files must be created
/// @return     no
/// @note	    The constructor creates an instance with specified sheetX.xml filename
///             and without any frozen panes
// ****************************************************************************
CWorksheet::CWorksheet(uint32_t index, const _tstring& temp_path) : m_temp_path(temp_path)
{
	std::vector<ColumnWidth> colWidths;
    Init(index, 0, 0, colWidths);
}

// ****************************************************************************
/// @brief      The class constructor to create sheet with frozen pane
/// @param      index index of a sheet to be created (for example, sheet1.xml)
/// @param      width frozen pane with
/// @param      height frozen pane height
/// @param		temp_path path to the temporary directory where sheet files must be created
/// @return     no
// ****************************************************************************
CWorksheet::CWorksheet(uint32_t index, uint32_t width, uint32_t height, const _tstring& temp_path) : m_temp_path(temp_path)
{
	std::vector<ColumnWidth> colWidths;
    Init(index, width, height, colWidths);
}

// ****************************************************************************
/// @brief      The class constructor
/// @param      index index of a sheet to be created (for example, sheet1.xml)
/// @param		colWidths list of pairs colNumber:Width
/// @param		temp_path path to the temporary directory where sheet files must be created
/// @return     no
/// @note	    The constructor creates an instance with specified sheetX.xml filename
///             and without any frozen panes
// ****************************************************************************
CWorksheet::CWorksheet(uint32_t index, std::vector<ColumnWidth>& colWidths, const _tstring& temp_path) : m_temp_path(temp_path)
{
	Init(index, 0, 0, colWidths);
}

// ****************************************************************************
/// @brief      The class constructor to create sheet with frozen pane
/// @param      index index of a sheet to be created (for example, sheet1.xml)
/// @param      width frozen pane with
/// @param      height frozen pane height
/// @param		colWidths list of pairs colNumber:Width
/// @param		temp_path path to the temporary directory where sheet files must be created
/// @return     no
// ****************************************************************************
CWorksheet::CWorksheet(uint32_t index, uint32_t width, uint32_t height, std::vector<ColumnWidth>& colWidths, const _tstring& temp_path) : m_temp_path(temp_path)
{
	Init(index, width, height, colWidths);
}

// ****************************************************************************
/// @brief  The class destructor (virtual)
/// @return no
// ****************************************************************************
CWorksheet::~CWorksheet()
{
    if (m_xmlStream) {
        delete m_xmlStream;
        m_xmlStream = NULL;
    }

    if (m_f) {
        m_f->close();
        delete m_f;
        m_f = NULL;
    }

    m_calcChain.clear();
}

// ****************************************************************************
/// @brief  Initializes internal variables and creates a header of a sheet xml tree
/// @param  index index of a sheet to be created (for example, sheet1.xml)
/// @param  frozenWidth frozen pane with
/// @param  frozenHeight frozen pane height
/// @param	colWidths list of pairs colNumber:Width
/// @return no
// ****************************************************************************
void CWorksheet::Init(uint32_t index, uint32_t frozenWidth, uint32_t frozenHeight, std::vector<ColumnWidth>& colWidths)
{
    m_isOk = true;
    m_row_opened = false;
    m_current_column = 0;
    m_offset_column = 0;
    m_index = index;
    m_title = _T("Sheet 1");
    m_withFormula = false;
    m_withComments = false;
    m_calcChain.clear();
    m_sharedStrings = NULL;
    m_comments = NULL;
    m_mergedCells.clear();
	m_filterCells.clear();
    m_row_index = 0;
    m_page_orientation = PAGE_PORTRAIT;

    TCHAR szPathToFile[MAX_PATH] = { 0 };
    _stprintf(szPathToFile, _T("%s/xl/worksheets/sheet%d.xml"), m_temp_path.c_str(), m_index);
    MakeDirectory(szPathToFile);
    char szPath[MAX_PATH] = { 0 };
#ifdef UNICODE
	int32_t res = wcstombs(szPath, szPathToFile, MAX_PATH);
	if (res == -1) {
		m_isOk = false;
		return;
	}
#else
	_stprintf(szPath, _T("%s"), szPathToFile);
#endif
	m_f = new (std::nothrow) _tofstream(szPath);
	m_f->imbue(locale());

    m_xmlStream = new (std::nothrow) XmlStream(*m_f);
    if (m_xmlStream == NULL) {
        m_isOk = false;
        return;
    }

    // Create core sheet structure
    (*m_xmlStream) << prolog()
		<< tag(_T("worksheet")) << attr(_T("xmlns")) << ns_book << attr(_T("xmlns:r")) << ns_book_r << attr(_T("xmlns:mc")) << ns_mc
                            << attr(_T("mc:Ignorable")) << _T("x14ac") << attr(_T("xmlns:x14ac")) << ns_x14ac
			<< tag(_T("dimension")) << attr(_T("ref")) << _T("A1") << endtag()
			<< tag(_T("sheetViews"))
				<< tag(_T("sheetView")) << attr(_T("tabSelected")) << 0 << attr(_T("workbookViewId")) << 0;

            if (frozenWidth != 0 || frozenHeight != 0)
                AddFrozenPane(frozenWidth, frozenHeight);

            (*m_xmlStream)
                << endtag()

			<< endtag()
			<< tag(_T("sheetFormatPr")) << attr(_T("defaultRowHeight")) << 15 << attr(_T("x14ac:dyDescent")) << 0.25 << endtag();

	if (!colWidths.empty()) {
		(*m_xmlStream)
			<< tag(_T("cols"));
	}

	for (size_t i = 0; i < colWidths.size(); i++) {
		(*m_xmlStream)
				<< tag(_T("col"))
					<< attr(_T("min")) << colWidths[i].colFrom + 1
					<< attr(_T("max")) << colWidths[i].colTo + 1
					<< attr(_T("width")) << colWidths[i].width
				<< endtag();
	}

	if (!colWidths.empty()) {
		(*m_xmlStream)
			<< endtag();
	}

	(*m_xmlStream)
			<< tag(_T("sheetData"));    // open sheetData tag
}

// ****************************************************************************
/// @brief	Add std::string-formatted cell with specified style
/// @param	data reference to data
/// @return	no
// ****************************************************************************
void CWorksheet::AddCell(const CellDataStr& data)
{
	if (data.value != _T("")) {
		TCHAR szCoord[15] = { 0 };
		GetCellCoord(CellCoord(m_row_index, m_offset_column + m_current_column), szCoord);
		(*m_xmlStream) << tag(_T("c")) << attr(_T("r")) << szCoord;

		if (data.style_id != 0) {  // default style is not necessary to sign explisitly
			(*m_xmlStream) << attr(_T("s")) << data.style_id;
		}

		if (data.value[0] == _T('=')) {
			(*m_xmlStream) << tag(_T("f")) << chardata() << data.value.substr(1, data.value.length() - 1).c_str() << endtag();

			m_withFormula = true;
			m_calcChain.push_back(szCoord);
		}
		else {
			if (m_sharedStrings != NULL) {
				uint64_t str_index = 0;
				map<_tstring, uint64_t>::iterator it = m_sharedStrings->find(data.value);
				if (it == m_sharedStrings->end()) {
				    str_index = m_sharedStrings->size();
                    (*m_sharedStrings)[data.value] = str_index;
				}
				else {
                    str_index = it->second;
				}

				(*m_xmlStream) << attr(_T("t")) << _T("s") << tag(_T("v")) << chardata() << str_index << endtag();
			}
			else {
				(*m_xmlStream) << tag(_T("v")) << chardata() << data.value.c_str() << endtag();
			}
		}

		(*m_xmlStream) << endtag(); // c
	}

	m_current_column++;
}

// ****************************************************************************
/// @brief  Appends another row into the sheet
/// @param  data reference to the vector of pairs <std::string, style>
/// @param  offset the offset from the row begining (0 by default)
/// @param	height row height (deafult if 0)
/// @return no
// ****************************************************************************
void CWorksheet::AddRow(const std::vector<CellDataStr>& data, uint32_t offset, uint32_t height)
{
    m_offset_column = (int32_t)offset;

    TCHAR szSpans[15] = { 0 };
    _stprintf(szSpans, _T("%d:%d"), m_offset_column + 1, (int32_t)data.size() + m_offset_column + 1);

    (*m_xmlStream) << tag(_T("row")) << attr(_T("r")) << ++m_row_index << attr(_T("spans")) << szSpans << attr(_T("x14ac:dyDescent")) << 0.25;
    if (height != 0) {
		(*m_xmlStream) << attr(_T("ht")) << height << attr(_T("customHeight")) << 1;
    }

    m_current_column = 0;
	AddCells(data);

    (*m_xmlStream) << endtag(); // row

    m_offset_column = 0;
}

// ****************************************************************************
/// @brief	Add time-formatted cell with specified style
/// @param	data reference to data
/// @return	no
// ****************************************************************************
void CWorksheet::AddCell(const CellDataTime& data)
{
	TCHAR szCoord[15] = { 0 };
	GetCellCoord(CellCoord(m_row_index, m_offset_column + m_current_column), szCoord);
	(*m_xmlStream) << tag(_T("c")) << attr(_T("r")) << szCoord;// << attr(_T("t")) << _T("str");

	if (data.style_id != 0) {  // default style is not necessary to sign explisitly
		(*m_xmlStream) << attr(_T("s")) << data.style_id;
	}

	const int64_t secondsFrom1900to1970 = 2208988800;
	const double excelOneSecond = 0.0000115740740740741;

	struct tm *t = localtime(&data.value);
	time_t timeSinceEpoch = t->tm_sec + t->tm_min*60 + t->tm_hour*3600 + t->tm_yday*86400 +
							(t->tm_year-70)*31536000 + ((t->tm_year-69)/4)*86400 -
							((t->tm_year-1)/100)*86400 + ((t->tm_year+299)/400)*86400;

	double value = excelOneSecond * (secondsFrom1900to1970 + timeSinceEpoch) + 2;

	(*m_xmlStream) << tag(_T("v")) << chardata() << std::setprecision(std::numeric_limits<double>::digits10) << value << endtag();
	(*m_xmlStream) << endtag(); // c

	m_current_column++;
}

// ****************************************************************************
/// @brief  Appends another row into the sheet
/// @param  data reference to the vector of pairs <std::string, style>
/// @param  offset the offset from the row begining (0 by default)
/// @param	height row height (deafult if 0)
/// @return no
// ****************************************************************************
void CWorksheet::AddRow(const std::vector<CellDataTime>& data, uint32_t offset, uint32_t height)
{
    m_offset_column = (int32_t)offset;

    TCHAR szSpans[15] = { 0 };
    _stprintf(szSpans, _T("%d:%d"), m_offset_column + 1, (int32_t)data.size() + m_offset_column + 1);

    (*m_xmlStream) << tag(_T("row")) << attr(_T("r")) << ++m_row_index << attr(_T("spans")) << szSpans << attr(_T("x14ac:dyDescent")) << 0.25;
    if (height != 0) {
		(*m_xmlStream) << attr(_T("ht")) << height << attr(_T("customHeight")) << 1;
    }

	m_current_column = 0;
    AddCells(data);

    (*m_xmlStream) << endtag(); // row

    m_offset_column = 0;
}

// ****************************************************************************
// ****************************************************************************
void CWorksheet::AddCell()
{
	m_current_column++;
}

// ****************************************************************************
// ****************************************************************************
void CWorksheet::AddCell(const CellDataInt& data)
{
	AddCellRoutine(data.value, data.style_id);
}

// ****************************************************************************
// ****************************************************************************
void CWorksheet::AddCells(const std::vector<CellDataInt>& data)
{
	for (size_t i = 0; i < data.size(); i++) {
		AddCellRoutine(data[i].value, data[i].style_id);
	}
}

// ****************************************************************************
// ****************************************************************************
void CWorksheet::AddCell(const CellDataUInt& data)
{
	AddCellRoutine(data.value, data.style_id);
}

// ****************************************************************************
// ****************************************************************************
void CWorksheet::AddCells(const std::vector<CellDataUInt>& data)
{
	for (size_t i = 0; i < data.size(); i++) {
		AddCellRoutine(data[i].value, data[i].style_id);
	}
}

// ****************************************************************************
// ****************************************************************************
void CWorksheet::AddCell(const CellDataDbl& data)
{
	AddCellRoutine(data.value, data.style_id);
}

// ****************************************************************************
// ****************************************************************************
void CWorksheet::AddCells(const std::vector<CellDataDbl>& data)
{
	for (size_t i = 0; i < data.size(); i++) {
		AddCellRoutine(data[i].value, data[i].style_id);
	}
}

// ****************************************************************************
// ****************************************************************************
void CWorksheet::AddCell(const CellDataFlt& data)
{
	AddCellRoutine(data.value, data.style_id);
}

// ****************************************************************************
// ****************************************************************************
void CWorksheet::AddCells(const std::vector<CellDataFlt>& data)
{
	for (size_t i = 0; i < data.size(); i++) {
		AddCellRoutine(data[i].value, data[i].style_id);
	}
}

// ****************************************************************************
/// @brief  Appends another row into the sheet
/// @param  data reference to the vector of pairs <int32_t, style>
/// @param  offset the offset from the row begining (0 by default)
/// @param	height row height (deafult if 0)
/// @return no
// ****************************************************************************
void CWorksheet::AddRow(const std::vector<CellDataInt>& data, uint32_t offset, uint32_t height)
{
	m_offset_column = (int32_t)offset;

	TCHAR szSpans[15] = { 0 };
    _stprintf(szSpans, _T("%d:%d"), m_offset_column + 1, (int32_t)data.size() + m_offset_column + 1);

    (*m_xmlStream) << tag(_T("row")) << attr(_T("r")) << ++m_row_index << attr(_T("spans")) << szSpans << attr(_T("x14ac:dyDescent")) << 0.25;
    if (height != 0) {
		(*m_xmlStream) << attr(_T("ht")) << height << attr(_T("customHeight")) << 1;
    }

    m_current_column = 0;
	for (size_t i = 0; i < data.size(); i++) {
		AddCellRoutine(data[i].value, data[i].style_id);
	}

    (*m_xmlStream) << endtag(); // row

    m_offset_column = 0;
}

// ****************************************************************************
/// @brief  Appends another row into the sheet
/// @param  data reference to the vector of pairs <uint32_t, style>
/// @param  offset the offset from the row begining (0 by default)
/// @param	height row height (deafult if 0)
/// @return no
// ****************************************************************************
void CWorksheet::AddRow(const std::vector<CellDataUInt>& data, uint32_t offset, uint32_t height)
{
	m_offset_column = (int32_t)offset;

	TCHAR szSpans[15] = { 0 };
    _stprintf(szSpans, _T("%d:%d"), m_offset_column + 1, (int32_t)data.size() + m_offset_column + 1);

    (*m_xmlStream) << tag(_T("row")) << attr(_T("r")) << ++m_row_index << attr(_T("spans")) << szSpans << attr(_T("x14ac:dyDescent")) << 0.25;
    if (height != 0) {
		(*m_xmlStream) << attr(_T("ht")) << height << attr(_T("customHeight")) << 1;
    }

    m_current_column = 0;
	for (size_t i = 0; i < data.size(); i++) {
		AddCellRoutine(data[i].value, data[i].style_id);
	}

    (*m_xmlStream) << endtag(); // row

    m_offset_column = 0;
}

// ****************************************************************************
/// @brief  Appends another row into the sheet
/// @param  data reference to the vector of pairs <double, style>
/// @param  offset the offset from the row begining (0 by default)
/// @param	height row height (deafult if 0)
/// @return no
// ****************************************************************************
void CWorksheet::AddRow(const std::vector<CellDataDbl>& data, uint32_t offset, uint32_t height)
{
	m_offset_column = (int32_t)offset;

	TCHAR szSpans[15] = { 0 };
    _stprintf(szSpans, _T("%d:%d"), m_offset_column + 1, (int32_t)data.size() + m_offset_column + 1);

    (*m_xmlStream) << tag(_T("row")) << attr(_T("r")) << ++m_row_index << attr(_T("spans")) << szSpans << attr(_T("x14ac:dyDescent")) << 0.25;
    if (height != 0) {
		(*m_xmlStream) << attr(_T("ht")) << height << attr(_T("customHeight")) << 1;
    }

    m_current_column = 0;
	for (size_t i = 0; i < data.size(); i++) {
		AddCellRoutine(data[i].value, data[i].style_id);
	}

    (*m_xmlStream) << endtag(); // row

    m_offset_column = 0;
}

// ****************************************************************************
/// @brief  Appends another row into the sheet
/// @param  data reference to the vector of pairs <float, style>
/// @param  offset the offset from the row begining (0 by default)
/// @param	height row height (deafult if 0)
/// @return no
// ****************************************************************************
void CWorksheet::AddRow(const std::vector<CellDataFlt>& data, uint32_t offset, uint32_t height)
{
	m_offset_column = (int32_t)offset;

	TCHAR szSpans[15] = { 0 };
    _stprintf(szSpans, _T("%d:%d"), m_offset_column + 1, (int32_t)data.size() + m_offset_column + 1);

    (*m_xmlStream) << tag(_T("row")) << attr(_T("r")) << ++m_row_index << attr(_T("spans")) << szSpans << attr(_T("x14ac:dyDescent")) << 0.25;
    if (height != 0) {
		(*m_xmlStream) << attr(_T("ht")) << height << attr(_T("customHeight")) << 1;
    }

    m_current_column = 0;
	for (size_t i = 0; i < data.size(); i++) {
		AddCellRoutine(data[i].value, data[i].style_id);
	}

    (*m_xmlStream) << endtag(); // row

    m_offset_column = 0;
}

// ****************************************************************************
/// @brief  Internal initializatino method adds frozen pane`s information into sheet
/// @param  width frozen pane width (in number of cells)
/// @param  height frozen pane height (in number of cells)
/// @return no
// ****************************************************************************
void CWorksheet::AddFrozenPane(uint32_t width, uint32_t height)
{
    (*m_xmlStream) << tag(_T("pane"));

    if (width != 0) {
        (*m_xmlStream) << attr(_T("xSplit")) << width;
    }

    if (height != 0) {
        (*m_xmlStream) << attr(_T("ySplit")) << height;
    }

    TCHAR szCoord[15] = { 0 };
    GetCellCoord(CellCoord(height + 1, width), szCoord);

    (*m_xmlStream) << attr(_T("topLeftCell")) << szCoord;

    if (width != 0 && height != 0) {
        (*m_xmlStream) << attr(_T("activePane")) << _T("bottomRight");
    }
    else if (width == 0 && height != 0) {
        (*m_xmlStream) << attr(_T("activePane")) << _T("bottomLeft");
    }
    else if (width != 0 && height == 0) {
        (*m_xmlStream) << attr(_T("activePane")) << _T("topRight");
    }

    (*m_xmlStream) << attr(_T("state")) << _T("frozen")
    << endtag();

    if (width > 0 && height > 0) {
        TCHAR szCoordBL[15] = { 0 };
        GetCellCoord(CellCoord(height + 1, width - 1), szCoordBL);
        TCHAR szCoordTR[15] = { 0 };
        GetCellCoord(CellCoord(height, width), szCoordTR);

        (*m_xmlStream)
        << tag(_T("selection")) << attr(_T("pane")) << _T("topRight") << attr(_T("activeCell")) << szCoordTR << attr(_T("sqref")) << szCoordTR << endtag()
        << tag(_T("selection")) << attr(_T("pane")) << _T("bottomLeft") << attr(_T("activeCell")) << szCoordBL << attr(_T("sqref")) << szCoordBL << endtag()
        << tag(_T("selection")) << attr(_T("pane")) << _T("bottomRight") << attr(_T("activeCell")) << szCoord << attr(_T("sqref")) << szCoord << endtag();
    }
    else if (width > 0) {
        (*m_xmlStream)
        << tag(_T("selection")) << attr(_T("pane")) << _T("topRight") << attr(_T("activeCell")) << szCoord << attr(_T("sqref")) << szCoord << endtag();
    }
    else if (height > 0) {
        (*m_xmlStream)
        << tag(_T("selection")) << attr(_T("pane")) << _T("bottomLeft") << attr(_T("activeCell")) << szCoord << attr(_T("sqref")) << szCoord << endtag();
    }
}

// ****************************************************************************
/// @brief  Appends merged cells range into the sheet
/// @param  cellFrom (row value from 1, col value from 0)
/// @param  cellTo (row value from 1, col value from 0)
/// @return no
// ****************************************************************************
void CWorksheet::MergeCells(CellCoord cellFrom, CellCoord cellTo)
{
	if (cellFrom.row == 0 || cellTo.row == 0) return;

	TCHAR szCellFrom[15] = { 0 };
	TCHAR szCellTo[15] = { 0 };

	GetCellCoord(cellFrom, szCellFrom);
	GetCellCoord(cellTo, szCellTo);

	TCHAR szRange[30] = { 0 };
	_stprintf(szRange, _T("%s:%s"), szCellFrom, szCellTo);

	m_mergedCells.push_back(szRange);
}

void CWorksheet::filterCells(CellCoord cellFrom, CellCoord cellTo)
{
	if (cellFrom.row == 0 || cellTo.row == 0) return;
	if (cellFrom.row - cellTo.row != 0)return;

	TCHAR szCellFrom[15] = { 0 };
	TCHAR szCellTo[15] = { 0 };

	GetCellCoord(cellFrom, szCellFrom);
	GetCellCoord(cellTo, szCellTo);

	TCHAR szRange[30] = { 0 };
	_stprintf(szRange, _T("%s:%s"), szCellFrom, szCellTo);

	m_filterCells.push_back(szRange);
}

// ****************************************************************************
///	@brief	Receives next to write cell`s coordinates
/// @param	currCell (row value from 1, col value from 0)
/// @return	no
// ****************************************************************************
void CWorksheet::GetCurrentCellCoord(CellCoord& currCell)
{
	currCell.row = m_row_index;
	currCell.col = m_current_column;
}

// ****************************************************************************
/// @brief  Transforms the row and the column numerics from int32_t to coordinate std::string format
/// @param  cell (row value from 1, col value from 0)
/// @param  szCoord output parameter to be filled (pointer must be valid)
/// @return no
// ****************************************************************************
void CWorksheet::GetCellCoord(CellCoord cell, TCHAR *szCoord)
{
    const int32_t iAlphLen = 26;
    const TCHAR *szAlph = _T("ABCDEFGHIJKLMNOPQRSTUVWXYZ");

    _tstring strCol;
    strCol.append(1, szAlph[cell.col % iAlphLen]);  // last
    int32_t div = cell.col / iAlphLen;
    while (true) {
        if (div == 0 ) break;

        div--;
        strCol.insert(strCol.begin(), szAlph[div % iAlphLen]);
        div /= iAlphLen;
    };

    _stprintf(szCoord, _T("%s%d"), strCol.c_str(), cell.row);
}

// ****************************************************************************
/// @brief  Saves current xml document into a file with preset name
/// @return Boolean result of the operation
// ****************************************************************************
bool CWorksheet::Save()
{
    (*m_xmlStream) << endtag(); // close sheetData tag

	//<autoFilter ref="A1:D1"/>
	for (size_t i = 0; i < m_filterCells.size(); i++) {
		(*m_xmlStream) << tag(_T("autoFilter")) << attr(_T("ref")) << m_filterCells[i].c_str() << endtag();
	}

	if (!m_mergedCells.empty()) {
        (*m_xmlStream) << tag(_T("mergeCells")) << attr(_T("count")) << m_mergedCells.size();
	}

	for (size_t i = 0; i < m_mergedCells.size(); i++) {
		(*m_xmlStream) << tag(_T("mergeCell")) << attr(_T("ref")) << m_mergedCells[i].c_str() << endtag();
	}

	if (!m_mergedCells.empty()) {
        (*m_xmlStream) << endtag();
	}

	_tstring sOrient;
	if (m_page_orientation == PAGE_PORTRAIT) {
		sOrient = _T("portrait");
	}
	else if (m_page_orientation == PAGE_LANDSCAPE) {
		sOrient = _T("landscape");
	}

	(*m_xmlStream)
        << tag(_T("pageMargins"))
            << attr(_T("left")) << 0.7
            << attr(_T("right")) << 0.7
            << attr(_T("top")) << 0.75
            << attr(_T("bottom")) << 0.75
            << attr(_T("header")) << 0.3
            << attr(_T("footer")) << 0.3
        << endtag()
        << tag(_T("pageSetup"))
			<< attr(_T("paperSize")) << 9	// A4 paper size
			<< attr(_T("orientation")) << sOrient.c_str()
		<< endtag();

	if (m_withComments) {
		(*m_xmlStream)
		<< tag(_T("legacyDrawing"))
			<< attr(_T("r:id")) << _T("rId1")
		<< endtag();

		if (SaveCommentsRels() == false) return false;
	}

    delete m_xmlStream; // by deleting the stream the end of file writes and closes the stream
    m_xmlStream = NULL;

    m_f->close();
    delete m_f;
    m_f = NULL;

    m_isOk = false;
    return true;
}

// ****************************************************************************
/// @brief  Saves current sheet relations file
/// @return no
// ****************************************************************************
bool CWorksheet::SaveCommentsRels()
{
	// [- zip/xl/worksheets/_rels/sheetN.xml.rels
	TCHAR szPathToFile[MAX_PATH] = { 0 };
	_stprintf(szPathToFile, _T("%s/xl/worksheets/_rels/sheet%d.xml.rels"), m_temp_path.c_str(), m_index);
	MakeDirectory(szPathToFile);
	char szPath[MAX_PATH] = { 0 };
#ifdef UNICODE
	int32_t res = wcstombs(szPath, szPathToFile, MAX_PATH);
	if (res == -1) return false;
#else
	_stprintf(szPath, _T("%s"), szPathToFile);
#endif
	_tofstream f(szPath);
	XmlStream xml_stream(f);

	TCHAR szVml[MAX_PATH] = { 0 };
	_stprintf(szVml, _T("../drawings/vmlDrawing%d.vml"), m_index);

	TCHAR szComments[MAX_PATH] = { 0 };
	_stprintf(szComments, _T("../comments%d.xml"), m_index);

	xml_stream << prolog()
	<< tag(_T("Relationships")) << attr(_T("xmlns")) << ns_relationships
		<< tag(_T("Relationship")) << attr(_T("Id")) << _T("rId1") << attr(_T("Type")) << type_vml << attr(_T("Target")) << szVml
		<< endtag()
		<< tag(_T("Relationship")) << attr(_T("Id")) << _T("rId2") << attr(_T("Type")) << type_comments << attr(_T("Target")) << szComments
		<< endtag()
	<< endtag();
	// zip/xl/worksheets/_rels/sheetN.xml.rels -]

	return true;
}

}	// namespace SimpleXlsx
















