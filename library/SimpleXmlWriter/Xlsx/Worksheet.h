#ifndef __WORKSHEET_H__
#define __WORKSHEET_H__

#include "../xmlwriter.h"

int32_t _stprintf(TCHAR *buffer, const TCHAR *format, ...);

namespace SimpleXlsx {
// ****************************************************************************
/// @brief	The class CWorksheet is used for creation and population
///         .xlsx file sheet table with data
// ****************************************************************************
class CWorksheet {
public:
	enum EPageOrientation {
		PAGE_PORTRAIT = 0,
		PAGE_LANDSCAPE
	};

private:
	_tofstream	            *m_f;				///< stl output file stream is used for xml writing
	xmlw::XmlStream         *m_xmlStream;       ///< xml output stream
	std::vector<_tstring>	m_calcChain;        ///< list of cells with formulae
	std::map<_tstring, uint64_t>*m_sharedStrings;///< pointer to the list of std::string supposed to be into shared area
	std::vector<Comment> 	*m_comments;		///< pointer to the list of comments
	std::vector<_tstring>	m_mergedCells;		///< list of merged cells` ranges (e.g. A1:B2)
	std::vector<_tstring>	m_filterCells;
	int32_t					m_index;            ///< current sheet number
	_tstring                m_temp_path;        ///< path to the temporary directory (unique for a book)
	_tstring             	m_title;            ///< page title
	bool                    m_withFormula;      ///< indicates whether the sheet contains formulae
	bool					m_withComments;		///< indicates whether the sheet contains any comments
	bool                    m_isOk;             ///< indicates initialization successfulness
	bool                    m_isDataPresented;  ///< indicates whether the sheet contains a data
	int32_t					m_row_index;        ///< since data add row-by-row it contains current row to write
	bool					m_row_opened;		///< indicates whether row tag is opened
	int32_t					m_current_column;	///< used at separate row generation - last cell column number to be added
	int32_t					m_offset_column;	///< used at entire row addition (implicit parameter for AddCell method)

	EPageOrientation		m_page_orientation;	///< defines page orientation for printing

public:
	// @section   SEC_CONSTRUCTOR Constructors
	CWorksheet();
	CWorksheet(uint32_t index, const _tstring& temp_path);
	CWorksheet(uint32_t index, std::vector<ColumnWidth>& colHeights, const _tstring& temp_path);
	CWorksheet(uint32_t index, uint32_t width, uint32_t height, const _tstring& temp_path);
	CWorksheet(uint32_t index, uint32_t width, uint32_t height, std::vector<ColumnWidth>& colHeights, const _tstring& temp_path);
	virtual ~CWorksheet();

	// @section    SEC_INTERNAL Interclass internal interface methods
	bool IsThereComment() const								{ return m_withComments; }
	bool IsThereFormula() const                             { return m_withFormula; }
	int32_t GetIndex() const                                    { return m_index; }
	void GetCalcChain(std::vector<_tstring>& chain) const{ chain = m_calcChain; }
	void SetSharedStr(std::map<_tstring, uint64_t>* share) 		{ m_sharedStrings = share; }
	void SetComments(std::vector<Comment>* share) {m_comments = share; }

	// @section    SEC_USER User interface
	_tstring GetTitle() const								{ return m_title; }
	void SetTitle(const _tstring& title)					{ m_title = title.length() > 31 ? title.substr(0, 31) : title; }

	void AddComment(const Comment& comment)	{
		if (m_comments != NULL) {
			m_comments->push_back(comment);
			m_comments->at(m_comments->size() - 1).sheetIndex = m_index;

			m_withComments = true;
		}
	}
	void SetPageOrientation(EPageOrientation orient)		{ m_page_orientation = orient; }

	void BeginRow(uint32_t height = 0);
	void AddCell();
	void AddCell(const CellDataStr& data);
	void AddCells(const std::vector<CellDataStr>& data);
	void AddCell(const CellDataTime& data);
	void AddCells(const std::vector<CellDataTime>& data);
	void AddCell(const CellDataInt& data);
	void AddCells(const std::vector<CellDataInt>& data);
	void AddCell(const CellDataUInt& data);
	void AddCells(const std::vector<CellDataUInt>& data);
	void AddCell(const CellDataDbl& data);
	void AddCells(const std::vector<CellDataDbl>& data);
	void AddCell(const CellDataFlt& data);
	void AddCells(const std::vector<CellDataFlt>& data);
	void EndRow();

	void AddRow(const std::vector<CellDataStr>& data, uint32_t offset = 0, uint32_t height = 0);
	void AddRow(const std::vector<CellDataTime>& data, uint32_t offset = 0, uint32_t height = 0);
	void AddRow(const std::vector<CellDataInt>& data, uint32_t offset = 0, uint32_t height = 0);
	void AddRow(const std::vector<CellDataUInt>& data, uint32_t offset = 0, uint32_t height = 0);
	void AddRow(const std::vector<CellDataDbl>& data, uint32_t offset = 0, uint32_t height = 0);
	void AddRow(const std::vector<CellDataFlt>& data, uint32_t offset = 0, uint32_t height = 0);

	void MergeCells(CellCoord cellFrom, CellCoord cellTo);
	void filterCells(CellCoord cellFrom, CellCoord cellTo);
	void GetCurrentCellCoord(CellCoord& currCell);

	bool Save();
	bool IsOk() const                                       { return m_isOk; }

	// @section    General functions
	static void GetCellCoord(CellCoord cell, TCHAR *szCoord);

private:
	void Init(uint32_t index, uint32_t frozenWidth, uint32_t frozenHeight, std::vector<ColumnWidth>& colHeights);
	void AddFrozenPane(uint32_t width, uint32_t height);

	template<typename T>
	void AddCellRoutine(T data, int32_t style);

	bool SaveCommentsRels();
};

// ****************************************************************************
/// @brief  Appends another a group of cells into a row
/// @param  data template data value
/// @param	style style index
/// @return no
// ****************************************************************************
template<typename T>
void CWorksheet::AddCellRoutine(T data, int32_t style)
{
	using namespace xmlw;

	TCHAR szCoord[15] = { 0 };
	GetCellCoord(CellCoord(m_row_index, m_offset_column + m_current_column), szCoord);
	(*m_xmlStream) << tag(_T("c")) << attr(_T("r")) << szCoord;

	if (style != 0) {  // default style is not necessary to sign explicitly
		(*m_xmlStream) << attr(_T("s")) << style;
	}

	(*m_xmlStream)
		<< tag(_T("v")) << chardata() << data << endtag()
	<< endtag(); // c

	m_current_column++;
}

// ****************************************************************************
/// @brief	Generates a header for another row
/// @param	height row height (default if 0)
/// @return	no
// ****************************************************************************
inline void CWorksheet::BeginRow(uint32_t height)
{
	using namespace xmlw;

	if (m_row_opened)
		(*m_xmlStream) << endtag(); // row

	(*m_xmlStream) << tag(_T("row")) << attr(_T("r")) << ++m_row_index << attr(_T("x14ac:dyDescent")) << 0.25;
	if (height != 0) {
		(*m_xmlStream) << attr(_T("ht")) << height << attr(_T("customHeight")) << 1;
	}

	m_current_column = 0;
	m_row_opened = true;
}

// ****************************************************************************
/// @brief	Adds a group of cells into a row
/// @param  data reference to the vector of pairs <std::string, style_index>
/// @return	no
// ****************************************************************************
inline void CWorksheet::AddCells(const std::vector<CellDataStr>& data)
{
	for (size_t i = 0; i < data.size(); i++)
		AddCell(data[i]);
}

// ****************************************************************************
/// @brief	Adds a group of cells into a row
/// @param  data reference to the vector of pairs <time_t, style_index>
/// @return	no
// ****************************************************************************
inline void CWorksheet::AddCells(const std::vector<CellDataTime>& data)
{
	for (size_t i = 0; i < data.size(); i++)
		AddCell(data[i]);
}

// ****************************************************************************
/// @brief	Closes previously began row
/// @return	no
// ****************************************************************************
inline void CWorksheet::EndRow()
{
	using namespace xmlw;

	if (m_row_opened) {
		(*m_xmlStream) << endtag(); // row
		m_row_opened = false;
	}
}

} // namespace SimpleXlsx

#endif	// __WORKSHEET_H__
