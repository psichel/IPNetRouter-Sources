// ===========================================================================//	URL_Caption.h			�199 Sustainable Softworks. All rights reserved.// ===========================================================================////	LCaption that will invoke contents as a URL when clicked#ifndef _H_URL_Caption#define _H_URL_Caption#pragma once#if defined(__CFM68K__) && !defined(__USING_STATIC_LIBS__)	#pragma import on#endif#include <LCaption.h>class	URL_Caption : public LCaption {public:	enum { class_ID = 'URLc' };	static URL_Caption*	CreateURL_CaptionStream(								LStream			*inStream);						URL_Caption(								LStream			*inStream);	virtual				~URL_Caption();	protected:	virtual void		ClickSelf(const SMouseDownEvent &inMouseDown);};#if defined(__CFM68K__) && !defined(__USING_STATIC_LIBS__)	#pragma import reset#endif#endif