# $FreeBSD: head/tests/sys/kqueue/Makefile 281427 2015-04-11 09:15:33Z ngie $
#
# svn://mark.heily.com/libkqueue/trunk/test
# Last update: r114
#
# libkqueue and test suite by Mark Heily <mark@heily.com>
#

TAP_TESTS_SH=	kqueue_test

TESTSDIR=	${TESTSBASE}/sys/kqueue
BINDIR=		${TESTSDIR}

PROGS=	kqtest

SRCS.kqtest=		\
	main.c		\
	read.c		\
	timer.c		\
	vnode.c		\
	proc.c		\
	signal.c	\
	user.c
WARNS?=	2

.include <bsd.test.mk>
