// ===========================================================================//	CDHCPData.h			�1999 Sustainable Softworks, All rights reserved.// ===========================================================================// DHCP Data (corresponds to DHCP window)#ifndef _H_CDHCPData#define _H_CDHCPData#pragma once#include <LArray.h>#include "MyTypes.h"// maximum size a corresponding text entry in config document#define kMaxDHCPSize		512#define kInterfaceNameDim	40		// size of InterfaceNameconst Str31 kDelimeterStr		= "\p\\";const Str31 kDelimeterEOLStr	= "\p\r";// DHCP Status Data Elementtypedef struct DHCPStatusEntry_t {	UInt32		leaseAddr;	UInt32		lastUpdate;	UInt32		expireTime;	MACAddr_t	hardwareAddr;	Str255		clientID;	UInt8		leaseState;} DHCPStatusEntry_t;// lease state in DHCP Status elementconst SInt16 kLeaseNone			    = 0;const SInt16 kLeaseOffered			= 1;const SInt16 kLeaseBound			= 2;const SInt16 kLeaseReleased		    = 3;const SInt16 kLeaseExpired			= 4;const SInt16 kLeaseDeclined		    = 5;const SInt16 kLeaseInUse			= 6;const SInt16 kLeaseBootP			= 7;const Str31 kLeaseOfferedStr	    = "\poffered";const Str31 kLeaseBoundStr		    = "\pbound";const Str31 kLeaseReleasedStr	    = "\preleased";const Str31 kLeaseExpiredStr	    = "\pexpired";const Str31 kLeaseDeclinedStr	    = "\pdeclined";const Str31 kLeaseInUseStr		    = "\pin use";const Str31 kLeaseBootPStr		    = "\pBootP";const Str31 kDHCPStatusStr		    = "\p+dhcpStatus";const Str31 kDHCPStaticCfgStr	    = "\p+dhcpStatic";const Str31 kDHCPDynamicCfgStr	    = "\p+dhcpDynamic";const Str31 kDHCPLeaseDataStr	    = "\p+dhcpLeaseOptions";const Str31 kDHCPmLeaseDataStr	    = "\p-dhcpLeaseOptions";const Str31 kDHCPServerOnStr		= "\p#DHCPServerOn";const Str31	kDHCPVerboseLoggingStr	= "\p#DHCPVerboseLogging";const SInt16 kLeaseOfferPeriod		= 120;	// period for changing offered to expiredconst SInt16 kLeaseGracePeriod		= 120;	// grace period for changing bound to expiredconst SInt32 kLeaseReclaimPeriod	= 5184000;	// 60 days// DHCP Static Config Data Elementtypedef struct DHCPStaticCfgEntry_t {	UInt32		interfaceAddr;	UInt32		leaseAddr;	MACAddr_t	hardwareAddr;	Str255		clientID;	Boolean		isDone;} DHCPStaticCfgEntry_t;// DHCP Dynamic Config Data Elementtypedef struct DHCPDynamicCfgEntry_t {	UInt32		interfaceAddr;	UInt32		startLeaseAddr;	UInt32		endLeaseAddr;	Boolean		isDone;} DHCPDynamicCfgEntry_t;// DHCP Lease Data Entrytypedef struct DHCPLeaseDataEntry_t {	UInt32		interfaceAddr;	UInt32		netMask;	UInt32		leaseDefaultTime;	UInt32		leaseMaxTime;	Str255		routerAddrStr;		// list of router Addr separated by CRs	Str255		nameServerAddrStr;	// list of NS Addr separated by CRs	Str255		domainNameStr;	Boolean		isEnabled;	Boolean		isDone;} DHCPLeaseDataEntry_t;// lease time is in seconds, but displays as minutes, hours, or daysconst SInt32 kMinuteSeconds	    = 60;const SInt32 kHourSeconds		= 3600;const SInt32 kDaySeconds		= kHourSeconds * 24;#include "CSignalDataEvent.h"class	CDHCPData : public CSignalDataEvent {public:					CDHCPData();	virtual			~CDHCPData();	void			Init();		void			SetDataDHCPServerOn(Boolean inValue);	Boolean			GetDataDHCPServerOn();		void			SetDataDHCPVerboseLogging(Boolean inValue);	Boolean			GetDataDHCPVerboseLogging();		ArrayIndexT		SetDataDHCPStatusArray(ArrayIndexT index, DHCPStatusEntry_t* inEntry);	Boolean			GetDataDHCPStatusArray(ArrayIndexT index, DHCPStatusEntry_t* outEntry);	Boolean			RemoveDataDHCPStatusArray(ArrayIndexT index);	ArrayIndexT		GetDataCountDHCPStatus();	ArrayIndexT		SetDataCountDHCPStatus(ArrayIndexT count);	UInt32			StatusEntryToText(DHCPStatusEntry_t* inEntry, UInt8* dp, UInt32 maxLen);	Boolean			TextToStatusEntry(UInt8* dp, UInt32 start, UInt32 end, DHCPStatusEntry_t* outEntry);		ArrayIndexT		SetDataDHCPStaticCfgArray(ArrayIndexT index, DHCPStaticCfgEntry_t* inEntry);	Boolean			GetDataDHCPStaticCfgArray(ArrayIndexT index, DHCPStaticCfgEntry_t* outEntry);	ArrayIndexT		GetDataCountDHCPStaticCfg();	ArrayIndexT		SetDataCountDHCPStaticCfg(ArrayIndexT count);	UInt32			StaticCfgEntryToText(DHCPStaticCfgEntry_t* inEntry, UInt8* dp, UInt32 maxLen);	Boolean			TextToStaticCfgEntry(UInt8* dp, UInt32 start, UInt32 end, DHCPStaticCfgEntry_t* outEntry);		ArrayIndexT		SetDataDHCPDynamicCfgArray(ArrayIndexT index, DHCPDynamicCfgEntry_t* inEntry);	Boolean			GetDataDHCPDynamicCfgArray(ArrayIndexT index, DHCPDynamicCfgEntry_t* outEntry);	ArrayIndexT		GetDataCountDHCPDynamicCfg();	ArrayIndexT		SetDataCountDHCPDynamicCfg(ArrayIndexT count);	UInt32			DynamicCfgEntryToText(DHCPDynamicCfgEntry_t* inEntry, UInt8* dp, UInt32 maxLen);	Boolean			TextToDynamicCfgEntry(UInt8* dp, UInt32 start, UInt32 end, DHCPDynamicCfgEntry_t* outEntry);		ArrayIndexT		SetDataDHCPLeaseDataArray(ArrayIndexT index, DHCPLeaseDataEntry_t* inEntry);	Boolean			GetDataDHCPLeaseDataArray(ArrayIndexT index, DHCPLeaseDataEntry_t* outEntry);	Boolean			RemoveDataDHCPLeaseDataArray(ArrayIndexT index);	ArrayIndexT		GetDataCountDHCPLeaseData();	ArrayIndexT		SetDataCountDHCPLeaseData(ArrayIndexT count);	UInt32			LeaseDataEntryToText(DHCPLeaseDataEntry_t* inEntry, UInt8* dp, UInt32 maxLen);	Boolean			TextToLeaseDataEntry(UInt8* dp, UInt32 start, UInt32 end, DHCPLeaseDataEntry_t* outEntry);	void			SignalDataChange();	Boolean			mSeenDHCPServerOn;	Boolean			mSeenDHCPVerboseLogging;				private:	LArray*			mDHCPStatusArray;	LArray*			mDHCPStaticCfgArray;	LArray*			mDHCPDynamicCfgArray;	LArray*			mDHCPLeaseDataArray;		Boolean			mDHCPServerOn;	Boolean			mDHCPVerboseLogging;};// constants for signalling what data has changedconst SInt16 kDHCPStatusData			= 'd';const SInt16 kDHCPStaticCfgData		    = 'h';const SInt16 kDHCPDynamicCfgData		= 'c';const SInt16 kDHCPLeaseDataData		    = 'p';#define kDataChange	1const SInt16 kDataDHCPServerOn			= 2;const SInt16 kDataDHCPVerboseLogging	= 3;const SInt16 kDataDHCPCount			    = 4;	// count of elements in arrayconst SInt16 kDataDHCPArrayOffset		= 5;	// use offset+1..n#endif