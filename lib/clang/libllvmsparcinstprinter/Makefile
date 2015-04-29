# $FreeBSD: head/lib/clang/libllvmsparcinstprinter/Makefile 276479 2014-12-31 20:31:32Z dim $

.include <bsd.own.mk>

LIB=	llvmsparcinstprinter

SRCDIR=	lib/Target/Sparc/InstPrinter
INCDIR=	lib/Target/Sparc
SRCS=	SparcInstPrinter.cpp

TGHDRS=	SparcGenAsmWriter \
	SparcGenInstrInfo \
	SparcGenRegisterInfo \
	SparcGenSubtargetInfo

.include "../clang.lib.mk"
