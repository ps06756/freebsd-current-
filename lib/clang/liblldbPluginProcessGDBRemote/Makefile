# $FreeBSD: head/lib/clang/liblldbPluginProcessGDBRemote/Makefile 276479 2014-12-31 20:31:32Z dim $

.include <bsd.own.mk>

LIB=	lldbPluginProcessGDBRemote

SRCDIR=	tools/lldb/source/Plugins/Process/gdb-remote
SRCS=	GDBRemoteCommunication.cpp \
	GDBRemoteCommunicationClient.cpp \
	GDBRemoteCommunicationServer.cpp \
	GDBRemoteRegisterContext.cpp \
	ProcessGDBRemote.cpp \
	ProcessGDBRemoteLog.cpp \
	ThreadGDBRemote.cpp

TGHDRS=	AttrList \
	Attrs \
	DiagnosticCommonKinds \
	DeclNodes \
	StmtNodes \
	CommentCommandList

.include "../lldb.lib.mk"
