# $FreeBSD: head/tests/sys/kern/Makefile 282104 2015-04-27 17:53:09Z ngie $

TESTSDIR=	${TESTSBASE}/sys/kern

ATF_TESTS_C+=	kern_descrip_test
ATF_TESTS_C+=	unix_seqpacket_test
TEST_METADATA.unix_seqpacket_test+=	timeout="15"

LDADD.unix_seqpacket_test+=		-lpthread

WARNS?=	5

TESTS_SUBDIRS+=	execve

.include <bsd.test.mk>
