# $FreeBSD: head/usr.sbin/ntp/libparse/Makefile 280849 2015-03-30 13:30:15Z cy $

.PATH: ${.CURDIR}/../../../contrib/ntp/libparse

LIB=	parse
INTERNALLIB=

SRCS=	binio.c	clk_computime.c	clk_dcf7000.c	clk_hopf6021.c \
	clk_meinberg.c	clk_rawdcf.c	clk_rcc8000.c	clk_schmid.c \
	clk_sel240x.c	clk_trimtaip.c	clk_trimtsip.c	clk_varitext.c \
	clk_wharton.c	data_mbg.c	gpstolfp.c	ieee754io.c \
	info_trimble.c	mfp_mul.c	parse.c	parse_conf.c \
	trim_info.c

CFLAGS+= -I${.CURDIR}/../../../contrib/ntp/include \
	-I${.CURDIR}/../../../contrib/ntp/lib/isc/unix/include \
	-I${.CURDIR}/../../../contrib/ntp/lib/isc/include -I${.CURDIR}/../

.include <bsd.lib.mk>
