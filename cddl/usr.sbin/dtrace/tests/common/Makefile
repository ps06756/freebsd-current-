# $FreeBSD: head/cddl/usr.sbin/dtrace/tests/common/Makefile 279418 2015-02-28 23:30:06Z markj $

# We exclude several subdirectories: nfs and sysevent do not compile on
# FreeBSD, and docsExamples doesn't seem amenable to automated testing.

TESTSDIR=	${TESTSBASE}/cddl/usr.sbin/dtrace/common
TESTS_SUBDIRS+= aggs \
		arithmetic \
		arrays \
		assocs \
		begin \
		bitfields \
		buffering \
		builtinvar \
		cg \
		clauses \
		cpc \
		decls \
		drops \
		dtraceUtil \
		end \
		enum \
		error \
		exit \
		fbtprovider \
		funcs \
		grammar \
		include \
		inline \
		io \
		ip \
		java_api \
		json \
		lexer \
		llquantize \
		mdb \
		mib \
		misc \
		multiaggs \
		offsetof \
		operators \
		pid \
		plockstat \
		pointers \
		pragma \
		predicates \
		preprocessor \
		print \
		printa \
		printf \
		privs \
		probes \
		proc \
		profile-n \
		providers \
		raise \
		rates \
		safety \
		scalars \
		sched \
		scripting \
		sdt \
		sizeof \
		speculation \
		stability \
		stack \
		stackdepth \
		stop \
		strlen \
		strtoll \
		struct \
		syscall \
		tick-n \
		trace \
		tracemem \
		translators \
		typedef \
		types \
		uctf \
		union \
		usdt \
		ustack \
		vars \
		version \

.PATH:		${.CURDIR:H:H:H:H:H}/tests
KYUAFILE=	YES

SUBDIR_PARALLEL=

.include <bsd.test.mk>
