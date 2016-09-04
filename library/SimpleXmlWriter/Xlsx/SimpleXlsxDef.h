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

#ifndef __SIMPLE_XLSX_DEF_H__
#define __SIMPLE_XLSX_DEF_H__

#include <stdint.h>
#include <vector>
#include <map>
#include <string>
#include <utility>
#include <ctime>
#include <fstream>

#ifdef _WIN32
#include <tchar.h>
#else
#include "../tchar.h"
#endif  // _WIN32

#ifdef _UNICODE
	typedef std::wofstream 		_tofstream;
	typedef std::wstring		_tstring;
	typedef std::wostringstream	_tstringstream;
	typedef std::wostream		_tstream;
#else
	typedef std::ofstream		_tofstream;
	typedef std::string			_tstring;
	typedef std::ostringstream	_tstringstream;
	typedef std::ostream		_tstream;
#endif // _UNICODE

#define SIMPLE_XLSX_VERSION	_T("0.20")

namespace SimpleXlsx {

/// @brief	Possible chart types
enum EChartTypes {
	CHART_NONE = -1,
	CHART_LINEAR = 0,
	CHART_BAR,
	CHART_SCATTER,
	CHART_PIE,
};

/// @brief  Possible border attributes
enum EBorderStyle {
	BORDER_NONE = 0,
	BORDER_THIN,
	BORDER_MEDIUM,
	BORDER_DASHED,
	BORDER_DOTTED,
	BORDER_THICK,
	BORDER_DOUBLE,
	BORDER_HAIR,
	BORDER_MEDIUM_DASHED,
	BORDER_DASH_DOT,
	BORDER_MEDIUM_DASH_DOT,
	BORDER_DASH_DOT_DOT,
	BORDER_MEDIUM_DASH_DOT_DOT,
	BORDER_SLANT_DASH_DOT
};

/// @brief  Additional font attributes
enum EFontAttributes {
	FONT_NORMAL     = 0,
	FONT_BOLD       = 1,
	FONT_ITALIC     = 2,
	FONT_UNDERLINED = 4,
	FONT_STRIKE     = 8,
	FONT_OUTLINE    = 16,
	FONT_SHADOW     = 32,
	FONT_CONDENSE   = 64,
	FONT_EXTEND     = 128,
};

/// @brief  Fill`s pattern type enumeration
enum EPatternType {
	PATTERN_NONE = 0,
	PATTERN_SOLID,
	PATTERN_MEDIUM_GRAY,
	PATTERN_DARK_GRAY,
	PATTERN_LIGHT_GRAY,
	PATTERN_DARK_HORIZ,
	PATTERN_DARK_VERT,
	PATTERN_DARK_DOWN,
	PATTERN_DARK_UP,
	PATTERN_DARK_GRID,
	PATTERN_DARK_TRELLIS,
	PATTERN_LIGHT_HORIZ,
	PATTERN_LIGHT_VERT,
	PATTERN_LIGHT_DOWN,
	PATTERN_LIGHT_UP,
	PATTERN_LIGHT_GRID,
	PATTERN_LIGHT_TRELLIS,
	PATTERN_GRAY_125,
	PATTERN_GRAY_0625
};

/// @brief	Text horizontal alignment enumerations
enum EAlignHoriz {
	ALIGN_H_NONE = 0,
	ALIGN_H_LEFT,
	ALIGN_H_CENTER,
	ALIGN_H_RIGHT
};

/// @brief	Text vertical alignment enumerations
enum EAlignVert {
	ALIGN_V_NONE = 0,
	ALIGN_V_TOP,
	ALIGN_V_CENTER,
	ALIGN_V_BOTTOM
};

/// @brief	Number styling most general enumeration
enum ENumericStyle {
	NUMSTYLE_GENERAL = 0,
	NUMSTYLE_NUMERIC,
	NUMSTYLE_PERCENTAGE,
	NUMSTYLE_EXPONENTIAL,
	NUMSTYLE_FINANCIAL,
	NUMSTYLE_MONEY,
	NUMSTYLE_DATE,
	NUMSTYLE_TIME,
	NUMSTYLE_DATETIME,
};

/// @brief	Number can be colored differently depending on whether it positive or negavite or zero
enum ENumericStyleColor {
	NUMSTYLE_COLOR_DEFAULT = 0,
	NUMSTYLE_COLOR_BLACK,
	NUMSTYLE_COLOR_GREEN,
	NUMSTYLE_COLOR_WHITE,
	NUMSTYLE_COLOR_BLUE,
	NUMSTYLE_COLOR_MAGENTA,
	NUMSTYLE_COLOR_YELLOW,
	NUMSTYLE_COLOR_CYAN,
	NUMSTYLE_COLOR_RED
};

/// @brief  Font describes a font that can be added into final document stylesheet
/// @see    EFontAttributes
class Font {
public:
	int32_t size;		///< size font size
	_tstring name;		///< name font name (there is no enumeration or preset values, it should be used carefully)
	bool theme;			///< theme if true then color is not taken into account
	_tstring color;		///< color color format: AARRGGBB - (alpha, red, green, blue). If empty default theme is used
	int32_t attributes;	///< combination of additinal font flags (EFontAttributes)

public:
	Font() {
		Clear();
	}

	void Clear() {
		size = 11;
		name = _T("Calibri");
		theme = true;
		color = _T("");
		attributes = FONT_NORMAL;
	}

	bool operator==(const Font& _font) const {
		return (size == _font.size && name == _font.name && theme == _font.theme &&
				color == _font.color && attributes == _font.attributes);
	}
};

/// @brief  Fill describes a fill that can be added into final document stylesheet
/// @note   Current version describes the pattern fill only
/// @see    EPatternType
class Fill {
public:
	EPatternType patternType;	///< patternType
	_tstring fgColor;			///< fgColor foreground color format: AARRGGBB - (alpha, red, green, blue). Can be left unset
	_tstring bgColor;			///< bgColor background color format: AARRGGBB - (alpha, red, green, blue). Can be left unset

public:
	Fill() {
		Clear();
	}

	void Clear() {
		patternType = PATTERN_NONE;
		fgColor = _T("");
		bgColor = _T("");
	}

	bool operator==(const Fill& _fill) const {
		return (patternType == _fill.patternType && fgColor == _fill.fgColor && bgColor == _fill.bgColor);
	}
};

/// @brief  Border describes a border style that can be added into final document stylesheet
/// @see    EBorderStyle
class Border {
public:
	/// @brief	BorderItem describes border items (e.g. left, right, bottom, top, diagonal sides)
	struct BorderItem {
		EBorderStyle style;		///< style border style
		_tstring color;			///< colour border colour format: AARRGGBB - (alpha, red, green, blue). Can be left unset

		BorderItem() {
			Clear();
		}

		void Clear() {
			style = BORDER_NONE;
			color = _T("");
		}

		bool operator==(const BorderItem& _borderItem) const {
			return (color == _borderItem.color && style == _borderItem.style);
		}
	};

public:
	BorderItem left;			///< left left side style
	BorderItem right;			///< right right side style
	BorderItem bottom;			///< bottom bottom side style
	BorderItem top;				///< top top side style
	BorderItem diagonal;		///< diagonal diagonal side style
	bool isDiagonalUp;			///< isDiagonalUp indicates whether this diagonal border should be used
	bool isDiagonalDown;		///< isDiagonalDown indicates whether this diagonal border should be used

public:
	Border() {
		Clear();
	}

	void Clear() {
		isDiagonalUp = false;
		isDiagonalDown = false;

		left.Clear();
		right.Clear();
		bottom.Clear();
		top.Clear();
	}

	bool operator==(const Border& _border) const {
		return (isDiagonalUp == _border.isDiagonalUp && isDiagonalDown == _border.isDiagonalDown &&
				left == _border.left && right == _border.right &&
				bottom == _border.bottom && top == _border.top);
	}
};

/// @brief	NumFormat helps to create a customized number format
class NumFormat {
public:
	mutable size_t id;					///< id internal format number (must not be changed manually)

	_tstring formatString;				///< formatString final format form. If set manually, then all other options are ignored

	ENumericStyle numberStyle;			///< numberStyle most general style definition from enumeration
	ENumericStyleColor positiveColor;	///< positiveColor positive numbers color (switched off by default)
	ENumericStyleColor negativeColor;	///< negativeColor negative numbers color (switched off by default)
	ENumericStyleColor zeroColor;		///< zeroColor zero color (switched off by default)
	bool showThousandsSeparator;		///< thousandsSeparator specifies whether to show thousands separator
	size_t numberOfDigitsAfterPoint;	///< numberOfDigitsAfterPoint number of digits after point

public:
	NumFormat() {
		Clear();
	}

	void Clear() {
		id = 164;
		formatString = _T("");

		numberStyle = NUMSTYLE_GENERAL;
		positiveColor = NUMSTYLE_COLOR_DEFAULT;
		negativeColor = NUMSTYLE_COLOR_DEFAULT;
		zeroColor = NUMSTYLE_COLOR_DEFAULT;
		showThousandsSeparator = false;
		numberOfDigitsAfterPoint = 2;
	}

	bool operator==(const NumFormat& _num) const {
		return (formatString == _num.formatString &&
				numberStyle == _num.numberStyle && numberOfDigitsAfterPoint == _num.numberOfDigitsAfterPoint &&
				positiveColor == _num.positiveColor && negativeColor == _num.negativeColor &&
				zeroColor == _num.zeroColor && showThousandsSeparator == _num.showThousandsSeparator);
	}
};

/// @brief  Style describes a set of styling parameter that can be used into final document
/// @see    EBorder
/// @see    EAlignHoriz
/// @see    EAlignVert
class Style {
public:
	Font font;				///< font structure object describes font
	Fill fill;				///< fill structure object describes fill
	Border border;			///< border combination of border attributes
	NumFormat numFormat;	///< numFormat structure object describes numeric cell representation
	EAlignHoriz horizAlign;	///< horizAlign cell content horizontal alignment value
	EAlignVert vertAlign;	///< vertAlign cell content vertical alignment value
	bool wrapText;			///< wrapText text wrapping property

public:
	Style() {
		Clear();
	}

	void Clear() {
		font.Clear();
		fill.Clear();
		border.Clear();
		horizAlign = ALIGN_H_NONE;
		vertAlign = ALIGN_V_NONE;
		wrapText = false;
	}
};

/// @brief	Cell coordinate structure
class CellCoord {
public:
	uint32_t row;	///< row (starts from 1)
	uint32_t col;	///< col (starts from 0)

public:
	CellCoord() { Clear(); }
	CellCoord(uint32_t _r, uint32_t _c) : row(_r), col(_c) {}

	void Clear() {
		row = 1;
		col = 0;
	}
};

/// @brief	Column width structure
class ColumnWidth {
public:
	uint32_t colFrom;	///< column range from (starts from 0)
	uint32_t colTo;		///< column range to (starts from 0)
	float width;		///< specified width

public:
	ColumnWidth() {colFrom = colTo = 0; width = 15;}
	ColumnWidth(uint32_t min, uint32_t max, float w) : colFrom(min), colTo(max), width(w) {}
};

class CellDataStr {
public:
    _tstring value;
    int32_t style_id;

public:
    CellDataStr() : value(_T("")), style_id(0) {}
    CellDataStr(const TCHAR *pStr) : value(pStr), style_id(0) {}
    CellDataStr(const _tstring &_str) : value(_str), style_id(0) {}

    CellDataStr& operator=(const CellDataStr& obj) {
		value = obj.value;
		style_id = obj.style_id;
		return *this;
    }

    CellDataStr& operator=(const TCHAR *pStr) {
		value = pStr;
		return *this;
    }

    CellDataStr& operator=(const _tstring &_str) {
		value = _str;
		return *this;
    }
};	///< cell data:style pair
class CellDataTime {
public:
    time_t value;
    int32_t style_id;

public:
    CellDataTime() : value(0), style_id(0) {}
    CellDataTime(time_t _val) : value(_val), style_id(0) {}

    CellDataTime& operator=(const CellDataTime& obj) {
		value = obj.value;
		style_id = obj.style_id;
		return *this;
    }

    CellDataTime& operator=(time_t _val) {
		value = _val;
		return *this;
    }
};	///< cell data:style pair
class CellDataInt {
public:
    int32_t value;
    int32_t style_id;

public:
    CellDataInt() : value(0), style_id(0) {}
    CellDataInt(int32_t _val) : value(_val), style_id(0) {}

    CellDataInt& operator=(const CellDataInt& obj) {
		value = obj.value;
		style_id = obj.style_id;
		return *this;
    }

    CellDataInt& operator=(int32_t _val) {
		value = _val;
		return *this;
    }
};	///< cell data:style pair
class CellDataUInt {
public:
    uint32_t value;
    int32_t style_id;

public:
    CellDataUInt() : value(0), style_id(0) {}
    CellDataUInt(uint32_t _val) : value(_val), style_id(0) {}

    CellDataUInt& operator=(const CellDataUInt& obj) {
		value = obj.value;
		style_id = obj.style_id;
		return *this;
    }

    CellDataUInt& operator=(uint32_t _val) {
		value = _val;
		return *this;
    }
};	///< cell data:style pair
class CellDataDbl {
public:
    double value;
    int32_t style_id;

public:
    CellDataDbl() : value(0.0), style_id(0) {}
    CellDataDbl(double _val) : value(_val), style_id(0) {}

    CellDataDbl& operator=(const CellDataDbl& obj) {
		value = obj.value;
		style_id = obj.style_id;
		return *this;
    }

    CellDataDbl& operator=(double _val) {
		value = _val;
		return *this;
    }
};	///< cell data:style pair
class CellDataFlt {
public:
    float value;
    int32_t style_id;

public:
    CellDataFlt() : value(0.0), style_id(0) {}
    CellDataFlt(float _val) : value(_val), style_id(0) {}

    CellDataFlt& operator=(const CellDataFlt& obj) {
		value = obj.value;
		style_id = obj.style_id;
		return *this;
    }

    CellDataFlt& operator=(float _val) {
		value = _val;
		return *this;
    }
};	///< cell data:style pair

/// @brief	This structure describes comment item that can added to a cell
struct Comment {
	int sheetIndex;										///< sheetIndex internal page index (must not be changed manually)
	std::vector<std::pair<Font, _tstring> > contents;	///< contents set of contents with specified fonts
	CellCoord cellRef;									///< cellRef reference to the cell
	_tstring fillColor;									///< fillColor comment box background colour (format: #RRGGBB)
	bool isHidden;										///< isHidden determines if comments box is hidden
	int x;												///< x absolute position in pt (can be left unset)
	int y;												///< y absolute position in pt (can be left unset)
	int width;											///< width width in pt (can be left unset)
	int height;											///< height height in pt (can be left unset)

	Comment() {
		Clear();
	}

	void Clear() {
		contents.clear();
		cellRef.Clear();
		fillColor = _T("#FFEFD5");	// papaya whip
		isHidden = true;
		x = y = 50;
		width = height = 100;
	}

	bool operator < (const Comment& _comm) const {
        return (sheetIndex < _comm.sheetIndex);
    }
};

}	// namespace SimpleXlsx

#endif // __SIMPLE_XLSX_DEF_H__
