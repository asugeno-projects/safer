2013-09-01 Version 0.20
	Change log:
		- Bug fix (SaveStyles and GetFormatCodeString methods fixed)

2013-05-05 Version 0.19 update
	Change log:
		- License headers were added (BSD "3-clause license")

2013-04-13 Version 0.19
	Change log:
		- Numeric, dates and times styling
			Most general formats are available. It is also possible to create custom formats
			Some information about how to create a format is here:
				http://office.microsoft.com/en-us/excel-help/create-or-delete-a-custom-number-format-HP001216503.aspx
		- Minor bug fixes
		- Makefile to build the project as a library (by Eduardo Zacarias)

2012-12-16 Version 0.18
	Change log:
		- Comments functionality added;
		- Font name bug fix
		- Borders functionality changed;
		- Sheet orientation feature added;
		- Some additional features in Chartsheet.

2012-10-31 Version 0.17
	Change log:
		- Creation time format bug fix.

2012-10-26 Version 0.16
	Change log:
		- Temporary directory location bug fix;
		- Creation time format bug fix.

2012-10-02 Version 0.15
	Change log:
		- Linux are supported now (tested on Ubuntu with GCC 4.6.3)
			- ZIP library ported (under Linux only compressing of files is available)

2012-09-21 Version 0.14
	Change log (bug fix):
		- Overflow in shared strings;

2012-09-21 Version 0.13
	Change log (bug fix):
		- It is enough to include just 'Xlsx/Workbook.h' instead of three files;
		- AddCell();
		- Unique directory for each new book;
		- Temporary files are deleted after book saving even if saving is failed;
		- In Worksheet/Chartsheet classes` objects 'ok' flag is set to false after saving (since stream either closed or data corrupt at another trying);
		- Some minor changes inside;

2012-09-09 Version 0.12
	Change log:
		- Proper (without repetitions) shared strings implemented;
		- Bug fix - wrong span range for row;
		- Bug fix - nonsense style id by default

2012-09-01 Version 0.11
	Change log:
		- AddCell() method to easier addition of empty cell added.

2012-08-05 Version 0.1
	First draft of the library. 
	
	Main features for data sheets formatting:
		- adjustable height for each row;
		- adjustable width for columns. Can be specified only at sheet creation;
		- frozen panes;
		- page title designation;
		- data can be added by row, by cell or by cell groups;
		- cells merging;
		- formulae recognition (without formula`s content verification);
		- styles support (fonts, fills, borders, alignment, multirow text);

	Main features for chart sheet configuration:
		- supported chart types: linear, bar, scatter;
		- customizable settings: legend presence and position, axes` labels;
		- data table (for linear and bar chart types);
		- additional axes pair (the type defines independently from main chart type);
		- data source can set for both X and Y axes for both main and additional axes pairs;
		- curve`s smoothness, node`s mark;
		- grid settings for each axis;
		- minimum and maximum values for each axis.