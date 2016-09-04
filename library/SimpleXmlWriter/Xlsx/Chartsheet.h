#ifndef __CHARTSHEET_H__
#define __CHARTSHEET_H__

namespace xmlw {
	class XmlStream;
}

// ****************************************************************************
/// @brief  The following namespace contains declarations of a number of classes
///         which allow writing .xlsx files with formulae and charts
/// @note   All classes inside the namepace are used together, so a separate
///         using will not guarantee stability and reliability
// ****************************************************************************
namespace SimpleXlsx {

class CWorksheet;

// ****************************************************************************
/// @brief	The class CChartsheet is used for creation sheet with a number of
///         charts.
/// @see    EChartTypes supplying types of charts
/// @note   All created chartsheets inside workbook will be allocated on its` own sheet
// ****************************************************************************
class CChartsheet {
public:
	/// @brief  General property
	enum EPosition {
		POS_NONE = 0,
		POS_LEFT,
		POS_RIGHT,
		POS_TOP,
		POS_BOTTOM,
		POS_LEFT_ASIDE,
		POS_RIGHT_ASIDE,
		POS_TOP_ASIDE,
		POS_BOTTOM_ASIDE
	};

	/// @brief  Axis grid detalization property
	enum EGridLines {
		GRID_NONE = 0,
		GRID_MAJOR,
		GRID_MINOR,
		GRID_MAJOR_N_MINOR
	};

	/// @brief  The enumeration contains values that determine diagramm diagram data table property
	enum ETableData {
		TBL_DATA_NONE = 0,
		TBL_DATA,
		TBL_DATA_N_KEYS
	};

	/// @brief  The enumeration contains values that demermine point where axis will pass through
	enum EAxisCross {
		CROSS_AUTO_ZERO = 0,
		CROSS_MIN,
		CROSS_MAX
	};

	/// @brief	The enumeration determines bars` direction in bar diagramm
	enum EBarDirection {
		BAR_DIR_VERTICAL = 0,
		BAR_DIR_HORIZONTAL
	};

	/// @brief	The enumeration determinese several bar relative position
	enum EBarGrouping {
		BAR_GROUP_CLUSTERED = 0,
		BAR_GROUP_STACKED,
		BAR_GROUP_PERCENT_STACKED
	};

	/// @brief	The enumeration determines scatter diagram styles
	enum EScatterStyle {
		SCATTER_FILL = 0,
		SCATTER_POINT
	};

	/// @brief  Structure to setup a chart
	/// @note	category sheet parameters can left unset, then a default category axis is used
	/// @note	if values` sheet is unset the series will not be added to the list of charts
	struct Series {
		CWorksheet *catSheet;							///< catSheet pointer to a sheet contains category axis
		CellCoord catAxisFrom;							///< catAxisFrom data range (from cell (row, col), beginning since 0)
		CellCoord catAxisTo;							///< catAxisTo data range (to cell (row, col), beginning since 0)
		CWorksheet *valSheet;							///< valSheet pointer to a sheet contains values axis
		CellCoord valAxisFrom;							///< valAxisFrom data range (from cell (row, col), beginning since 0)
		CellCoord valAxisTo;							///< valAxisTo data range (to cell (row, col), beginning since 0)

		_tstring title;									///< title series title (in the legend)
		bool isMarked;									///< isMarked indicates whether nodes are marked
		bool isSmoothed;								///< isSmoothed indicates whether series must be smoothed at rendering
		bool isDashed;									///< isDashed indicates whether line will be rendered dashed or not

		Series() {
			catSheet = NULL;
			valSheet = NULL;

			title = _T("");
			isMarked = false;
			isSmoothed = false;
			isDashed = false;
		}
	};

private:
	/// @brief  Structure that describes axis properties
	/// @note	gridLines and sourceLinked properties will have no effect at using with additional axes because of Microsoft Open XML format restrictions
	struct Axis {
		uint32_t id;			///< axis axis id
		_tstring name;			///< name axis name (that will be depicted)
		uint32_t nameSize;		///< nameSize font size
		EPosition pos;			///< pos axis position
		EGridLines gridLines;	///< gridLines grid detalization
		EAxisCross cross;		///< cross determines axis position relative to null
		bool sourceLinked;		///< sourceLinked indicates if axis has at least one linked source
		_tstring minValue;		///< minValue minimum value for axis (type std::string is used for generality and processing simplicity)
		_tstring maxValue;		///< maxValue minimum value for axis (type std::string is used for generality and processing simplicity)
		int lblSkipInterval;	///< space between two neighbour labels
		int markSkipInterval;	///< space between two neighbour marks
		int lblAngle;			///< axis labels angle in degrees

		Axis() {
			id = 0;
			name = _T("");
			nameSize = 10;
			pos = POS_LEFT;
			gridLines = GRID_NONE;
			cross = CROSS_AUTO_ZERO;
			sourceLinked = false;

			minValue = _T("");
			maxValue = _T("");

			lblSkipInterval = -1;	// auto
			markSkipInterval = -1;	// auto
			lblAngle = -1;			// none
		}
	};

	/// @brief  Structure describes diagramm properties
	struct Diagramm {
		_tstring name;				///< name diagramm name (that will be depicted above the chart)
		uint32_t nameSize;			///< nameSize font size
		EPosition legend_pos;		///< legend_pos legend position
		ETableData tableData;		///< tableData table data state
		EChartTypes typeMain;		///< typeMain main chart type
		EChartTypes typeAdditional;	///< typeAdditional additional chart type (optional)

		EBarDirection barDir;
		EBarGrouping barGroup;
		EScatterStyle scatterStyle;

		Diagramm() {
			name = _T("");
			nameSize = 18;
			legend_pos = POS_RIGHT;
			tableData = TBL_DATA_NONE;
			typeMain = CHART_LINEAR;
			typeAdditional = CHART_NONE;

			barDir = BAR_DIR_VERTICAL;
			barGroup = BAR_GROUP_CLUSTERED;
			scatterStyle = SCATTER_FILL;
		}
	};

private:
	_tofstream	        *m_f;               ///< file stream for xml stream
	xmlw::XmlStream     *m_xmlStream;       ///< xml output stream
	int32_t             m_index;            ///< chart ID number
	_tstring            m_temp_path;        ///< path to the temporary directory (unique for a book)
	_tstring         	m_title;            ///< chart sheet title
	bool                m_isOk;             ///< initialization status

	std::vector<Series> m_seriesSet;        ///< series set for main chart type
	std::vector<Series> m_seriesSetAdd;     ///< series set for additional chart type
	Diagramm            m_diagramm;			///< diagram object
	Axis                m_xAxis;            ///< main X axis object
	Axis                m_yAxis;            ///< main Y axis object
	Axis                m_x2Axis;           ///< additional X axis object
	Axis                m_y2Axis;           ///< additional Y axis object

public:
	// @section    Constructors / destructor
	CChartsheet();
	CChartsheet(uint32_t index, const _tstring& temp_path);
	CChartsheet(uint32_t index, EChartTypes type, const _tstring& temp_path);
	virtual ~CChartsheet();

	// @section    Interclasses internal interface methods
	int32_t GetIndex() const                            { return m_index; }

	// @section    User interface
	_tstring GetTitle() const                    { return m_title; }
	void SetTitle(const _tstring& title)         { m_title = title.length() > 31 ? title.substr(0, 31) : title; }

	EChartTypes GetMainType() const                 { return m_diagramm.typeMain; }
	void SetMainType(EChartTypes type)              { m_diagramm.typeMain = type; }

	EChartTypes GetAddType() const                  { return m_diagramm.typeAdditional; }
	void SetAddType(EChartTypes type)               { m_diagramm.typeAdditional = type; }

	void SetDiagrammNameSize(uint32_t size)         { m_diagramm.nameSize = size; }
	void SetDiagrammName(const _tstring& name)   	{ m_diagramm.name = name; }
	void SetTableDataState(ETableData state)        { m_diagramm.tableData = state; }
	void SetLegendPos(EPosition pos)                { m_diagramm.legend_pos = pos; }

	void SetBarDirection(EBarDirection barDir)		{ m_diagramm.barDir = barDir; }
	void SetBarGrouping(EBarGrouping barGroup)		{ m_diagramm.barGroup = barGroup; }
	void SetScatterStyle(EScatterStyle style)		{ m_diagramm.scatterStyle = style; }

	void SetXAxisLblInterval(int value)			{ m_xAxis.lblSkipInterval = value; }
	void SetXAxisMarkInterval(int value)		{ m_xAxis.markSkipInterval = value; }
	void SetXAxisLblAngle(int degrees)			{ m_xAxis.lblAngle = degrees; }
	void SetXAxisMin(const _tstring& value)      { m_xAxis.minValue = value; }
	void SetXAxisMax(const _tstring& value)      { m_xAxis.maxValue = value; }
	void SetYAxisMin(const _tstring& value)      { m_yAxis.minValue = value; }
	void SetYAxisMax(const _tstring& value)      { m_yAxis.maxValue = value; }
	void SetXAxisNameSize(uint32_t size)            { m_xAxis.nameSize = size; }
	void SetYAxisNameSize(uint32_t size)            { m_yAxis.nameSize = size; }
	void SetXAxisName(const _tstring& name)      { m_xAxis.name = name; }
	void SetYAxisName(const _tstring& name)      { m_yAxis.name = name; }
	void SetXAxisPos(EPosition pos)                 { m_xAxis.pos = pos; }
	void SetYAxisPos(EPosition pos)                 { m_yAxis.pos = pos; }
	void SetXAxisGrid(EGridLines state)             { m_xAxis.gridLines = state; }
	void SetYAxisGrid(EGridLines state)             { m_yAxis.gridLines = state; }
	void SetXAxisCross(EAxisCross cross)			{ m_xAxis.cross = cross; }
	void SetYAxisCross(EAxisCross cross)			{ m_yAxis.cross = cross; }

	void SetX2AxisLblInterval(int value)		{ m_x2Axis.lblSkipInterval = value; }
	void SetX2AxisMarkInterval(int value)		{ m_x2Axis.markSkipInterval = value; }
	void SetX2AxisLblAngle(int degrees)			{ m_x2Axis.lblAngle = degrees; }
	void SetX2AxisMin(const _tstring& value)     { m_x2Axis.minValue = value; }
	void SetX2AxisMax(const _tstring& value)     { m_x2Axis.maxValue = value; }
	void SetY2AxisMin(const _tstring& value)     { m_y2Axis.minValue = value; }
	void SetY2AxisMax(const _tstring& value)     { m_y2Axis.maxValue = value; }
	void SetX2AxisNameSize(uint32_t size)           { m_x2Axis.nameSize = size; }
	void SetY2AxisNameSize(uint32_t size)           { m_y2Axis.nameSize = size; }
	void SetX2AxisName(const _tstring& name)     { m_x2Axis.name = name; }
	void SetY2AxisName(const _tstring& name)     { m_y2Axis.name = name; }
	void SetX2AxisPos(EPosition pos)                { m_x2Axis.pos = pos; }
	void SetY2AxisPos(EPosition pos)                { m_y2Axis.pos = pos; }
	void SetX2AxisGrid(EGridLines state)            { m_x2Axis.gridLines = state; }
	void SetY2AxisGrid(EGridLines state)            { m_y2Axis.gridLines = state; }
	void SetX2AxisCross(EAxisCross cross)			{ m_x2Axis.cross = cross; }
	void SetY2AxisCross(EAxisCross cross)			{ m_y2Axis.cross = cross; }

	bool AddSeries(const Series& series, bool mainChart = true);
	bool Save();
	bool IsOk() const                               { return m_isOk; }

private:
	void Init(uint32_t index, EChartTypes type);

	void AddTitle(const TCHAR* name, uint32_t size, bool vertPos);
	void AddTableData();
	void AddLegend();
	void AddXAxis(const Axis& x, uint32_t crossAxisId = 0);
	void AddYAxis(const Axis& y, uint32_t crossAxisId = 0);

	void AddLineChart(Axis& xAxis, uint32_t yAxisId, const std::vector<Series>& series, uint32_t firstSeriesId);
	void AddBarChart(Axis& xAxis, uint32_t yAxisId, const std::vector<Series>& series, uint32_t firstSeriesId, EBarDirection barDir, EBarGrouping barGroup);
	void AddScatterChart(uint32_t xAxisId, uint32_t yAxisId, const std::vector<Series>& series, uint32_t firstSeriesId, EScatterStyle style);
	void AddPieChart(Axis& xAxis, uint32_t yAxisId, const std::vector<Series>& series, uint32_t firstSeriesId);
};

}	// namespace SimpleXlsx

#endif	// __CHARTSHEET_H__
