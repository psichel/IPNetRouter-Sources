// =================================================================================//	CPrintView.h	 		�1996 Sustainable Softworks. All rights reserved.// =================================================================================//  Custom LView used to print the contens of a window#pragma once#include <LView.h>class CPrintView : public LView {public:	enum { class_ID = 'Prvw' };	static CPrintView *	CreatePrintViewStream( LStream *inStream );								CPrintView( LStream *inStream );	virtual		 				~CPrintView();	virtual void		PrintPanelSelf(const PanelSpec &inPanel);	};