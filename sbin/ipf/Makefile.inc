#	$FreeBSD: head/sbin/ipf/Makefile.inc 275030 2014-11-25 11:23:12Z bapt $

.include <src.opts.mk>

WARNS?=		2
NO_WFORMAT=
NO_WARRAY_BOUNDS=

CFLAGS+=	-I${.CURDIR}/../../../contrib/ipfilter
CFLAGS+=	-I${.CURDIR}/../../../contrib/ipfilter/tools
CFLAGS+=	-I${.CURDIR}/../../../sys
CFLAGS+=	-I${.CURDIR}/../../../sys/contrib/ipfilter
CFLAGS+=	-DSTATETOP -D__UIO_EXPOSE

.if ${MK_INET6_SUPPORT} != "no"
CFLAGS+=	-DUSE_INET6
.else
CFLAGS+=	-DNOINET6
.endif

LIBADD+=	ipf

CLEANFILES+=	y.tab.c y.tab.h

.PATH:	${.CURDIR}/../../../contrib/ipfilter	\
	${.CURDIR}/../../../contrib/ipfilter/lib	\
	${.CURDIR}/../../../contrib/ipfilter/tools	\
	${.CURDIR}/../../../contrib/ipfilter/man

.include "../Makefile.inc"