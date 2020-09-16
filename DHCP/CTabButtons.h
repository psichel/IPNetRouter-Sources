// =================================================================================//	CTabButtons.h	 		�1996 Sustainable Softworks. All rights reserved.// =================================================================================//	Display multiple index tab buttons#pragma once#include <LTextButton.h>#include "IPRouterCommon.h"#define kMaxTabs kMaxColstypedef	UInt32	TabIndexT;class LView;class CTabButtons : public LTextButton {public:	enum { class_ID = 'CTab' };	static CTabButtons *	CreateTabButtonsStream( LStream *inStream );								CTabButtons( LStream *inStream );	virtual		 				~CTabButtons();	void			InitTabAreas();	virtual void	SetTabWidth(SInt16 inWidth);	SInt16			GetTabWidth( TabIndexT inTab );	void			SetTabView(LView* inView, TabIndexT inTab);	void			DrawSelf();	void			DrawTab(TabIndexT inTab, ConstStr255Param inText);	virtual void	SetValue(SInt32 inValue);protected:	virtual SInt16		FindHotSpot(								Point				inPoint) const;	virtual Boolean		PointInHotSpot(								Point				inPoint,								SInt16				inHotSpot) const;	virtual void		HotSpotAction(SInt16 inHotSpot, Boolean inCurrInside, Boolean inPrevInside);	virtual void		HotSpotResult(SInt16 inHotSpot);	void				RefreshTab(TabIndexT inTab);	private:	TabIndexT 		mTabs;	SInt32			mTabWidth;	LView* 			mTabView[kMaxTabs+1];	// the view pane associated with each tab											// number from 1..n		Rect			mTabAreaFrame[kMaxTabs+1];	RgnHandle		mTabAreaRgn[kMaxTabs+1];};const SInt16 kFadedOutlineColor			= 8;const SInt16 kFadedTabColor				= 1;const SInt16 kHiliteColor				= 5;