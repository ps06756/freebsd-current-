# $FreeBSD: head/usr.sbin/ntp/libntpevent/Makefile 280849 2015-03-30 13:30:15Z cy $

.PATH:	${.CURDIR}/../../../contrib/ntp/sntp/libevent

LIB= ntpevent
INTERNALLIB=

SRCS=	buffer.c bufferevent.c bufferevent_filter.c bufferevent_openssl.c \
	bufferevent_pair.c epoll.c evdns.c event.c event_tagging.c \
	evmap.c evport.c evrpc.c evthread.c evthread_pthread.c evutil.c \
	evutil_rand.c evutil_time.c http.c kqueue.c listener.c log.c poll.c \
	select.c signal.c strlcpy.c

.if ${MACHINE_ARCH} == "i386"
NTP_ATOMIC=x86_32
.elif ${MACHINE_ARCH} == "amd64"
NTP_ATOMIC=x86_64
.elif ${MACHINE_ARCH} == "ia64"
NTP_ATOMIC=ia64
.elif ${MACHINE_ARCH} == "powerpc64"
NTP_ATOMIC=powerpc
.elif ${MACHINE_ARCH} == "sparc64"
NTP_ATOMIC=sparc64
.else
NTP_ATOMIC=noatomic
.endif

CFLAGS+= -I${.CURDIR}/../../../contrib/ntp/include \
	-I${.CURDIR}/../../../contrib/ntp/sntp/libevent/include \
	-I${.CURDIR}/

CFLAGS+= -DHAVE_BSD_NICE -DHAVE_STDINT_H

.include <bsd.lib.mk>
