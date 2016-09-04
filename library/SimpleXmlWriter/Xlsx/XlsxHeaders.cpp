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

#ifdef _WIN32
#include <tchar.h>
#else
#include "../tchar.h"
#endif  // _WIN32

const TCHAR *ns_content_types	= _T("http://schemas.openxmlformats.org/package/2006/content-types");
const TCHAR *content_printer 	= _T("application/vnd.openxmlformats-officedocument.spreadsheetml.printerSettings");
const TCHAR *content_rels 		= _T("application/vnd.openxmlformats-package.relationships+xml");
const TCHAR *content_xml 		= _T("application/xml");
const TCHAR *content_book 		= _T("application/vnd.openxmlformats-officedocument.spreadsheetml.sheet.main+xml");
const TCHAR *content_sheet 		= _T("application/vnd.openxmlformats-officedocument.spreadsheetml.worksheet+xml");
const TCHAR *content_theme 		= _T("application/vnd.openxmlformats-officedocument.theme+xml");
const TCHAR *content_styles 	= _T("application/vnd.openxmlformats-officedocument.spreadsheetml.styles+xml");
const TCHAR *content_sharedStr 	= _T("application/vnd.openxmlformats-officedocument.spreadsheetml.sharedStrings+xml");
const TCHAR *content_comment	= _T("application/vnd.openxmlformats-officedocument.spreadsheetml.comments+xml");
const TCHAR *content_vml		= _T("application/vnd.openxmlformats-officedocument.vmlDrawing");
const TCHAR *content_core 		= _T("application/vnd.openxmlformats-package.core-properties+xml");
const TCHAR *content_app 		= _T("application/vnd.openxmlformats-officedocument.extended-properties+xml");
const TCHAR *content_chart      = _T("application/vnd.openxmlformats-officedocument.drawingml.chart+xml");
const TCHAR *content_drawing    = _T("application/vnd.openxmlformats-officedocument.drawing+xml");
const TCHAR *content_chartsheet = _T("application/vnd.openxmlformats-officedocument.spreadsheetml.chartsheet+xml");
const TCHAR *content_chain      = _T("application/vnd.openxmlformats-officedocument.spreadsheetml.calcChain+xml");

const TCHAR *ns_relationships	= _T("http://schemas.openxmlformats.org/package/2006/relationships");
const TCHAR *type_book			= _T("http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument");
const TCHAR *type_core			= _T("http://schemas.openxmlformats.org/package/2006/relationships/metadata/core-properties");
const TCHAR *type_app			= _T("http://schemas.openxmlformats.org/officeDocument/2006/relationships/extended-properties");

const TCHAR *ns_cp				= _T("http://schemas.openxmlformats.org/package/2006/metadata/core-properties");
const TCHAR *ns_dc				= _T("http://purl.org/dc/elements/1.1/");
const TCHAR *ns_dcterms			= _T("http://purl.org/dc/terms/");
const TCHAR *ns_dcmitype		= _T("http://purl.org/dc/dcmitype/");
const TCHAR *ns_xsi				= _T("http://www.w3.org/2001/XMLSchema-instance");

const TCHAR *xsi_type			= _T("dcterms:W3CDTF");

const TCHAR *ns_doc_prop		= _T("http://schemas.openxmlformats.org/officeDocument/2006/extended-properties");
const TCHAR *ns_vt				= _T("http://schemas.openxmlformats.org/officeDocument/2006/docPropsVTypes");

const TCHAR *ns_book			= _T("http://schemas.openxmlformats.org/spreadsheetml/2006/main");
const TCHAR *ns_book_r			= _T("http://schemas.openxmlformats.org/officeDocument/2006/relationships");

const TCHAR *ns_mc				= _T("http://schemas.openxmlformats.org/markup-compatibility/2006");
const TCHAR *ns_x14ac			= _T("http://schemas.microsoft.com/office/spreadsheetml/2009/9/ac");

const TCHAR *ns_x14				= _T("http://schemas.microsoft.com/office/spreadsheetml/2009/9/main");

const TCHAR *type_comments		= _T("http://schemas.openxmlformats.org/officeDocument/2006/relationships/comments");
const TCHAR *type_vml			= _T("http://schemas.openxmlformats.org/officeDocument/2006/relationships/vmlDrawing");
const TCHAR *type_sheet			= _T("http://schemas.openxmlformats.org/officeDocument/2006/relationships/worksheet");
const TCHAR *type_style			= _T("http://schemas.openxmlformats.org/officeDocument/2006/relationships/styles");
const TCHAR *type_sharedStr		= _T("http://schemas.openxmlformats.org/officeDocument/2006/relationships/sharedStrings");
const TCHAR *type_theme			= _T("http://schemas.openxmlformats.org/officeDocument/2006/relationships/theme");
const TCHAR *type_chain         = _T("http://schemas.openxmlformats.org/officeDocument/2006/relationships/calcChain");
const TCHAR *type_chartsheet    = _T("http://schemas.openxmlformats.org/officeDocument/2006/relationships/chartsheet");
const TCHAR *type_chart         = _T("http://schemas.openxmlformats.org/officeDocument/2006/relationships/chart");
const TCHAR *type_drawing       = _T("http://schemas.openxmlformats.org/officeDocument/2006/relationships/drawing");

const TCHAR *ns_a				= _T("http://schemas.openxmlformats.org/drawingml/2006/main");
const TCHAR *ns_c               = _T("http://schemas.openxmlformats.org/drawingml/2006/chart");
const TCHAR *ns_xdr             = _T("http://schemas.openxmlformats.org/drawingml/2006/spreadsheetDrawing");
