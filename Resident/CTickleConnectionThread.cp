// ===========================================================================//	CTickleConnectionThread.cp �1997 Sustainable Softworks. All rights reserved.// ===========================================================================//	Thread object to tickle connection every few minutes//	so it doesn't drop from inactivity#include "CTickleConnectionThread.h"#include "CResidentIfInfo.h"#include "CSendUDPThread.h"#include "CTurboUDPEndpoint.h"//#include <OpenTransport.h>//#include <OpenTptInternet.h>#include "CLogAction.h"#include "IPSupport.h"// Globalsextern CLogAction*		gLogAction;// ===========================================================================#pragma mark ��� CTickleConnectionThread ���CTickleConnectionThread::CTickleConnectionThread(	CResidentIfInfo*		inResInfo)		: LThread(false, thread_DefaultStack, threadOption_Default, nil)		  {	mResInfo = inResInfo;	mAbort = false;	mEndpoint = nil;	mPingAddress = 0;	mPingCounter = 0;}// ---------------------------------------------------------------------------//		� ~CTickleConnectionThread// ---------------------------------------------------------------------------CTickleConnectionThread::~CTickleConnectionThread(){	// Release the Endpoint we created	if (mEndpoint) {		delete mEndpoint;		mEndpoint = nil;	}}// ---------------------------------------------------------------------------//		� Abort// ---------------------------------------------------------------------------voidCTickleConnectionThread::Abort(){	if (!mAbort) {		mAbort = true;		if (this->GetState() == threadState_Suspended) this->Resume();		if (this->GetState() == threadState_Sleeping) this->Wake();	}}// ---------------------------------------------------------------------------//		� ProviderIsClosed// ---------------------------------------------------------------------------voidCTickleConnectionThread::ProviderIsClosed(){	// Release the Endpoint we created	if (mEndpoint) {		delete mEndpoint;		mEndpoint = nil;	}}// ---------------------------------------------------------------------------//		� Run// ---------------------------------------------------------------------------void*CTickleConnectionThread::Run(){	UInt32		delay;	mResult = kOTNoError;	while (!mAbort) {			// sleep for approximately 3 minutes		delay = 1000 * 60 * 3;		Sleep(delay);				if (mAbort) break;				// Ping other side of connection		try {		    SetupPing();		    SendPing();		    ProviderIsClosed();		}		catch (...) {}	}	SetResult(&mResult);	// set result so Object Master can retrieve it.	mResInfo->ObjectThreadDied(this);	return &mResult;}// ---------------------------------------------------------------------------//		� SetupPing// ---------------------------------------------------------------------------voidCTickleConnectionThread::SetupPing(){	// set Ping address	if (mPingAddress == 0) mPingAddress = mResInfo->PPPGetRemoteAddress();	if (mPingAddress == 0) mPingAddress = CalcPseudoGateway(mResInfo->mPPPlocalAddress);		//	Create a "rawip" endpoint	if (!mEndpoint) {		mEndpoint = new CTurboUDPEndpoint(kRawIPName);		ThrowIfNil_(mEndpoint);		// Initialization: Bind to any local port		// We can do this directly since we're already in a thread		LInternetAddress address(0, 0);		mEndpoint->Bind(address);	}}// ---------------------------------------------------------------------------//		� SendPing// ---------------------------------------------------------------------------voidCTickleConnectionThread::SendPing(){	// setup to send packet	LInternetAddress address(mPingAddress, 0);	UInt8 data[64];	icmpheader *icmp_header;	// Initialize ICMP header	icmp_header = (icmpheader*)&data[0];	icmp_header->type = 8;	// echo request	icmp_header->code = 0;	icmp_header->checksum = 0;	mPingCounter += 1;	icmp_header->identifier = mPingCounter;	icmp_header->sequence_number = 0;		// ICMP data	OTStrCopy((char*)&data[8], "� Sustainable Softworks.");	icmp_header->checksum = IpSum((UInt16*)&data[0], (UInt16*)&data[32]);	// send data	// check to be sure endpoint hasn't died unexpectedly	if (mEndpoint) mEndpoint->SendPacketData(address, data, 32);}