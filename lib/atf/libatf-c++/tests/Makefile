# $FreeBSD: head/lib/atf/libatf-c++/tests/Makefile 273929 2014-11-01 11:17:54Z jmmv $

.include <bsd.init.mk>

TESTSDIR=	${TESTSBASE}/lib/atf/libatf-c++
TESTS_SUBDIRS=	detail

ATF=		${.CURDIR:H:H:H:H}/contrib/atf
.PATH:		${ATF}/atf-c++
.PATH:		${ATF}/atf-c++/detail

CFLAGS+=	-DATF_C_TESTS_BASE='"${TESTSBASE}/lib/atf/libatf-c"'
CFLAGS+=	-DATF_INCLUDEDIR='"${INCLUDEDIR}"'
CFLAGS+=	-I${ATF}

FILESDIR=	${TESTSDIR}
FILES=		macros_hpp_test.cpp
FILES+=		unused_test.cpp

.for _T in	atf_c++_test \
		build_test \
		check_test \
		macros_test \
		tests_test \
		utils_test
ATF_TESTS_CXX+=	${_T}
SRCS.${_T}=	${_T}.cpp test_helpers.cpp
.endfor

.include <bsd.test.mk>
