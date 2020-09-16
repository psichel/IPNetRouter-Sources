// =================================================================================//	IPSupport.h				�1996 Sustainable Softworks.  All rights reserved.// =================================================================================// IP Support Routines#include <stropts.h>#include "MyTypes.h"const SInt16 kMaxRouterPorts = 8;	// Number of low IP addresses reserved for named router portsconst SInt16 kMaxProtocols = 4;	// Number of named IP protocolsextern const Str31 gRouterPorts[];extern const Str31 gProtocols[];const SInt16 kIPAddressClassA				=  8;const SInt16 kIPAddressClassB				= 16;const SInt16 kIPAddressClassC				= 24;const SInt16 kIPAddressMulticast			=  4;const SInt16 kIPAddressReserved			    =  5;const SInt16 kIPAddressLoopback			    =  6;const SInt16 kIPAddressBroadcast			=  7;// Well known protocol port names are stored as 'STR ' resources// starting from kPortStartconst SInt16 kPortStart					= 10000;// Examples:// kPortStart+7 = "echo"// kPortStart+20 = "ftp_data"// ---------------------------------------------------------------------------//		� IP_StrToInt// ---------------------------------------------------------------------------//	Return the integer value represented by the IP address string//		Return 0 if don't recognize a valid quad.SInt32		IP_StrToInt(ConstStr255Param inStr);StringPtr 	PortToString(SInt32 inValue, Str255 outStr);UInt8		Endpoint_StrToInt(ConstStr255Param inStr, UInt32* outAddr, UInt16* outPortA, UInt16* outPortB);UInt8		IPprefixLen_StrToInt(ConstStr255Param inStr);Boolean		IsDigitString(ConstStr255Param inStr);// ---------------------------------------------------------------------------//		� IP_NumToStr// ---------------------------------------------------------------------------//	Return the dotted quad string representing an integer IP addressStringPtr	IP_NumToStr(SInt32	inValue, Str255	outStr);StringPtr   IP_NetToStr(NetNumber_t net, Str255 outStr);StringPtr	IP_NumToHexStr(SInt32	inValue, Str255	outStr);StringPtr	Port_NumToHexStr(SInt32	inValue, Str255	outStr);StringPtr	EncodeHexStr(Str255 inStr, Str255	outStr);StringPtr	DecodeHexStr(Str255 inStr, Str255	outStr);Boolean		IsHexChar(UInt16 inChar);StringPtr	HW_NumToHexStr(MACAddr_t* inValue, Str255	outStr);Boolean		HW_StrToInt(ConstStr255Param inStr, MACAddr_t* outValue);SInt32		HEX_StrToInt(ConstStr255Param inStr);// Forward DeclarationsSInt32		GetIPAddressClass( SInt32 theIPAddress, Str255 classString );UInt32		CalcPseudoGateway(UInt32 inAddress);UInt8		FindRightBit(UInt32 inData, UInt8 inStart=32);UInt8		FindLeftBit(UInt32 inData, UInt8 inStart=1);void		SetupIoctl( struct strioctl *stri, long cmd, char* ptr, long len );void 		bzero(UInt8* start, UInt32 size);void 		SmallStrCopy(UInt8 *inDest, const char *inSource);UInt32		HashName(ConstStr255Param inName);UInt16		IpSum( UInt16* dataptr, UInt16* endptr);UInt16		AdjustIpSum( UInt16 oldsum, UInt16 olddata, UInt16 newdata);Boolean		IsDigit(UInt8 inDigit);// ---------------------------------------------------------------------------//		� DecodePortRange// ---------------------------------------------------------------------------//	Convert RTSP client_port="aaaa-bbbb" command to a protocol port range//	Return length of port range data, or Zero if command error detectedUInt8DecodePortRange(char* inStr, UInt16* outPortA, UInt16* outPortB);// ---------------------------------------------------------------------------//		� EncodePortRange// ---------------------------------------------------------------------------//	Convert protocol port range to RTSP client_port="aaaa-bbbb" command//	Output as null terminated string.UInt8EncodePortRange(UInt16 inPortA, UInt16 inPortB, char* outStr);// ---------------------------------------------------------------------------//		� itoc// ---------------------------------------------------------------------------//	Convert integer to character string and return length of string.//	May be called at interrupt time (unlike NumToString).//	Leave unterminated so we can append digits in place.////	A 32-bit integer can produce 10 decimal digits plus a sign and null byte//	so outStr should be at least 12 characters long.UInt8itoc(SInt32 inNum, char* outStr);// ---------------------------------------------------------------------------//		� ctoi// ---------------------------------------------------------------------------//	Convert sequence of decimal digits to an integer//	Stop at first non-digit character and return number of characters converted.//	A 32-bit int can hold a 10 digit decimal, so convert up to 10 characters max.UInt8ctoi(char* inStr, SInt32* outNum);// ---------------------------------------------------------------------------//		� IP packet structures// ---------------------------------------------------------------------------struct ipheader	{	UInt8	hlen;	UInt8	tos;	UInt16	total_length;	UInt16	identification;	UInt16	fragment_offset;	UInt8	ttl;	UInt8	protocol;	UInt16	header_checksum;	UInt32	source_addr;	UInt32	dest_addr;	};typedef struct ipheader ipheader;struct icmpheader	{	UInt8	type;	UInt8	code;	UInt16	checksum;	UInt16	identifier;			// icmp echo	UInt16	sequence_number;	// icmp echo	UInt8	data[28];			// optional data		// IP header + first 64 bits of datagram	};typedef struct icmpheader icmpheader;const SInt16 kIcmpEchoReply 					= 0;const SInt16 kIcmpTypeDestUnreachable			= 3;const SInt16 kIcmpTypeSourceQuench				= 4;const SInt16 kIcmpTypeRedirect					= 5;const SInt16 kIcmpTypeEchoRequest				= 8;const SInt16 kIcmpTypeTimeExceeded 			    = 11;const SInt16 rSTRN_IcmpCode					    = 133;const SInt16 kIcmpCodeNetworkUnreachable		= 0;const SInt16 kIcmpCodeHostUnreachable			= 1;const SInt16 kIcmpCodeProtocolUnreachable		= 2;const SInt16 kIcmpCodePortUnreachable			= 3;const SInt16 kIcmpCodeDestNetworkUnknown		= 6;const SInt16 kIcmpCodeDestHostUnknown			= 7;const SInt16 kIcmpCodeDestNetworkAdmin			= 9;const SInt16 kIcmpCodeDestHostAdmin			    = 10;const SInt16 kIcmpCodeCommAdminFilter			= 13;const SInt16 kIcmpCodeNone						= 64;	// not a valid type code// UDP headerstruct udp_pseudo_t	{	UInt32	sourceAddress;	UInt32	destAddress;	UInt8	zero;	UInt8	proto;	UInt16	length;	};typedef struct udp_pseudo_t udp_pseudo_t;struct udp_header_t	{	UInt16	sourcePort;	UInt16	destPort;	UInt16	messageLength;	UInt16	checksum;	UInt8	data[36];			// optional data	};typedef struct udp_header_t udp_header_t;