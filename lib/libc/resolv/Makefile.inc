# $FreeBSD: head/lib/libc/resolv/Makefile.inc 269867 2014-08-12 12:36:06Z ume $

# resolv sources
.PATH: ${LIBC_SRCTOP}/resolv

SRCS+=	herror.c h_errno.c mtctxres.c res_comp.c res_data.c res_debug.c \
	res_findzonecut.c res_init.c res_mkquery.c res_mkupdate.c \
	res_query.c res_send.c res_state.c res_update.c

SYM_MAPS+= ${LIBC_SRCTOP}/resolv/Symbol.map

CFLAGS+=-I${LIBC_SRCTOP}/../libmd
