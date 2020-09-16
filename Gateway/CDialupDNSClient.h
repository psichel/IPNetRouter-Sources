// =================================================================================//	CDialupDNSClient.h	 	�2000 Sustainable Softworks. All rights reserved.// =================================================================================//	Establish connection to dialup DNS server as needed#pragma once#include <LCleanupTask.h>#include <LPeriodical.h>#include <LListener.h>#include "CObjectMaster.h"class LTCPEndpoint;class CReceiveTCPThread;class CSendTCPThread;class CDialupDNSClient :	public LPeriodical, LListener, CObjectMaster {public:								CDialupDNSClient();	virtual		 				~CDialupDNSClient();	void				Terminate(Boolean inCanWait=true);	Boolean				InitConnection();	Boolean				CloseConnection(Boolean inCanWait=true);	virtual void		SpendTime(const EventRecord &inMacEvent);	virtual void		ObjectThreadDied(LThread *inThread);	void				ListenToMessage(							MessageT	inEventCode,							void		*ioParam );	void				ReceiveData(LDataArrived* inMessage);protected:	Boolean				OpenConnection();	void				LastComplete();	private:		LTCPEndpoint*		mTCPEndpoint;				// our TCP network endpoint object	CReceiveTCPThread*	mReceiveTCPThread;	CSendTCPThread*		mSendTCPThread;	OTTimeStamp			mLastStamp;	UInt32				mIdleDelay;	UInt8				mRetryCount;	Boolean				mIsActive;	Boolean				mWaitingToRestart;};const ResID rDialupDNS_STRN		        = 251;const SInt16 kDialupDNS_ServerStr		= 1;const SInt16 kDialupDNSServerPort		= 81;const SInt16 kDialupDNSIdleRestart		= 2000;const SInt16 kDialupDNSRetryCount		= 5;