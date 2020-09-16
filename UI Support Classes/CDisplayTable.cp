// =================================================================================//	CDisplayTable.cp				�1996 Sustainable Softworks All rights reserved.// =================================================================================//	Display a grid of cells#include "CDisplayTable.h"#include "AGAColors.h"#include "IPRouterCommon.h"#include "MacSupport.h"#include "Cwindow.h"#include <LString.h>#include <Icons.h>// ---------------------------------------------------------------------------------//		� CDisplayTable(LStream*)// ---------------------------------------------------------------------------------CDisplayTable::CDisplayTable(	LStream	*inStream )		: LTable( inStream ){	ClearTable();}// ---------------------------------------------------------------------------------//		� ~CDisplayTable// ---------------------------------------------------------------------------------CDisplayTable::~CDisplayTable(){}// ---------------------------------------------------------------------------//		� Fetch Local Content Frame// ---------------------------------------------------------------------------//	Get the content frame leaving room for a cell borderBooleanCDisplayTable::FetchLocalContentFrame(	const TableCellT	&inCell,	Rect				&outCellFrame){	Boolean	intersectsFrame = FetchLocalCellFrame( inCell, outCellFrame );	outCellFrame.bottom -= 1;	outCellFrame.right -= 1;	return intersectsFrame;}// ---------------------------------------------------------------------------//		� Select Cell// ---------------------------------------------------------------------------//  Handle click in cell.  Display only so don't do anything.voidCDisplayTable::SelectCell(	const TableCellT&	/* inCell */){}// ---------------------------------------------------------------------------//		� Refresh Cell// ---------------------------------------------------------------------------//	Cause a table cell to be redrawnvoidCDisplayTable::RefreshCell(	const TableCellT	&inCell){	// get cell position (local coordinates)	Rect	cellFrame;	if (FetchLocalCellFrame( inCell, cellFrame ) && IsVisible()) {		// convert to port coordinates		LocalToPortPoint(topLeft(cellFrame));		LocalToPortPoint(botRight(cellFrame));		// invalidate the cell in the grafport		InvalPortRect( &cellFrame );	}}// ---------------------------------------------------------------------------//		� Refresh Row// ---------------------------------------------------------------------------//	Cause a table row to be redrawnvoidCDisplayTable::RefreshRow(	const TableIndexT	&inRow){	TableCellT	theCell;	Rect		rowFrame, cellFrame;	theCell.row = inRow;	theCell.col = 1;	// get row frame position (local coordinates)	FetchLocalCellFrame( theCell, rowFrame );	theCell.col = mCols;	FetchLocalCellFrame( theCell, cellFrame );	rowFrame.right = cellFrame.right;	// convert to port coordinates	LocalToPortPoint(topLeft(rowFrame));	LocalToPortPoint(botRight(rowFrame));	// invalidate the cell in the grafport	InvalPortRect( &rowFrame );}// ---------------------------------------------------------------------------//		� ScrollBits// ---------------------------------------------------------------------------//	Scroll the pixels of a ViewvoidCDisplayTable::ScrollBits(	SInt32		inLeftDelta,			// Pixels to scroll horizontally	SInt32		inTopDelta)				// Pixels to scroll vertically{	if (FocusDraw()) {										// Get Frame in Port coords		Rect	frame = mRevealedRect;		frame.bottom -= 2;				// [PAS] adjust scroll area to miss bottom		PortToLocalPoint(topLeft(frame));		PortToLocalPoint(botRight(frame));												// Scroll Frame, clipping to the										//   update region		RgnHandle	updateRgnH = NewRgn();		ApplyForeAndBackColors();		::ScrollRect(&frame, -inLeftDelta, -inTopDelta, updateRgnH);														// Force redraw of update region										// Use our InvalPortRgn rather than										// Toolbox InvalRgn		::OffsetRgn(updateRgnH, -mPortOrigin.h, -mPortOrigin.v);		InvalPortRgn(updateRgnH);		::DisposeRgn(updateRgnH);	}}// ---------------------------------------------------------------------------//		� ApplyForeAndBackColors()// ---------------------------------------------------------------------------// Override to define our own colors separate from superviewvoidCDisplayTable::ApplyForeAndBackColors(){RGBColor		emptyColor;	emptyColor = gAGAColorArray[B];	::RGBForeColor(&emptyColor);	emptyColor = gAGAColorArray[W];	::RGBBackColor(&emptyColor);}// ---------------------------------------------------------------------------//		� ClearTable()// ---------------------------------------------------------------------------voidCDisplayTable::ClearTable(){	// initialize table data	TableCellT	tCell;	TableItem	tData;	tData = 0;	for (tCell.row=1; tCell.row<=mRows; tCell.row++) {		for (tCell.col=1; tCell.col<=mCols; tCell.col++) {						SetCellData( tCell, &tData );		}	}}