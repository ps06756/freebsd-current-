# $FreeBSD: head/usr.sbin/ntp/libntp/Makefile 280849 2015-03-30 13:30:15Z cy $

.PATH: ${.CURDIR}/../../../contrib/ntp/libntp \
	${.CURDIR}/../../../contrib/ntp/lib/isc \
	${.CURDIR}/../../../contrib/ntp/lib/isc/nls \
	${.CURDIR}/../../../contrib/ntp/lib/isc/pthreads \
	${.CURDIR}/../../../contrib/ntp/lib/isc/unix \

LIB= ntp
INTERNALLIB=

NTP_SRCS= systime.c	a_md5encrypt.c	adjtime.c	atoint.c \
	atolfp.c	atouint.c	audio.c	authkeys.c \
	authreadkeys.c	authusekey.c	bsd_strerror.c	buftvtots.c \
	caljulian.c	caltontp.c	calyearstart.c	clocktime.c \
	clocktypes.c	decodenetnum.c	dofptoa.c	dolfptoa.c \
	emalloc.c	findconfig.c	getopt.c	hextoint.c \
	hextolfp.c	humandate.c	icom.c		iosignal.c \
	lib_strbuf.c	machines.c	mktime.c	modetoa.c \
	mstolfp.c	msyslog.c	netof.c		ntp_calendar.c \
	ntp_crypto_rnd.c		ntp_intres.c	ntp_libopts.c \
	ntp_lineedit.c	ntp_random.c	ntp_rfc2553.c	ntp_worker.c \
	numtoa.c	numtohost.c	octtoint.c	prettydate.c \
	recvbuff.c	refnumtoa.c	snprintf.c	socket.c \
	socktoa.c	socktohost.c	ssl_init.c	statestr.c \
	strdup.c	strl_obsd.c	syssignal.c	timetoa.c \
	timevalops.c	uglydate.c	vint64ops.c	work_fork.c \
	work_thread.c	ymd2yd.c

ISC_PTHREADS_SRCS= condition.c \
	thread.c \
	mutex.c

ISC_UNIX_SRCS= dir.c \
	errno2result.c \
	file.c \
	interfaceiter.c \
	net.c \
	stdio.c \
	stdtime.c \
	strerror.c \
	time.c

ISC_NLS_SRCS= msgcat.c

ISC_SRCS= assertions.c \
	buffer.c \
	backtrace-emptytbl.c \
	backtrace.c \
	error.c \
	event.c \
	inet_ntop.c \
	inet_pton.c \
	lib.c \
	log.c \
	md5.c \
	netaddr.c \
	netscope.c \
	ondestroy.c \
	random.c \
	result.c \
	task.c \
	sha1.c \
	sockaddr.c \
	${ISC_NLS_SRCS} \
	${ISC_PTHREADS_SRCS} \
	${ISC_UNIX_SRCS}

SRCS=	${NTP_SRCS} ${ISC_SRCS} version.c

CFLAGS+= -I${.CURDIR}/../../../contrib/ntp/include \
	-I${.CURDIR}/../../../contrib/ntp/lib/isc/include \
	-I${.CURDIR}/../../../contrib/ntp/lib/isc/unix/include \
	-I${.CURDIR}/../../../contrib/ntp/lib/isc/pthreads/include \
	-I${.CURDIR}/../../../contrib/ntp/sntp/libopts \
	-I${.CURDIR}/../../../lib/libc/${MACHINE_ARCH} \
	-I${.CURDIR}/../../../lib/libedit/edit \
	-I${.CURDIR}/../ \
	-I${.CURDIR}/

CFLAGS+= -DHAVE_BSD_NICE -DHAVE_STDINT_H

CLEANFILES+= .version version.c

version.c:
	sh -e ${.CURDIR}/../scripts/mkver ntpd

.include <bsd.lib.mk>
