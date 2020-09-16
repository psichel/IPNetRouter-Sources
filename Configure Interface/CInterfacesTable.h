// =================================================================================//	CInterfacesTable.h	 		   �1996 Sustainable Softworks. All rights reserved.// =================================================================================//	Table object to display trace route information#pragma once#include "CConfigureTable.h"#include "CInterfacesData.h"#include <LCommander.h>const SInt16 kMaxInterfacesTableCols				= 10;const SInt16 kMaxInterfacesTableRows				= 200;class CInterfacesTable : public CConfigureTable {public:	enum { class_ID = 'IPif' };								CInterfacesTable( LStream *inStream );	virtual		 				~CInterfacesTable();	void			FinishCreateSelf();	void			ClickCell(						const TableCellT		&inCell,						const SMouseDownEvent&	inMouseDown);	Boolean			GetInterfaceEntry(TableIndexT inRow, InterfaceEntry_t* outInterfaceEntry);	void			SetInterfaceEntry(TableIndexT inRow, InterfaceEntry_t* inEntry);	void			DrawCell(const TableCellT	&inCell);};// Constants for CInterfacesTableconst SInt16 kColumnState					= 1;const SInt16 kColumnPortName				= 2;const SInt16 kColumnInterfaceName			= 3;const SInt16 kColumnIPAddress				= 4;const SInt16 kColumnMask					= 5;