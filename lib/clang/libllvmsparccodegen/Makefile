# $FreeBSD: head/lib/clang/libllvmsparccodegen/Makefile 280031 2015-03-15 13:31:13Z dim $

.include <bsd.own.mk>

LIB=	llvmsparccodegen

SRCDIR=	lib/Target/Sparc
SRCS=	DelaySlotFiller.cpp \
	SparcAsmPrinter.cpp \
	SparcFrameLowering.cpp \
	SparcISelDAGToDAG.cpp \
	SparcISelLowering.cpp \
	SparcInstrInfo.cpp \
	SparcMCInstLower.cpp \
	SparcMachineFunctionInfo.cpp \
	SparcRegisterInfo.cpp \
	SparcSelectionDAGInfo.cpp \
	SparcSubtarget.cpp \
	SparcTargetMachine.cpp \
	SparcTargetObjectFile.cpp

TGHDRS=	Intrinsics \
	SparcGenAsmWriter \
	SparcGenCallingConv \
	SparcGenCodeEmitter \
	SparcGenDAGISel \
	SparcGenInstrInfo \
	SparcGenMCCodeEmitter \
	SparcGenRegisterInfo \
	SparcGenSubtargetInfo

.include "../clang.lib.mk"
