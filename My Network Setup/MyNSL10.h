// =================================================================================//	MyNSL10.h	 			�1999 Sustainable Softworks. All rights reserved.// =================================================================================// We use this file to access the NSL 1.0 APIs via <NSLAPI.h> which// is not compatible with the NSL 1.1 API defined in <NSL.h>.// function declarationsOSStatus MyRegisterWithNSL10(char* inPath, void** outNSLDataPtr);OSStatus MyDeregisterWithNSL10(void** ioNSLDataPtr);