// ===========================================================================//	CGURLThread.cp	 	�1997 Sustainable Softworks. All rights reserved.// ===========================================================================//	Thread object to step through handling a GURL apple event#include "CGURLThread.h"#include "CIPRouterApp.h"#include "MacSupport.h"#include "IPSupport.h"#include "IPRouterCommon.h"// ===========================================================================#pragma mark ��� CGURLThread ���CGURLThread::CGURLThread(	CIPRouterApp*		inApp,	ConstStr255Param	inURL)		: LThread(false, thread_DefaultStack, threadOption_Default, nil)		  {	mApp = inApp;	mURLString = inURL;}// ---------------------------------------------------------------------------//		� ~CGURLThread// ---------------------------------------------------------------------------CGURLThread::~CGURLThread(){}// ---------------------------------------------------------------------------//		� Run// ---------------------------------------------------------------------------void*CGURLThread::Run(){	LWindow		*theWindow;	UInt8		pos;	LStr255		str;	UInt8		theAction;		// index of possible URL actions	mResult = 0;	try {		// step through GURL sequence		// if URL begins with a valid scheme, invoke corresponding window		theAction = kURLnone;		theWindow = nil;		pos = mURLString.Find(':');		str.Assign(mURLString, 1, pos);//		if ( ::EqualString(str, "\ptraceroute:", false, false) ) {	// caseSens, diacSens//			mURLString.Assign(mURLString, 12);//			theAction = kURLtraceroute;//			theWindow = InvokeWindow("\pTrace Route", rPPob_TraceRouteWindow, mApp);//		}		// show window if any		if (theWindow) theWindow->Show();		// is it a host name or IP address?		if ( mURLString.BeginsWith("\p//") &&			 ((mURLString[3] < '0') || ('9' < mURLString[3])) ) {			// begins //<non-digit>, get host name			mURLString.Assign(mURLString, 3);			pos = mURLString.Find('/');			if (pos == 0) pos = mURLString.Length() + 1;			mURLString.Assign(mURLString, 1, pos-1);		} else {			// it's an IP address			theAction += 1;			// remove "//" if present			if ( mURLString.BeginsWith("\p//") ) mURLString.Assign(mURLString, 3);		}		//		switch (theAction) {//			case kURLtraceroute://				((CTraceRouteWindow*)theWindow)->InvokeTraceName(mURLString);//				break;//		}			}	catch (const LException& inErr) {		mResult = inErr.GetErrorCode();		// handle exception results		// just report result to Object Master	} 	catch(...) {	    SysBeep(30);	} 	SetResult(&mResult);	// set result so Object Master can retrieve it.	mApp->ObjectThreadDied(this);	return &mResult;}