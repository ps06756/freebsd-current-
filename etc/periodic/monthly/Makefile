# $FreeBSD: head/etc/periodic/monthly/Makefile 265420 2014-05-06 04:22:01Z imp $

.include <src.opts.mk>

FILES=	450.status-security \
	999.local

# NB: keep these sorted by MK_* knobs

.if ${MK_UTMPX} != "no"
FILES+=	200.accounting
.endif

.include <bsd.prog.mk>
