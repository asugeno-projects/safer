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
#include <locale>
#include <algorithm>
#include <string.h>
#include <errno.h>

#ifdef _WIN32
#include <windows.h>	// for ZIP
#include <direct.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/syscall.h>
#endif  // _WIN32_WINNT

#include "../Zip/zip.h"

#include "XlsxHeaders.h"
#include "Workbook.h"

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

namespace SimpleXlsx {

using namespace std;
using namespace xmlw;

// ****************************************************************************
/// @brief  Namespace-owned global function to create nested directories` tree
/// @param  dirName directories` tree to be created
/// @return Boolean result of the operation
// ****************************************************************************
bool MakeDirectory(const _tstring& dirName)
{
    _tstring part = _T("");
    int32_t oldPointer = 0;
    int32_t currPointer = 0;

    int32_t res = -1;

    for (size_t i = 0; i < dirName.length(); i++) {
        if (dirName.at(i) == _T('\\') || dirName.at(i) == _T('/')) {
            part += dirName.substr(oldPointer, currPointer - oldPointer) + _T("/");
            oldPointer = currPointer + 1;

        #ifdef _WIN32
            res = _tmkdir(part.c_str());
        #else
            res = _tmkdir(part.c_str(), 0777);
        #endif
            if (res == -1 && errno == ENOENT) return false;
        }

        currPointer++;
    }

    return true;
}

// ****************************************************************************
//
// CWorksheet class implementation
//
// ****************************************************************************

// ****************************************************************************
/// @brief  The class default constructor
/// @return no
// ****************************************************************************
CWorkbook::CWorkbook()
{
	m_commLastId = 0;

    m_charts.clear();
    m_worksheets.clear();
    m_sharedStrings.clear();

    m_contentFiles.push_back(_T("[Content_Types].xml"));            // 0
    m_contentFiles.push_back(_T("_rels/.rels"));                   // 1
    m_contentFiles.push_back(_T("docProps/core.xml"));             // 2
    m_contentFiles.push_back(_T("docProps/app.xml"));              // 3
    m_contentFiles.push_back(_T("xl/workbook.xml"));               // 4
    m_contentFiles.push_back(_T("xl/styles.xml"));                 // 5
    m_contentFiles.push_back(_T("xl/_rels/workbook.xml.rels"));   // 6
    m_contentFiles.push_back(_T("xl/theme/theme1.xml"));          // 7

    Style style;

	style.numFormat.id = 0;
    m_styleList.Add(style);  // default style

    style.numFormat.id = 1;
    style.fill.patternType = PATTERN_GRAY_125;
    m_styleList.Add(style);  // default style

    TCHAR szTempDir[MAX_PATH] = { 0 };
#ifdef _WIN32
    TCHAR *szPath = _tgetenv(_T("TEMP"));
    _stprintf(szTempDir, _T("xlsx_%lu_%ld"), GetCurrentThreadId(), clock());
#else
    TCHAR *szPath = _tgetenv(_T("TMPDIR"));
    _stprintf(szTempDir, _T("xlsx_%lu_%ld"), syscall(SYS_gettid), clock());
#endif

    if (szPath) {
        m_temp_path = szPath;
        m_temp_path.append(_T("/"));
    }
    m_temp_path.append(szTempDir);
}

// ****************************************************************************
/// @brief  The class destructor
/// @return no
// ****************************************************************************
CWorkbook::~CWorkbook()
{
    for (size_t i = 0; i < m_worksheets.size(); i++)
        delete m_worksheets[i];

    for (size_t i = 0; i < m_charts.size(); i++)
        delete m_charts[i];

    m_charts.clear();
    m_worksheets.clear();
    m_contentFiles.clear();
}

// ****************************************************************************
/// @brief  Adds another data sheet into the workbook
/// @param  title chart page title
/// @return Reference to a newly created object
// ****************************************************************************
CWorksheet& CWorkbook::AddSheet(const _tstring& title)
{
    CWorksheet *sheet = new (std::nothrow) CWorksheet(m_worksheets.size() + 1, m_temp_path);
    sheet->SetTitle(title);
    sheet->SetSharedStr(&m_sharedStrings);
    sheet->SetComments(&m_comments);
    m_worksheets.push_back(sheet);

    return *m_worksheets[m_worksheets.size() - 1];
}

// ****************************************************************************
/// @brief  Adds another data sheet into the workbook
/// @param  title chart page title
/// @param	colWidths list of pairs colNumber:Width
/// @return Reference to a newly created object
// ****************************************************************************
CWorksheet& CWorkbook::AddSheet(const _tstring& title, std::vector<ColumnWidth>& colWidths)
{
    CWorksheet *sheet = new (std::nothrow) CWorksheet(m_worksheets.size() + 1, colWidths, m_temp_path);
    sheet->SetTitle(title);
    sheet->SetSharedStr(&m_sharedStrings);
    sheet->SetComments(&m_comments);
    m_worksheets.push_back(sheet);

    return *m_worksheets[m_worksheets.size() - 1];
}

// ****************************************************************************
/// @brief  Adds another data sheet with a frozen pane into the workbook
/// @param  title chart page title
/// @param  frozenWidth frozen pane width (in number of cells from 0)
/// @param  frozenHeight frozen pane height (in number of cells from 0)
/// @return Reference to a newly created object
// ****************************************************************************
CWorksheet& CWorkbook::AddSheet(const _tstring& title, uint32_t frozenWidth, uint32_t frozenHeight)
{
    CWorksheet *sheet = new (std::nothrow) CWorksheet(m_worksheets.size() + 1, frozenWidth, frozenHeight, m_temp_path);
    sheet->SetTitle(title);
    sheet->SetSharedStr(&m_sharedStrings);
    sheet->SetComments(&m_comments);
    m_worksheets.push_back(sheet);

    return *m_worksheets[m_worksheets.size() - 1];
}

// ****************************************************************************
/// @brief  Adds another data sheet with a frozen pane into the workbook
/// @param  title chart page title
/// @param  frozenWidth frozen pane width (in number of cells from 0)
/// @param  frozenHeight frozen pane height (in number of cells from 0)
/// @param	colWidths list of pairs colNumber:Width
/// @return Reference to a newly created object
// ****************************************************************************
CWorksheet& CWorkbook::AddSheet(const _tstring& title, uint32_t frozenWidth, uint32_t frozenHeight, std::vector<ColumnWidth>& colWidths)
{
    CWorksheet *sheet = new (std::nothrow) CWorksheet(m_worksheets.size() + 1, frozenWidth, frozenHeight, colWidths, m_temp_path);
    sheet->SetTitle(title);
    sheet->SetSharedStr(&m_sharedStrings);
    sheet->SetComments(&m_comments);
    m_worksheets.push_back(sheet);

    return *m_worksheets[m_worksheets.size() - 1];
}

// ****************************************************************************
/// @brief  Adds another chart into the workbook
/// @param  title chart page title
/// @return Reference to a newly created object
// ****************************************************************************
CChartsheet& CWorkbook::AddChart(const _tstring& title)
{
    CChartsheet *chart = new (std::nothrow) CChartsheet(m_charts.size() + 1, m_temp_path);
    chart->SetTitle(title);
    m_charts.push_back(chart);

    return *m_charts[m_charts.size() - 1];
}

// ****************************************************************************
/// @brief  Adds another chart into the workbook
/// @param  title chart page title
/// @param  type type of main chart
/// @return Reference to a newly created object
// ****************************************************************************
CChartsheet& CWorkbook::AddChart(const _tstring& title, EChartTypes type)
{
    CChartsheet *chart = new (std::nothrow) CChartsheet(m_charts.size() + 1, type, m_temp_path);
    chart->SetTitle(title);
    m_charts.push_back(chart);

    return *m_charts[m_charts.size() - 1];
}

// ****************************************************************************
/// @brief  Saves workbook into the specified file
/// @param  name full path to the file
/// @return Boolean result of the operation
// ****************************************************************************
bool CWorkbook::Save(const _tstring& name)
{
    if ( !SaveCore() || !SaveApp() || !SaveContentType() || !SaveTheme() ||
         !SaveComments() || !SaveSharedStrings() || !SaveStyles() || !SaveWorkbook()) {
         ClearTemp();
         return false;
    }

    TCHAR szFilename[MAX_PATH] = { 0 };

    for (size_t i = 0; i < m_worksheets.size(); i++) {
        _stprintf(szFilename, _T("xl/worksheets/sheet%d.xml"), m_worksheets[i]->GetIndex());
        m_contentFiles.push_back(szFilename);

        if (m_worksheets[i]->IsThereComment()) {
			_stprintf(szFilename, _T("xl/worksheets/_rels/sheet%d.xml.rels"), m_worksheets[i]->GetIndex());
			m_contentFiles.push_back(szFilename);
        }

        if (m_worksheets[i]->Save() == false) {
            ClearTemp();
            return false;
        }
    }

    for (size_t i = 0; i < m_charts.size(); i++) {
        int32_t index = m_charts[i]->GetIndex();

        _stprintf(szFilename, _T("xl/charts/chart%d.xml"), index);
        m_contentFiles.push_back(szFilename);

        _stprintf(szFilename, _T("xl/chartsheets/sheet%d.xml"), index);
        m_contentFiles.push_back(szFilename);
        _stprintf(szFilename, _T("xl/chartsheets/_rels/sheet%d.xml.rels"), index);
        m_contentFiles.push_back(szFilename);

        _stprintf(szFilename, _T("xl/drawings/drawing%d.xml"), index);
        m_contentFiles.push_back(szFilename);
        _stprintf(szFilename, _T("xl/drawings/_rels/drawing%d.xml.rels"), index);
        m_contentFiles.push_back(szFilename);

        if (m_charts[i]->Save() == false) {
            ClearTemp();
            return false;
        }
    }

    bool bRetCode = true;

    HZIP hZip = CreateZip(name.c_str(), NULL);  // create .zip without encryption
    if (hZip != 0) {
		TCHAR szPath[MAX_PATH] = { 0 };
		for (size_t i = 0; i < m_contentFiles.size(); i++) {
			memset(szPath, 0, sizeof(szPath));
			_stprintf(szPath, _T("%s/%s"), m_temp_path.c_str(), m_contentFiles[i].c_str());
			uint32_t res = ZipAdd(hZip, m_contentFiles[i].c_str(), szPath);
			if (res != ZR_OK) {
				bRetCode = false;
				break;
			}
		}

		CloseZip(hZip);
    }
    else {
		bRetCode = false;
    }

    ClearTemp();
    return bRetCode;
}

// ****************************************************************************
/// @brief  ...
/// @return Boolean result of the operation
// ****************************************************************************
bool CWorkbook::SaveCore() const
{
    {
        TCHAR szUserName[MAX_PATH] = { 0 };
        TCHAR *szTempName = _tgetenv(_T("USERNAME"));
        if (szTempName != NULL) _stprintf(szUserName, _T("%s"), szTempName);
        else _stprintf(szUserName, _T("Unknown"));

        TCHAR szTime[MAX_PATH] = { 0 };
		time_t t = time(0);
		struct tm *timeinfo = localtime(&t);
		_stprintf(szTime, _T("%4d-%02d-%02dT%02d:%02d:%02dZ"),
            timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday,
            timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

		// [- zip/docProps/core.xml
		TCHAR szPathToFile[MAX_PATH] = { 0 };
        _stprintf(szPathToFile, _T("%s/%s"), m_temp_path.c_str(), m_contentFiles[2].c_str());
        MakeDirectory(szPathToFile);
        char szPath[MAX_PATH] = { 0 };
#ifdef UNICODE
        int32_t res = wcstombs(szPath, szPathToFile, MAX_PATH);
        if (res == -1) return false;
#else
		_stprintf(szPath, _T("%s"), szPathToFile);
#endif
        _tofstream f(szPath);
        xmlw::XmlStream xml_stream(f);

		xml_stream << prolog()
		<< tag(_T("cp:coreProperties")) << attr(_T("xmlns:cp")) << ns_cp << attr(_T("xmlns:dc")) << ns_dc
										<< attr(_T("xmlns:dcterms")) << ns_dcterms << attr(_T("xmlns:dcmitype")) << ns_dcmitype
										<< attr(_T("xmlns:xsi")) << ns_xsi
            << tag(_T("dc:creator")) << chardata() << szUserName << endtag()
            << tag(_T("cp:lastModifiedBy")) << chardata() << szUserName << endtag()
            << tag(_T("dcterms:created")) << attr(_T("xsi:type")) << xsi_type << chardata() << szTime << endtag()
            << tag(_T("dcterms:modified")) << attr(_T("xsi:type")) << xsi_type << chardata() << szTime << endtag();
		// zip/docProps/core.xml -]
	}

	{
		// [- zip/_rels/.rels
		TCHAR szPathToFile[MAX_PATH] = { 0 };
        _stprintf(szPathToFile, _T("%s/%s"), m_temp_path.c_str(), m_contentFiles[1].c_str());
        MakeDirectory(szPathToFile);
        char szPath[MAX_PATH] = { 0 };
#ifdef UNICODE
        int32_t res = wcstombs(szPath, szPathToFile, MAX_PATH);
        if (res == -1) return false;
#else
		_stprintf(szPath, _T("%s"), szPathToFile);
#endif
        _tofstream f(szPath);
        xmlw::XmlStream xml_stream(f);

        xml_stream << prolog()
        << tag(_T("Relationships")) << attr(_T("xmlns")) << ns_relationships
            << tag(_T("Relationship"))  << attr(_T("Id")) << _T("rId3")
                                    << attr(_T("Type")) << type_app
                                    << attr(_T("Target")) << _T("docProps/app.xml")
            << endtag()
            << tag(_T("Relationship"))  << attr(_T("Id")) << _T("rId2")
									<< attr(_T("Type")) << type_core
                                    << attr(_T("Target")) << _T("docProps/core.xml")
            << endtag()
            << tag(_T("Relationship"))  << attr(_T("Id")) << _T("rId1")
                                    << attr(_T("Type")) << type_book
                                    << attr(_T("Target")) << _T("xl/workbook.xml")
            << endtag();
		// zip/_rels/.rels -]
	}

	return true;
}

// ****************************************************************************
/// @brief  ...
/// @return Boolean result of the operation
// ****************************************************************************
bool CWorkbook::SaveContentType()
{
    TCHAR szPropValue[100] = { 0 };

    // [- zip/[Content_Types].xml
    TCHAR szPathToFile[MAX_PATH] = { 0 };
    _stprintf(szPathToFile, _T("%s/%s"), m_temp_path.c_str(), m_contentFiles[0].c_str());
    MakeDirectory(szPathToFile);
    char szPath[MAX_PATH] = { 0 };
#ifdef UNICODE
        int32_t res = wcstombs(szPath, szPathToFile, MAX_PATH);
        if (res == -1) return false;
#else
		_stprintf(szPath, _T("%s"), szPathToFile);
#endif
        _tofstream f(szPath);
    xmlw::XmlStream xml_stream(f);

    xml_stream << prolog()
    << tag(_T("Types")) << attr(_T("xmlns")) << ns_content_types
        << tag(_T("Default")) << attr(_T("Extension")) << _T("rels") << attr(_T("ContentType")) << content_rels << endtag()
        << tag(_T("Default")) << attr(_T("Extension")) << _T("xml") << attr(_T("ContentType")) << content_xml << endtag()
        << tag(_T("Override")) << attr(_T("PartName")) << _T("/xl/workbook.xml") << attr(_T("ContentType")) << content_book << endtag()
        << tag(_T("Override")) << attr(_T("PartName")) << _T("/xl/theme/theme1.xml") << attr(_T("ContentType")) << content_theme << endtag()
        << tag(_T("Override")) << attr(_T("PartName")) << _T("/xl/styles.xml") << attr(_T("ContentType")) << content_styles << endtag()
        << tag(_T("Override")) << attr(_T("PartName")) << _T("/docProps/core.xml") << attr(_T("ContentType")) << content_core << endtag()
        << tag(_T("Override")) << attr(_T("PartName")) << _T("/docProps/app.xml") << attr(_T("ContentType")) << content_app << endtag();

        if (!m_sharedStrings.empty()) {
			xml_stream
			<< tag(_T("Override")) << attr(_T("PartName")) << _T("/xl/sharedStrings.xml") << attr(_T("ContentType")) << content_sharedStr << endtag();
        }

        if (!m_comments.empty()) {
			xml_stream
			<< tag(_T("Default")) << attr(_T("Extension")) << _T("vml") << attr(_T("ContentType")) << content_vml << endtag();

			TCHAR szTemp[200] = { 0 };
			for (size_t i = 0; i < m_worksheets.size(); i++) {
				if (m_worksheets[i]->IsThereComment()) {
					_stprintf(szTemp, _T("/xl/comments%u.xml"), m_worksheets[i]->GetIndex());

					xml_stream
					<< tag(_T("Override")) << attr(_T("PartName")) << szTemp << attr(_T("ContentType")) << content_comment << endtag();
				}
			}
        }

        bool bFormula = false;
        for (size_t i = 0; i < m_worksheets.size(); i++) {
            _stprintf(szPropValue, _T("/xl/worksheets/sheet%d.xml"), m_worksheets[i]->GetIndex());
            xml_stream << tag(_T("Override")) << attr(_T("PartName")) << szPropValue << attr(_T("ContentType")) << content_sheet << endtag();

            if (m_worksheets[i]->IsThereFormula()) bFormula = true;
        }

        if (bFormula) {
            xml_stream << tag(_T("Override"))   << attr(_T("PartName")) << _T("/xl/calcChain.xml")
                                            << attr(_T("ContentType")) << content_chain << endtag();

            if (SaveChain() == false) {
                return false;
            }
        }

        for (size_t i = 0; i < m_charts.size(); i++) {
            int32_t index = m_charts[i]->GetIndex();

            _stprintf(szPropValue, _T("/xl/charts/chart%d.xml"), index);
            xml_stream << tag(_T("Override")) << attr(_T("PartName")) << szPropValue << attr(_T("ContentType")) << content_chart << endtag();

            _stprintf(szPropValue, _T("/xl/drawings/drawing%d.xml"), index);
            xml_stream << tag(_T("Override")) << attr(_T("PartName")) << szPropValue << attr(_T("ContentType")) << content_drawing << endtag();

            _stprintf(szPropValue, _T("/xl/chartsheets/sheet%d.xml"), index);
            xml_stream << tag(_T("Override")) << attr(_T("PartName")) << szPropValue << attr(_T("ContentType")) << content_chartsheet << endtag();
        }
    // zip/[ContentTypes].xml -]

    return true;
}

// ****************************************************************************
/// @brief  ...
/// @return Boolean result of the operation
// ****************************************************************************
bool CWorkbook::SaveApp() const
{
    // [- zip/docProps/app.xml
    TCHAR szPathToFile[MAX_PATH] = { 0 };
    _stprintf(szPathToFile, _T("%s/%s"), m_temp_path.c_str(), m_contentFiles[3].c_str());
    MakeDirectory(szPathToFile);
    char szPath[MAX_PATH] = { 0 };
#ifdef UNICODE
        int32_t res = wcstombs(szPath, szPathToFile, MAX_PATH);
        if (res == -1) return false;
#else
		_stprintf(szPath, _T("%s"), szPathToFile);
#endif
        _tofstream f(szPath);
    xmlw::XmlStream xml_stream(f);

    size_t nSheets = m_worksheets.size();
    size_t nCharts = m_charts.size();
    size_t nVectorSize = (nCharts > 0) ? 4 : 2;

    xml_stream << prolog()
    << tag(_T("Properties")) << attr(_T("xmlns")) << ns_doc_prop << attr(_T("xmlns:vt")) << ns_vt
        << tag(_T("Application")) << chardata() << _T("Microsoft Excel") << endtag()
        << tag(_T("DocSecurity")) << chardata() << 0 << endtag()
        << tag(_T("ScaleCrop")) << chardata() << _T("false") << endtag()

        << tag(_T("HeadingPairs"))
            << tag(_T("vt:vector")) << attr(_T("size")) << nVectorSize << attr(_T("baseType")) << _T("variant")
                << tag(_T("vt:variant")) << tag(_T("vt:lpstr")) << chardata() << _T("Worksheets") << endtag() << endtag()
                << tag(_T("vt:variant")) << tag(_T("vt:i4")) << chardata() << nSheets << endtag() << endtag();

    if (nCharts > 0) {
        xml_stream
        << tag(_T("vt:variant")) << tag(_T("vt:lpstr")) << chardata() << _T("Diagramms") << endtag() << endtag()
        << tag(_T("vt:variant")) << tag(_T("vt:i4")) << chardata() << nCharts << endtag() << endtag();
    }

    xml_stream
            << endtag()
        << endtag()

        << tag(_T("TitlesOfParts"))
            << tag(_T("vt:vector")) << attr(_T("size")) << nSheets + nCharts << attr(_T("baseType")) << _T("lpstr");

    for (size_t i = 0; i < nSheets; i++) {
        xml_stream << tag(_T("vt:lpstr")) << chardata() << m_worksheets[i]->GetTitle() << endtag();
    }

    for (size_t i = 0; i < nCharts; i++) {
        xml_stream << tag(_T("vt:lpstr")) << chardata() << m_charts[i]->GetTitle() << endtag();
    }

    xml_stream
    << endtag()
    << endtag()
    << tag(_T("Company")) << chardata() << _T("") << endtag()
    << tag(_T("LinksUpToDate")) << chardata() << _T("false") << endtag()
    << tag(_T("SharedDoc")) << chardata() << _T("false") << endtag()
    << tag(_T("HyperlinksChanged")) << chardata() << _T("false") << endtag()
    << tag(_T("AppVersion")) << chardata() << 14.03/*Microsoft Excel 2010*/ << endtag();
    // zip/docProps/app.xml -]

    return true;
}

// ****************************************************************************
/// @brief  ...
/// @return Boolean result of the operation
// ****************************************************************************
bool CWorkbook::SaveTheme() const
{
    // [- zip/xl/theme/theme1.xml
    TCHAR szPathToFile[MAX_PATH] = { 0 };
    _stprintf(szPathToFile, _T("%s/%s"), m_temp_path.c_str(), m_contentFiles[7].c_str());
    MakeDirectory(szPathToFile);
    char szPath[MAX_PATH] = { 0 };
#ifdef UNICODE
        int32_t res = wcstombs(szPath, szPathToFile, MAX_PATH);
        if (res == -1) return false;
#else
		_stprintf(szPath, _T("%s"), szPathToFile);
#endif
        _tofstream f(szPath);
    xmlw::XmlStream xml_stream(f);

    const TCHAR *szAFont = _T("a:font");
    const TCHAR *szScript = _T("script");
    const TCHAR *szTypeface = _T("typeface");

    xml_stream << prolog()
    << tag(_T("a:theme")) << attr(_T("xmlns:a")) << ns_a << attr(_T("name")) << _T("Office Theme")
        << tag(_T("a:themeElements"))
            << tag(_T("a:clrScheme")) << attr(_T("name")) << _T("Office")
                << tag(_T("a:dk1")) << tag(_T("a:sysClr")) << attr(_T("val")) << _T("windowText") << attr(_T("lastClr")) << _T("000000") << endtag() << endtag()
                << tag(_T("a:lt1")) << tag(_T("a:sysClr")) << attr(_T("val")) << _T("window") << attr(_T("lastClr")) << _T("FFFFFF") << endtag() << endtag()
                << tag(_T("a:dk2")) << tag(_T("a:srgbClr")) << attr(_T("val")) << _T("1F497D") << endtag() << endtag()
                << tag(_T("a:lt2")) << tag(_T("a:srgbClr")) << attr(_T("val")) << _T("EEECE1") << endtag() << endtag()
                << tag(_T("a:accent1")) << tag(_T("a:srgbClr")) << attr(_T("val")) << _T("4F81BD") << endtag() << endtag()
                << tag(_T("a:accent2")) << tag(_T("a:srgbClr")) << attr(_T("val")) << _T("C0504D") << endtag() << endtag()
                << tag(_T("a:accent3")) << tag(_T("a:srgbClr")) << attr(_T("val")) << _T("9BBB59") << endtag() << endtag()
                << tag(_T("a:accent4")) << tag(_T("a:srgbClr")) << attr(_T("val")) << _T("8064A2") << endtag() << endtag()
                << tag(_T("a:accent5")) << tag(_T("a:srgbClr")) << attr(_T("val")) << _T("4BACC6") << endtag() << endtag()
                << tag(_T("a:accent6")) << tag(_T("a:srgbClr")) << attr(_T("val")) << _T("F79646") << endtag() << endtag()
                << tag(_T("a:hlink")) << tag(_T("a:srgbClr")) << attr(_T("val")) << _T("0000FF") << endtag() << endtag()
                << tag(_T("a:folHlink")) << tag(_T("a:srgbClr")) << attr(_T("val")) << _T("800080") << endtag() << endtag()
            << endtag()

            << tag(_T("a:fontScheme")) << attr(_T("name")) << _T("Office")
                << tag(_T("a:majorFont"))
                    << tag(_T("a:latin")) << attr(szTypeface) << _T("Cambria") << endtag()
                    << tag(_T("a:ea")) << attr(szTypeface) << _T("") << endtag()
                    << tag(_T("a:cs")) << attr(szTypeface) << _T("") << endtag()
                    << tag(szAFont) << attr(szScript) << _T("Arab") << attr(szTypeface) << _T("Times New Roman") << endtag()
                    << tag(szAFont) << attr(szScript) << _T("Herb") << attr(szTypeface) << _T("Times New Roman") << endtag()
                    << tag(szAFont) << attr(szScript) << _T("Thai") << attr(szTypeface) << _T("Tahoma") << endtag()
                    << tag(szAFont) << attr(szScript) << _T("Ethi") << attr(szTypeface) << _T("Nyala") << endtag()
                    << tag(szAFont) << attr(szScript) << _T("Beng") << attr(szTypeface) << _T("Vrinda") << endtag()
                    << tag(szAFont) << attr(szScript) << _T("Gujr") << attr(szTypeface) << _T("Shruti") << endtag()
                    << tag(szAFont) << attr(szScript) << _T("Khmr") << attr(szTypeface) << _T("MoolBoran") << endtag()
                    << tag(szAFont) << attr(szScript) << _T("Knda") << attr(szTypeface) << _T("Tunga") << endtag()
                    << tag(szAFont) << attr(szScript) << _T("Guru") << attr(szTypeface) << _T("Raavi") << endtag()
                    << tag(szAFont) << attr(szScript) << _T("Cans") << attr(szTypeface) << _T("Euphemia") << endtag()
                    << tag(szAFont) << attr(szScript) << _T("Cher") << attr(szTypeface) << _T("Plantagenet Cherokee") << endtag()
                    << tag(szAFont) << attr(szScript) << _T("Yiii") << attr(szTypeface) << _T("Microsoft Yi Baiti") << endtag()
                    << tag(szAFont) << attr(szScript) << _T("Tibt") << attr(szTypeface) << _T("Microsoft Himalaya") << endtag()
                    << tag(szAFont) << attr(szScript) << _T("Thaa") << attr(szTypeface) << _T("MV Boli") << endtag()
                    << tag(szAFont) << attr(szScript) << _T("Deva") << attr(szTypeface) << _T("Mangal") << endtag()
                    << tag(szAFont) << attr(szScript) << _T("Telu") << attr(szTypeface) << _T("Gautami") << endtag()
                    << tag(szAFont) << attr(szScript) << _T("Taml") << attr(szTypeface) << _T("Latha") << endtag()
                    << tag(szAFont) << attr(szScript) << _T("Syrc") << attr(szTypeface) << _T("Estrangelo Edessa") << endtag()
                    << tag(szAFont) << attr(szScript) << _T("Orya") << attr(szTypeface) << _T("Kalinga") << endtag()
                    << tag(szAFont) << attr(szScript) << _T("Mlym") << attr(szTypeface) << _T("Kartika") << endtag()
                    << tag(szAFont) << attr(szScript) << _T("Laoo") << attr(szTypeface) << _T("DokChampa") << endtag()
                    << tag(szAFont) << attr(szScript) << _T("Sinh") << attr(szTypeface) << _T("Iskoola Pota") << endtag()
                    << tag(szAFont) << attr(szScript) << _T("Mong") << attr(szTypeface) << _T("Mongolian Baiti") << endtag()
                    << tag(szAFont) << attr(szScript) << _T("Viet") << attr(szTypeface) << _T("Times New Roman") << endtag()
                    << tag(szAFont) << attr(szScript) << _T("Uigh") << attr(szTypeface) << _T("Microsoft Uighur") << endtag()
                    << tag(szAFont) << attr(szScript) << _T("Geor") << attr(szTypeface) << _T("Sylfaen") << endtag()
                << endtag()
                << tag(_T("a:minorFont"))
                    << tag(_T("a:latin")) << attr(szTypeface) << _T("Cambria") << endtag()
                    << tag(_T("a:ea")) << attr(szTypeface) << _T("") << endtag()
                    << tag(_T("a:cs")) << attr(szTypeface) << _T("") << endtag()
                    << tag(szAFont) << attr(szScript) << _T("Arab") << attr(szTypeface) << _T("Times New Roman") << endtag()
                    << tag(szAFont) << attr(szScript) << _T("Herb") << attr(szTypeface) << _T("Times New Roman") << endtag()
                    << tag(szAFont) << attr(szScript) << _T("Thai") << attr(szTypeface) << _T("Tahoma") << endtag()
                    << tag(szAFont) << attr(szScript) << _T("Ethi") << attr(szTypeface) << _T("Nyala") << endtag()
                    << tag(szAFont) << attr(szScript) << _T("Beng") << attr(szTypeface) << _T("Vrinda") << endtag()
                    << tag(szAFont) << attr(szScript) << _T("Gujr") << attr(szTypeface) << _T("Shruti") << endtag()
                    << tag(szAFont) << attr(szScript) << _T("Khmr") << attr(szTypeface) << _T("MoolBoran") << endtag()
                    << tag(szAFont) << attr(szScript) << _T("Knda") << attr(szTypeface) << _T("Tunga") << endtag()
                    << tag(szAFont) << attr(szScript) << _T("Guru") << attr(szTypeface) << _T("Raavi") << endtag()
                    << tag(szAFont) << attr(szScript) << _T("Cans") << attr(szTypeface) << _T("Euphemia") << endtag()
                    << tag(szAFont) << attr(szScript) << _T("Cher") << attr(szTypeface) << _T("Plantagenet Cherokee") << endtag()
                    << tag(szAFont) << attr(szScript) << _T("Yiii") << attr(szTypeface) << _T("Microsoft Yi Baiti") << endtag()
                    << tag(szAFont) << attr(szScript) << _T("Tibt") << attr(szTypeface) << _T("Microsoft Himalaya") << endtag()
                    << tag(szAFont) << attr(szScript) << _T("Thaa") << attr(szTypeface) << _T("MV Boli") << endtag()
                    << tag(szAFont) << attr(szScript) << _T("Deva") << attr(szTypeface) << _T("Mangal") << endtag()
                    << tag(szAFont) << attr(szScript) << _T("Telu") << attr(szTypeface) << _T("Gautami") << endtag()
                    << tag(szAFont) << attr(szScript) << _T("Taml") << attr(szTypeface) << _T("Latha") << endtag()
                    << tag(szAFont) << attr(szScript) << _T("Syrc") << attr(szTypeface) << _T("Estrangelo Edessa") << endtag()
                    << tag(szAFont) << attr(szScript) << _T("Orya") << attr(szTypeface) << _T("Kalinga") << endtag()
                    << tag(szAFont) << attr(szScript) << _T("Mlym") << attr(szTypeface) << _T("Kartika") << endtag()
                    << tag(szAFont) << attr(szScript) << _T("Laoo") << attr(szTypeface) << _T("DokChampa") << endtag()
                    << tag(szAFont) << attr(szScript) << _T("Sinh") << attr(szTypeface) << _T("Iskoola Pota") << endtag()
                    << tag(szAFont) << attr(szScript) << _T("Mong") << attr(szTypeface) << _T("Mongolian Baiti") << endtag()
                    << tag(szAFont) << attr(szScript) << _T("Viet") << attr(szTypeface) << _T("Times New Roman") << endtag()
                    << tag(szAFont) << attr(szScript) << _T("Uigh") << attr(szTypeface) << _T("Microsoft Uighur") << endtag()
                    << tag(szAFont) << attr(szScript) << _T("Geor") << attr(szTypeface) << _T("Sylfaen") << endtag()
                << endtag()
            << endtag()

            << tag(_T("a:fmtScheme")) << attr(_T("name")) << _T("Office")
                << tag(_T("a:fillStyleLst"))
                    << tag(_T("a:solidFill")) << tag(_T("a:schemeClr")) << attr(_T("val")) << _T("phClr") << endtag() << endtag()
                    << tag(_T("a:gradFill")) << attr(_T("rotWithShape")) << 1
                        << tag(_T("a:gsLst"))
                            << tag(_T("a:gs")) << attr(_T("pos")) << 0
                                << tag(_T("a:schemeClr")) << attr(_T("val")) << _T("phClr")
                                    << tag(_T("a:tint")) << attr(_T("val")) << 50000 << endtag()
                                    << tag(_T("a:satMod")) << attr(_T("val")) << 300000 << endtag()
                                << endtag()
                            << endtag()
                            << tag(_T("a:gs")) << attr(_T("pos")) << 35000
                                << tag(_T("a:schemeClr")) << attr(_T("val")) << _T("phClr")
                                    << tag(_T("a:tint")) << attr(_T("val")) << 37000 << endtag()
                                    << tag(_T("a:satMod")) << attr(_T("val")) << 300000 << endtag()
                                << endtag()
                            << endtag()
                            << tag(_T("a:gs")) << attr(_T("pos")) << 100000
                                << tag(_T("a:schemeClr")) << attr(_T("val")) << _T("phClr")
                                    << tag(_T("a:tint")) << attr(_T("val")) << 15000 << endtag()
                                    << tag(_T("a:satMod")) << attr(_T("val")) << 350000 << endtag()
                                << endtag()
                            << endtag()
                        << endtag()
                        << tag(_T("a:lin")) << attr(_T("a:ang")) << 16200000 << attr(_T("scaled")) << 1 << endtag()
                    << endtag()

                    << tag(_T("a:gradFill")) << attr(_T("rotWithShape")) << 1
                        << tag(_T("a:gsLst"))
                            << tag(_T("a:gs")) << attr(_T("pos")) << 0
                                << tag(_T("a:schemeClr")) << attr(_T("val")) << _T("phClr")
                                    << tag(_T("a:tint")) << attr(_T("val")) << 51000 << endtag()
                                    << tag(_T("a:satMod")) << attr(_T("val")) << 130000 << endtag()
                                << endtag()
                            << endtag()
                            << tag(_T("a:gs")) << attr(_T("pos")) << 80000
                                << tag(_T("a:schemeClr")) << attr(_T("val")) << _T("phClr")
                                    << tag(_T("a:tint")) << attr(_T("val")) << 93000 << endtag()
                                    << tag(_T("a:satMod")) << attr(_T("val")) << 130000 << endtag()
                                << endtag()
                            << endtag()
                            << tag(_T("a:gs")) << attr(_T("pos")) << 100000
                                << tag(_T("a:schemeClr")) << attr(_T("val")) << _T("phClr")
                                    << tag(_T("a:tint")) << attr(_T("val")) << 94000 << endtag()
                                    << tag(_T("a:satMod")) << attr(_T("val")) << 135000 << endtag()
                                << endtag()
                            << endtag()
                        << endtag()
                        << tag(_T("a:lin")) << attr(_T("a:ang")) << 16200000 << attr(_T("scaled")) << 0 << endtag()
                    << endtag()
                << endtag()

                << tag(_T("a:lnStyleLst"))
                    << tag(_T("a:ln")) << attr(_T("w")) << 9525 << attr(_T("cap")) << _T("flat") << attr(_T("cmpd")) << _T("sng") << attr(_T("algn")) << _T("ctr")
                        << tag(_T("a:solidFill"))
                            << tag(_T("a:schemeClr")) << attr(_T("val")) << _T("phClr")
                                << tag(_T("a:shade")) << attr(_T("val")) << 95000 << endtag()
                                << tag(_T("a:satMod")) << attr(_T("val")) << 105000 << endtag()
                            << endtag()
                        << endtag()
                        << tag(_T("a:prstDash")) << attr(_T("val")) << _T("solid") << endtag()
                    << endtag()
                    << tag(_T("a:ln")) << attr(_T("w")) << 25400 << attr(_T("cap")) << _T("flat") << attr(_T("cmpd")) << _T("sng") << attr(_T("algn")) << _T("ctr")
                        << tag(_T("a:solidFill")) << tag(_T("a:schemeClr")) << attr(_T("val")) << _T("phClr") << endtag() << endtag()
                        << tag(_T("a:prstDash")) << attr(_T("val")) << _T("solid") << endtag()
                    << endtag()
                    << tag(_T("a:ln")) << attr(_T("w")) << 38100 << attr(_T("cap")) << _T("flat") << attr(_T("cmpd")) << _T("sng") << attr(_T("algn")) << _T("ctr")
                        << tag(_T("a:solidFill")) << tag(_T("a:schemeClr")) << attr(_T("val")) << _T("phClr") << endtag() << endtag()
                        << tag(_T("a:prstDash")) << attr(_T("val")) << _T("solid") << endtag()
                    << endtag()
                << endtag()

                << tag(_T("a:effectStyleLst"))
                    << tag(_T("a:effectStyle"))
                        << tag(_T("a:effectLst"))
                            << tag(_T("a:outerShdw")) << attr(_T("blurRad")) << 40000 << attr(_T("dist")) << 20000 << attr(_T("dir")) << 5400000 << attr(_T("rotWithShape")) << 0
                                << tag(_T("a:srgbClr")) << attr(_T("val")) << _T("000000") << tag(_T("a:alpha")) << attr(_T("val")) << 38000 << endtag() << endtag()
                            << endtag()
                            << tag(_T("a:outerShdw")) << attr(_T("blurRad")) << 40000 << attr(_T("dist")) << 23000 << attr(_T("dir")) << 5400000 << attr(_T("rotWithShape")) << 0
                                << tag(_T("a:srgbClr")) << attr(_T("val")) << _T("000000") << tag(_T("a:alpha")) << attr(_T("val")) << 35000 << endtag() << endtag()
                            << endtag()
                            << tag(_T("a:outerShdw")) << attr(_T("blurRad")) << 40000 << attr(_T("dist")) << 23000 << attr(_T("dir")) << 5400000 << attr(_T("rotWithShape")) << 0
                                << tag(_T("a:srgbClr")) << attr(_T("val")) << _T("000000") << tag(_T("a:alpha")) << attr(_T("val")) << 35000 << endtag() << endtag()
                            << endtag()
                        << endtag()
                        << tag(_T("a:scene3d"))
                            << tag(_T("a:camera")) << attr(_T("prst")) << _T("orthographicFront")
                                << tag(_T("a:rot")) << attr(_T("lat")) << 0 << attr(_T("lon")) << 0 << attr(_T("rev")) << 0 << endtag()
                            << endtag()
                            << tag(_T("a:lightRig")) << attr(_T("rig")) << _T("threePt") << attr(_T("dir")) << _T("t")
                                << tag(_T("a:rot")) << attr(_T("lat")) << 0 << attr(_T("lon")) << 0 << attr(_T("rev")) << 1200000 << endtag()
                            << endtag()
                        << endtag()
                        << tag(_T("a:sp3d")) << tag(_T("bevelT")) << attr(_T("w")) << 63500 << attr(_T("h")) << 25400 << endtag() << endtag()
                    << endtag()
                << endtag()

                << tag(_T("a:bgFillStyleLst"))
                    << tag(_T("a:solidFill")) << tag(_T("a:schemeClr")) << attr(_T("val")) << _T("phClr") << endtag() << endtag()
                << endtag()

                << tag(_T("a:fillStyleLst"))
                    << tag(_T("a:gradFill")) << attr(_T("rotWithShape")) << 1
                        << tag(_T("a:gsLst"))
                            << tag(_T("a:gs")) << attr(_T("pos")) << 0
                                << tag(_T("a:schemeClr")) << attr(_T("val")) << _T("phClr")
                                    << tag(_T("a:tint")) << attr(_T("val")) << 40000 << endtag()
                                    << tag(_T("a:satMod")) << attr(_T("val")) << 350000 << endtag()
                                << endtag()
                            << endtag()
                            << tag(_T("a:gs")) << attr(_T("pos")) << 40000
                                << tag(_T("a:schemeClr")) << attr(_T("val")) << _T("phClr")
                                    << tag(_T("a:tint")) << attr(_T("val")) << 45000 << endtag()
                                    << tag(_T("a:shade")) << attr(_T("val")) << 99000 << endtag()
                                    << tag(_T("a:satMod")) << attr(_T("val")) << 350000 << endtag()
                                << endtag()
                            << endtag()
                            << tag(_T("a:gs")) << attr(_T("pos")) << 100000
                                << tag(_T("a:schemeClr")) << attr(_T("val")) << _T("phClr")
                                    << tag(_T("a:shade")) << attr(_T("val")) << 20000 << endtag()
                                    << tag(_T("a:satMod")) << attr(_T("val")) << 255000 << endtag()
                                << endtag()
                            << endtag()
                        << endtag()
                        << tag(_T("a:path")) << attr(_T("path")) << _T("circle")
                            << tag(_T("a:fillToRect")) << attr(_T("l")) << 50000 << attr(_T("t")) << -80000 << attr(_T("r")) << 50000 << attr(_T("b")) << 180000 << endtag()
                        << endtag()
                    << endtag()
                    << tag(_T("a:gradFill")) << attr(_T("rotWithShape")) << 1
                        << tag(_T("a:gsLst"))
                            << tag(_T("a:gs")) << attr(_T("pos")) << 0
                                << tag(_T("a:schemeClr")) << attr(_T("val")) << _T("phClr")
                                    << tag(_T("a:tint")) << attr(_T("val")) << 80000 << endtag()
                                    << tag(_T("a:satMod")) << attr(_T("val")) << 300000 << endtag()
                                << endtag()
                            << endtag()
                            << tag(_T("a:gs")) << attr(_T("pos")) << 100000
                                << tag(_T("a:schemeClr")) << attr(_T("val")) << _T("phClr")
                                    << tag(_T("a:shade")) << attr(_T("val")) << 30000 << endtag()
                                    << tag(_T("a:satMod")) << attr(_T("val")) << 200000 << endtag()
                                << endtag()
                            << endtag()
                        << endtag()
                        << tag(_T("a:path")) << attr(_T("path")) << _T("circle")
                            << tag(_T("a:fillToRect")) << attr(_T("l")) << 50000 << attr(_T("t")) << 50000 << attr(_T("r")) << 50000 << attr(_T("b")) << 50000 << endtag()
                        << endtag()
                    << endtag()
                << endtag()
            << endtag()
        << endtag()
        << tag(_T("a:objectDefaults")) << endtag()
        << tag(_T("a:extraClrShemeLst")) << endtag();
    // zip/xl/theme/theme1.xml -]

    return true;
}

// ****************************************************************************
/// @brief  ...
/// @return Boolean result of the operation
// ****************************************************************************
bool CWorkbook::SaveStyles() const
{
    // [- zip/xl/styles.xml
    TCHAR szPathToFile[MAX_PATH] = { 0 };
    _stprintf(szPathToFile, _T("%s/%s"), m_temp_path.c_str(), m_contentFiles[5].c_str());
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
    << tag(_T("styleSheet"))    << attr(_T("xmlns")) << ns_book << attr(_T("xmlns:mc")) << ns_mc
								<< attr(_T("mc:Ignorable")) << _T("x14ac") << attr(_T("xmlns:x14ac")) << ns_x14ac;

        AddNumberFormats(xml_stream);
        AddFonts(xml_stream);
        AddFills(xml_stream);
        AddBorders(xml_stream);

    xml_stream
        << tag(_T("cellStyleXfs")) << attr(_T("count")) << 1
            << tag(_T("xf")) << attr(_T("numFmtId")) << 0 << attr(_T("fontId")) << 0 << attr(_T("fillId")) << 0 << attr(_T("borderId")) << 0 << endtag()
        << endtag()

        << tag(_T("cellXfs"));

    vector<vector<size_t> > styleIndexes = m_styleList.GetIndexes();
    vector< pair<pair<EAlignHoriz, EAlignVert>, bool> > styleAligns = m_styleList.GetPositions();
    for (size_t i = 0; i < styleIndexes.size(); i++) {
        vector<size_t> index = styleIndexes[i];
        pair<pair<EAlignHoriz, EAlignVert>, bool> align = styleAligns[i];

        xml_stream
            << tag(_T("xf")) << attr(_T("numFmtId")) << index[StyleList::STYLE_LINK_NUM_FORMAT]
							<< attr(_T("fontId")) << index[StyleList::STYLE_LINK_FONT]
							<< attr(_T("fillId")) << index[StyleList::STYLE_LINK_FILL]
							<< attr(_T("borderId")) << index[StyleList::STYLE_LINK_BORDER];

        if (index[StyleList::STYLE_LINK_FONT] != 0)
            xml_stream   << attr(_T("applyFont")) << 1;
        if (index[StyleList::STYLE_LINK_FILL] != 0)
            xml_stream   << attr(_T("applyFill")) << 1;
        if (index[StyleList::STYLE_LINK_BORDER] != 0)
            xml_stream   << attr(_T("applyBorder")) << 1;
		if (index[StyleList::STYLE_LINK_NUM_FORMAT] != 0)
			xml_stream   << attr(_T("applyNumberFormat")) << 1;

		if (align.first.first != ALIGN_H_NONE || align.first.second != ALIGN_V_NONE || align.second != false) {
			xml_stream
				<< tag(_T("alignment"));

			switch (align.first.first) {
			case ALIGN_H_LEFT:	xml_stream << attr(_T("horizontal")) << _T("left"); break;
			case ALIGN_H_CENTER:xml_stream << attr(_T("horizontal")) << _T("center"); break;
			case ALIGN_H_RIGHT:	xml_stream << attr(_T("horizontal")) << _T("right"); break;
			case ALIGN_H_NONE:
			default: 			break;
			}

			switch (align.first.second) {
			case ALIGN_V_BOTTOM:	xml_stream << attr(_T("vertical")) << _T("bottom"); break;
			case ALIGN_V_CENTER:	xml_stream << attr(_T("vertical")) << _T("center"); break;
			case ALIGN_V_TOP:		xml_stream << attr(_T("vertical")) << _T("top"); break;
			case ALIGN_V_NONE:
			default: 				break;
			}

			if (align.second == true) {
				xml_stream << attr(_T("wrapText")) << 1;
			}

			xml_stream
				<< endtag();
		}

        xml_stream
            << endtag();
    }

    xml_stream
        << endtag()

        << tag(_T("cellStyles")) << attr(_T("count")) << 1
            << tag(_T("cellStyle")) << attr(_T("name")) << _T("Normal") << attr(_T("xfId")) << 0 << attr(_T("builtinId")) << 0 << endtag()
        << endtag()

        << tag(_T("dxfs")) << attr(_T("count")) << 0 << endtag()
        << tag(_T("tableStyles")) << attr(_T("count")) << 0 << attr(_T("defaultTableStyle")) << _T("TableStyleMedium2") << attr(_T("defaultPivotStyle")) << _T("PivotStyleLight16") << endtag()
        << tag(_T("extLst"))
            << tag(_T("ext"))   << attr(_T("uri")) << _T("{EB79DEF2-80B8-43e5-95BD-54CBDDF9020C}")
								<< attr(_T("xmlns:x14")) << ns_x14
                << tag(_T("x14:slicerStyles")) << attr(_T("defaultSlicerStyle")) << _T("SlicerStyleLight1") << endtag()
            << endtag();
    // zip/xl/styles.xml -]

    return true;
}

// ****************************************************************************
/// @brief  Appends number format section into styles file
/// @param  stream reference to xml stream
/// @return no
// ****************************************************************************
void CWorkbook::AddNumberFormats(xmlw::XmlStream& stream) const
{
	vector<NumFormat> nums = m_styleList.GetNumFormats();

	size_t built_in_formats = 0;
	for (size_t i = 0; i < nums.size(); i++)
		if (nums[i].id < StyleList::BUILT_IN_STYLES_NUMBER)
			built_in_formats++;

	if (nums.size() - built_in_formats == 0) return;

    stream
    << tag(_T("numFmts")) << attr(_T("count")) << nums.size() - built_in_formats;

    for (size_t i = 0; i < nums.size(); i++) {
    	if (nums[i].id < StyleList::BUILT_IN_STYLES_NUMBER)
			continue;

        stream
        << tag(_T("numFmt"))
			<< attr(_T("numFmtId")) << nums[i].id
			<< attr(_T("formatCode")) << CWorkbook::GetFormatCodeString(nums[i])
        << endtag();
    }

	stream
    << endtag();
}

// ****************************************************************************
/// @brief  Converts numeric format object into its std::string representation
/// @param  fmt reference to format to be converted
/// @return std::string format code
// ****************************************************************************
_tstring CWorkbook::GetFormatCodeString(const NumFormat &fmt)
{
	if (fmt.formatString != _T("")) return fmt.formatString;

	bool addNegative = false;
	bool addZero = false;

	if (fmt.positiveColor != NUMSTYLE_COLOR_DEFAULT) {
		addNegative = addZero = true;
	}

	if (fmt.negativeColor != NUMSTYLE_COLOR_DEFAULT) {
		addNegative = true;
	}

	if (fmt.zeroColor != NUMSTYLE_COLOR_DEFAULT) {
		addZero = true;
	}

	_tstring thousandPrefix;
	if (fmt.showThousandsSeparator) {
		thousandPrefix = _T("#,##");
	}

	locale loc("");
	string char_currency = use_facet<moneypunct<char> >(loc).curr_symbol();
	char szCurrency[10] = { 0 };
#ifdef UNICODE
	std::wstring wsTmp(char_currency.begin(), char_currency.end());

	int32_t res = wcstombs(szCurrency, wsTmp.c_str(), sizeof(szCurrency));
	if (res == -1) return _T("");
#else
	_stprintf(szCurrency, _T("%s"), char_currency.c_str());
#endif
	_tstring currency = _tstring(_T("&quot;")) + (TCHAR)szCurrency + _T("&quot;");

	_tstring resCode;
	_tstring affix;
	_tstring digits = _T("0");
	if (fmt.numberOfDigitsAfterPoint != 0) {
		digits.append(_T("."));
		digits.append(fmt.numberOfDigitsAfterPoint, _T('0'));
	}

	switch (fmt.numberStyle) {
		case NUMSTYLE_EXPONENTIAL:
			affix = _T("E+00");
			break;
		case NUMSTYLE_PERCENTAGE:
			affix = _T("%");
			break;
		case NUMSTYLE_FINANCIAL:
			//_-* #,##0.00"$"_-;\-* #,##0.00"$"_-;_-* "-"??"$"_-;_-@_-

			resCode = 	GetFormatCodeColor(fmt.positiveColor) + _T("_-* ") + thousandPrefix + digits + currency + _T("_-;") +
						GetFormatCodeColor(fmt.negativeColor) + _T("\\-* ") + thousandPrefix + digits + currency + _T("_-;") +
						_T("_-* &quot;-&quot;??") + currency + _T("_-;_-@_-");
			break;
		case NUMSTYLE_MONEY:
			affix = currency;
			break;
		case NUMSTYLE_DATETIME:
			resCode = _T("yyyy.mm.dd hh:mm:ss");
			break;
		case NUMSTYLE_DATE:
			resCode = _T("yyyy.mm.dd");
			break;
		case NUMSTYLE_TIME:
			resCode = _T("hh:mm:ss");
			break;

		case NUMSTYLE_GENERAL:
		case NUMSTYLE_NUMERIC:
		default:
			affix = _T("");
			break;
	}

	if (fmt.numberStyle == NUMSTYLE_GENERAL || fmt.numberStyle == NUMSTYLE_NUMERIC ||
		fmt.numberStyle == NUMSTYLE_EXPONENTIAL || fmt.numberStyle == NUMSTYLE_PERCENTAGE ||
		fmt.numberStyle == NUMSTYLE_MONEY) {

		resCode = GetFormatCodeColor(fmt.positiveColor) + thousandPrefix + digits + affix;
		if (addNegative) {
			resCode += _T(";") + GetFormatCodeColor(fmt.negativeColor) + _T("\\-") + thousandPrefix + digits + affix;
		}
		if (addZero) {
			if (addNegative == false) resCode += _T(";");
			resCode += _T(";") + GetFormatCodeColor(fmt.zeroColor) + _T("0") + affix;
		}
	}

	return resCode;
}

// ****************************************************************************
/// @brief  Converts numeric format color into its std::string representation
/// @param  color color code
/// @return std::string color code
// ****************************************************************************
_tstring CWorkbook::GetFormatCodeColor(ENumericStyleColor color)
{
	switch (color) {
		case NUMSTYLE_COLOR_BLACK:	return _T("[BLACK]");
		case NUMSTYLE_COLOR_GREEN:	return _T("[Green]");
		case NUMSTYLE_COLOR_WHITE:	return _T("[White]");
		case NUMSTYLE_COLOR_BLUE:	return _T("[Blue]");
		case NUMSTYLE_COLOR_MAGENTA:return _T("[Magenta]");
		case NUMSTYLE_COLOR_YELLOW:	return _T("[Yellow]");
		case NUMSTYLE_COLOR_CYAN:	return _T("[Cyan]");
		case NUMSTYLE_COLOR_RED:	return _T("[Red]");

		case NUMSTYLE_COLOR_DEFAULT:
		default:					return _T("");
	}
}

// ****************************************************************************
/// @brief  Appends fonts section into styles file
/// @param  stream reference to xml stream
/// @return no
// ****************************************************************************
void CWorkbook::AddFonts(xmlw::XmlStream& stream) const
{
    const int32_t defaultCharset = 204;
    vector<Font> fonts = m_styleList.GetFonts();

    stream
    << tag(_T("fonts")) << attr(_T("count")) << fonts.size();

    for (size_t i = 0; i < fonts.size(); i++) {
        Font &font = fonts[i];

        stream
        << tag(_T("font"));

        if (font.attributes & FONT_BOLD)
            stream << tag(_T("b")) << endtag();
        if (font.attributes & FONT_ITALIC)
            stream << tag(_T("i")) << endtag();
        if (font.attributes & FONT_UNDERLINED)
            stream << tag(_T("u")) << endtag();
        if (font.attributes & FONT_STRIKE)
            stream << tag(_T("strike")) << endtag();
        if (font.attributes & FONT_OUTLINE)
            stream << tag(_T("outline")) << endtag();
        if (font.attributes & FONT_SHADOW)
            stream << tag(_T("shadow")) << endtag();
        if (font.attributes & FONT_CONDENSE)
            stream << tag(_T("condense")) << endtag();
        if (font.attributes & FONT_EXTEND)
            stream << tag(_T("extend")) << endtag();

        stream
            << tag(_T("sz")) << attr(_T("val")) << font.size << endtag()
            << tag(_T("name")) << attr(_T("val")) << font.name << endtag()
            << tag(_T("charset")) << attr(_T("val")) << defaultCharset << endtag();

        if (font.theme || font.color == _T("")) {
            stream
            << tag(_T("color")) << attr(_T("theme")) << 1 << endtag();
        }
        else {
            stream
            << tag(_T("color")) << attr(_T("rgb")) << font.color.c_str() << endtag();
        }

        stream
        << endtag();
    }

	stream
    << endtag();
}

// ****************************************************************************
/// @brief  Appends fills section into styles file
/// @param  stream reference to xml stream
/// @return no
// ****************************************************************************
void CWorkbook::AddFills(xmlw::XmlStream& stream) const
{
    vector<Fill> fills = m_styleList.GetFills();

    stream
    << tag(_T("fills")) << attr(_T("count")) << fills.size();

    _tstring strPattern;
    for (size_t i = 0; i < fills.size(); i++) {
        Fill &fill = fills[i];

        stream
        << tag(_T("fill"))
            << tag(_T("patternFill")) << attr(_T("patternType"));

        switch (fill.patternType) {
        case PATTERN_DARK_DOWN:     strPattern = _T("darkDown"); break;
        case PATTERN_DARK_GRAY:     strPattern = _T("darkGray"); break;
        case PATTERN_DARK_GRID:     strPattern = _T("darkGrid"); break;
        case PATTERN_DARK_HORIZ:    strPattern = _T("darkHorizontal"); break;
        case PATTERN_DARK_TRELLIS:  strPattern = _T("darkTrellis"); break;
        case PATTERN_DARK_UP:       strPattern = _T("darkUp"); break;
        case PATTERN_DARK_VERT:     strPattern = _T("darkVertical"); break;
        case PATTERN_GRAY_0625:     strPattern = _T("gray0625"); break;
        case PATTERN_GRAY_125:      strPattern = _T("gray125"); break;
        case PATTERN_LIGHT_DOWN:    strPattern = _T("lightDown"); break;
        case PATTERN_LIGHT_GRAY:    strPattern = _T("lightGray"); break;
        case PATTERN_LIGHT_GRID:    strPattern = _T("lightGrid"); break;
        case PATTERN_LIGHT_HORIZ:   strPattern = _T("lightHorizontal"); break;
        case PATTERN_LIGHT_TRELLIS: strPattern = _T("lightTrellis"); break;
        case PATTERN_LIGHT_UP:      strPattern = _T("lightUp"); break;
        case PATTERN_LIGHT_VERT:    strPattern = _T("lightVertical"); break;
        case PATTERN_MEDIUM_GRAY:   strPattern = _T("mediumGray"); break;
        case PATTERN_NONE:          strPattern = _T("none"); break;
        case PATTERN_SOLID:         strPattern = _T("solid"); break;
        }

        stream << strPattern.c_str();

        if (fill.bgColor != _T("")) {
            stream
                << tag(_T("bgColor")) << attr(_T("rgb")) << fill.bgColor.c_str() << endtag();
        }
        if (fill.fgColor != _T("")) {
            stream
                << tag(_T("fgColor")) << attr(_T("rgb")) << fill.fgColor.c_str() << endtag();
        }

        stream
            << endtag()
        << endtag();
    }

    stream
    << endtag();
}

// ****************************************************************************
/// @brief  Appends borders section into styles file
/// @param  stream reference to xml stream
/// @return no
// ****************************************************************************
void CWorkbook::AddBorders(xmlw::XmlStream& stream) const
{
    vector<Border> borders = m_styleList.GetBorders();

    stream
    << tag(_T("borders")) << attr(_T("count")) << borders.size();

    for (size_t i = 0; i < borders.size(); i++) {
        stream
        << tag(_T("border"));

        if (borders[i].isDiagonalUp) {
			stream << attr(_T("diagonalUp")) << 1;
        }

        if (borders[i].isDiagonalDown) {
			stream << attr(_T("diagonalDown")) << 1;
        }

		AddBorder(stream, _T("left"), borders[i].left);
		AddBorder(stream, _T("right"), borders[i].right);
		AddBorder(stream, _T("top"), borders[i].top);
		AddBorder(stream, _T("bottom"), borders[i].bottom);
		AddBorder(stream, _T("diagonal"), borders[i].diagonal);

        stream
        << endtag();
    }

    stream
    << endtag();
}

// ****************************************************************************
/// @brief  Appends border item section into borders set
/// @param  stream reference to xml stream
/// @param	borderName border name
/// @param	border style set
/// @return no
// ****************************************************************************
void CWorkbook::AddBorder(xmlw::XmlStream& stream, const TCHAR *borderName, Border::BorderItem border) const
{
	_tstring sStyle;

	switch(border.style) {
	case BORDER_THIN:				sStyle = _T("thin"); break;
	case BORDER_MEDIUM:				sStyle = _T("medium"); break;
	case BORDER_DASHED:				sStyle = _T("dashed"); break;
	case BORDER_DOTTED:				sStyle = _T("dotted"); break;
	case BORDER_THICK:				sStyle = _T("thick"); break;
	case BORDER_DOUBLE:				sStyle = _T("double"); break;
	case BORDER_HAIR:				sStyle = _T("hair"); break;
	case BORDER_MEDIUM_DASHED:		sStyle = _T("mediumDashed"); break;
	case BORDER_DASH_DOT:			sStyle = _T("dashDot"); break;
	case BORDER_MEDIUM_DASH_DOT:	sStyle = _T("mediumDashDot"); break;
	case BORDER_DASH_DOT_DOT:		sStyle = _T("dashDotDot"); break;
	case BORDER_MEDIUM_DASH_DOT_DOT:sStyle = _T("mediumDashDotDot"); break;
	case BORDER_SLANT_DASH_DOT:		sStyle = _T("slantDashDot"); break;

	case BORDER_NONE:
	default:						break;
	}

	stream << tag(borderName);

	if (sStyle != _T("")) {
		stream << attr(_T("style")) << sStyle.c_str();

		stream << tag(_T("color"));
		if (border.color != _T("")) {
			stream << attr(_T("rgb")) << border.color.c_str();
		}
		else {
			stream << attr(_T("indexed")) << 64;
		}
		stream << endtag();
	}

	stream << endtag();
}

// ****************************************************************************
/// @brief  ...
/// @return Boolean result of the operation
// ****************************************************************************
bool CWorkbook::SaveChain()
{
    // [- zip/xl/calcChain.xml
    const TCHAR *szFilename = _T("xl/calcChain.xml");
    TCHAR szPathToFile[MAX_PATH] = { 0 };
    _stprintf(szPathToFile, _T("%s/%s"), m_temp_path.c_str(), szFilename);
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

    m_contentFiles.push_back(szFilename);

    xml_stream << prolog()
    << tag(_T("calcChain")) << attr(_T("xmlns")) << ns_book;

    for (size_t i = 0; i < m_worksheets.size(); i++) {
        if (m_worksheets[i]->IsThereFormula()) {
            vector<_tstring> vstrChain;
            m_worksheets[i]->GetCalcChain(vstrChain);

            for (size_t j = 0; j < vstrChain.size(); j++) {
                xml_stream << tag(_T("c"));
                if (j == 0) xml_stream << attr(_T("i")) << i+1;
                xml_stream << attr(_T("r")) << vstrChain[j].c_str() << endtag();
            }
        }
    }
    // zip/xl/calcChain.xml -]

    return true;
}

// ****************************************************************************
/// @brief  ...
/// @return Boolean result of the operation
// ****************************************************************************
bool CWorkbook::SaveComments()
{
	if (m_comments.empty()) return true;

	std::vector<Comment*> sheet_comments;
	std::vector<std::vector<Comment*> > comments;

	std::sort(m_comments.begin(), m_comments.end());

	int active_sheet = m_comments[0].sheetIndex;
	for (size_t i = 0; i < m_comments.size(); i++) {
		if (m_comments[i].sheetIndex == active_sheet) {
			sheet_comments.push_back(&m_comments[i]);
		}
		else {
			active_sheet = m_comments[i].sheetIndex;
			comments.push_back(sheet_comments);
			sheet_comments.clear();

			i--;
		}
	}

	comments.push_back(sheet_comments);
	sheet_comments.clear();

	for (size_t i = 0; i < comments.size(); i++) {
		if (SaveCommentList(comments[i]) == false)
			return false;
	}

	return true;
}

// ****************************************************************************
/// @brief  ...
/// @param	comments comment list on the sheet
/// @return Boolean result of the operation
// ****************************************************************************
bool CWorkbook::SaveCommentList(std::vector<Comment*> &comments)
{
	// [- zip/xl/commentsN.xml
	{
		TCHAR szFilename[MAX_PATH] = { 0 };
		_stprintf(szFilename, _T("xl/comments%d.xml"), comments[0]->sheetIndex);
		TCHAR szPathToFile[MAX_PATH] = { 0 };
		_stprintf(szPathToFile, _T("%s/%s"), m_temp_path.c_str(), szFilename);
		MakeDirectory(szPathToFile);
		char szPath[MAX_PATH] = { 0 };

		m_contentFiles.push_back(szFilename);

#ifdef UNICODE
		int32_t res = wcstombs(szPath, szPathToFile, MAX_PATH);
		if (res == -1) return false;
#else
		_stprintf(szPath, _T("%s"), szPathToFile);
#endif
		_tofstream f(szPath);
		XmlStream xml_stream(f);

		TCHAR szUserName[MAX_PATH] = { 0 };
        TCHAR *szTempName = _tgetenv(_T("USERNAME"));
        if (szTempName != NULL) _stprintf(szUserName, _T("%s"), szTempName);
        else _stprintf(szUserName, _T("Unknown"));

		xml_stream << prolog()
		<< tag(_T("comments")) << attr(_T("xmlns")) << ns_book
			<< tag(_T("authors"))
				<< tag(_T("author")) << chardata() << szUserName << endtag()
			<< endtag()
			<< tag(_T("commentList"));

		for (size_t i = 0; i < comments.size(); i++) {
			AddComment(xml_stream, *(comments[i]));
		}

		xml_stream
			<< endtag()
		<< endtag();
	}
	// zip/xl/commentsN.xml -]

	// [- zip/xl/drawings/vmlDrawingN.xml
	{
		TCHAR szFilename[MAX_PATH] = { 0 };
		_stprintf(szFilename, _T("xl/drawings/vmlDrawing%d.vml"), comments[0]->sheetIndex);
		TCHAR szPathToFile[MAX_PATH] = { 0 };
		_stprintf(szPathToFile, _T("%s/%s"), m_temp_path.c_str(), szFilename);
		MakeDirectory(szPathToFile);
		char szPath[MAX_PATH] = { 0 };

		_tstring sFilename = szFilename;
		m_contentFiles.push_back(sFilename);

#ifdef UNICODE
		int32_t res = wcstombs(szPath, szPathToFile, MAX_PATH);
		if (res == -1) return false;
#else
		_stprintf(szPath, _T("%s"), szPathToFile);
#endif
		_tofstream f(szPath);
		XmlStream xml_stream(f);

		xml_stream
		<< tag(_T("xml"))
			<< attr(_T("xmlns:v")) << _T("urn:schemas-microsoft-com:vml")
			<< attr(_T("xmlns:o")) << _T("urn:schemas-microsoft-com:office:office")
			<< attr(_T("xmlns:x")) << _T("urn:schemas-microsoft-com:office:excel")

			<< tag(_T("o:shapelayout")) << attr(_T("v:ext")) << _T("edit")
				<< tag(_T("o:idmap")) << attr(_T("v:ext")) << _T("edit") << attr(_T("data")) << 1 << endtag()
			<< endtag()

			<< tag(_T("v:shapetype"))
				<< attr(_T("id")) << _T("_x0000_t202")
				<< attr(_T("coordsize")) << _T("21600,21600")
				<< attr(_T("o:spt")) << 202
				<< attr(_T("path")) << _T("m,l,21600r21600,l21600,xe")

				<< tag(_T("v:stroke")) << attr(_T("joinstyle")) << _T("miter") << endtag()
				<< tag(_T("v:path")) << attr(_T("gradientshapeok")) << _T("t") << attr(_T("o:connecttype")) << _T("rect") << endtag()
			<< endtag();


		for (size_t i = 0; i < comments.size(); i++) {
			AddCommentDrawing(xml_stream, *(comments[i]));
		}

		//xml_stream
		//<< endtag();
	}
	// zip/xl/drawings/vmlDrawingN.xml -]

	return true;
}

// ****************************************************************************
/// @brief  ...
/// @param  stream reference to xml stream
/// @param	comments comment list on the sheet
/// @return Boolean result of the operation
// ****************************************************************************
void CWorkbook::AddComment(xmlw::XmlStream &xml_stream, const Comment &comment) const
{
	TCHAR szCell[15] = { 0 };
	CWorksheet::GetCellCoord(comment.cellRef, szCell);

	xml_stream
	<< tag(_T("comment")) << attr(_T("ref")) << szCell << attr(_T("authorId")) << 0
		<< tag(_T("text"));

	for (size_t r = 0; r < comment.contents.size(); r++) {
		xml_stream
			<< tag(_T("r"))
				<< tag(_T("rPr"));

        const Font &font = comment.contents[r].first;
        if (font.attributes & FONT_BOLD)
            xml_stream << tag(_T("b")) << endtag();
        if (font.attributes & FONT_ITALIC)
            xml_stream << tag(_T("i")) << endtag();
        if (font.attributes & FONT_UNDERLINED)
            xml_stream << tag(_T("u")) << endtag();
        if (font.attributes & FONT_STRIKE)
            xml_stream << tag(_T("strike")) << endtag();
        if (font.attributes & FONT_OUTLINE)
            xml_stream << tag(_T("outline")) << endtag();
        if (font.attributes & FONT_SHADOW)
            xml_stream << tag(_T("shadow")) << endtag();
        if (font.attributes & FONT_CONDENSE)
            xml_stream << tag(_T("condense")) << endtag();
        if (font.attributes & FONT_EXTEND)
            xml_stream << tag(_T("extend")) << endtag();

        xml_stream
            << tag(_T("sz")) << attr(_T("val")) << font.size << endtag()
            << tag(_T("rFont")) << attr(_T("val")) << font.name << endtag()
            << tag(_T("charset")) << attr(_T("val")) << 1 << endtag();

        if (font.theme || font.color == _T("")) {
            xml_stream
            << tag(_T("color")) << attr(_T("theme")) << 1 << endtag();
        }
        else {
            xml_stream
            << tag(_T("color")) << attr(_T("rgb")) << font.color.c_str() << endtag();
        }

		xml_stream
				<< endtag()
				<< tag(_T("t")) << chardata() << comment.contents[r].second.c_str() << endtag()
			<< endtag();
	}

	xml_stream
		<< endtag()
	<< endtag();
}

// ****************************************************************************
/// @brief  ...
/// @param  stream reference to xml stream
/// @param	comments comment list on the sheet
/// @return Boolean result of the operation
// ****************************************************************************
void CWorkbook::AddCommentDrawing(xmlw::XmlStream &xml_stream, const Comment &comment) const
{
	TCHAR szId[20] = { 0 };
	_stprintf(szId, _T("_x0000_s%d"), 1000 + (++m_commLastId));

	TCHAR szStyle[MAX_PATH] = { 0 };
	if (comment.x >= 0 && comment.y >= 0 && comment.width > 0 && comment.height > 0) {
		_stprintf(szStyle, _T("position:absolute; margin-left:%dpt;margin-top:%dpt;width:%dpt;height:%dpt; z-index:2"),
					comment.x, comment.y, comment.width, comment.height);
	}

	if (comment.isHidden) {
		_tcscat(szStyle, _T(";visibility:hidden"));
	}

	bool wrapText = false;
	for (size_t i = 0; i < comment.contents.size(); i++) {
		if (comment.contents[i].second.find(_T("\n")) != std::string::npos) {
			wrapText = true;
			break;
		}
	}

	if (wrapText) {
		_tcscat(szStyle, _T(";mso-wrap-style:tight"));
	}

	xml_stream
	<< tag(_T("v:shape"))
		<< attr(_T("id")) << szId
		<< attr(_T("type")) << _T("#_x0000_t202")
		<< attr(_T("style")) << szStyle
		<< attr(_T("fillcolor")) << comment.fillColor.c_str()
		<< attr(_T("o:insetmode")) << _T("auto")

		<< tag(_T("v:fill")) << attr(_T("color2")) << comment.fillColor.c_str() << endtag()
		<< tag(_T("v:shadow")) << attr(_T("on")) << _T("t") << attr(_T("color")) << _T("black") << attr(_T("obscured")) << _T("t") << endtag()
		<< tag(_T("v:path")) << attr(_T("o:connecttype")) << _T("none") << endtag()
		<< tag(_T("v:textbox")) << attr(_T("style")) << _T("mso-direction-alt:auto")
			<< tag(_T("div")) << attr(_T("style")) << _T("text-align:left") << endtag()
		<< endtag()
		<< tag(_T("x:ClientData")) << attr(_T("ObjectType")) << _T("Note")
			<< tag(_T("x:MoveWithCells")) << endtag()
			<< tag(_T("x:SizeWithCells")) << endtag()
			//<< tag(_T("x:Anchor")) << chardata() << "...." << endtag()
			<< tag(_T("x:AutoFill")) << chardata() << _T("False") << endtag()
			<< tag(_T("x:Row")) << chardata() << comment.cellRef.row - 1 << endtag()
			<< tag(_T("x:Column")) << chardata() << comment.cellRef.col << endtag()
		<< endtag()
	<<endtag();
}

// ****************************************************************************
/// @brief  ...
/// @return Boolean result of the operation
// ****************************************************************************
bool CWorkbook::SaveSharedStrings()
{
	// [- zip/xl/sharedStrings.xml
    if (m_sharedStrings.empty()) return true;

    const TCHAR *szFilename = _T("xl/sharedStrings.xml");
    TCHAR szPathToFile[MAX_PATH] = { 0 };
    _stprintf(szPathToFile, _T("%s/%s"), m_temp_path.c_str(), szFilename);
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

    m_contentFiles.push_back(szFilename);

    xml_stream << prolog()
    << tag(_T("sst")) << attr(_T("xmlns")) << ns_book << attr(_T("count")) << m_sharedStrings.size() << attr(_T("uniqueCount")) << m_sharedStrings.size();

    vector<_tstring*> pointers_to_hash;
    pointers_to_hash.resize(m_sharedStrings.size());
    for (map<_tstring, uint64_t>::iterator it = m_sharedStrings.begin(); it != m_sharedStrings.end(); it++) {
        pointers_to_hash[it->second] = const_cast<_tstring*>(&it->first);
    }

    for (size_t i = 0; i < pointers_to_hash.size(); i++) {
        xml_stream
        << tag(_T("si")) << tag(_T("t")) << chardata() << *(pointers_to_hash[i]) << endtag() << endtag();
    }
    // zip/xl/sharedStrings.xml -]

    return true;
}

// ****************************************************************************
/// @brief  ...
/// @return Boolean result of the operation
// ****************************************************************************
bool CWorkbook::SaveWorkbook() const
{
    {
		// [- zip/xl/_rels/workbook.xml.rels
		TCHAR szPathToFile[MAX_PATH] = { 0 };
        _stprintf(szPathToFile, _T("%s/%s"), m_temp_path.c_str(), m_contentFiles[6].c_str());
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
		<< tag(_T("Relationships")) << attr(_T("xmlns")) << ns_relationships
            << tag(_T("Relationship")) << attr(_T("Id")) << _T("rId1") << attr(_T("Type")) << type_style << attr(_T("Target")) << _T("styles.xml")
            << endtag()
            << tag(_T("Relationship")) << attr(_T("Id")) << _T("rId2") << attr(_T("Type")) << type_theme << attr(_T("Target")) << _T("theme/theme1.xml")
            << endtag();

        TCHAR szId[10] = { 0 };
        TCHAR szPropValue[100] = { 0 };

        int32_t id = 3;
        bool bFormula = false;
        for (size_t i = 0; i < m_worksheets.size(); i++) {
            _stprintf(szId, _T("rId%d"), id++);
            _stprintf(szPropValue, _T("worksheets/sheet%d.xml"), m_worksheets[i]->GetIndex());

            xml_stream
            << tag(_T("Relationship")) << attr(_T("Id")) << szId << attr(_T("Type")) << type_sheet << attr(_T("Target")) << szPropValue
            << endtag();

            if (m_worksheets[i]->IsThereFormula()) bFormula = true;
        }

        if (bFormula) {
            _stprintf(szId, _T("rId%d"), id++);

            xml_stream
            << tag(_T("Relationship")) << attr(_T("Id")) << szId << attr(_T("Type")) << type_chain << attr(_T("Target")) << _T("calcChain.xml")
            << endtag();
        }

        for (size_t i = 0; i < m_charts.size(); i++) {
            _stprintf(szId, _T("rId%d"), id++);
            _stprintf(szPropValue, _T("chartsheets/sheet%d.xml"), m_charts[i]->GetIndex());

            xml_stream
            << tag(_T("Relationship")) << attr(_T("Id")) << szId << attr(_T("Type")) << type_chartsheet << attr(_T("Target")) << szPropValue
            << endtag();
        }

        if (!m_sharedStrings.empty()) {
			_stprintf(szId, _T("rId%d"), id++);
			xml_stream
			<< tag(_T("Relationship")) << attr(_T("Id")) << szId << attr(_T("Type")) << type_sharedStr << attr(_T("Target")) << _T("sharedStrings.xml")
            << endtag();
        }
		// zip/xl/_rels/workbook.xml.rels -]
	}

	{
		// [- zip/xl/workbook.xml
		TCHAR szPathToFile[MAX_PATH] = { 0 };
        _stprintf(szPathToFile, _T("%s/%s"), m_temp_path.c_str(), m_contentFiles[4].c_str());
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
		<< tag(_T("workbook")) << attr(_T("xmlns")) << ns_book << attr(_T("xmlns:r")) << ns_book_r
            << tag(_T("fileVersion"))   << attr(_T("appName")) << _T("xl") << attr(_T("lastEdited")) << 5
										<< attr(_T("lowestEdited")) << 5 << attr(_T("rupBuild")) << 9302
            << endtag()
            << tag(_T("workbookPr")) << attr(_T("codeName")) << _T("ThisWorkbook") << attr(_T("defaultThemeVersion")) << 124226
            << endtag()
            << tag(_T("bookViews"))
                << tag(_T("workbookView"))  << attr(_T("xWindow")) << 270 << attr(_T("yWindow")) << 630
											<< attr(_T("windowWidth")) << 24615 << attr(_T("windowHeight")) << 11445
                << endtag()
            << endtag();

		TCHAR szId[10] = { 0 };
        TCHAR szPropValue[100] = { 0 };

        if ( !m_worksheets.empty() || !m_charts.empty() ) {
            xml_stream << tag(_T("sheets"));
        }

        int32_t sheetId = 1;
        int32_t rId = 3;
        bool bFormula = false;
        for (size_t i = 0; i < m_worksheets.size(); i++) {
            _stprintf(szId, _T("rId%d"), rId++);
            _stprintf(szPropValue, _T("worksheets/sheet%d.xml"), m_worksheets[i]->GetIndex());

            xml_stream << tag(_T("sheet"))  << attr(_T("name")) << m_worksheets[i]->GetTitle()
											<< attr(_T("sheetId")) << sheetId++
											<< attr(_T("r:id")) << szId << endtag();

            if (m_worksheets[i]->IsThereFormula()) bFormula = true;
        }

        if (bFormula) rId++;

        for (size_t i = 0; i < m_charts.size(); i++) {
            _stprintf(szId, _T("rId%d"), rId++);
            _stprintf(szPropValue, _T("chartsheets/sheet%d.xml"), m_charts[i]->GetIndex());

            xml_stream << tag(_T("sheet"))  << attr(_T("name")) << m_charts[i]->GetTitle()
											<< attr(_T("sheetId")) << sheetId++
											<< attr(_T("r:id")) << szId << endtag();
        }

        if ( !m_worksheets.empty() || !m_charts.empty() ) {
            xml_stream << endtag();
        }

        xml_stream << tag(_T("calcPr")) << attr(_T("calcId")) << 124519 << endtag();
		// zip/xl/workbook.xml -]
	}

    return true;
}

// ****************************************************************************
/// @brief  Deletes all temporary files and directories which have been created
/// @return no
// ****************************************************************************
void CWorkbook::ClearTemp()
{
    TCHAR szPath[MAX_PATH] = { 0 };
    for (size_t i = 0; i < m_contentFiles.size(); i++) {
    	_stprintf(szPath, _T("%s/%s"), m_temp_path.c_str(), m_contentFiles[i].c_str());
        _tremove(szPath);
    }

    if (m_worksheets.empty() == false) {
        if (m_comments.empty() == false) {
			_trmdir((m_temp_path + _T("/xl/worksheets/_rels")).c_str());
			_trmdir((m_temp_path + _T("/xl/drawings")).c_str());
        }

        _trmdir((m_temp_path + _T("/xl/worksheets")).c_str());
    }
    if (m_charts.empty() == false) {
        _trmdir((m_temp_path + _T("/xl/charts")).c_str());
        _trmdir((m_temp_path + _T("/xl/chartsheets/_rels")).c_str());
        _trmdir((m_temp_path + _T("/xl/chartsheets")).c_str());
        _trmdir((m_temp_path + _T("/xl/drawings/_rels")).c_str());
        _trmdir((m_temp_path + _T("/xl/drawings")).c_str());
    }

    _trmdir((m_temp_path + _T("/xl/_rels")).c_str());
    _trmdir((m_temp_path + _T("/xl/theme")).c_str());
    _trmdir((m_temp_path + _T("/xl")).c_str());
    _trmdir((m_temp_path + _T("/docProps")).c_str());
    _trmdir((m_temp_path + _T("/_rels")).c_str());
    _trmdir(m_temp_path.c_str());
}

}	// namespace SimpleXlsx
