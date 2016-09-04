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

#include "SimpleXlsxDef.h"
#include "Chartsheet.h"
#include "Worksheet.h"
#include "XlsxHeaders.h"
#include "../xmlwriter.h"

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

namespace SimpleXlsx {

using namespace std;
using namespace xmlw;

extern bool MakeDirectory(const _tstring& dirName);

// ****************************************************************************
/// @brief  The class default constructor
/// @return no
// ****************************************************************************
CChartsheet::CChartsheet()
{
    Init(1, CHART_LINEAR);
}

// ****************************************************************************
/// @brief  The class constructor
/// @param  index index of a sheet to be created (for example, chart1.xml)
/// @param	temp_path path to the temporary directory where chart files must be created
/// @return no
// ****************************************************************************
CChartsheet::CChartsheet(uint32_t index, const _tstring& temp_path) : m_temp_path(temp_path)
{
    Init(index, CHART_LINEAR);
}

// ****************************************************************************
/// @brief  The class constructor
/// @param  index index of a sheet to be created (for example, chart1.xml)
/// @param  type chart type (BAR or LINEAR)
/// @param	temp_path path to the temporary directory where chart files must be created
/// @return no
// ****************************************************************************
CChartsheet::CChartsheet(uint32_t index, EChartTypes type, const _tstring& temp_path) : m_temp_path(temp_path)
{
    Init(index, type);
}

// ****************************************************************************
/// @brief  The class destructor (virtual)
/// @return no
// ****************************************************************************
CChartsheet::~CChartsheet()
{
    if (m_xmlStream) {
        delete m_xmlStream;
        m_xmlStream = NULL;
    }

    if (m_f) {
        m_f->close();
        delete m_f;
    }
}

// ****************************************************************************
/// @brief  Initializes internal variables and creates the core structure of a sheet tree
/// @param  index index of a sheet to be created (for example, chart1.xml)
/// @param  type chart type (BAR or LINEAR)
/// @return no
// ****************************************************************************
void CChartsheet::Init(uint32_t index, EChartTypes type)
{
    m_isOk = true;
    m_index = index;
    m_title = _T("Diagramm 1");

    m_seriesSet.clear();

    m_diagramm.typeMain = type;

    m_xAxis.id = 100;
    m_yAxis.id = 101;
    m_x2Axis.id = 102;
    m_y2Axis.id = 103;

    m_xAxis.pos = POS_BOTTOM;
    m_xAxis.sourceLinked = false;
    m_yAxis.pos = POS_LEFT;
    m_yAxis.sourceLinked = true;
    m_x2Axis.pos = POS_TOP;
    m_x2Axis.sourceLinked = false;
    m_x2Axis.cross = CROSS_MAX;
    m_y2Axis.pos = POS_RIGHT;
    m_y2Axis.sourceLinked = true;
    m_y2Axis.cross = CROSS_MAX;

    TCHAR szPathToFile[MAX_PATH] = { 0 };
    _stprintf(szPathToFile, _T("%s/xl/charts/chart%d.xml"), m_temp_path.c_str(), m_index);
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

    m_xmlStream = new (std::nothrow) XmlStream(*m_f);
    if (m_xmlStream == NULL) {
        m_isOk = false;
        return;
    }
}

// ****************************************************************************
/// @brief  This internal method adds title section into xml object
/// @param  name    Title value
/// @param  size    Font size
/// @param  vertPos indicates whether the title is allocated on X axis (false) or vertical (true)
/// @return no
// ****************************************************************************
void CChartsheet::AddTitle(const TCHAR* name, uint32_t size, bool vertPos)
{
    (*m_xmlStream)
    << tag(_T("c:title"))
        << tag(_T("c:tx"))
            << tag(_T("c:rich"))
                << tag(_T("a:bodyPr"));

            if (vertPos) {
                (*m_xmlStream) << attr(_T("rot")) << -5400000 << attr(_T("vert")) << _T("horz");
            }
                (*m_xmlStream)
                << endtag()
                << tag(_T("a:lstStyle")) << endtag()
                << tag(_T("a:p"))
                    << tag(_T("a:pPr"))
                        << tag(_T("a:defRPr")) << attr(_T("lang")) << _T("en-US") << attr(_T("sz")) << size*100 << endtag()
                    << endtag()
                    << tag(_T("a:r"))
                        << tag(_T("a:rPr")) << attr(_T("lang")) << _T("en-US") << attr(_T("sz")) << size*100 << endtag()
                        << tag(_T("a:t")) << chardata() << name << endtag()
                    << endtag()
                    << tag(_T("a:endParaRPr")) << attr(_T("lang")) << _T("en-US") << endtag()
                << endtag()
            << endtag()
        << endtag()
        << tag(_T("c:layout")) << endtag()
        << tag(_T("c:overlay")) << attr(_T("val")) << 0 << endtag()
    << endtag();
}

// ****************************************************************************
/// @brief  Adds table data into chart if it is necessary
/// @return no
// ****************************************************************************
void CChartsheet::AddTableData()
{
    bool showKeys = false;

    switch(m_diagramm.tableData) {
        case TBL_DATA_NONE:     return;
        case TBL_DATA:          break;
        case TBL_DATA_N_KEYS:   showKeys = true; break;
        default:                break;
    }

    (*m_xmlStream)
    << tag(_T("c:dTable"))
        << tag(_T("c:showHorzBorder")) << attr(_T("val")) << 1 << endtag()
        << tag(_T("c:showVertBorder")) << attr(_T("val")) << 1 << endtag()
        << tag(_T("c:showOutline")) << attr(_T("val")) << 1 << endtag();

    if (showKeys) {
        (*m_xmlStream) << tag(_T("c:showKeys")) << attr(_T("val")) << 1 << endtag();
    }

    (*m_xmlStream) << endtag();
}

// ****************************************************************************
/// @brief  Adds legend into chart if it is necessary
/// @return no
// ****************************************************************************
void CChartsheet::AddLegend()
{
    int overlay = 1;
    TCHAR pos = _T('\0');

    switch(m_diagramm.legend_pos) {
        case POS_NONE:  return;

        case POS_LEFT_ASIDE:overlay = 0;
        case POS_LEFT:      pos = _T('l'); break;

        case POS_RIGHT_ASIDE:overlay = 0;
        case POS_RIGHT:     pos = _T('r'); break;

        case POS_TOP_ASIDE:overlay = 0;
        case POS_TOP:       pos = _T('t'); break;

        case POS_BOTTOM_ASIDE:overlay = 0;
        case POS_BOTTOM:    pos = _T('b'); break;
    }

    (*m_xmlStream)
    << tag(_T("c:legend"))
        << tag(_T("c:legendPos")) << attr(_T("val")) << pos << endtag()
        << tag(_T("c:layout")) << endtag()
        << tag(_T("c:overlay")) << attr(_T("val")) << overlay << endtag()
    << endtag();
}

// ****************************************************************************
/// @brief  Internal method adds X axis block into chart
/// @param  x reference to an axis object
/// @param  crossAxisId id of axis that is used in pair with setting
/// @return no
// ****************************************************************************
void CChartsheet::AddXAxis(const Axis& x, uint32_t crossAxisId)
{
    TCHAR xpos = _T('b');
    switch(x.pos) {
        case POS_LEFT:      xpos = _T('l'); break;
        case POS_RIGHT:     xpos = _T('r'); break;
        case POS_TOP:       xpos = _T('t'); break;
        case POS_BOTTOM:    xpos = _T('b'); break;
        default:        break;
    }

    (*m_xmlStream)
    << tag(_T("c:catAx"))
        << tag(_T("c:axId")) << attr(_T("val")) << x.id << endtag()
        << tag(_T("c:scaling"))
            << tag(_T("c:orientation")) << attr(_T("val")) << _T("minMax") << endtag();

        if (x.minValue != _T("")) {
            (*m_xmlStream)
            << tag(_T("c:min")) << attr(_T("val")) << x.minValue.c_str() << endtag();
        }

        if (x.maxValue != _T("")) {
            (*m_xmlStream)
            << tag(_T("c:max")) << attr(_T("val")) << x.maxValue.c_str() << endtag();
        }

    (*m_xmlStream)
        << endtag();

    if (x.pos == POS_NONE) {
        (*m_xmlStream)
        << tag(_T("c:delete")) << attr(_T("val")) << 1 << endtag();
    }
    else {
        (*m_xmlStream)
        << tag(_T("c:delete")) << attr(_T("val")) << 0 << endtag();
    }

    (*m_xmlStream)
        << tag(_T("c:axPos")) << attr(_T("val")) << xpos << endtag();

    if (x.gridLines == GRID_MAJOR) {
        (*m_xmlStream)
        << tag(_T("c:majorGridlines")) << endtag();
    }
    else if (x.gridLines == GRID_MINOR) {
        (*m_xmlStream)
        << tag(_T("c:minorGridlines")) << endtag();
    }
    else if (x.gridLines == GRID_MAJOR_N_MINOR) {
        (*m_xmlStream)
        << tag(_T("c:majorGridlines")) << endtag()
        << tag(_T("c:minorGridlines")) << endtag();
    }

    if (x.name != _T(""))
        AddTitle(x.name.c_str(), x.nameSize, false);

    if (x.sourceLinked) {
        (*m_xmlStream)
        << tag(_T("c:numFmt")) << attr(_T("formatCode")) << _T("General") << attr(_T("sourceLinked")) << 1 << endtag();
    }

    (*m_xmlStream)
        << tag(_T("c:tickLblPos")) << attr(_T("val")) << _T("nextTo") << endtag();

	if (x.lblAngle != -1) {
		(*m_xmlStream)
        << tag(_T("c:txPr"))
			<< tag(_T("a:bodyPr")) << attr(_T("rot")) << x.lblAngle*60000 << endtag()
			<< tag(_T("a:lstStyle")) << endtag()
			<< tag(_T("a:p"))
				<< tag(_T("a:pPr"))
					<< tag(_T("a:defRPr")) << endtag()
				<< endtag()
				<< tag(_T("a:endParaRPr")) << attr(_T("lang")) << _T("en-US") << endtag()
			<< endtag()
		<< endtag();
	}

    if (crossAxisId != 0) {
        (*m_xmlStream)
        << tag(_T("c:crossAx")) << attr(_T("val")) << crossAxisId << endtag()
        << tag(_T("c:crosses"));

        if (x.cross == CROSS_AUTO_ZERO) {
            (*m_xmlStream)
            << attr(_T("val")) << _T("autoZero") << endtag();
        }
        else if (x.cross == CROSS_MIN) {
            (*m_xmlStream)
            << attr(_T("val")) << _T("min") << endtag();
        }
        else if (x.cross == CROSS_MAX) {
            (*m_xmlStream)
            << attr(_T("val")) << _T("max") << endtag();
        }
    }

    (*m_xmlStream)
        << tag(_T("c:auto")) << attr(_T("val")) << 1 << endtag()
        << tag(_T("c:lblAlgn")) << attr(_T("val")) << _T("ctr") << endtag()
        << tag(_T("c:lblOffset")) << attr(_T("val")) << 100 << endtag();

	if (x.lblSkipInterval != -1) {
		(*m_xmlStream)
		<< tag(_T("c:tickLblSkip")) << attr(_T("val")) << x.lblSkipInterval << endtag();
	}
	if (x.markSkipInterval != -1) {
		(*m_xmlStream)
		<< tag(_T("c:tickMarkSkip")) << attr(_T("val")) << x.markSkipInterval << endtag();
	}

	(*m_xmlStream)
    << endtag(); // catAx
}

// ****************************************************************************
/// @brief  Internal method adds Y axis block into chart
/// @param  y reference to an axis object
/// @param  crossAxisId id of axis that is used in pair with setting
/// @return no
// ****************************************************************************
void CChartsheet::AddYAxis(const Axis& y, uint32_t crossAxisId)
{
    TCHAR ypos = _T('l');
    switch(y.pos) {
        case POS_LEFT:      ypos = _T('l'); break;
        case POS_RIGHT:     ypos = _T('r'); break;
        case POS_TOP:       ypos = _T('t'); break;
        case POS_BOTTOM:    ypos = _T('b'); break;
        default:        break;
    }

    (*m_xmlStream)
    << tag(_T("a:valAx"))
        << tag(_T("c:axId")) << attr(_T("val")) << y.id << endtag()  // x axis
        << tag(_T("c:scaling"))
            << tag(_T("c:orientation")) << attr(_T("val")) << _T("minMax") << endtag();

        if (y.minValue != _T("")) {
            (*m_xmlStream)
            << tag(_T("c:min")) << attr(_T("val")) << y.minValue.c_str() << endtag();
        }

        if (y.maxValue != _T("")) {
            (*m_xmlStream)
            << tag(_T("c:max")) << attr(_T("val")) << y.maxValue.c_str() << endtag();
        }

    (*m_xmlStream)
        << endtag();

    if (y.pos == POS_NONE) {
        (*m_xmlStream)
        << tag(_T("c:delete")) << attr(_T("val")) << 1 << endtag();
    }
    else {
        (*m_xmlStream)
        << tag(_T("c:delete")) << attr(_T("val")) << 0 << endtag();
    }

    (*m_xmlStream)
        << tag(_T("c:axPos")) << attr(_T("val")) << ypos << endtag();

    if (y.gridLines == GRID_MAJOR) {
        (*m_xmlStream)
        << tag(_T("c:majorGridlines")) << endtag();
    }
    else if (y.gridLines == GRID_MINOR) {
        (*m_xmlStream)
        << tag(_T("c:minorGridlines")) << endtag();
    }
    else if (y.gridLines == GRID_MAJOR_N_MINOR) {
        (*m_xmlStream)
        << tag(_T("c:majorGridlines")) << endtag()
        << tag(_T("c:minorGridlines")) << endtag();
    }

    if (y.name != _T(""))
        AddTitle(y.name.c_str(), y.nameSize, true);

    if (y.sourceLinked) {
        (*m_xmlStream)
        << tag(_T("c:numFmt")) << attr(_T("formatCode")) << _T("General") << attr(_T("sourceLinked")) << 1 << endtag();
    }

    (*m_xmlStream)
        << tag(_T("c:tickLblPos")) << attr(_T("val")) << _T("nextTo") << endtag();

    if (crossAxisId != 0) {
        (*m_xmlStream)
        << tag(_T("c:crossAx")) << attr(_T("val")) << crossAxisId << endtag()
        << tag(_T("c:crosses"));

        if (y.cross == CROSS_AUTO_ZERO) {
            (*m_xmlStream)
            << attr(_T("val")) << _T("autoZero") << endtag();
        }
        else if (y.cross == CROSS_MIN) {
            (*m_xmlStream)
            << attr(_T("val")) << _T("min") << endtag();
        }
        else if (y.cross == CROSS_MAX) {
            (*m_xmlStream)
            << attr(_T("val")) << _T("max") << endtag();
        }

        (*m_xmlStream)
        << tag(_T("c:crossBetween")) << attr(_T("val")) << _T("between") << endtag();
    }

    (*m_xmlStream)
    << endtag(); // valAx
}

// ****************************************************************************
/// @brief  Adds line chart xml section
/// @param  xAxis           reference to axis object
/// @param  yAxisId         id of using y axis
/// @param  series          reference to the vector of line sets
/// @param  firstSeriesId   is used for synchronization between different line charts
/// @return no
// ****************************************************************************
void CChartsheet::AddLineChart(Axis& xAxis, uint32_t yAxisId, const std::vector<Series>& series, uint32_t firstSeriesId)
{
    (*m_xmlStream)
    << tag(_T("c:lineChart"))
        << tag(_T("c:grouping")) << attr(_T("val")) << _T("standard") << endtag();

    for (size_t i = 0; i < series.size(); i++) {
        (*m_xmlStream)
        << tag(_T("c:ser"))
        << tag(_T("c:idx")) << attr(_T("val")) << firstSeriesId << endtag()
        << tag(_T("c:order")) << attr(_T("val")) << firstSeriesId << endtag();

        TCHAR szMarker[20] = { 0 };
        if (series[i].isMarked == false)_stprintf(szMarker, _T("none"));
        else                            _stprintf(szMarker, _T("diamond"));

        (*m_xmlStream)
        << tag(_T("c:marker"))
            << tag(_T("c:symbol")) << attr(_T("val")) << szMarker << endtag()
        << endtag();

        if (series[i].title != _T("")) {
            (*m_xmlStream)
            << tag(_T("c:tx")) << tag(_T("c:v")) << chardata() << series[i].title.c_str() << endtag() << endtag();
        }


        if (series[i].isDashed) {
        	(*m_xmlStream)
            << tag(_T("c:spPr")) << tag(_T("a:ln"))
				<< tag(_T("a:prstDash")) << attr(_T("val")) << _T("dash") << endtag()
			<< endtag() << endtag();
        }


        TCHAR szRange[100] = { 0 };
        TCHAR szCellFrom[15] = { 0 };
        TCHAR szCellTo[15] = { 0 };

        if (series[i].catSheet != NULL && (series[i].catAxisTo.row != 0 || series[i].catAxisTo.col != 0)) {
            xAxis.sourceLinked = true;

            CWorksheet::GetCellCoord(CellCoord(series[i].catAxisFrom.row + 1, series[i].catAxisFrom.col), szCellFrom);
            CWorksheet::GetCellCoord(CellCoord(series[i].catAxisTo.row + 1, series[i].catAxisTo.col), szCellTo);
            _stprintf(szRange, _T("'%s'!$%s:$%s"), series[i].catSheet->GetTitle().c_str(), szCellFrom, szCellTo);

            (*m_xmlStream)
            << tag(_T("c:cat"))
                << tag(_T("c:numRef"))
                    << tag(_T("c:f")) << chardata() << szRange << endtag()
                << endtag()
            << endtag();
        }

        CWorksheet::GetCellCoord(CellCoord(series[i].valAxisFrom.row + 1, series[i].valAxisFrom.col), szCellFrom);
        CWorksheet::GetCellCoord(CellCoord(series[i].valAxisTo.row + 1, series[i].valAxisTo.col), szCellTo);
        _stprintf(szRange, _T("'%s'!$%s:$%s"), series[i].valSheet->GetTitle().c_str(), szCellFrom, szCellTo);

        (*m_xmlStream)
            << tag(_T("c:val"))
                << tag(_T("c:numRef"))
                    << tag(_T("c:f")) << chardata() << szRange << endtag()
                << endtag()
            << endtag()
        << tag(_T("c:smooth")) << attr(_T("val")) << (series[i].isSmoothed ? 1 : 0) << endtag()
        << endtag();

        firstSeriesId++;
    }

    (*m_xmlStream)
        << tag(_T("c:marker")) << attr(_T("val")) << 1 << endtag()
        << tag(_T("c:smooth")) << attr(_T("val")) << 0 << endtag()
        << tag(_T("c:axId")) << attr(_T("val")) << xAxis.id << endtag()
        << tag(_T("c:axId")) << attr(_T("val")) << yAxisId << endtag()
    << endtag();
}

// ****************************************************************************
/// @brief  Adds bar chart xml section
/// @param  xAxis           reference to axis object
/// @param  yAxisId         id of using y axis
/// @param  series          reference to the vector of line sets
/// @param  firstSeriesId   is used for synchronization between different line charts
/// @param	barDir			bars` direction (horizontal or vertical)
/// @param	barGroup		bars` relative position
/// @see	EBarDirection
/// @see	EBarGrouping
/// @return no
// ****************************************************************************
void CChartsheet::AddBarChart(Axis& xAxis, uint32_t yAxisId, const std::vector<Series>& series, uint32_t firstSeriesId, EBarDirection barDir, EBarGrouping barGroup)
{
	(*m_xmlStream)
    << tag(_T("c:barChart"));

    if (barDir == BAR_DIR_VERTICAL) {
		(*m_xmlStream)
		<< tag(_T("c:barDir")) << attr(_T("val")) << _T("col") << endtag();
    }
    else if (barDir == BAR_DIR_HORIZONTAL) {
        (*m_xmlStream)
        << tag(_T("c:barDir")) << attr(_T("val")) << _T("bar") << endtag();
    }

    if (barGroup == BAR_GROUP_CLUSTERED) {
		(*m_xmlStream)
		<< tag(_T("c:grouping")) << attr(_T("val")) << _T("clustered") << endtag();
    }
    else if (barGroup == BAR_GROUP_STACKED) {
		(*m_xmlStream)
		<< tag(_T("c:grouping")) << attr(_T("val")) << _T("stacked") << endtag();
    }
    else if (barGroup == BAR_GROUP_PERCENT_STACKED) {
		(*m_xmlStream)
		<< tag(_T("c:grouping")) << attr(_T("val")) << _T("percentStacked") << endtag();
    }

    for (size_t i = 0; i < series.size(); i++) {
    	const Series &ser = series[i];

        (*m_xmlStream)
        << tag(_T("c:ser"))
        << tag(_T("c:idx")) << attr(_T("val")) << firstSeriesId << endtag()
        << tag(_T("c:order")) << attr(_T("val")) << firstSeriesId << endtag();

        if (ser.title != _T("")) {
            (*m_xmlStream)
            << tag(_T("c:tx")) << tag(_T("c:v")) << chardata() << ser.title.c_str() << endtag() << endtag();
        }

        TCHAR szRange[100] = { 0 };
        TCHAR szCellFrom[15] = { 0 };
        TCHAR szCellTo[15] = { 0 };

        if (ser.catSheet != NULL && (ser.catAxisTo.row != 0 || ser.catAxisTo.col != 0)) {
            xAxis.sourceLinked = true;

            CWorksheet::GetCellCoord(CellCoord(ser.catAxisFrom.row + 1, ser.catAxisFrom.col), szCellFrom);
            CWorksheet::GetCellCoord(CellCoord(ser.catAxisTo.row + 1, ser.catAxisTo.col), szCellTo);
            _stprintf(szRange, _T("'%s'!$%s:$%s"), ser.catSheet->GetTitle().c_str(), szCellFrom, szCellTo);

            (*m_xmlStream)
            << tag(_T("c:cat"))
                << tag(_T("c:numRef"))
                    << tag(_T("c:f")) << chardata() << szRange << endtag()
                << endtag()
            << endtag();
        }

        CWorksheet::GetCellCoord(CellCoord(ser.valAxisFrom.row + 1, ser.valAxisFrom.col), szCellFrom);
        CWorksheet::GetCellCoord(CellCoord(ser.valAxisTo.row + 1, ser.valAxisTo.col), szCellTo);
        _stprintf(szRange, _T("'%s'!$%s:$%s"), ser.valSheet->GetTitle().c_str(), szCellFrom, szCellTo);

        (*m_xmlStream)
            << tag(_T("c:val"))
                << tag(_T("c:numRef"))
                    << tag(_T("c:f")) << chardata() << szRange << endtag()
                << endtag()
            << endtag()
        << tag(_T("c:smooth")) << attr(_T("val")) << (ser.isSmoothed ? 1 : 0) << endtag()
        << endtag();

        firstSeriesId++;
    }

    (*m_xmlStream)
        << tag(_T("c:marker")) << attr(_T("val")) << 1 << endtag()
        << tag(_T("c:smooth")) << attr(_T("val")) << 0 << endtag()
        << tag(_T("c:axId")) << attr(_T("val")) << xAxis.id << endtag()
        << tag(_T("c:axId")) << attr(_T("val")) << yAxisId << endtag()
    << endtag();
}

void CChartsheet::AddPieChart(Axis& xAxis, uint32_t yAxisId, const std::vector<Series>& series, uint32_t firstSeriesId)
{
	(*m_xmlStream)
		<< tag(_T("c:pieChart"));

		(*m_xmlStream)
			<< tag(_T("c:varyColors")) << attr(_T("val")) << 1 << endtag();


	for (size_t i = 0; i < series.size(); i++) {
		const Series &ser = series[i];

		(*m_xmlStream)
			<< tag(_T("c:ser"))
			<< tag(_T("c:idx")) << attr(_T("val")) << firstSeriesId << endtag()
			<< tag(_T("c:order")) << attr(_T("val")) << firstSeriesId << endtag();

		if (ser.title != _T("")) {
			(*m_xmlStream)
				<< tag(_T("c:tx")) << tag(_T("c:v")) << chardata() << ser.title.c_str() << endtag() << endtag();
		}

		TCHAR szRange[100] = { 0 };
		TCHAR szCellFrom[15] = { 0 };
		TCHAR szCellTo[15] = { 0 };

		if (ser.catSheet != NULL && (ser.catAxisTo.row != 0 || ser.catAxisTo.col != 0)) {
			xAxis.sourceLinked = true;

			CWorksheet::GetCellCoord(CellCoord(ser.catAxisFrom.row + 1, ser.catAxisFrom.col), szCellFrom);
			CWorksheet::GetCellCoord(CellCoord(ser.catAxisTo.row + 1, ser.catAxisTo.col), szCellTo);
			_stprintf(szRange, _T("'%s'!$%s:$%s"), ser.catSheet->GetTitle().c_str(), szCellFrom, szCellTo);

			(*m_xmlStream)
				<< tag(_T("c:cat"))
				<< tag(_T("c:strRef"))
				<< tag(_T("c:f")) << chardata() << szRange << endtag()
				<< endtag()
				<< endtag();
		}

		CWorksheet::GetCellCoord(CellCoord(ser.valAxisFrom.row + 1, ser.valAxisFrom.col), szCellFrom);
		CWorksheet::GetCellCoord(CellCoord(ser.valAxisTo.row + 1, ser.valAxisTo.col), szCellTo);
		_stprintf(szRange, _T("'%s'!$%s:$%s"), ser.valSheet->GetTitle().c_str(), szCellFrom, szCellTo);

		(*m_xmlStream)
			<< tag(_T("c:val"))
			<< tag(_T("c:numRef"))
			<< tag(_T("c:f")) << chardata() << szRange << endtag()
			<< endtag()
			<< endtag();

		firstSeriesId++;
	}

	(*m_xmlStream)
		<< tag(_T("c:dLbls"))
		<< tag(_T("c:showVal")) << attr(_T("val")) << 0 << endtag()
		<< tag(_T("c:showCatName")) << attr(_T("val")) << 1 << endtag()
		<< tag(_T("c:showSerName")) << attr(_T("val")) << 0 << endtag()
		<< tag(_T("c:showPercent")) << attr(_T("val")) << 1 << endtag()
		<< tag(_T("c:showBubbleSize")) << attr(_T("val")) << 0 << endtag()
		<< tag(_T("c:showLeaderLines")) << attr(_T("val")) << 1 << endtag()
		<< endtag()
		<< endtag();
}

// ****************************************************************************
/// @brief  Adds scatter chart xml section
/// @param  xAxisId         id of using x axis
/// @param  yAxisId         id of using y axis
/// @param  series          reference to the vector of line sets
/// @param  firstSeriesId   is used for synchronization between different line charts
/// @param	style			determines diagramm style
/// @see	EScatterStyle
/// @return no
// ****************************************************************************
void CChartsheet::AddScatterChart(uint32_t xAxisId, uint32_t yAxisId, const std::vector<Series>& series, uint32_t firstSeriesId, EScatterStyle style)
{
	(*m_xmlStream)
    << tag(_T("c:scatterChart"));

    if (style == SCATTER_FILL) {
		(*m_xmlStream)
		<< tag(_T("c:scatterStyle")) << attr(_T("val")) << _T("smoothMarker") << endtag();
    }
    else if (style == SCATTER_POINT) {
		(*m_xmlStream)
		<< tag(_T("c:scatterStyle")) << attr(_T("val")) << _T("lineMarker") << endtag();
    }

    for (size_t i = 0; i < series.size(); i++) {
        (*m_xmlStream)
        << tag(_T("c:ser"))
        << tag(_T("c:idx")) << attr(_T("val")) << firstSeriesId << endtag()
        << tag(_T("c:order")) << attr(_T("val")) << firstSeriesId << endtag();

        TCHAR szMarker[20] = { 0 };
        if (series[i].isMarked == false && style != SCATTER_POINT)
			_stprintf(szMarker, _T("none"));
        else
			_stprintf(szMarker, _T("diamond"));

        (*m_xmlStream)
        << tag(_T("c:marker"))
            << tag(_T("c:symbol")) << attr(_T("val")) << szMarker << endtag()
        << endtag();

        if (series[i].title != _T("")) {
            (*m_xmlStream)
            << tag(_T("c:tx")) << tag(_T("c:v")) << chardata() << series[i].title.c_str() << endtag() << endtag();
        }

        if (style == SCATTER_POINT) {
			(*m_xmlStream)
			<< tag(_T("c:spPr"))
				<< tag(_T("a:ln")) << attr(_T("w")) << 28575
					<< tag(_T("a:noFill")) << endtag()
				<< endtag()
			<< endtag();
        }

        TCHAR szRange[100] = { 0 };
        TCHAR szCellFrom[15] = { 0 };
        TCHAR szCellTo[15] = { 0 };

		CWorksheet::GetCellCoord(CellCoord(series[i].catAxisFrom.row + 1, series[i].catAxisFrom.col), szCellFrom);
		CWorksheet::GetCellCoord(CellCoord(series[i].catAxisTo.row + 1, series[i].catAxisTo.col), szCellTo);
		_stprintf(szRange, _T("'%s'!$%s:$%s"), series[i].catSheet->GetTitle().c_str(), szCellFrom, szCellTo);

		(*m_xmlStream)
		<< tag(_T("c:xVal"))
			<< tag(_T("c:numRef"))
				<< tag(_T("c:f")) << chardata() << szRange << endtag()
			<< endtag()
		<< endtag();

        CWorksheet::GetCellCoord(CellCoord(series[i].valAxisFrom.row + 1, series[i].valAxisFrom.col), szCellFrom);
        CWorksheet::GetCellCoord(CellCoord(series[i].valAxisTo.row + 1, series[i].valAxisTo.col), szCellTo);
        _stprintf(szRange, _T("'%s'!$%s:$%s"), series[i].valSheet->GetTitle().c_str(), szCellFrom, szCellTo);

        (*m_xmlStream)
            << tag(_T("c:yVal"))
                << tag(_T("c:numRef"))
                    << tag(_T("c:f")) << chardata() << szRange << endtag()
                << endtag()
            << endtag()
        << tag(_T("c:smooth")) << attr(_T("val")) << (series[i].isSmoothed ? 1 : 0) << endtag()
        << endtag();

        firstSeriesId++;
    }

    (*m_xmlStream)
        << tag(_T("c:marker")) << attr(_T("val")) << 1 << endtag()
        << tag(_T("c:smooth")) << attr(_T("val")) << 0 << endtag()
        << tag(_T("c:axId")) << attr(_T("val")) << xAxisId << endtag()
        << tag(_T("c:axId")) << attr(_T("val")) << yAxisId << endtag()
    << endtag();
}

// ****************************************************************************
/// @brief  Adds specified series into chartsheet
/// @param  series reference to series object to be added
/// @param  mainChart indicates whether series must be added into main chart or additional (main by default)
/// @return Boolean result of the oepration
// ****************************************************************************
bool CChartsheet::AddSeries(const CChartsheet::Series& series, bool mainChart)
{
    if (series.valSheet == NULL || (series.valAxisTo.row == 0 && series.valAxisTo.col == 0)) {
        return false;
    }

    if (mainChart) {
    	if (m_diagramm.typeMain == CHART_SCATTER && (series.catSheet == NULL ||
			(series.catAxisTo.row == 0 && series.catAxisTo.col == 0))) {
			return false;
		}

        m_seriesSet.push_back(series);
    }
    else {
        if (m_diagramm.typeAdditional == CHART_SCATTER && (series.catSheet == NULL ||
			(series.catAxisTo.row == 0 && series.catAxisTo.col == 0))) {
			return false;
		}

        m_seriesSetAdd.push_back(series);
    }

    return true;
}

// ****************************************************************************
/// @brief  For each chart it creates and saves 5 files:
///         chartXX.xml,
///         sheetXX.xml, sheetXX.xml.rels,
///         drawingXX.xml, drawingXX.xml.rels
/// @return Boolean result of the operation
// ****************************************************************************
bool CChartsheet::Save()
{
    {
        // [- /xl/charts/chartX.xml
        if (m_seriesSet.empty()) return false;

        m_isOk = false; // whether saving succeed or not, the object is no more usable

        (*m_xmlStream) << prolog()
        << tag(_T("c:chartSpace")) << attr(_T("xmlns:c")) << ns_c << attr(_T("xmlns:a")) << ns_a << attr(_T("xmlns:r")) << ns_relationships
            << tag(_T("c:roundedCorners")) << attr(_T("val")) << 1 << endtag()
            << tag(_T("c:lang")) << attr(_T("val")) << _T("en-US") << endtag()
            << tag(_T("c:chart"));

        if (m_diagramm.name != _T(""))
            AddTitle(m_diagramm.name.c_str(), m_diagramm.nameSize, false);

        (*m_xmlStream)
                << tag(_T("c:plotArea"))
                    << tag(_T("c:layout")) << endtag();

        switch(m_diagramm.typeMain) {
            case CHART_LINEAR: {
                AddLineChart(m_xAxis, m_yAxis.id, m_seriesSet, 0);
                break;
            }
            case CHART_BAR: {
                AddBarChart(m_xAxis, m_yAxis.id, m_seriesSet, 0, m_diagramm.barDir, m_diagramm.barGroup);
                break;
            }
            case CHART_SCATTER: {
            	m_xAxis.sourceLinked = true;
				AddScatterChart(m_xAxis.id, m_yAxis.id, m_seriesSet, 0, m_diagramm.scatterStyle);
				break;
            }
			case CHART_PIE: {
				AddPieChart(m_xAxis, m_yAxis.id, m_seriesSet, 0);
				break;
			}
            case CHART_NONE:
            default:
                return false;
        }

        switch(m_diagramm.typeAdditional) {
            case CHART_LINEAR: {
                AddLineChart(m_x2Axis, m_y2Axis.id, m_seriesSetAdd, (int32_t)m_seriesSet.size());
                break;
            }
            case CHART_BAR: {
                AddBarChart(m_x2Axis, m_y2Axis.id, m_seriesSetAdd, (int32_t)m_seriesSet.size(), m_diagramm.barDir, m_diagramm.barGroup);
                break;
            }
            case CHART_SCATTER: {
            	m_xAxis.sourceLinked = true;
				AddScatterChart(m_xAxis.id, m_yAxis.id, m_seriesSet, (int32_t)m_seriesSet.size(), m_diagramm.scatterStyle);
				break;
            }
			case CHART_PIE: {
				AddPieChart(m_xAxis, m_yAxis.id, m_seriesSet, (int32_t)m_seriesSet.size());
				break;
			}
            case CHART_NONE:
            default:
                break;
        }

        switch(m_diagramm.typeMain) {
            case CHART_LINEAR:
            case CHART_BAR:
                AddXAxis(m_xAxis, m_yAxis.id);
				AddYAxis(m_yAxis, m_xAxis.id);
                break;
            case CHART_SCATTER:
			case CHART_PIE:
            	AddYAxis(m_xAxis, m_yAxis.id);
				AddYAxis(m_yAxis, m_xAxis.id);
				break;
            case CHART_NONE:
            default:
                return false;
        }


        if (!m_seriesSetAdd.empty()) {
            switch(m_diagramm.typeAdditional) {
				case CHART_LINEAR:
				case CHART_BAR:
					AddXAxis(m_x2Axis, m_y2Axis.id);
					AddYAxis(m_y2Axis, m_x2Axis.id);
					break;
				case CHART_SCATTER:
				case CHART_PIE:
					AddYAxis(m_x2Axis, m_y2Axis.id);
					AddYAxis(m_y2Axis, m_x2Axis.id);
					break;
				case CHART_NONE:
				default:
					return false;
			}
        }

        AddTableData();

        (*m_xmlStream)
                << endtag();// plotArea

        AddLegend();

        (*m_xmlStream)
            << tag(_T("c:plotVisOnly")) << attr(_T("val")) << 1 << endtag()
        << endtag(); // chart

        delete m_xmlStream; // by deleting the stream the end of file writes and closes the stream
        m_xmlStream = NULL;

        m_f->close();
        delete m_f;
        m_f = NULL;
        // /xl/charts/chartX.xml -]
    }

    {
        {
            // [- /xl/chartsheets/_rels/sheetX.xml.rels
            TCHAR szPathToFile[MAX_PATH] = { 0 };
            _stprintf(szPathToFile, _T("%s/xl/chartsheets/_rels/sheet%d.xml.rels"), m_temp_path.c_str(), m_index);
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

            TCHAR szTarget[MAX_PATH] = { 0 };
            _stprintf(szTarget, _T("../drawings/drawing%d.xml"), m_index);

            xml_stream << prolog()
            << tag(_T("Relationships")) << attr(_T("xmlns")) << ns_relationships
                << tag(_T("Relationship"))  << attr(_T("Id")) << _T("rId1")
                                        << attr(_T("Type")) << type_drawing
                                        << attr(_T("Target")) << szTarget
                << endtag();
            // /xl/chartsheets/_rels/sheetX.xml.rels -]
        }

        {
            // [- /xl/chartsheets/sheetX.xml
            TCHAR szPathToFile[MAX_PATH] = { 0 };
            _stprintf(szPathToFile, _T("%s/xl/chartsheets/sheet%d.xml"), m_temp_path.c_str(), m_index);
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

            xml_stream << prolog()
            << tag(_T("chartsheet")) << attr(_T("xmlns")) << ns_book << attr(_T("xmlns:r")) << ns_book_r
                << tag(_T("sheetPr")) << endtag()
                << tag(_T("sheetViews"))
                    << tag(_T("sheetView")) << attr(_T("zoomScale")) << 85 << attr(_T("workbookViewId")) << 0 << attr(_T("zoomToFit")) << 1
                    << endtag()
                << endtag()
                << tag(_T("pageMargins"))
                    << attr(_T("left")) << 0.7
                    << attr(_T("right")) << 0.7
                    << attr(_T("top")) << 0.75
                    << attr(_T("bottom")) << 0.75
                    << attr(_T("header")) << 0.3
                    << attr(_T("footer")) << 0.3
                << endtag()
                << tag(_T("drawing")) << attr(_T("r:id")) << _T("rId1");
            // /xl/chartsheets/sheetX.xml -]
        }
    }

    {
        {
            // [- /xl/drawings/_rels/drawingX.xml.rels
            TCHAR szPathToFile[MAX_PATH] = { 0 };
            _stprintf(szPathToFile, _T("%s/xl/drawings/_rels/drawing%d.xml.rels"), m_temp_path.c_str(), m_index);
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

            TCHAR szTarget[MAX_PATH] = { 0 };
            _stprintf(szTarget, _T("../charts/chart%d.xml"), m_index);

            xml_stream << prolog()
            << tag(_T("Relationships")) << attr(_T("xmlns")) << ns_relationships
                << tag(_T("Relationship"))  << attr(_T("Id")) << _T("rId1")
                                        << attr(_T("Type")) << type_chart
                                        << attr(_T("Target")) << szTarget
                << endtag();
            // /xl/drawings/_rels/drawingX.xml.rels -]
        }

        {
            // [- /xl/drawings/drawingX.xml
            TCHAR szPathToFile[MAX_PATH] = { 0 };
            _stprintf(szPathToFile, _T("%s/xl/drawings/drawing%d.xml"), m_temp_path.c_str(), m_index);
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

            xml_stream << prolog()
            << tag(_T("xdr:wsDr")) << attr(_T("xmlns:xdr")) << ns_xdr << attr(_T("xmlns:a")) << ns_a
                << tag(_T("xdr:absoluteAnchor"))
                    << tag(_T("xdr:pos")) << attr(_T("x")) << 0 << attr(_T("y")) << 0 << endtag()
                    << tag(_T("xdr:ext")) << attr(_T("cx")) << 9312088 << attr(_T("cy")) << 6084794 << endtag()
                    << tag(_T("xdr:graphicFrame")) << attr(_T("macro")) << _T("")
                        << tag(_T("xdr:nvGraphicFramePr"))
                            << tag(_T("xdr:cNvPr")) << attr(_T("id")) << 2 << attr(_T("name")) << m_title.c_str() << endtag()
                            << tag(_T("xdr:cNvGraphicFramePr"))
                                << tag(_T("a:graphicFrameLocks")) << attr(_T("noGrp")) << 1 << endtag()
                            << endtag()
                        << endtag()
                        << tag(_T("xdr:xfrm"))
                            << tag(_T("a:off")) << attr(_T("x")) << 0 << attr(_T("y")) << 0 << endtag()
                            << tag(_T("a:ext")) << attr(_T("cx")) << 0 << attr(_T("cy")) << 0 << endtag()
                        << endtag()
                        << tag(_T("a:graphic"))
                            << tag(_T("a:graphicData")) << attr(_T("uri")) << ns_c
                                << tag(_T("c:chart")) << attr(_T("xmlns:c")) << ns_c << attr(_T("xmlns:r")) << ns_book_r << attr(_T("r:id")) << _T("rId1")
                                << endtag()
                            << endtag()
                        << endtag()
                    << endtag()
                    << tag(_T("xdr:clientData")) << endtag()
                << endtag();
            // /xl/drawings/drawingX.xml -]
        }
    }

    return true;
}

}	// namespace SimpleXlsx
