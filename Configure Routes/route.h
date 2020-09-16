/*	File:		route.h	Contains:	Change History (most recent first):	To Do:*//** ** route.h 35.2, last change 02/20/99 **/#ifndef _ROUTE_#define _ROUTE_#ifdef MI_H_ID_STRINGSstatic	char	route_h_sccsid[] = "@(#)route.h	35.2";#endif/* * Copyright (c) 1980, 1986 Regents of the University of California. * All rights reserved. * * Redistribution and use in source and binary forms are permitted * provided that the above copyright notice and this paragraph are * duplicated in all such forms and that any documentation, * advertising materials, and other materials related to such * distribution and use acknowledge that the software was developed * by the University of California, Berkeley.  The name of the * University may not be used to endorse or promote products derived * from this software without specific prior written permission. * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. * *	@(#)route.h	7.4 (Berkeley) 6/27/88 *//* * Kernel resident routing tables. *  * The routing tables are initialized when interface addresses * are set by making entries for all directly connected interfaces. *//* * A route consists of a destination address and a reference * to a routing entry.  These are often held by protocols * in their control blocks, e.g. inpcb. */struct route {	struct	rtentry *ro_rt;	struct	sockaddr ro_dst;};/* * We distinguish between routes to hosts and routes to networks, * preferring the former if available.  For each route we infer * the interface to use from the gateway address supplied when * the route was entered.  Routes that forward packets through * gateways are marked so that the output routines know to address the * gateway rather than the ultimate destination. */struct rtentry {	ulong	rt_hash;		/* to speed lookups */	struct	sockaddr rt_dst;	/* key */	struct	sockaddr rt_gateway;	/* value */	short	rt_flags;		/* up/down?, host/net */	short	rt_refcnt;		/* # held references */	ulong	rt_use;			/* raw # packets forwarded */	struct	ifnet *rt_ifp;		/* the answer: interface to use */	ushort	rt_pmtu;		/* path mtu value; max 65536 */	ulong	rt_subnetmask;		/* subnet mask */};#define	RTF_UP		0x1		/* route useable */#define	RTF_GATEWAY	0x2		/* destination is a gateway */#define	RTF_HOST	0x4		/* host entry (net otherwise) */#define	RTF_DYNAMIC	0x10		/* created dynamically (by redirect) */#define	RTF_MODIFIED	0x20		/* modified dynamically (by redirect) *//* * Used by SIOCGRTENTRY ioctl. */struct rtreq {	uint	rtr_destaddr;		/* destination IP address */	uint	rtr_gwayaddr;		/* gateway IP address (route) */	ushort	rtr_flags;		/* RTF_* flags */	ushort	rtr_refcnt;		/* # held references */	uint	rtr_use;		/* raw # packets forwarded */	int	rtr_proto;		/* netwk mgmt protocol */	uint	rtr_upd;		/* update time */	int	rtr_pmtu_timer;		/* time-out: minutes remaining */	ushort	rtr_pmtu;		/* PMTU size */	ushort	rtr_ifmtu;		/* interface MTU size */	char	rtr_ifname[IFNAMSIZ];	/* interface name (e.g., le0) */	uint	rtr_subnetmask;		/* subnet mask */};/* * Routing statistics. */struct	rtstat {	short	rts_badredirect;	/* bogus redirect calls */	short	rts_dynamic;		/* routes created by redirects */	short	rts_newgateway;		/* routes modified by redirects */	short	rts_unreach;		/* lookups which failed */	short	rts_wildcard;		/* lookups satisfied by a wildcard */};#ifdef KERNEL#define	RTFREE(rt) \	if ((rt)->rt_refcnt == 1) \		rtfree(rt); \	else \		(rt)->rt_refcnt--;#ifdef	GATEWAY#define	RTHASHSIZ	64#else#define	RTHASHSIZ	8#endif#if	(RTHASHSIZ & (RTHASHSIZ - 1)) == 0#define RTHASHMOD(h)	((h) & (RTHASHSIZ - 1))#else#define RTHASHMOD(h)	((h) % RTHASHSIZ)#endif#endif#endif	/* _ROUTE_	*/