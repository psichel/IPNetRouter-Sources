// ===========================================================================//	CRoutesData.cp			�1998 Sustainable Softworks, All rights reserved.// ===========================================================================// Routes data (corresponds to routes window)#include "CRoutesData.h"#include "CRoutesConst.h"#include "CResidentIfInfo.h"#include "CConfigDocument.h"#include "IPSupport.h"// Globalsextern CResidentIfInfo* 	gResInfo;// ---------------------------------------------------------------------------------//		� CRoutesData	[Constructor]// ---------------------------------------------------------------------------------CRoutesData::CRoutesData(){	mRoutesArray = nil;	Init();	bzero((UInt8*)&mRouteEntry, sizeof(RouteEntry_t));	mRouteEntry.routeType = IRE_GATEWAY;	mRoutesData = nil;}// ---------------------------------------------------------------------------------//		� ~CRoutesData	[Destructor]// ---------------------------------------------------------------------------------CRoutesData::~CRoutesData(){	if (mRoutesArray) delete mRoutesArray;	mRoutesArray = nil;	if (mRoutesData) delete mRoutesData;	mRoutesData = nil;}// ---------------------------------------------------------------------------------//		� Init// ---------------------------------------------------------------------------------//	Initialize routes data storagevoidCRoutesData::Init(){	UInt32	count;		// Routes array	if (mRoutesArray == nil) {		// allocate array object if needed		mRoutesArray	= new LArray(sizeof(RouteEntry_t));		ThrowIfNil_(mRoutesArray);	} else {		// remove any previous elements		count = mRoutesArray->GetCount();		if (count > 0) {			// (inCount, atIndex)			mRoutesArray->RemoveItemsAt(count, 1);		}	}}// ---------------------------------------------------------------------------------//		� MatchRouteInConfig// ---------------------------------------------------------------------------------SInt32CRoutesData::MatchRouteInConfig(		RouteEntry_t*	inRouteEntry,		CRoutesData*	inRouteConfig,		RouteEntry_t*	outRouteEntry){	ArrayIndexT		numConfigE, row;	SInt32			result = kEntryNoMatch;	// get number of Routes in config	numConfigE = inRouteConfig->GetDataCount();	// for each Route in config	for (row=numConfigE; row>=1; row--) {		// get entry from config		if ( !inRouteConfig->GetDataRoutesArray(row, outRouteEntry) ) continue;		// do they match?		result = MatchRouteEntry(inRouteEntry, outRouteEntry);		if (result != kEntryNoMatch) break;	}	return result;}// ---------------------------------------------------------------------------------//		� MatchRouteEntry// ---------------------------------------------------------------------------------//	Compare Route Entries//	Return values://		kEntryMatch//		kEntryAdd		(1 to match 2)//		kEntryDelete	(1 to match 2)//		kEntryNoMatchSInt32CRoutesData::MatchRouteEntry(RouteEntry_t* inRouteEntry1, RouteEntry_t* inRouteEntry2){	SInt32	result = kEntryNoMatch;		do {		// same route type?		if (inRouteEntry1->routeType != inRouteEntry2->routeType) break;		// same destination?		if (inRouteEntry1->destination != inRouteEntry2->destination) break;		// same mask if specified?		if ((inRouteEntry1->netMask != 0) && (inRouteEntry2->netMask != 0)) {			if (inRouteEntry1->netMask != inRouteEntry2->netMask) break;		}		// same gateway?		// treat zero as matching any existing gateway		if ((inRouteEntry1->gateway != 0) && (inRouteEntry2->gateway != 0)) {			if (inRouteEntry1->gateway != inRouteEntry2->gateway) break;		}		// test add or delete route		if (inRouteEntry1->addRoute == inRouteEntry2->addRoute) {			result = kEntryMatch;		// the same			break;		}		// set to match entry 2		if (inRouteEntry2->addRoute) result = kEntryAdd;		else result = kEntryDelete;	} while (false);		return result;}// ---------------------------------------------------------------------------------//		� RouteEntryToString// ---------------------------------------------------------------------------------// Convert an Routes Entry to a text string for exportStringPtrCRoutesData::RouteEntryToString(RouteEntry_t* inEntry, Str255 outStr){	LStr255	textStr;	Str31	str;	// addRoute?	if (inEntry->addRoute) textStr = "\p+route";	else textStr = "\p-route";	textStr += kDelimiterStr;			// routeType	switch (inEntry->routeType) {		case IRE_RESOLVER:			textStr += kStrDirect;			break;		case IRE_SUBNET:			textStr += kStrPointToPoint;			break;		case IRE_GATEWAY:			textStr += kStrDefaultGateway;			break;		case IRE_NET:			textStr += kStrNetwork;			break;		case IRE_ROUTE_ASSOC:			textStr += kStrHost;			break;		default:			NumToString(inEntry->routeType, str);							textStr += str;	}	textStr += kDelimiterStr;	// destination	NetNumber_t net;	net.address = inEntry->destination;	net.mask = inEntry->netMask;	IP_NetToStr(net, str);	textStr += str;	textStr += kDelimiterStr;	// gateway	// if it's a pseudo-gateway, use zero to match any default gateway	if (inEntry->gateway == gResInfo->mPseudoGateway) {		IP_NumToStr(0, str);	} else {		IP_NumToStr(inEntry->gateway, str);	}	textStr += str;	textStr += kDelimiterStr;	return LString::CopyPStr(textStr, outStr, sizeof(Str255));}#pragma mark --- Accessors ---// ---------------------------------------------------------------------------------//		� SignalDataChange// ---------------------------------------------------------------------------------// Used to tell our document when data has changedvoidCRoutesData::SignalDataChange(){	SignalDataEvent( DATA_ID(kRoutesData, kDataChange) );}// ---------------------------------------------------------------------------------//		� SetDataRouteEntry// ---------------------------------------------------------------------------------// Set of Get Config Entry (corresponds to parameters in config area of window)voidCRoutesData::SetDataRouteEntry(RouteEntry_t* inEntry){	mRouteEntry = *inEntry;	SignalDataEvent( DATA_ID(kRoutesData, kDataRouteEntry) );}voidCRoutesData::GetDataRouteEntry(RouteEntry_t* outEntry){	*outEntry = mRouteEntry;}// ---------------------------------------------------------------------------------//		� SetDataRoutesArray// ---------------------------------------------------------------------------------// Write element to Routes array, insert new element if beyond end of array// and return index of where element was written.ArrayIndexTCRoutesData::SetDataRoutesArray(ArrayIndexT index, RouteEntry_t* inEntry){	ArrayIndexT	count;	ArrayIndexT	result;		count = mRoutesArray->GetCount();	if (index <= count) {		// assign element in Array		//	(inCount, inAtIndex, *inItem, inItemSize)		result = mRoutesArray->AssignItemsAt(1, index, inEntry, sizeof(RouteEntry_t));	} else {		// insert element in Array		//	(inCount, inAtIndex, *inItem, inItemSize)		result = mRoutesArray->InsertItemsAt(1, index, inEntry, sizeof(RouteEntry_t));	}	SignalDataEvent( DATA_ID(kRoutesData, kDataRoutesArrayOffset + result) );		return result;}// ---------------------------------------------------------------------------------//		� GetDataRoutesArray// ---------------------------------------------------------------------------------// Get element from Routes array at index.// Return false if index is out of range.BooleanCRoutesData::GetDataRoutesArray(ArrayIndexT index, RouteEntry_t* outEntry){	return mRoutesArray->FetchItemAt(index, outEntry);}// ---------------------------------------------------------------------------------//		� GetDataCount// ---------------------------------------------------------------------------------//	return number of elements in Routes arrayArrayIndexTCRoutesData::GetDataCount(){	return mRoutesArray->GetCount();}// ---------------------------------------------------------------------------------//		� SetDataCount// ---------------------------------------------------------------------------------// Set number of elements in Routes array.// Remove any elements after specified count and return count of last element// which could be less than the number requested.ArrayIndexTCRoutesData::SetDataCount(ArrayIndexT count){	ArrayIndexT	num;	ArrayIndexT result;	ArrayIndexT index;		// remove any elements beyond count	result = count;	num = mRoutesArray->GetCount();	if (num < count) {		result = num;	}	else if (num > count) {		// remove extras		// (inCount, atIndex)		mRoutesArray->RemoveItemsAt(num-count, count+1);		// signal corresponding elements have changed		for (index=count+1; index<=num; index++) {			SignalDataEvent( DATA_ID(kRoutesData, kDataRoutesArrayOffset + index) );		}	}		return result;}