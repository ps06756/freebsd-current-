# $FreeBSD: head/usr.bin/mkesdb_static/Makefile 264400 2014-04-13 05:21:56Z imp $

.PATH: ${.CURDIR}/../../lib/libc/iconv ${.CURDIR}/../mkesdb

PROG=	mkesdb_static
SRCS=	citrus_bcs.c citrus_db_factory.c citrus_db_hash.c \
	citrus_lookup_factory.c
MAN=
NO_SHARED= yes

build-tools: mkesdb_static

.include "${.CURDIR}/../mkesdb/Makefile.inc"
.include <bsd.prog.mk>
