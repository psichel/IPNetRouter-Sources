// =================================================================================//	CInterfacesWindow.cp			�1997 Sustainable Softworks All rights reserved.// =================================================================================//	Configure Interface Window#include "CompileFlags.h"#include "CInterfacesWindow.h"#include "CInterfacesConst.h"#include "CRoutesWindow.h"#include "CGatewayWindow.h"#include "CRoutesConst.h"#include "CGatewayData.h"#include "CPortMapWindow.h"#include "CFilterWindow.h"#include "CArpWindow.h"#include "CDHCPWindow.h"#include "CConfigDocument.h"#include "CHelpWindow.h"#include "CDeviceData.h"#include "CConnectionData.h"#include "CGatewayData.h"#include "CInterfacesData.h"#include "CInterfacesAction.h"#include "CPortMapAction.h"#include "CRoutesAction.h"#include "CIPNumberEditField.h"#include "IPRouterCommon.h"#include "CSubnetCalcWindow.h"#include "UIPNumKeyFilters.h"//#include "CInterfacesTable.h"#include "CMultiCaption.h"#include "CIfCaption.h"#include "MacSupport.h"#include "IPSupport.h"#include "CLogAction.h"#include <UReanimator.h>#include <LString.h>#include <LPreferencesFile.h>#include <UResourceMgr.h>#include <LArrayIterator.h>#include <OpenTransport.h>// Globalsextern CLogAction*			gLogAction;extern CResidentIfInfo*		gResInfo;extern CDeviceData*			gDeviceData;extern CConnectionData* 	gConnectionData;extern CGatewayData*		gGatewayData;extern CInterfacesData*		gInterfacesData;extern CPortMapAction*		gPortMapAction;extern CRoutesAction*		gRoutesAction;extern CInterfacesAction*	gInterfacesAction;// ---------------------------------------------------------------------------------//		� CInterfacesWindow(LStream*)// ---------------------------------------------------------------------------------CInterfacesWindow::CInterfacesWindow(	LStream	*inStream )		: CWindow( inStream ){	// more initialization	mRoutesWindow = nil;	mGatewayWindow = nil;	mPortMapWindow = nil;	mFilterWindow = nil;	mArpWindow = nil;	mDHCPWindow = nil;}// ---------------------------------------------------------------------------------//		� ~CInterfacesWindow// ---------------------------------------------------------------------------------CInterfacesWindow::~CInterfacesWindow(){	// save which windows were open	SaveAtClose();		// request to stop notification	gConnectionData->RemoveListener(this);	// disconnect from device data	gDeviceData->RemoveListener(this);	// disconnect from interfaces data	gInterfacesData->RemoveListener(this);	// close any sub windows we created	if (mRoutesWindow) delete mRoutesWindow;	mRoutesWindow = nil;	if (mGatewayWindow) delete mGatewayWindow;	mGatewayWindow = nil;	if (mPortMapWindow) delete mPortMapWindow;	mPortMapWindow = nil;	if (mFilterWindow) delete mFilterWindow;	mFilterWindow = nil;	if (mArpWindow) delete mArpWindow;	mArpWindow = nil;	if (mDHCPWindow) delete mDHCPWindow;	mDHCPWindow = nil;}// ---------------------------------------------------------------------------------//		� FinishCreateSelf// ---------------------------------------------------------------------------------//	Finish creating the Interfaces window.voidCInterfacesWindow::FinishCreateSelf(){	// setup up IP interface display table	// get the table view	mInterfacesTable = (CInterfacesTable*) this->FindPaneByID( kInterfacesTablePane );	ThrowIfNil_( mInterfacesTable );	mInterfacesTable->SetWindow(this);	// tell table it's parent window	// setup up table captions	CMultiCaption	*theCaption;	theCaption = (CMultiCaption*) this->FindPaneByID( kInterfacesTableCaptionPane );	ThrowIfNil_( theCaption );	// customize the caption widths to match table outline	TableIndexT rows, cols, i;	short width;	mInterfacesTable->GetTableSize(rows, cols);	for (i=1; i<=cols; i++) {		width = mInterfacesTable->GetColWidth(i);		theCaption->SetColWidth(width, i, i);	}	// initialize interface state caption	ListenToMessage(kBringUpCheckBox, nil);		// Link the window (the listener) with the controls in	// the window (the broadcasters).	UReanimator::LinkListenerToControls(		(CInterfacesWindow *)this, this, rRidL_InterfacesWindow );	// setup pointer to status caption	mStatusCaption = (LCaption*)FindPaneByID( kStatusCaption );	ThrowIfNil_(mStatusCaption);	// setup pointer to connect button	mConnectButton = (LControl*) FindPaneByID(kConnectButton);	ThrowIfNil_(mConnectButton);	ShowConnection();						// show current state	gConnectionData->AddListener(this);		// ask to be updated		// Build Connect Via popup menu	mPortNamePopup = (LStdPopupMenu*)this->FindPaneByID( kPortNamePopup );	ThrowIfNil_(mPortNamePopup);	LoadPortMenu(mPortNamePopup);	// count how many items we got and set max value	MenuHandle theMenuH;	theMenuH = mPortNamePopup->GetMacMenuH();	mPortNamePopupMax = ::CountMItems(theMenuH);	mPortNamePopup->SetMaxValue(mPortNamePopupMax);	mPortNamePopup->SetValue(0);	// show default value	mPortNamePopup->SetValue(1);	// load the IP interface table    gInterfacesAction->UpdateInterfacesData();    UpdateTable();    // update Interfaces entry in Window    InterfaceEntry_t interfaceEntry;    gInterfacesData->GetDataInterfaceEntry(&interfaceEntry);    DisplayInterfaceEntry(&interfaceEntry);	// connect to device data	gDeviceData->AddListener(this);	// connect to interfaces data	gInterfacesData->AddListener(this);//	gInterfacesAction->UpdateInterfacesData();	// open any other Windows that were open when we last quite the app	StNewResource windowOpen( (ResType)'WOpn',		(ResIDT)129, kMaxOpenWindows*4, true );	// inDefaultSize, inGet1Resource	if (windowOpen.mResourceH) {		UInt8	index;		SInt32	windowCommand;		for (index=0; index<kMaxOpenWindows; index++) {			windowCommand = ((SInt32*)(*windowOpen.mResourceH))[index];			if (windowCommand > 0) ObeyCommand(windowCommand, nil);		}	}	UDesktop::SelectDeskWindow( this );}// ---------------------------------------------------------------------------//		� SaveAtClose// ---------------------------------------------------------------------------//	Try to close a Window as a result of direct user actionvoidCInterfacesWindow::SaveAtClose(){	GrafPtr		theWindowP;	LWindow		*theWindow;	UInt8		index;	// set resource in prefs file to remember which windows were open	StNewResource windowOpen( (ResType)'WOpn',		(ResIDT)129, kMaxOpenWindows*4, true );	// inDefaultSize, inGet1Resource	if (windowOpen.mResourceH) {		for (index=0; index<kMaxOpenWindows; index++) {			((SInt32*)(*windowOpen.mResourceH))[index] = 0;		}	}	index = 0;	// Make sure windows are closed in an orderly manner	// Filter Window	theWindowP = FindNamedIDWindow( kNameFilter );	if (theWindowP != nil) {	// exists?		// yes, close it		theWindow = LWindow::FetchWindowObject(theWindowP);		theWindow->AttemptClose();		((SInt32*)(*windowOpen.mResourceH))[index] = cmd_Filter;		index += 1;	}	// Arp Window	theWindowP = FindNamedIDWindow( kNameARP );	if (theWindowP != nil) {	// exists?		// yes, close it		theWindow = LWindow::FetchWindowObject(theWindowP);		theWindow->AttemptClose();		((SInt32*)(*windowOpen.mResourceH))[index] = cmd_Arp;		index += 1;	}	// DHCP Window	theWindowP = FindNamedIDWindow( kNameDHCP );	if (theWindowP != nil) {	// exists?		// yes, close it		theWindow = LWindow::FetchWindowObject(theWindowP);		theWindow->AttemptClose();		((SInt32*)(*windowOpen.mResourceH))[index] = cmd_DHCP;		index += 1;	}	// PortMap Window	theWindowP = FindNamedIDWindow( kNamePortMap );	if (theWindowP != nil) {	// exists?		// yes, close it		theWindow = LWindow::FetchWindowObject(theWindowP);		theWindow->AttemptClose();		((SInt32*)(*windowOpen.mResourceH))[index] = cmd_PortMap;		index += 1;	}	// Gateway Window	theWindowP = FindNamedIDWindow( kNameGateway );	if (theWindowP != nil) {	// exists?		// yes, close it		theWindow = LWindow::FetchWindowObject(theWindowP);		theWindow->AttemptClose();		((SInt32*)(*windowOpen.mResourceH))[index] = cmd_Gateway;		index += 1;	}	// Configure Routes Window	theWindowP = FindNamedIDWindow( kNameRoutes );	if (theWindowP != nil) {	// exists?		// yes, close it		theWindow = LWindow::FetchWindowObject(theWindowP);		theWindow->AttemptClose();		((SInt32*)(*windowOpen.mResourceH))[index] = cmd_ConfigureRt;		index += 1;	}}// ---------------------------------------------------------------------------------//		� LoadPortMenu// ---------------------------------------------------------------------------------voidCInterfacesWindow::LoadPortMenu(LStdPopupMenu* inPortNamePopup){	MenuHandle		theMenuH;	short			num;	short			index;	DeviceEntry_t	deviceEntry;	// Scan OT port registry to get port info if needed	if (!gDeviceData->GetDataDeviceArray(1, &deviceEntry)) gDeviceData->LoadDataDeviceArray();		// Build Connect Via popup menu	theMenuH = inPortNamePopup->GetMacMenuH();	if (theMenuH) {		// remove any previous menu items		num = ::CountMItems(theMenuH);		for (index=num; index>0; index--) {			::DeleteMenuItem(theMenuH, index);		}		// walk through device data to append new items		index = 1;		while ( gDeviceData->GetDataDeviceArray(index, &deviceEntry) ) {			// append userPortName to Connect Via menu			::AppendMenu(theMenuH, "\pDummy");			::SetMenuItemText(theMenuH, index, deviceEntry.userPortName);			// advance			index += 1;		}	}}// ---------------------------------------------------------------------------//		� HandleKeyPress// ---------------------------------------------------------------------------//	Handle <Return>, <Enter> passed up from an EditField////	Return true if the Window handles the keystrokeBooleanCInterfacesWindow::HandleKeyPress(	const EventRecord&	inKeyEvent){	Boolean		keyHandled = true;	LControl	*keyButton = nil;	SInt16		theKey = inKeyEvent.message & charCodeMask;	SInt16		menuChoice;	switch (theKey) {			//case char_Enter:		//case char_Return:		//	keyButton =  (LControl*) FindPaneByID(kAddButton);		//	break;		case char_UpArrow:			if (inKeyEvent.modifiers & cmdKey) {				menuChoice = mPortNamePopup->GetValue();				if (menuChoice > 1) {					menuChoice -= 1;					mPortNamePopup->SetValue(menuChoice);				}			}			break;		case char_DownArrow:			if (inKeyEvent.modifiers & cmdKey) {				menuChoice = mPortNamePopup->GetValue();				if (menuChoice < mPortNamePopupMax) {					menuChoice += 1;					mPortNamePopup->SetValue(menuChoice);				}			}			break;		default:			if (UKeyFilters::IsCmdPeriod(inKeyEvent)) {				// reset PPP controller				gResInfo->StopMonitoring();				gResInfo->MonitorPPP(nil);							//keyButton =  (LControl*) FindPaneByID(kTestButtonPane);			} else {				keyHandled = CWindow::HandleKeyPress(inKeyEvent);			}			break;	}				if (keyButton != nil) {		keyButton->SimulateHotSpotClick(kControlButtonPart);		keyHandled = true;	}		return keyHandled;}// ---------------------------------------------------------------------------------//		� ListenToMessage// ---------------------------------------------------------------------------------voidCInterfacesWindow::ListenToMessage(	MessageT	inMessage,	void		*ioParam ){	OSStatus 			err = noErr;	CIPNumberEditField* theEditField;	LStdCheckBox*		theCheckBox;	CIfCaption*			interfaceCaption;	SInt32				interfaceState;	UInt32				linkState;	Boolean				foundInfo;	DeviceEntry_t		deviceEntry;	InterfaceEntry_t	interfaceEntry;	Boolean				doClear = false;	Str31				str;	try {		switch ( inMessage ) {						case msg_SignalDataEvent:				UInt32 dataID = *(UInt32*)ioParam;				if (dataID == DATA_ID(kConnectionData, kDataLinkState)) {					// handle change to Connection Data					ShowConnection();				}				else if (dataID == DATA_ID(kDeviceData, kDataArrayLoaded)) {					// Device data updated					LoadPortMenu(mPortNamePopup);					// count how many items we got and set max value					MenuHandle theMenuH;					theMenuH = mPortNamePopup->GetMacMenuH();					mPortNamePopupMax = ::CountMItems(theMenuH);					mPortNamePopup->SetMaxValue(mPortNamePopupMax);					mPortNamePopup->SetValue(0);	// show default value					mPortNamePopup->SetValue(1);									}				else if (dataID == DATA_ID(kInterfacesData, kDataInterfaceEntry)) {					// Interfaces parameter data has changed										gInterfacesData->GetDataInterfaceEntry(&interfaceEntry);					DisplayInterfaceEntry(&interfaceEntry);				}				else if (dataID>>16 == kInterfacesData) {					// a row in the interfaces array has changed					TableIndexT	row;					row = dataID & 0xFFFF;					if (row > kDataInterfacesArrayOffset) {						row -= kDataInterfacesArrayOffset;						// try to get data for this row						if (gInterfacesData->GetDataInterfacesArray(row, &interfaceEntry)) {							// got it, update table and refresh display if needed							mInterfacesTable->SetInterfaceEntry(row, &interfaceEntry);						}						else {							// no data, row must have been deleted							mInterfacesTable->ClearRow(row);						}					}				}				break;							case msg_AddButton:			case msg_RemoveButton:				// Get the menu item text - it's the user name of an OT Registered Port				::GetMenuItemText( mPortNamePopup->GetMacMenuH(),					mPortNamePopup->GetValue(), interfaceEntry.userPortName );				foundInfo = gDeviceData->FindDataDeviceArray(interfaceEntry.userPortName, nil, nil, &deviceEntry);				if (foundInfo) LString::CopyPStr(deviceEntry.portName, interfaceEntry.portName);				// Get interface name we want to configure				GetDisplayIfName(interfaceEntry.interfaceName);				// Get interface address				theEditField = (CIPNumberEditField*)FindPaneByID( kIPAddressField );					ThrowIfNil_(theEditField);				interfaceEntry.interfaceAddr = theEditField->GetValue();				// Get interface mask				theEditField = (CIPNumberEditField*)FindPaneByID( kSubnetMaskField );					ThrowIfNil_(theEditField);				interfaceEntry.netMask = theEditField->GetValue();				// Get Interface flags				theCheckBox = (LStdCheckBox*)this->FindPaneByID(kBringUpCheckBox);				ThrowIfNil_(theCheckBox);				if (theCheckBox->GetValue() == 1) {					interfaceEntry.bringUp = true;				} else {					interfaceEntry.bringUp = false;				}				// Unnumbered				theCheckBox = (LStdCheckBox*)this->FindPaneByID(kUnnumberedCheckBox);				ThrowIfNil_(theCheckBox);				if (theCheckBox->GetValue() == 1) {					interfaceEntry.unnumbered = true;				} else {					interfaceEntry.unnumbered = false;				}				// DHCP				theCheckBox = (LStdCheckBox*)this->FindPaneByID(kDhcpCheckBox);				ThrowIfNil_(theCheckBox);				if (theCheckBox->GetValue() == 1) {					interfaceEntry.dhcp = true;				} else {					interfaceEntry.dhcp = false;				}				// Masquerading				theCheckBox = (LStdCheckBox*)this->FindPaneByID(kMasqueradingCheckBox);				ThrowIfNil_(theCheckBox);				if (theCheckBox->GetValue() == 1) {					interfaceEntry.masquerading = true;				} else {					interfaceEntry.masquerading = false;				}				// Need Gateway?				if ((interfaceEntry.interfaceAddr == 0) ||					(CalcPseudoGateway(interfaceEntry.interfaceAddr) == gResInfo->mPseudoGateway)) {					interfaceEntry.needGateway = true;				} else interfaceEntry.needGateway = false;				// Wamnet				interfaceEntry.wamnet = false;				// MTU				interfaceEntry.mtu = 0;								// Add or Remove as requested				if (inMessage == msg_AddButton) {					err = gInterfacesAction->DoInterfaceEntry(&interfaceEntry, true);					IoctlResponse(err);				}				else if (inMessage == msg_RemoveButton) {					// update interfaceEntry display					if (CalcPseudoGateway(interfaceEntry.interfaceAddr) == gResInfo->mPseudoGateway) {						doClear = true;					}					// set flags to bring down					interfaceEntry.bringUp = false;					// Try to remove it					err = gInterfacesAction->RemoveInterface(&interfaceEntry, true);					IoctlResponse(err);					if ((err == kOTNoError) && doClear) {						gInterfacesData->GetDataInterfaceEntry(&interfaceEntry);						interfaceEntry.interfaceAddr = 0;						interfaceEntry.netMask = 0;						gInterfacesData->SetDataInterfaceEntry(&interfaceEntry);					}				}				gInterfacesData->SignalDataChange();				break;						case msg_ConnectButton:				linkState = gConnectionData->GetDataLinkState();				if (linkState == kLinkStateConnected) {					gGatewayData->SetDataRemainConnected(false);					gResInfo->DisconnectPPP(nil);				} else if (linkState == kLinkStateIdle) {					gResInfo->ConnectPPP(nil);				}				break;							case msg_InterfaceRefreshButton:				// load the interface table				err = gInterfacesAction->UpdateInterfacesData();				IoctlResponse(err);				UpdateTable();								break;						case msg_HelpButton:				// test code				//gInterfacesAction->ListenToMessage(kOTProviderWillClose, nil);				//break;				// end of test code											// Create the Help Window				CHelpWindow		*theWindow;				theWindow = (CHelpWindow*)InvokeWindow(kNameHelp, this);				theWindow->Show();				theWindow->LoadTextInWindow(rTEXT_ConfigureIFHelp);				break;							case kPortNamePopup:				{					// Get interface name field					// Get the menu item text - it's the name of an OT Registered Port					::GetMenuItemText( mPortNamePopup->GetMacMenuH(),						mPortNamePopup->GetValue(), interfaceEntry.userPortName );										// Find corresponding interface name					if (gDeviceData->FindDataDeviceArray(interfaceEntry.userPortName, nil, nil, &deviceEntry)) {						SetDisplayIfName(deviceEntry.interfaceName);						mStatusCaption->SetDescriptor("\pOK");					} else {						// tell user what happened						SetDisplayIfName("\p");		// empty						if (interfaceEntry.userPortName[0] != 0) {							gLogAction->LogText("\p\rFindDataDeviceArray - Requested device not found: ");							gLogAction->LogText(interfaceEntry.userPortName);						}					}				}				break;						case kIfNameUp: {				LStr255 ifName;				Str31	moduleName;				SInt32	deviceNumber, interfaceNumber;												GetDisplayIfName(ifName);				gInterfacesAction->ParseIfName(ifName, moduleName, deviceNumber, interfaceNumber);				interfaceNumber += 1;				if (interfaceNumber >= 200) interfaceNumber = 200;				ifName = moduleName;				::NumToString(deviceNumber, str);				ifName += str;				if (interfaceNumber > 0) {					::NumToString(interfaceNumber, str);					ifName += "\p:";					ifName += str;				}				SetDisplayIfName(ifName);				break;			}			case kIfNameDown: {				LStr255 ifName;				Str31	moduleName;				SInt32	deviceNumber, interfaceNumber;												GetDisplayIfName(ifName);				gInterfacesAction->ParseIfName(ifName, moduleName, deviceNumber, interfaceNumber);				interfaceNumber -= 1;				if (interfaceNumber < 0) interfaceNumber = 0;				ifName = moduleName;				::NumToString(deviceNumber, str);				ifName += str;				if (interfaceNumber > 0) {					::NumToString(interfaceNumber, str);					ifName += "\p:";					ifName += str;				}				SetDisplayIfName(ifName);				break;			}			case kBringUpCheckBox:			case kUnnumberedCheckBox:			case kDhcpCheckBox:			case kMasqueradingCheckBox:				// get currently displayed interface state				interfaceCaption = (CIfCaption*)this->FindPaneByID(kIfCaption);				ThrowIfNil_(interfaceCaption);				interfaceState = interfaceCaption->GetValue();				// update based on check boxes					// bring up?				theCheckBox = (LStdCheckBox*)this->FindPaneByID(kBringUpCheckBox);				ThrowIfNil_(theCheckBox);				if (theCheckBox->GetValue() == 1) interfaceState = kItemUp;				else interfaceState = kItemDown;					// unnumbered?				theCheckBox = (LStdCheckBox*)this->FindPaneByID(kUnnumberedCheckBox);				ThrowIfNil_(theCheckBox);				if (theCheckBox->GetValue() == 1) interfaceState |= kItemUnnumbered;					// DHCP?				theCheckBox = (LStdCheckBox*)this->FindPaneByID(kDhcpCheckBox);				ThrowIfNil_(theCheckBox);				if (theCheckBox->GetValue() == 1) interfaceState |= kItemDhcp;					// masquerading?				theCheckBox = (LStdCheckBox*)this->FindPaneByID(kMasqueradingCheckBox);				ThrowIfNil_(theCheckBox);				if (theCheckBox->GetValue() == 1) interfaceState |= kItemMasquerading;				interfaceCaption->SetValue(interfaceState);				break;			}	}	catch (const LException& inErr) {		Str31 errStr;		::NumToString( inErr.GetErrorCode(), errStr);		::ParamText(errStr, nil, nil, nil);		UDesktop::Deactivate();		// Alert will swallow Deactivate event		::Alert(ALRT_OTError, nil);		UDesktop::Activate();	} 	catch (...) {		gLogAction->LogText("\p\rCInterfacesWindow::ListenToMessage unexpected Exception");		::SysBeep(1);	}}// ---------------------------------------------------------------------------//		� ObeyCommand// ---------------------------------------------------------------------------//	Respond to commandsBooleanCInterfacesWindow::ObeyCommand(	CommandT	inCommand,	void		*ioParam){	Boolean		cmdHandled = true;	GrafPtr		theWindowP;	LWindow*	theWindow;	LEditField*	theEditField;	Str255		str;	switch (inCommand) {		case cmd_ConfigureRt:			// look for window by name			theWindowP = FindNamedIDWindow( kNameRoutes );			if (theWindowP != nil) {	// Already exists?				// yes, bring to front				mRoutesWindow = (CRoutesWindow*)LWindow::FetchWindowObject(theWindowP);				mRoutesWindow->Show();				UDesktop::SelectDeskWindow( mRoutesWindow );			} else {				// Create the Configure Routes Window				mRoutesWindow = (CRoutesWindow*)InvokeWindow(kNameRoutes, this);				mRoutesWindow->Show();			}			break;		case cmd_Gateway:			// look for window by name			theWindowP = FindNamedIDWindow( kNameGateway );			if (theWindowP != nil) {	// Already exists?				// yes, bring to front				mGatewayWindow = (CGatewayWindow*)LWindow::FetchWindowObject(theWindowP);				mGatewayWindow->Show();				UDesktop::SelectDeskWindow( mGatewayWindow );			} else {				// Create the Configure Gateway Window				mGatewayWindow = (CGatewayWindow*)InvokeWindow(kNameGateway, this);				mGatewayWindow->Show();			}			break;		case cmd_PortMap:			// look for window by name			theWindowP = FindNamedIDWindow( kNamePortMap );			if (theWindowP != nil) {	// Already exists?				// yes, bring to front				mPortMapWindow = (CPortMapWindow*)LWindow::FetchWindowObject(theWindowP);				mPortMapWindow->Show();				UDesktop::SelectDeskWindow( mPortMapWindow );			} else {				// Create the PortMapping Window				mPortMapWindow = (CPortMapWindow*)InvokeWindow(kNamePortMap, this);				mPortMapWindow->Show();			}			break;		case cmd_Filter:			// look for window by name			theWindowP = FindNamedIDWindow( kNameFilter );			if (theWindowP != nil) {	// Already exists?				// yes, bring to front				mFilterWindow = (CFilterWindow*)LWindow::FetchWindowObject(theWindowP);				mFilterWindow->Show();				UDesktop::SelectDeskWindow( mFilterWindow );			} else {				// Create the Filterping Window				mFilterWindow = (CFilterWindow*)InvokeWindow(kNameFilter, this);				mFilterWindow->Show();			}			break;		case cmd_Arp:			// look for window by name			theWindowP = FindNamedIDWindow( kNameARP );			if (theWindowP != nil) {	// Already exists?				// yes, bring to front				mArpWindow = (CArpWindow*)LWindow::FetchWindowObject(theWindowP);				mArpWindow->Show();				UDesktop::SelectDeskWindow( mArpWindow );			} else {				// Create the Arp Window				mArpWindow = (CArpWindow*)InvokeWindow(kNameARP, this);				mArpWindow->Show();			}			break;		case cmd_DHCP:			// look for window by name			theWindowP = FindNamedIDWindow( kNameDHCP );			if (theWindowP != nil) {	// Already exists?				// yes, bring to front				mDHCPWindow = (CDHCPWindow*)LWindow::FetchWindowObject(theWindowP);				mDHCPWindow->Show();				UDesktop::SelectDeskWindow( mDHCPWindow );			} else {				// Create the DHCP Window				mDHCPWindow = (CDHCPWindow*)InvokeWindow(kNameDHCP, this);				mDHCPWindow->Show();			}			break;		case cmd_SubnetCalculator:			// Create the Subnet Calculator Window			theWindow = InvokeWindow(kNameSubnetCalculator, mSuperCommander);			theWindow->Show();			theEditField = (LEditField*) this->FindPaneByID( kIPAddressField );			ThrowIfNil_(theEditField);			theEditField->GetDescriptor(str);			((CSubnetCalcWindow*)theWindow)->InvokeSubnetCalcWindow(str);			break;					default:			cmdHandled = CWindow::ObeyCommand(inCommand, ioParam);			break;	}	return cmdHandled;}// ---------------------------------------------------------------------------//		� FindCommandStatus// ---------------------------------------------------------------------------//	This function enables menu commands.//voidCInterfacesWindow::FindCommandStatus(	CommandT	inCommand,	Boolean		&outEnabled,	Boolean		&outUsesMark,	UInt16		&outMark,	Str255		outName){		switch (inCommand) {			// Return menu item status according to command messages.		// Any that we don't handle will be passed to supercommander		case cmd_ConfigureRt:				outEnabled = true;	// enable the Routes Window			outUsesMark = true;			outMark = (FindNamedIDWindow(kNameRoutes) != nil) ?				checkMark : noMark;			break;		case cmd_Gateway:				outEnabled = true;	// enable the Gateway Window			outUsesMark = true;			outMark = (FindNamedIDWindow(kNameGateway) != nil) ?				checkMark : noMark;			break;		case cmd_PortMap:				outEnabled = true;	// enable the PortMap Window			outUsesMark = true;			outMark = (FindNamedIDWindow(kNamePortMap) != nil) ?				checkMark : noMark;			break;		case cmd_Filter:				outEnabled = true;	// enable the Filter Window			outUsesMark = true;			outMark = (FindNamedIDWindow(kNameFilter) != nil) ?				checkMark : noMark;			break;		case cmd_Arp:				outEnabled = true;	// enable the Arp Window			outUsesMark = true;			outMark = (FindNamedIDWindow(kNameARP) != nil) ?				checkMark : noMark;			break;		case cmd_DHCP:				outEnabled = true;	// enable the DHCP Window			outUsesMark = true;			outMark = (FindNamedIDWindow(kNameDHCP) != nil) ?				checkMark : noMark;			break;		default:			mSuperCommander->FindCommandStatus(inCommand, outEnabled,												outUsesMark, outMark, outName);			break;	}}// ---------------------------------------------------------------------------//		� Allow Sub Removal// ---------------------------------------------------------------------------//	Override to control which windows can be closed.//		Just track when a sub window closes.BooleanCInterfacesWindow::AllowSubRemoval(	LCommander*	inSub){	if (inSub == mRoutesWindow) mRoutesWindow = nil;	if (inSub == mGatewayWindow) mGatewayWindow = nil;	if (inSub == mPortMapWindow) mPortMapWindow = nil;	if (inSub == mFilterWindow) mFilterWindow = nil;	if (inSub == mArpWindow) mArpWindow = nil;	if (inSub == mDHCPWindow) mDHCPWindow = nil;	return true;}// ---------------------------------------------------------------------------------//		� ShowConnection()// ---------------------------------------------------------------------------------//	Show PPP connection state in windowvoidCInterfacesWindow::ShowConnection(){	UInt32 linkState;	linkState = gConnectionData->GetDataLinkState();		switch (linkState) {		case kLinkStateIdle:			mConnectButton->SetDescriptor("\pConnect");			mConnectButton->Enable();			break;		case kLinkStateConnected:			mConnectButton->SetDescriptor("\pDisconnect");			mConnectButton->Enable();			break;		case kLinkStateConnecting:			mConnectButton->Disable();			mConnectButton->SetDescriptor("\pConnecting");			break;		case kLinkStateDisconnecting:			mConnectButton->Disable();			mConnectButton->SetDescriptor("\pDisconnecting");			break;		default:			mConnectButton->Disable();			mConnectButton->SetDescriptor("\pConnect");			break;	}}#pragma mark --- Interface Table ---// ---------------------------------------------------------------------------------//		� UpdateTable// ---------------------------------------------------------------------------------//	Update Interfaces Table from data objectvoidCInterfacesWindow::UpdateTable(){	ArrayIndexT			num;	ArrayIndexT			index;	InterfaceEntry_t	interfaceEntry;		num = gInterfacesData->GetDataCount();	for (index=1; index<=num; index++) {		if (gInterfacesData->GetDataInterfacesArray(index, &interfaceEntry)) {			mInterfacesTable->SetInterfaceEntry(index, &interfaceEntry);		} else {			mInterfacesTable->ClearRow(index);		}	}}// ---------------------------------------------------------------------------------//		� DisplayInterfaceEntry// ---------------------------------------------------------------------------------//	Display interface entry in input fieldsvoidCInterfacesWindow::DisplayInterfaceEntry(InterfaceEntry_t* inInterfaceEntry){	// if userPortName is not empty	if (inInterfaceEntry->userPortName[0] != 0) {		// find the menu		MenuHandle	theMenuH;		short		i;		Str63		str;				theMenuH = mPortNamePopup->GetMacMenuH();		// look for match		for (i=1; i<= mPortNamePopupMax; i++) {			::GetMenuItemText( theMenuH, i, str );			if (::EqualString(inInterfaceEntry->userPortName, str, true, false)) {				mPortNamePopup->SetValue(i);				// found match, now copy other fields								CIPNumberEditField* theEditField;				LStdCheckBox*		theCheckBox;				// interfaceName								SetDisplayIfName(inInterfaceEntry->interfaceName);				// ip address				theEditField = (CIPNumberEditField*)FindPaneByID( kIPAddressField );					ThrowIfNil_(theEditField);				if (inInterfaceEntry->interfaceAddr) {					theEditField->SetValue(inInterfaceEntry->interfaceAddr);				} else {					theEditField->SetDescriptor("\p");				}				// net mask				theEditField = (CIPNumberEditField*)FindPaneByID( kSubnetMaskField );					ThrowIfNil_(theEditField);				if (inInterfaceEntry->netMask) {					theEditField->SetValue(inInterfaceEntry->netMask);				} else {					theEditField->SetDescriptor("\p");				}				// interface state				theCheckBox = (LStdCheckBox*)FindPaneByID(kBringUpCheckBox);				ThrowIfNil_(theCheckBox);				if (inInterfaceEntry->bringUp) theCheckBox->SetValue(1);				else theCheckBox->SetValue(0);				// Masquerading							theCheckBox = (LStdCheckBox*)FindPaneByID(kMasqueradingCheckBox);				ThrowIfNil_(theCheckBox);				if (inInterfaceEntry->masquerading) theCheckBox->SetValue(1);				else theCheckBox->SetValue(0);								// Unnumbered				theCheckBox = (LStdCheckBox*)FindPaneByID(kUnnumberedCheckBox);				ThrowIfNil_(theCheckBox);				if (inInterfaceEntry->unnumbered) theCheckBox->SetValue(1);				else theCheckBox->SetValue(0);								// DHCP				theCheckBox = (LStdCheckBox*)FindPaneByID(kDhcpCheckBox);				ThrowIfNil_(theCheckBox);				if (inInterfaceEntry->dhcp) theCheckBox->SetValue(1);				else theCheckBox->SetValue(0);							}									}			}	// portName is not empty}// ---------------------------------------------------------------------------------//		� GetDisplayIfName// ---------------------------------------------------------------------------------//	Get Mentat style Interface Name from display fields//	Mentat interface names are formed as//		<driver-module-name><device-number>[:<logical-interface-number>]//	Since the user only needs to edit the <device-number> and optionally//	the <logical-interface-number>, we display <driver-module-name> as//	and LCaption and the rest as an LEditField.StringPtrCInterfacesWindow::GetDisplayIfName(Str63 outStr){	Str63			str;	LStr255 		returnStr;	LCaption*		interfaceNameCaption;//	LEditField*		interfaceNameField;		// get access to corresponding fields	interfaceNameCaption = (LCaption*)this->FindPaneByID(kIfNameCaption);	ThrowIfNil_(interfaceNameCaption);//	interfaceNameField = (LEditField*)this->FindPaneByID(kIfNameField);//	ThrowIfNil_(interfaceNameField);	// build result	returnStr = interfaceNameCaption->GetDescriptor(str);//	interfaceNameField->GetDescriptor(str);//	if (str[0] != 0) {//		returnStr += "\p:";//		returnStr += str;//	}		return LString::CopyPStr(returnStr, outStr);}// ---------------------------------------------------------------------------------//		� SetDisplayIfName// ---------------------------------------------------------------------------------//	Set display fields from Mentat style Interface NamevoidCInterfacesWindow::SetDisplayIfName(Str63 inStr){	LCaption*		interfaceNameCaption;//	LEditField*		interfaceNameField;//	LStr255			moduleName;//	Str31			str;//	SInt32			deviceNumber, interfaceNumber;		// get access to corresponding fields	interfaceNameCaption = (LCaption*)this->FindPaneByID(kIfNameCaption);	ThrowIfNil_(interfaceNameCaption);//	interfaceNameField = (LEditField*)this->FindPaneByID(kIfNameField);//	ThrowIfNil_(interfaceNameField);		// parse name//	ParseIfName(inStr, moduleName, deviceNumber, interfaceNumber);		// display parts//	::NumToString(deviceNumber, str);//	moduleName += str;//	if (interfaceNumber > 0) {//		interfaceNameField->SetValue(interfaceNumber);//	}//	else interfaceNameField->SetDescriptor("\p");//	interfaceNameCaption->SetDescriptor(moduleName);	interfaceNameCaption->SetDescriptor(inStr);}// ---------------------------------------------------------------------------------//		� IoctlResponse// ---------------------------------------------------------------------------------//	IoctlResponse - an ioctl completed process the resultvoidCInterfacesWindow::IoctlResponse(long inResult){	switch (inResult) {		case 3:				// get NDD forwarding unknown result?		case 0:				// no error			mStatusCaption->SetDescriptor("\pOK");			break;		case kOTNotFoundErr:			mStatusCaption->SetDescriptor("\pkOTNotFoundErr.");			break;				case kEINVALErr:			mStatusCaption->SetDescriptor("\pkEINVALErr - Invalid parameter.");			break;		case kOTDuplicateFoundErr:					mStatusCaption->SetDescriptor("\pkOTDuplicateFoundErr.");			break;		case kEBUSYErr:			mStatusCaption->SetDescriptor("\pkEBUSYErr - Device or resource busy.");			break;		case kPPPNotPrimaryErr:			mStatusCaption->SetDescriptor("\pUnable to configure, PPP is not the primary interface.");			break;		case kENXIOErr:			mStatusCaption->SetDescriptor("\pNo such device or address.");			break;		default:			{				LStr255	status;				status = "\pioctl unexpected result code: ";				Str32 str;				NumToString(inResult, str);				status += str;				mStatusCaption->SetDescriptor( status );			}			break;	}	// end switch}