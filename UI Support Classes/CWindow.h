// =================================================================================//	CWindow.h	 	�1996 Sustainable Softworks. All rights reserved.// =================================================================================//	LWindow with GetForeAndBackColor accessor#pragma once#include <LWindow.h>class LWindow;class CWindow : public LWindow {public:	enum { class_ID = 'CWin' };								CWindow( LStream *inStream );	virtual		 				~CWindow();	void				GetForeAndBackColors(							RGBColor	*outForeColor,							RGBColor	*outBackColor);	virtual Boolean		ObeyCommand(								CommandT			inCommand,								void				*ioParam);	virtual void		FindCommandStatus(CommandT inCommand,							Boolean &outEnabled, Boolean &outUsesMark,							UInt16 &outMark, Str255 outName);	virtual void		HandlePrint();	virtual void		HandlePrintOne();protected:	void				DoPrint();		// add printing support	LPrintSpec			mPrintSpec;//	THPrint					mPrintRecordH;	LView*					mPrintView;};