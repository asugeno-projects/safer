#ifndef __WORKBOOK_H__
#define __WORKBOOK_H__

#include "Worksheet.h"
#include "Chartsheet.h"
#include "SimpleXlsxDef.h"

namespace xmlw {
	class XmlStream;
}

namespace SimpleXlsx {
bool MakeDirectory(const _tstring& dirName);

/// @brief  This structure represents a handle to manage newly adding styles to avoid dublicating
class StyleList {
public:
	static const size_t BUILT_IN_STYLES_NUMBER = 164;
	static const size_t STYLE_LINK_NUMBER = 4;

	enum {
		STYLE_LINK_BORDER = 0,
		STYLE_LINK_FONT,
		STYLE_LINK_FILL,
		STYLE_LINK_NUM_FORMAT
	};

private:
	size_t fmtLastId;				///< m_fmtLastId format counter. There are 164 (0 - 163) built-in numeric formats

	std::vector<Border> borders;	///< borders set of values represent styled borders
	std::vector<Font> fonts;		///< fonts set of fonts to be declared
	std::vector<Fill> fills;		///< fills set of fills to be declared
	std::vector<NumFormat> nums;	///< nums set of number formats to be declared
	std::vector<std::vector<size_t> > styleIndexes;///< styleIndexes vector of a number triplet contains links to style parts:
																				///         first - border id in borders
																				///         second - font id in fonts
																				///         third - fill id in fills
																				///			fourth - number format id in nums
	std::vector< std::pair<std::pair<EAlignHoriz, EAlignVert>, bool> > stylePos;///< stylePos vector of a number triplet contains style`s alignments and wrap sign:
																				///         first - EAlignHoriz value
																				///         second - EAlignVert value
																				///         third - wrap boolean value
public:
	StyleList() {
		fmtLastId = BUILT_IN_STYLES_NUMBER;

		borders.clear();
		fonts.clear();
		fills.clear();
		nums.clear();
		styleIndexes.clear();
		stylePos.clear();
	}

	/// @brief	For internal use (at the book saving)
	std::vector<Border> GetBorders()const { return borders; }
	/// @brief	For internal use (at the book saving)
	std::vector<Font> GetFonts()	const { return fonts; }
	/// @brief	For internal use (at the book saving)
	std::vector<Fill> GetFills()	const { return fills; }
	/// @brief	For internal use (at the book saving)
	std::vector<NumFormat> GetNumFormats()	const { return nums; }
	/// @brief	For internal use (at the book saving)
	std::vector<std::vector<size_t> > GetIndexes() const { return styleIndexes; }
	/// @brief	For internal use (at the book saving)
	std::vector< std::pair<std::pair<EAlignHoriz, EAlignVert>, bool> > GetPositions() const { return stylePos; }

	/// @brief  Adds a new style into collection if it is not exists yet
	/// @param  style Reference to the Style structure object
	/// @return Style index that should be used at data appending to a data sheet
	/// @note   If returned value is 0 - this is a default normal style and it is optional
	///         whether is will be added into column description or not
	///         (but better not to add to reduce size and resource consumption)
	size_t Add(const Style& style) {
		std::vector<size_t> styleLinks(STYLE_LINK_NUMBER);

		// Check border existance
		bool addItem = true;
		for (size_t i = 0; i < borders.size(); i++) {
			if (borders[i] == style.border) {
				addItem = false;
				styleLinks[STYLE_LINK_BORDER] = i;
				break;
			}
		}

		// Add border if it is not in collection yet
		if (addItem) {
			borders.push_back(style.border);
			styleLinks[STYLE_LINK_BORDER] = borders.size() - 1;
		}

		// Check font existance
		addItem = true;
		for (size_t i = 0; i < fonts.size(); i++) {
			if (fonts[i] == style.font) {
				addItem = false;
				styleLinks[STYLE_LINK_FONT] = i;
				break;
			}
		}

		// Add font if it is not in collection yet
		if (addItem) {
			fonts.push_back(style.font);
			styleLinks[STYLE_LINK_FONT] = fonts.size() - 1;
		}

		// Check fill existance
		addItem = true;
		for (size_t i = 0; i < fills.size(); i++) {
			if (fills[i] == style.fill) {
				addItem = false;
				styleLinks[STYLE_LINK_FILL] = i;
				break;
			}
		}

		// Add fill if it is not in collection yet
		if (addItem) {
			fills.push_back(style.fill);
			styleLinks[STYLE_LINK_FILL] = fills.size() - 1;
		}

		// Check number format existance
		addItem = true;
		for (size_t i = 0; i < nums.size(); i++) {
			if (nums[i] == style.numFormat) {
				addItem = false;
				styleLinks[STYLE_LINK_NUM_FORMAT] = i;
				break;
			}
		}

		// Add number format if it is not in collection yet
		if (addItem) {
			if (style.numFormat.id >= BUILT_IN_STYLES_NUMBER) {
				styleLinks[STYLE_LINK_NUM_FORMAT] = fmtLastId;
				style.numFormat.id = fmtLastId++;
			}
			else {
				styleLinks[STYLE_LINK_NUM_FORMAT] = nums.size();
			}

			nums.push_back(style.numFormat);
		}

		// Check style combination existance
		for (size_t i = 0; i < styleIndexes.size(); i++) {
			if (styleIndexes[i] == styleLinks &&
				stylePos[i].first.first == style.horizAlign &&
				stylePos[i].first.second == style.vertAlign &&
				stylePos[i].second == style.wrapText)
				return i;
		}

		std::pair<std::pair<EAlignHoriz, EAlignVert>, bool> pos;
		pos.first.first = style.horizAlign;
		pos.first.second = style.vertAlign;
		pos.second = style.wrapText;
		stylePos.push_back(pos);

		styleIndexes.push_back(styleLinks);
		return styleIndexes.size() - 1;
	}
};

// ****************************************************************************
/// @brief	The class CWorkbook is used to manage creation, population and saving .xlsx files
// ****************************************************************************
class CWorkbook {
	_tstring                    m_temp_path;		///< path to the temporary directory (unique for a book)
	std::vector<_tstring>    	m_contentFiles;		///< a series of relative file pathes to be saved inside xlsx archive
	std::vector<CWorksheet*>    m_worksheets;		///< a series of data sheets
	std::vector<CChartsheet*>   m_charts;			///< a series of chart sheets
	std::map<_tstring, uint64_t>m_sharedStrings;	///<
	std::vector<Comment>		m_comments;			///<

	mutable size_t				m_commLastId;		///< m_commLastId comments counter

public:
	// @section    Constructors / destructor
	CWorkbook();
	virtual ~CWorkbook();

	// @section    User interface
	StyleList m_styleList;

	CWorksheet& AddSheet(const _tstring& title);
	CWorksheet& AddSheet(const _tstring& title, std::vector<ColumnWidth>& colWidths);
	CWorksheet& AddSheet(const _tstring& title, uint32_t frozenWidth, uint32_t frozenHeight);
	CWorksheet& AddSheet(const _tstring& title, uint32_t frozenWidth, uint32_t frozenHeight, std::vector<ColumnWidth>& colWidths);

	CChartsheet& AddChart(const _tstring& title);
	CChartsheet& AddChart(const _tstring& title, EChartTypes type);

	bool Save(const _tstring& name);

private:
	void Init();

	bool SaveCore() const;
	bool SaveContentType();
	bool SaveApp() const;
	bool SaveTheme() const;
	bool SaveStyles() const;
	bool SaveChain();
	bool SaveComments();
	bool SaveSharedStrings();
	bool SaveWorkbook() const;

	bool SaveCommentList(std::vector<Comment*> &comments);
	void AddComment(xmlw::XmlStream &xml_stream, const Comment &comment) const;
	void AddCommentDrawing(xmlw::XmlStream &xml_stream, const Comment &comment) const;
	void AddNumberFormats(xmlw::XmlStream& stream) const;
	void AddFonts(xmlw::XmlStream& stream) const;
	void AddFills(xmlw::XmlStream& stream) const;
	void AddBorders(xmlw::XmlStream& stream) const;
	void AddBorder(xmlw::XmlStream& stream, const TCHAR *borderName, Border::BorderItem border) const;

	void ClearTemp();

	static _tstring GetFormatCodeString(const NumFormat &fmt);
	static _tstring GetFormatCodeColor(ENumericStyleColor color);
};

}	// namespace SimpleXlsx

#endif	// __WORKBOOK_H__
