// =================================================================================//	CControlLink.h	 		�1997-98 Sustainable Softworks. All rights reserved.//	[abstract]// =================================================================================//	Abstract class that encapsulates control of a PPP link for dial on demand.//	The basic methods are://	  	StartMonitoring()		Start monitoring link state (up or down)//								Broadcast a message when state transitions//	  	StopMonitoring()//		Connect()				Launch asynchronous connection attempt//								Broadcast message when attemp completes//		Disconnect()			Launch asynchronous disconnect attempt//								Broadcast message when attempt completes//		GetIPAddress()			Get the negotiated client IP address//								returns 0 if not available//		IsLinkUp()				Return whether link is currently connected//		IsMonitoring()			Return whether monitoring link state#ifndef _H_CControlLink#define _H_CControlLink#pragma once#include <LBroadcaster.h>#include <LString.h>#include <OpenTransport.h>class CControlLink : public LBroadcaster {public:							CControlLink();	virtual		 			~CControlLink();	virtual	void		Terminate() = 0;	virtual OTResult	StartMonitoring(Boolean inShowDialogs) = 0;	virtual OTResult	StopMonitoring() = 0;	virtual OTResult	Connect(Boolean inShowDialogs) = 0;	virtual OTResult	Disconnect(Boolean inShowDialogs) = 0;		virtual UInt32		GetIpAddress() = 0;	virtual UInt32		GetRemoteAddress() = 0;	virtual Boolean		IsLinkUp() = 0;	virtual Boolean		IsMonitoring() = 0;protected:	Str63				mPortName;			// remember which port we're controlling	private:};const SInt16 msg_AsynchCompletion		= kPRIVATEEVENT + 41;const SInt16 msg_ResetLink				= kPRIVATEEVENT + 42;const SInt16 msg_LinkUp				    = kPRIVATEEVENT + 43;const SInt16 msg_LinkDown				= kPRIVATEEVENT + 44;const SInt16 msg_RestorePrimary		    = kPRIVATEEVENT + 45; // restore primaryconst SInt16 msg_CallStarted			= kPRIVATEEVENT + 46;#if defined(__CFM68K__) && !defined(__USING_STATIC_LIBS__)	#pragma import reset#endif#endif