#       $NetBSD: Makefile,v 1.1 1999/11/23 05:28:20 mrg Exp $
# $FreeBSD: head/sbin/rcorder/Makefile 277674 2015-01-25 03:08:21Z ngie $

PROG=   rcorder
SRCS=   ealloc.c hash.c rcorder.c
MAN=	rcorder.8

LIBADD=	util

CFLAGS+= -DORDER

#CFLAGS+= -DDEBUG

.include <bsd.prog.mk>
