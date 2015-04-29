# $FreeBSD: head/lib/libc/arm/sys/Makefile.inc 281714 2015-04-18 21:50:13Z kib $

SRCS+=	trivial-vdso_tc.c

MDASM= Ovfork.S brk.S cerror.S pipe.S ptrace.S sbrk.S shmat.S sigreturn.S syscall.S

# Don't generate default code for these syscalls:
NOASM=	break.o exit.o getlogin.o openbsd_poll.o sstk.o vfork.o yield.o

PSEUDO= _exit.o _getlogin.o