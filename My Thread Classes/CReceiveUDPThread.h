// ===========================================================================//	CReceiveUDPThread.h		�1997 Sustainable Softworks. All rights reserved.// ===========================================================================//	Thread object to handle receive (Rx) part of Ping Test#pragma once#include <LInternetAddress.h>#include <UNetworkMessages.h>#include <LThread.h>const SInt16 kReceiveBufferSize		= 2048;		// larger than typical MTUclass CObjectMaster;class LUDPEndpoint;class CReceiveUDPThread : 		public LThread {public:						CReceiveUDPThread(								CObjectMaster*		inObjectMaster,								LUDPEndpoint*		inNetworkEndpoint);	virtual				~CReceiveUDPThread();	void				Bind(LInternetAddress&	inLocalAddress);	void				Abort();		protected:	virtual void*		Run();	CObjectMaster* 		mObjectMaster;				// Object master and PingTest object	LUDPEndpoint*		mEndpoint;					// network endpoint object	LInternetAddress	mLocalAddress;				// requested bind address		Boolean				mAbort;	OSStatus			mResult;};