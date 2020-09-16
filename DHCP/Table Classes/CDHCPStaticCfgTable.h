// =================================================================================//	CDHCPStaticCfgTable.h	  �1999 Sustainable Softworks. All rights reserved.// =================================================================================//	Table object to display DHCPStaticCfg information#pragma once#include "CEditTable.h"#include "CDHCPData.h"#include <LCommander.h>const SInt16 kMaxDHCPStaticCfgTableCols				= 10;const SInt16 kMaxDHCPStaticCfgTableRows				= 200;class CDHCPStaticCfgTable : public CEditTable {public:	enum { class_ID = 'DHsc' };							CDHCPStaticCfgTable( LStream *inStream );	virtual		 			~CDHCPStaticCfgTable();	void			FinishCreateSelf();	Boolean			GetDHCPStaticCfgEntry(TableIndexT inRow,							DHCPStaticCfgEntry_t* outEntry);	void			SetDHCPStaticCfgEntry(TableIndexT inRow,							DHCPStaticCfgEntry_t* inEntry);	void			UpdateTable();	void			UpdateData();	void			DrawCell(const TableCellT	&inCell);};// table constantsconst SInt16 kColDHCPStaticCfgInterfaceAddr 	= 1;const SInt16 kColDHCPStaticCfgIPAddr			= 2;const SInt16 kColDHCPStaticCfgHWAddr			= 3;const SInt16 kColDHCPStaticCfgClientID			= 4;