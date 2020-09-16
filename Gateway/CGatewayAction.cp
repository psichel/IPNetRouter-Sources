// =================================================================================//	CGatewayAction.cp	 		�1999 Sustainable Softworks. All rights reserved.// =================================================================================//	Action methods for gateway functions//  Corresponds to GatewayData and GatewayWindow#include "CompileFlags.h"#include "CGatewayAction.h"#include "CResidentIfInfo.h"#include "CPortMapConst.h"#include "CPortMapAction.h"#include "CPortMapData.h"#include "CDNSAction.h"#include "CDNSConst.h"#include "CLogAction.h"#include "CIPRouterApp.h"#include "IPRouterCommon.h"#include "MacSupport.h"#include "IPSupport.h"#include <LString.h>#include <LPreferencesFile.h>#include <UResourceMgr.h>#include <LThread.h>#include <OpenTransport.h>#include <OpenTptInternet.h>#include "CSendUDPThread.h"#include "CTurboUDPEndpoint.h"#include "CDialupDNSClient.h"//#include "CNetMessage.h"#include "CProxyControl.h"#include "CConfigDocument.h"#include "CGatewayData.h"		// pick up "UponOpen" constants#include "CConnectionData.h"#include "CInterfacesData.h"#include "CInterfacesAction.h"#include "CDeviceData.h"//#include "myDNS.h"// Globalsextern CLogAction*			gLogAction;extern CResidentIfInfo*		gResInfo;extern CProxyControl*		gProxyControl;extern CGatewayData*		gGatewayData;extern CConnectionData*		gConnectionData;extern CInterfacesData*	 	gInterfacesData;extern CDeviceData*			gDeviceData;extern CInterfacesAction*	gInterfacesAction;extern CPortMapAction*		gPortMapAction;extern CPortMapData*		gPortMapData;extern CDNSAction*			gDNSAction;extern UInt8				gApplicationMode;// ---------------------------------------------------------------------------------//		� CGatewayAction()// ---------------------------------------------------------------------------------CGatewayAction::CGatewayAction(){	// initialization	// request data change notification	gGatewayData->AddListener(this);	gConnectionData->AddListener(this);	mDialupDNSClient = nil;}// ---------------------------------------------------------------------------------//		� ~CGatewayAction// ---------------------------------------------------------------------------------CGatewayAction::~CGatewayAction(){	if (mDialupDNSClient) delete mDialupDNSClient;	// no longer a listener	gGatewayData->RemoveListener(this);	gConnectionData->RemoveListener(this);}// ---------------------------------------------------------------------------------//		� ListenToMessage// ---------------------------------------------------------------------------------voidCGatewayAction::ListenToMessage(	MessageT	inEventCode,	void		*ioParam ){	Str32		str;	switch ( inEventCode ) {		case msg_SignalDataEvent:			UInt32 dataID = *(UInt32*)ioParam;			// handle change to Enable Local NAT state			if (dataID == DATA_ID(kGatewayData, kDataEnableLocalNAT)) {				OTResult err;				if (gGatewayData->GetDataEnableLocalNAT()) {					err = gProxyControl->ProxyIoctl(I_LocalNatOn, nil);					if (err == kOTNoError)						gLogAction->LogText("\p\rEnable NAT on local interface");				} else {					err = gProxyControl->ProxyIoctl(I_LocalNatOff, nil);					if (err == kOTNoError)						gLogAction->LogText("\p\rDisable NAT on local interface");				}			}			// handle change to DNSForwarding state			else if (dataID == DATA_ID(kGatewayData, kDataDNSForwarding)) {				OTResult err;				if (gGatewayData->GetDataDNSForwarding()) {					err = gProxyControl->ProxyIoctl(I_DNSForwardingOn, nil);					if (err == kOTNoError)						gLogAction->LogText("\p\rEnable DNS Forwarding");				} else {					err = gProxyControl->ProxyIoctl(I_DNSForwardingOff, nil);					if (err == kOTNoError)						gLogAction->LogText("\p\rDisable DNS Forwarding");				}				SetupDNSForwarding();			}			// update DNS forwarding if link state changes			else if (dataID == DATA_ID(kConnectionData, kDataLinkState)) {				SetupDNSForwarding();			}			// handle change to DialupDNS state				// do this upon restore settings or user action from window//			else if (dataID == DATA_ID(kGatewayData, kDataDialupDNS)) {//				if (gGatewayData->GetDataDialupDNS()) DialupDNSClientStart();//				else DialupDNSClientStop(true);//			}			// handle change to LimitMTUForPPPoE state			else if (dataID == DATA_ID(kGatewayData, kDataLimitMTUForPPPoE)) {				//OTResult err;				if (gGatewayData->GetDataLimitMTUForPPPoE()) {					//err = gProxyControl->ProxyIoctl(I_LimitMTUOn, nil);					//if (err == kOTNoError)						gLogAction->LogText("\p\rEnable Better PPPoE Routing upon next Configure NAT");				} else {					//err = gProxyControl->ProxyIoctl(I_LimitMTUOff, nil);					//if (err == kOTNoError)						gLogAction->LogText("\p\rDisable Better PPPoE Routing upon next Configure NAT");				}			}			// handle change to TRCableModem state			else if (dataID == DATA_ID(kGatewayData, kDataTRCableModem)) {				OTResult err;				if (gGatewayData->GetDataTRCableModem()) {					err = gProxyControl->ProxyIoctl(I_TRCableModem, nil);					if (err == kOTNoError)						gLogAction->LogText("\p\rEnable TR Cable Modem");				} else {					err = gProxyControl->ProxyIoctl(I_TRCableModemOff, nil);					if (err == kOTNoError)						gLogAction->LogText("\p\rDisable TRCableModem");				}			}			// handle change to Exposed Host state			else if ( (dataID == DATA_ID(kGatewayData, kDataExposedHost)) ||					  (dataID == DATA_ID(kGatewayData, kDataExposedHostAddress)) ) {				OTResult err = kProxyNoError;				struct strioctl	streamCommand;				UInt32			value;				value = gGatewayData->GetDataExposedHost();				if (value == kExposedHostNone) {					value = 1;					gLogAction->LogText("\p\rExposed Host None");				}				else if (value == kExposedHostGateway) {					value = 2;					gLogAction->LogText("\p\rExposed Host Gateway");				}				else if (value == kExposedHostAddress) {					value = gGatewayData->GetDataExposedHostAddress();					gLogAction->LogText("\p\rExposed Host Address ");					IP_NumToStr(value, str);					gLogAction->LogText(str);					gLogAction->Update();				}				else {					value = 0;	// default					gLogAction->LogText("\p\rDefault to exposed Host Gateway");				}				// Send IOCTL to set exposed host address				proxy_network_t	exposedHost;				exposedHost.portNameHash = gPortMapData->GetDataPortMapHash();				exposedHost.net.address = value;				exposedHost.net.mask = 0;				SetupIoctl(&streamCommand, I_SetExposedHost,					(char*)&exposedHost, sizeof(proxy_network_t));				err = gProxyControl->ProxyIoctl(I_STR, &streamCommand);				if (err != kProxyNoError) {					gLogAction->LogText("\p\rCGatewayAction - Set Exposed Host error");					break;				}			}			// handle change to Remain Connected state			else if (dataID == DATA_ID(kGatewayData, kDataRemainConnected)) {				if (!gResInfo) break;				gResInfo->SetTickle( gGatewayData->GetDataRemainConnected() );			}			break;		default:			gLogAction->LogText("\p\rCGatewayAction unknown async message type: ");			IP_NumToHexStr(inEventCode, str);			gLogAction->LogText(str);	}}// ---------------------------------------------------------------------------------//		� SetupDNSForwarding// ---------------------------------------------------------------------------------// Setup the IPNR gateway as a DNS forwarding agent.// Any DNS requests (UDP port 53) received on a private interface are remapped// to the currently configured DNS server for the gateway.//// For each non masqueraded interface, create a port mapping entry to our DNS server//// To allow DNS Forwarding to be enabled or disabled on demand, we mark// each mapping entry we create with the kFlagDNSForwarding.  Anytime// the interface configuration is modified, we may need to adjust the// DNS forwarding entries, so we scan the existing entries and add// or remove as needed.OTResultCGatewayAction::SetupDNSForwarding(){	CPortMapData		tPortMapData;	PortMapEntry_t		tMapEntry;	ArrayIndexT			numTableE, row, index;	InterfaceEntry_t	ifTableEntry;	UInt32				result = 0;	Boolean				isEnable;	do {		if (!gDNSAction) break;				if (!gResInfo) break;		// check for DNS Forwarding enabled		isEnable = gGatewayData->GetDataDNSForwarding();		// enable deferral server for local DNS Proxy		if (isEnable && gResInfo->IsMonitoringPPP()) gDNSAction->StartServing();		else gDNSAction->StopServing();		// we disable forwarding to allow DNS Deferral if		// we have a PPP connection that is not connected		if (gResInfo->IsMonitoringPPP()) {			UInt32 linkState;			linkState = gConnectionData->GetDataLinkState();			if (linkState != kLinkStateConnected) {				isEnable = false;				gDNSAction->ResumeDeferral();			}		}				// make sure we init port map action		if (!gPortMapAction) break;		if (!gInterfacesAction) break;		gPortMapAction->Init();				// copy existing port map entries with kDNSForwarding flag		numTableE = gPortMapData->GetDataCount();		index = 1;		for (row=1; row<=numTableE; row++) {			if ( !gPortMapData->GetDataPortMapArray(row, &tMapEntry) ) continue;			if (tMapEntry.flags & kFlagDNSForwarding) {				tPortMapData.SetDataPortMapArray(index, &tMapEntry);				index += 1;			}		}				// if we're enabling, add any needed entries		if (isEnable) {					// scan list of interfaces			numTableE = gInterfacesData->GetDataCount();			for (row=1; row<=numTableE; row++) {				if ( !gInterfacesData->GetDataInterfacesArray(row, &ifTableEntry) ) continue;				// ignore the loopback interface				if (::EqualString("\plo0", ifTableEntry.interfaceName, false, false)) continue;				// if it's not masqueraded				if (ifTableEntry.bringUp && !ifTableEntry.masquerading) {								// build PortMap entry to					// map DNS port from private LAN IP to configured DNS IP					PortMapEntry_t	portMapEntry;					Str255			addrStr;					UInt32			addr;					bzero((UInt8*)&portMapEntry, sizeof(PortMapEntry_t));					// private LAN endpoint					portMapEntry.apparent.address = ifTableEntry.interfaceAddr;					portMapEntry.apparent.port = kDNSServerPort;					// configured DNS endpoint					gInterfacesAction->GetDNSAddress(addrStr);					addr = IP_StrToInt(addrStr);					if ((addr == 0) || (addr == ifTableEntry.interfaceAddr)) {						// DNS Address is invalid or not available						gLogAction->LogText("\p\rSetupDNSForwarding - DNS Address not available ");						gLogAction->LogText(addrStr);						continue;					}					portMapEntry.actual.address = addr;					portMapEntry.actual.port = kDNSServerPort;					portMapEntry.protocol = kProtocolUDP;					portMapEntry.flags |= kFlagActive + kFlagPermanent + kFlagDNSForwarding;					// check if we already have it					index = tPortMapData.FindMatchPortMapEntry(&portMapEntry);					if (index) {						// yes, remove entry from list						tPortMapData.RemoveDataPortMapArray(index);						// make sure we got them all						index = tPortMapData.FindMatchPortMapEntry(&portMapEntry);						while (index) {							tPortMapData.RemoveDataPortMapArray(index);							index = tPortMapData.FindMatchPortMapEntry(&portMapEntry);						}					} else {						// no, add to configuration						result = gPortMapAction->DoPortMapEntry(&portMapEntry, false);						if (result != kOTNoError) {							gLogAction->LogText("\p\rCGatewayAction: unexpected result adding PortMap entry", result);						}					}				}			}		}				// any remaining PortMap entries on temp list are no longer valid, remove them		numTableE = tPortMapData.GetDataCount();		for (row=1; row<=numTableE; row++) {			if ( !tPortMapData.GetDataPortMapArray(row, &tMapEntry) ) continue;			// mark inactive to remove			tMapEntry.flags &= ~kFlagActive;			result = gPortMapAction->DoPortMapEntry(&tMapEntry, false);		}				// refresh table		gPortMapAction->UpdatePortMapData();		// stop DNS Deferral if DNS forwarding is enabled		if (isEnable) gDNSAction->StopDeferral();	} while (false);	return result;}// ---------------------------------------------------------------------------------//		� DialupDNSClientStart// ---------------------------------------------------------------------------------// Login or close dialup DNS connection as needed based on://   input parameter//   dialupDNS setting in Gateway Window//// This function will normally be called when we potentially have a new IP address// such as after a new PPP connection is established or restore with a DHCP// aware interface. OTResultCGatewayAction::DialupDNSClientStart(){	OTResult result = kOTNoError;	// if dialupDNS is enabled	if (gGatewayData->GetDataDialupDNS()) {			// create dialupDNSClient object if needed		if (!mDialupDNSClient) mDialupDNSClient = new CDialupDNSClient();		if (mDialupDNSClient) {			// close any previous connection			mDialupDNSClient->CloseConnection(true);			// open a new connection			result = mDialupDNSClient->InitConnection();			// try to login		}	}	else {		// DialupDNSClient is off, close any previous connection		if (mDialupDNSClient) mDialupDNSClient->CloseConnection(true);	}		return result;}// ---------------------------------------------------------------------------------//		� DialupDNSClientStop// ---------------------------------------------------------------------------------voidCGatewayAction::DialupDNSClientStop(Boolean inCanWait){	if (mDialupDNSClient) mDialupDNSClient->CloseConnection(inCanWait);}