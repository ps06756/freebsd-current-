#	@(#)Makefile.inc	8.1 (Berkeley) 6/4/93
# $FreeBSD: head/lib/libc/arm/gen/Makefile.inc 277078 2015-01-12 18:13:38Z emaste $

SRCS+=	_ctx_start.S _setjmp.S _set_tp.c alloca.S fabs.c \
	infinity.c ldexp.c makecontext.c \
	__aeabi_read_tp.S setjmp.S signalcontext.c sigsetjmp.S flt_rounds.c \
	arm_initfini.c \
	trivial-getcontextx.c

.if ${MACHINE_ARCH} == "armv6hf"
SRCS+=	fpgetmask_vfp.c fpgetround_vfp.c fpgetsticky_vfp.c fpsetmask_vfp.c \
	fpsetround_vfp.c fpsetsticky_vfp.c
.endif
