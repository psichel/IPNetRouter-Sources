// =================================================================================//	CSubnetCalcWindow.cp				�1996 Sustainable Softworks All rights reserved.// =================================================================================#include "CSubnetCalcWindow.h"#include "CSubnetCalcConst.h"#include "IPRouterCommon.h"#include "CIPNumberEditField.h"#include "CIPNumberField.h"#include "CHelpWindow.h"#include "MacSupport.h"#include "IPSupport.h"#include <LString.h>#include "ICAPI.h"// Globalsextern ICInstance gICInstance;// ---------------------------------------------------------------------------------//		� CreateIPSubnetCalcWindowStream [static]// ---------------------------------------------------------------------------------CSubnetCalcWindow *CSubnetCalcWindow::CreateSubnetCalcWindowStream(	LStream	*inStream ){	return new CSubnetCalcWindow( inStream );}// ---------------------------------------------------------------------------------//		� CSubnetCalcWindow(LStream*)// ---------------------------------------------------------------------------------CSubnetCalcWindow::CSubnetCalcWindow(	LStream	*inStream )		: CWindow( inStream ){	// other initialization	mRangeDelta = 0;	mAggregateLen = 0;	mAggregateBlocks = 0;}// ---------------------------------------------------------------------------------//		� ~CSubnetCalcWindow// ---------------------------------------------------------------------------------CSubnetCalcWindow::~CSubnetCalcWindow(){}// ---------------------------------------------------------------------------------//		� FinishCreateSelf// ---------------------------------------------------------------------------------//	Finish creating the Subnet Calculator window.//	Notice we can't do this from the constructor because the SubPanes//		haven't been created yet.voidCSubnetCalcWindow::FinishCreateSelf(){	// Link the window (the listener) with the controls in	// the window (the broadcasters).	UReanimator::LinkListenerToControls( (CSubnetCalcWindow *)this, this, rRidL_SubnetCalcWindow );	// link the window (listener) to the IP Address field	mAddressField = (CIPNumberEditField*) this->FindPaneByID( kIPAddressPane );	ThrowIfNil_(mAddressField);	mAddressField->AddListener( this );	// make IP address field the latent subcommander for keyboard input	SetLatentSub(mAddressField);	// disable drop to Mask field	mMaskField = (CIPNumberField*) this->FindPaneByID( kMaskPane );	ThrowIfNil_(mMaskField);	mMaskField->DisableDrop();	// disable drop to Net field	mNetField = (CIPNumberField*) this->FindPaneByID( kShowNetworkNumPane );	ThrowIfNil_(mNetField);	mNetField->DisableDrop();}// ---------------------------------------------------------------------------------//		� ListenToMessage// ---------------------------------------------------------------------------------voidCSubnetCalcWindow::ListenToMessage(	MessageT	inMessage,	void		*ioParam ){#pragma unused( ioParam )LCaption		*prefixCaption, *theCaption;CIPNumberField*	hostField;UInt8			prefixLength, cidrLength;SInt32			netNumber, hostNumber, theValue;UInt32			ipAddress, ipAddress2;UInt32			netMask, hostMask, rangeDelta, rangeBlock, rangeAddress;SInt32			index, rangeLeft, rangeRight;LStr255			theString, prevString;		// make IP address field the current target for keyboard input	mAddressField->SwitchTarget(mAddressField);		// get prefix length from caption	prefixCaption = (LCaption*) this->FindPaneByID( kShowPrefixLengthPane );	ThrowIfNil_( prefixCaption );	prefixLength = prefixCaption->GetValue();	netMask  = 0xffffffff << (32 - prefixLength);	hostMask = 0xffffffff >> prefixLength;		// get network and host number from the IP address edit field	ipAddress  = mAddressField->GetValue();	cidrLength = mAddressField->GetValueLength();	netNumber  = ipAddress & netMask;	hostNumber = ipAddress & hostMask;	// check for address range	ipAddress2 = mAddressField->GetValue2();	if (ipAddress < ipAddress2) {		// get difference bits		rangeDelta = ipAddress2 - ipAddress;		// add one to network range to be inclusive		// start with address class		rangeRight = GetIPAddressClass(ipAddress2, theString);		if (rangeRight < 8) rangeRight = 8;		// adjust for CIDR alignment if needed		index = FindRightBit(rangeDelta);		if (index > rangeRight) rangeRight = index;		rangeDelta += (UInt32)0x01 << (32-rangeRight);	// increment network range		// find left most difference bit to bound block size		rangeLeft = FindLeftBit(rangeDelta);		// if address range has changed (or user pressed enter),		// reset aggregate length		if ((rangeDelta != mRangeDelta) || (inMessage == msg_FieldActionKey)) {			mRangeDelta = rangeDelta;			mAggregateBlocks = 0;		}		// get current aggregate address		rangeAddress = ipAddress + mAggregateBlocks;		// get current aggregate prefix length		// (the size of the next rangeDelta block to add)		// find largest block remaining		rangeDelta = mRangeDelta - mAggregateBlocks;	// subtract used blocks					rangeBlock = FindLeftBit(rangeDelta);		// look for right most 1 bit in agg address within remaining range		index = FindRightBit(rangeAddress, rangeRight);		// if not found, use largest block remaining		if (index < rangeBlock) index = rangeBlock;		mAggregateLen = index;				// set prefix length from aggregate length		prefixLength = mAggregateLen;		netMask  = 0xffffffff << (32 - prefixLength);	} else {		mRangeDelta = 0;		mAggregateLen = 0;	}	hostField = (CIPNumberField*) this->FindPaneByID( kShowHostNumPane );	ThrowIfNil_( hostField );		switch ( inMessage ) {		// identify spin button		case msg_Spin1Up:		{			// increment prefix length			prefixLength +=1;			if (prefixLength > 32) prefixLength = 32;			netMask  = 0xffffffff << (32 - prefixLength);			hostMask = 0xffffffff >> prefixLength;			netNumber  = ipAddress & netMask;			hostNumber = ipAddress & hostMask;			// disable address range and CIDR length if any			mRangeDelta = 0;			cidrLength = 0;		}		break;				case msg_Spin1Down:		{			// decrement prefix length			if (prefixLength > 0) prefixLength -= 1;			netMask  = 0xffffffff << (32 - prefixLength);			hostMask = 0xffffffff >> prefixLength;			netNumber  = ipAddress & netMask;			hostNumber = ipAddress & hostMask;			// disable address range and CIDR length if any			mRangeDelta = 0;			cidrLength = 0;		}		break;				case msg_Spin2Up:		{			// check for address range			if (mRangeDelta == 0) {				// no, increment net number				netNumber = netNumber >> (32 - prefixLength);				netNumber +=1;				netNumber = netNumber << (32 - prefixLength);				netNumber &= netMask;				// disable CIDR length if any				cidrLength = 0;			} else {				// yes, get next CIDR aggregate if any				// add current block size to get next address				rangeBlock = (UInt32)0x01 << (32-mAggregateLen);				rangeAddress += rangeBlock;				// if still within range				if (rangeAddress <= ipAddress2) {					// indicate block has been added					mAggregateBlocks += rangeBlock;					// get new aggregate prefix length					// (the size of the next rangeDelta block to add)					// find largest block remaining					rangeDelta = mRangeDelta - mAggregateBlocks;	// subtract used blocks								rangeBlock = FindLeftBit(rangeDelta);					// look for right most 1 bit in agg address within remaining range					index = FindRightBit(rangeAddress, rangeRight);					// if not found, use largest block remaining					if (index < rangeBlock) index = rangeBlock;					mAggregateLen = index;										// set prefix length from aggregate length					prefixLength = mAggregateLen;					netMask  = 0xffffffff << (32 - prefixLength);				}			}		}		break;				case msg_Spin2Down:		{			// check for address range			if (mRangeDelta == 0) {				// no, decrement net number				netNumber = netNumber >> (32 - prefixLength);				netNumber -=1;				netNumber = netNumber << (32 - prefixLength);				netNumber &= netMask;				// disable CIDR length if any				cidrLength = 0;			} else {				// yes, get prev CIDR aggregate if any				// (current aggregate minus the largest rangeDelta block we can subtract)				// find largest block added				rangeBlock = FindLeftBit(mAggregateBlocks);				if (rangeBlock != 0) {					// look for right most 1 bit in agg address within remaining range					// otherwise use largest block added					index = FindRightBit(rangeAddress, rangeRight);					if (index < rangeBlock) index = rangeBlock;					// subtract block size to get prev CIDR address					rangeBlock = (UInt32)0x01 << (32-index);					rangeAddress -= rangeBlock;					// if address is still in range					if (rangeAddress >= ipAddress) {						// indicate block no longer included in aggregate						mAggregateBlocks -= rangeBlock;						mAggregateLen = index;												// set prefix length from aggregate length						prefixLength = mAggregateLen;						netMask  = 0xffffffff << (32 - prefixLength);					}				}				// didn't find a block no prev CIDR address			}		}		break;		case msg_Spin3Up:		{			// increment host number			hostNumber +=1;			hostNumber &= hostMask;			// disable address range and CIDR length if any			mRangeDelta = 0;			cidrLength = 0;		}		break;				case msg_Spin3Down:		{			// decrement host number			hostNumber -=1;			hostNumber &= hostMask;			// disable address range and CIDR length if any			mRangeDelta = 0;			cidrLength = 0;		}		break;				case msg_DropInField:		case msg_FieldActionKey:			if (mRangeDelta == 0) {				// user pressed enter in IP Address field				// set prefix length to match address class or cidr length (if any)				theValue = GetIPAddressClass( ipAddress, theString );				if (cidrLength >= 8) theValue = cidrLength;						// use prefix length 8 for Loopback address				if (theValue == kIPAddressLoopback) theValue = kIPAddressClassA;				if (theValue >= kIPAddressClassA) {					prefixLength = theValue;					netMask  = 0xffffffff << (32 - prefixLength);					hostMask = 0xffffffff >> prefixLength;					netNumber  = ipAddress & netMask;					hostNumber = ipAddress & hostMask;				}				// fall through to update captions from edit field			}		break;				case msg_HelpButton:		{											// Create the Help Window			CHelpWindow		*theWindow;			theWindow = (CHelpWindow*)InvokeWindow(kNameHelp, this);			theWindow->Show();			theWindow->LoadTextInWindow(rTEXT_SubnetCalcHelp);			break;		}			break;			}	// Refresh dummy pane to set background color correctly	theCaption = (LCaption*) this->FindPaneByID( kDummyPane );	ThrowIfNil_(theCaption);	theCaption->Draw(nil);		prefixCaption->SetValue(prefixLength);	// refresh prefix length caption	// address range?	if (mRangeDelta != 0) {		// process address range		// calculate CIDR aggregate		rangeAddress = ipAddress + mAggregateBlocks;		netNumber = rangeAddress & netMask;			// mask to aggregate range		// display CIDR aggregate		mNetField->SetValueLength(netNumber, mAggregateLen);		// set host address to empty		hostField->SetDescriptor("\p");	} else if (cidrLength != 0) {		// process CIDR aggregate		mNetField->SetValueLength(netNumber, prefixLength);	// refresh net field		ipAddress2 = netNumber | (0xffffffff & hostMask);		hostField->SetValueRange(netNumber, ipAddress2);	} else {		// not an address range or CIDR aggregate		mNetField->SetValueLength(netNumber, prefixLength);	// refresh net field		hostField->SetValue(hostNumber);		// refresh host caption		// Update IP address field		ipAddress = netNumber | hostNumber;		mAddressField->SetValue( ipAddress );	}	// Update number of addresses and IP adress class	// Display is based on whether number of address	// is less than a classful network	//		Class C			or		n Class C	//      n addresses				networks	SInt32	numberAddresses;	UInt8	classLength;	Str32	classString;	classLength = GetIPAddressClass( netNumber, classString );	theCaption = (LCaption*) this->FindPaneByID( kNumberOfAddressesPane );	ThrowIfNil_(theCaption);	numberAddresses = (UInt32)1 << (32 - prefixLength);	if (classLength < prefixLength) {		theString = numberAddresses;		if (numberAddresses > 8192) {			theString = numberAddresses / 1024;			theString += "\pK";		}		if (numberAddresses > 1) theString += "\p addresses";		else theString += "\p address";	} else {		numberAddresses = numberAddresses >> (32 - classLength);		if (numberAddresses > 1) theString = "\pnetworks";		else theString = "\pnetwork";	}	// leave blank for Class A or larger block	if (prefixLength < 8) theString = "\p";		theCaption->GetDescriptor( prevString );	if (theString != prevString) {		theCaption->SetDescriptor( theString );	}	// Show address class	if (classLength < prefixLength) {		theString = classString;	} else {		theString = numberAddresses;		if (numberAddresses > 8192) {			theString = numberAddresses / 1024;			theString += "\pK";		}		theString += "\p ";		theString += classString;	}	// leave blank for Class A or larger block	if (prefixLength < 8) theString = "\p";		theCaption = (LCaption*) this->FindPaneByID( kIPAddressClassPane );	ThrowIfNil_(theCaption);	theCaption->GetDescriptor( prevString );	if (theString != prevString) {		theCaption->SetDescriptor( theString );	}					// Update the mask field	theValue = 0xffffffff << (32 - prefixLength);	mMaskField->SetValue( theValue );			}// ---------------------------------------------------------------------------//		� HandleKeyPress// ---------------------------------------------------------------------------//	Handle <Return>, <Enter>, <Tab>, or <ESC> passed up from an EditField////	Return true if the Window handles the keystrokeBooleanCSubnetCalcWindow::HandleKeyPress(	const EventRecord&	inKeyEvent){	Boolean		keyHandled = true;	SInt16		theKey = inKeyEvent.message & charCodeMask;	switch (theKey) {		// Generate GURL for Helper applications		case '1':		case '2':		case '3':		case '4':			if ((inKeyEvent.modifiers & cmdKey) && gICInstance) {				ICError err;				long selStart, selEnd;				Str255	str;				LStr255	URLStr;				// get URL scheme of corresponding helper from resource				::GetIndString( str , rSTRN_GURLHelpers, (theKey-'0') );				// if we found a scheme, try it				if (str[0] > 0) {					URLStr = str;					// get IP address from window					mAddressField->GetDescriptor(str);					// append address to scheme					URLStr += str;					// Launch URL via Internet Config					selStart = 0;					selEnd = URLStr[0];					err = ICLaunchURL(gICInstance, "\p", (char*)&URLStr[1], URLStr[0],	&selStart, &selEnd);				}			}			break;		default:			if ( UKeyFilters::IsActionKey(theKey) )				ListenToMessage(msg_FieldActionKey, nil);			else				keyHandled = LCommander::HandleKeyPress(inKeyEvent);			break;	}	return keyHandled;}// ---------------------------------------------------------------------------//		� ObeyCommand// ---------------------------------------------------------------------------//	Respond to commandsBooleanCSubnetCalcWindow::ObeyCommand(	CommandT	inCommand,	void		*ioParam){	Boolean		cmdHandled = true;//	LWindow		*theWindow;//	Str255		str;	switch (inCommand) {/* Send AppleEvents instead		case cmd_TraceRoute:			// Create the Trace Route Window			theWindow = InvokeWindow("\pTrace Route", rPPob_TraceRouteWindow, mSuperCommander);			theWindow->Show();			mAddressField->GetDescriptor(str);			((CTraceRouteWindow*)theWindow)->InvokeTraceAddress(str);			break;		case cmd_TestConnectivity:			// Create the Test Connectivity Window			theWindow = InvokeWindow("\pTest Connectivity", rPPob_PingWindow, mSuperCommander);			theWindow->Show();			mAddressField->GetDescriptor(str);			((CPingWindow*)theWindow)->InvokePingAddress(str);			break;		case cmd_AddressScan:			// Create the Address Scan Window			theWindow = InvokeWindow(kScanWindowName, rPPob_AddressScanWindow, mSuperCommander);			theWindow->Show();			mAddressField->GetDescriptor(str);			((CAddressScanWindow*)theWindow)->InvokeAddressScan(str);			break;*/		default:			cmdHandled = CWindow::ObeyCommand(inCommand, ioParam);			break;	}	return cmdHandled;}// ---------------------------------------------------------------------------//		� Invoke Subnet Calc Window// ---------------------------------------------------------------------------//	Insert address in IP address field//	This method is called from the Trace Route windowvoidCSubnetCalcWindow::InvokeSubnetCalcWindow(ConstStr255Param inAddress){	mAddressField->SetDescriptor( inAddress );	ListenToMessage(msg_FieldActionKey, nil);}// ---------------------------------------------------------------------------------//		� DrawSelf// ---------------------------------------------------------------------------------//	Override DrawSelf to not paint window background underneath the data fields//	to avoid flashing.voidCSubnetCalcWindow::DrawSelf(){	Rect	windowRect, excludeRect1, excludeRect2, excludeRect3; //	PaintRect(&mMacWindowP->portRect);		// DEBUG, for checking update region	if (HasAttribute(windAttr_EraseOnUpdate)) {		ApplyForeAndBackColors();		//::EraseRect(&mMacWindowP->portRect);		windowRect = mMacWindowP->portRect;		mMaskField->CalcPortFrameRect(excludeRect1);		mAddressField->CalcPortFrameRect(excludeRect2);		mNetField->CalcPortFrameRect(excludeRect3);		windowRect.bottom = excludeRect2.top;		EraseMinusRect(windowRect, excludeRect1);				windowRect.top = windowRect.bottom;		windowRect.bottom = excludeRect3.top;		EraseMinusRect(windowRect, excludeRect2);				windowRect.top = windowRect.bottom;		windowRect.bottom = mMacWindowP->portRect.bottom;		EraseMinusRect(windowRect, excludeRect3);	}		DrawSizeBox();}