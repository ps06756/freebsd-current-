# $FreeBSD: head/lib/clang/libllvmsparcdisassembler/Makefile 276479 2014-12-31 20:31:32Z dim $

.include <bsd.own.mk>

LIB=	llvmsparcdisassembler

SRCDIR=	lib/Target/Sparc/Disassembler
INCDIR=	lib/Target/Sparc
SRCS=	SparcDisassembler.cpp

TGHDRS=	SparcGenDisassemblerTables \
	SparcGenInstrInfo \
	SparcGenRegisterInfo \
	SparcGenSubtargetInfo

.include "../clang.lib.mk"
