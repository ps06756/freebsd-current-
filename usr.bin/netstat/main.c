/*-
 * Copyright (c) 1983, 1988, 1993
 *	Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef lint
char const copyright[] =
"@(#) Copyright (c) 1983, 1988, 1993\n\
	Regents of the University of California.  All rights reserved.\n";
#endif /* not lint */

#if 0
#ifndef lint
static char sccsid[] = "@(#)main.c	8.4 (Berkeley) 3/1/94";
#endif /* not lint */
#endif

#include <sys/cdefs.h>
__FBSDID("$FreeBSD: head/usr.bin/netstat/main.c 279122 2015-02-21 23:47:20Z marcel $");

#include <sys/param.h>
#include <sys/file.h>
#include <sys/protosw.h>
#include <sys/socket.h>
#include <sys/socketvar.h>

#include <netinet/in.h>

#ifdef NETGRAPH
#include <netgraph/ng_socket.h>
#endif

#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <kvm.h>
#include <limits.h>
#include <netdb.h>
#include <nlist.h>
#include <paths.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include "netstat.h"
#include <libxo/xo.h>

static struct nlist nl[] = {
#define	N_RTSTAT	0
	{ .n_name = "_rtstat" },
#define	N_RTREE		1
	{ .n_name = "_rt_tables"},
#define	N_MRTSTAT	2
	{ .n_name = "_mrtstat" },
#define	N_MFCHASHTBL	3
	{ .n_name = "_mfchashtbl" },
#define	N_VIFTABLE	4
	{ .n_name = "_viftable" },
#define	N_NGSOCKS	5
	{ .n_name = "_ngsocklist"},
#define	N_IP6STAT	6
	{ .n_name = "_ip6stat" },
#define	N_ICMP6STAT	7
	{ .n_name = "_icmp6stat" },
#define	N_IPSECSTAT	8
	{ .n_name = "_ipsec4stat" },
#define	N_IPSEC6STAT	9
	{ .n_name = "_ipsec6stat" },
#define	N_PIM6STAT	10
	{ .n_name = "_pim6stat" },
#define	N_MRT6STAT	11
	{ .n_name = "_mrt6stat" },
#define	N_MF6CTABLE	12
	{ .n_name = "_mf6ctable" },
#define	N_MIF6TABLE	13
	{ .n_name = "_mif6table" },
#define	N_PFKEYSTAT	14
	{ .n_name = "_pfkeystat" },
#define	N_RTTRASH	15
	{ .n_name = "_rttrash" },
#define	N_CARPSTAT	16
	{ .n_name = "_carpstats" },
#define	N_PFSYNCSTAT	17
	{ .n_name = "_pfsyncstats" },
#define	N_AHSTAT	18
	{ .n_name = "_ahstat" },
#define	N_ESPSTAT	19
	{ .n_name = "_espstat" },
#define	N_IPCOMPSTAT	20
	{ .n_name = "_ipcompstat" },
#define	N_TCPSTAT	21
	{ .n_name = "_tcpstat" },
#define	N_UDPSTAT	22
	{ .n_name = "_udpstat" },
#define	N_IPSTAT	23
	{ .n_name = "_ipstat" },
#define	N_ICMPSTAT	24
	{ .n_name = "_icmpstat" },
#define	N_IGMPSTAT	25
	{ .n_name = "_igmpstat" },
#define	N_PIMSTAT	26
	{ .n_name = "_pimstat" },
#define	N_TCBINFO	27
	{ .n_name = "_tcbinfo" },
#define	N_UDBINFO	28
	{ .n_name = "_udbinfo" },
#define	N_DIVCBINFO	29
	{ .n_name = "_divcbinfo" },
#define	N_RIPCBINFO	30
	{ .n_name = "_ripcbinfo" },
#define	N_UNP_COUNT	31
	{ .n_name = "_unp_count" },
#define	N_UNP_GENCNT	32
	{ .n_name = "_unp_gencnt" },
#define	N_UNP_DHEAD	33
	{ .n_name = "_unp_dhead" },
#define	N_UNP_SHEAD	34
	{ .n_name = "_unp_shead" },
#define	N_RIP6STAT	36
	{ .n_name = "_rip6stat" },
#define	N_SCTPSTAT	36
	{ .n_name = "_sctpstat" },
#define	N_MFCTABLESIZE	37
	{ .n_name = "_mfctablesize" },
#define	N_ARPSTAT	38
	{ .n_name = "_arpstat" },
#define	N_UNP_SPHEAD	39
	{ .n_name = "unp_sphead" },
#define	N_SFSTAT	40
	{ .n_name = "_sfstat"},
	{ .n_name = NULL },
};

struct protox {
	int	pr_index;		/* index into nlist of cb head */
	int	pr_sindex;		/* index into nlist of stat block */
	u_char	pr_wanted;		/* 1 if wanted, 0 otherwise */
	void	(*pr_cblocks)(u_long, const char *, int, int);
					/* control blocks printing routine */
	void	(*pr_stats)(u_long, const char *, int, int);
					/* statistics printing routine */
	void	(*pr_istats)(char *);	/* per/if statistics printing routine */
	const char	*pr_name;		/* well-known name */
	int	pr_usesysctl;		/* non-zero if we use sysctl, not kvm */
	int	pr_protocol;
} protox[] = {
	{ N_TCBINFO,	N_TCPSTAT,	1,	protopr,
	  tcp_stats,	NULL,		"tcp",	1,	IPPROTO_TCP },
	{ N_UDBINFO,	N_UDPSTAT,	1,	protopr,
	  udp_stats,	NULL,		"udp",	1,	IPPROTO_UDP },
#ifdef SCTP
	{ -1,		N_SCTPSTAT,	1,	sctp_protopr,
	  sctp_stats,	NULL,		"sctp",	1,	IPPROTO_SCTP },
#endif
#ifdef SDP
	{ -1,		-1,		1,	protopr,
	 NULL,		NULL,		"sdp",	1,	IPPROTO_TCP },
#endif
	{ N_DIVCBINFO,	-1,		1,	protopr,
	  NULL,		NULL,		"divert", 1,	IPPROTO_DIVERT },
	{ N_RIPCBINFO,	N_IPSTAT,	1,	protopr,
	  ip_stats,	NULL,		"ip",	1,	IPPROTO_RAW },
	{ N_RIPCBINFO,	N_ICMPSTAT,	1,	protopr,
	  icmp_stats,	NULL,		"icmp",	1,	IPPROTO_ICMP },
	{ N_RIPCBINFO,	N_IGMPSTAT,	1,	protopr,
	  igmp_stats,	NULL,		"igmp",	1,	IPPROTO_IGMP },
#ifdef IPSEC
	{ -1,		N_IPSECSTAT,	1,	NULL,	/* keep as compat */
	  ipsec_stats,	NULL,		"ipsec", 0,	0},
	{ -1,		N_AHSTAT,	1,	NULL,
	  ah_stats,	NULL,		"ah",	0,	0},
	{ -1,		N_ESPSTAT,	1,	NULL,
	  esp_stats,	NULL,		"esp",	0,	0},
	{ -1,		N_IPCOMPSTAT,	1,	NULL,
	  ipcomp_stats,	NULL,		"ipcomp", 0,	0},
#endif
	{ N_RIPCBINFO,	N_PIMSTAT,	1,	protopr,
	  pim_stats,	NULL,		"pim",	1,	IPPROTO_PIM },
	{ -1,		N_CARPSTAT,	1,	NULL,
	  carp_stats,	NULL,		"carp",	1,	0 },
#ifdef PF
	{ -1,		N_PFSYNCSTAT,	1,	NULL,
	  pfsync_stats,	NULL,		"pfsync", 1,	0 },
#endif
	{ -1,		N_ARPSTAT,	1,	NULL,
	  arp_stats,	NULL,		"arp", 1,	0 },
	{ -1,		-1,		0,	NULL,
	  NULL,		NULL,		NULL,	0,	0 }
};

#ifdef INET6
struct protox ip6protox[] = {
	{ N_TCBINFO,	N_TCPSTAT,	1,	protopr,
	  tcp_stats,	NULL,		"tcp",	1,	IPPROTO_TCP },
	{ N_UDBINFO,	N_UDPSTAT,	1,	protopr,
	  udp_stats,	NULL,		"udp",	1,	IPPROTO_UDP },
	{ N_RIPCBINFO,	N_IP6STAT,	1,	protopr,
	  ip6_stats,	ip6_ifstats,	"ip6",	1,	IPPROTO_RAW },
	{ N_RIPCBINFO,	N_ICMP6STAT,	1,	protopr,
	  icmp6_stats,	icmp6_ifstats,	"icmp6", 1,	IPPROTO_ICMPV6 },
#ifdef SDP
	{ -1,		-1,		1,	protopr,
	 NULL,		NULL,		"sdp",	1,	IPPROTO_TCP },
#endif
#ifdef IPSEC
	{ -1,		N_IPSEC6STAT,	1,	NULL,
	  ipsec_stats,	NULL,		"ipsec6", 0,	0 },
#endif
#ifdef notyet
	{ -1,		N_PIM6STAT,	1,	NULL,
	  pim6_stats,	NULL,		"pim6",	1,	0 },
#endif
	{ -1,		N_RIP6STAT,	1,	NULL,
	  rip6_stats,	NULL,		"rip6",	1,	0 },
	{ -1,		-1,		0,	NULL,
	  NULL,		NULL,		NULL,	0,	0 }
};
#endif /*INET6*/

#ifdef IPSEC
struct protox pfkeyprotox[] = {
	{ -1,		N_PFKEYSTAT,	1,	NULL,
	  pfkey_stats,	NULL,		"pfkey", 0,	0 },
	{ -1,		-1,		0,	NULL,
	  NULL,		NULL,		NULL,	0,	0 }
};
#endif

#ifdef NETGRAPH
struct protox netgraphprotox[] = {
	{ N_NGSOCKS,	-1,		1,	netgraphprotopr,
	  NULL,		NULL,		"ctrl",	0,	0 },
	{ N_NGSOCKS,	-1,		1,	netgraphprotopr,
	  NULL,		NULL,		"data",	0,	0 },
	{ -1,		-1,		0,	NULL,
	  NULL,		NULL,		NULL,	0,	0 }
};
#endif

struct protox *protoprotox[] = {
					 protox,
#ifdef INET6
					 ip6protox,
#endif
#ifdef IPSEC
					 pfkeyprotox,
#endif
					 NULL };

static void printproto(struct protox *, const char *, bool *);
static void usage(void);
static struct protox *name2protox(const char *);
static struct protox *knownname(const char *);

static kvm_t *kvmd;
static char *nlistf = NULL, *memf = NULL;

int	Aflag;		/* show addresses of protocol control block */
int	aflag;		/* show all sockets (including servers) */
int	Bflag;		/* show information about bpf consumers */
int	bflag;		/* show i/f total bytes in/out */
int	dflag;		/* show i/f dropped packets */
int	gflag;		/* show group (multicast) routing or stats */
int	hflag;		/* show counters in human readable format */
int	iflag;		/* show interfaces */
int	Lflag;		/* show size of listen queues */
int	mflag;		/* show memory stats */
int	noutputs = 0;	/* how much outputs before we exit */
int	numeric_addr;	/* show addresses numerically */
int	numeric_port;	/* show ports numerically */
static int pflag;	/* show given protocol */
int	Qflag;		/* show netisr information */
int	rflag;		/* show routing tables (or routing stats) */
int	Rflag;		/* show flow / RSS statistics */
int	sflag;		/* show protocol statistics */
int	Wflag;		/* wide display */
int	Tflag;		/* TCP Information */
int	xflag;		/* extra information, includes all socket buffer info */
int	zflag;		/* zero stats */

int	interval;	/* repeat interval for i/f stats */

char	*interface;	/* desired i/f for stats, or NULL for all i/fs */
int	unit;		/* unit number for above */

int	af;		/* address family */
int	live;		/* true if we are examining a live system */

int
main(int argc, char *argv[])
{
	struct protox *tp = NULL;  /* for printing cblocks & stats */
	int ch;
	int fib = -1;
	char *endptr;
	bool first = true;

	af = AF_UNSPEC;

	argc = xo_parse_args(argc, argv);

	while ((ch = getopt(argc, argv, "46AaBbdF:f:ghI:iLlM:mN:np:Qq:RrSTsuWw:xz"))
	    != -1)
		switch(ch) {
		case '4':
#ifdef INET
			af = AF_INET;
#else
			errx(1, "IPv4 support is not compiled in");
#endif
			break;
		case '6':
#ifdef INET6
			af = AF_INET6;
#else
			errx(1, "IPv6 support is not compiled in");
#endif
			break;
		case 'A':
			Aflag = 1;
			break;
		case 'a':
			aflag = 1;
			break;
		case 'B':
			Bflag = 1;
			break;
		case 'b':
			bflag = 1;
			break;
		case 'd':
			dflag = 1;
			break;
		case 'F':
			fib = strtol(optarg, &endptr, 0);
			if (*endptr != '\0' ||
			    (fib == 0 && (errno == EINVAL || errno == ERANGE)))
				xo_errx(1, "%s: invalid fib", optarg);
			break;
		case 'f':
			if (strcmp(optarg, "inet") == 0)
				af = AF_INET;
#ifdef INET6
			else if (strcmp(optarg, "inet6") == 0)
				af = AF_INET6;
#endif
#ifdef IPSEC
			else if (strcmp(optarg, "pfkey") == 0)
				af = PF_KEY;
#endif
			else if (strcmp(optarg, "unix") == 0)
				af = AF_UNIX;
#ifdef NETGRAPH
			else if (strcmp(optarg, "ng") == 0
			    || strcmp(optarg, "netgraph") == 0)
				af = AF_NETGRAPH;
#endif
			else if (strcmp(optarg, "link") == 0)
				af = AF_LINK;
			else {
				xo_errx(1, "%s: unknown address family",
				    optarg);
			}
			break;
		case 'g':
			gflag = 1;
			break;
		case 'h':
			hflag = 1;
			break;
		case 'I': {
			char *cp;

			iflag = 1;
			for (cp = interface = optarg; isalpha(*cp); cp++)
				continue;
			unit = atoi(cp);
			break;
		}
		case 'i':
			iflag = 1;
			break;
		case 'L':
			Lflag = 1;
			break;
		case 'M':
			memf = optarg;
			break;
		case 'm':
			mflag = 1;
			break;
		case 'N':
			nlistf = optarg;
			break;
		case 'n':
			numeric_addr = numeric_port = 1;
			break;
		case 'p':
			if ((tp = name2protox(optarg)) == NULL) {
				xo_errx(1, "%s: unknown or uninstrumented "
				    "protocol", optarg);
			}
			pflag = 1;
			break;
		case 'Q':
			Qflag = 1;
			break;
		case 'q':
			noutputs = atoi(optarg);
			if (noutputs != 0)
				noutputs++;
			break;
		case 'r':
			rflag = 1;
			break;
		case 'R':
			Rflag = 1;
			break;
		case 's':
			++sflag;
			break;
		case 'S':
			numeric_addr = 1;
			break;
		case 'u':
			af = AF_UNIX;
			break;
		case 'W':
		case 'l':
			Wflag = 1;
			break;
		case 'w':
			interval = atoi(optarg);
			iflag = 1;
			break;
		case 'T':
			Tflag = 1;
			break;
		case 'x':
			xflag = 1;
			break;
		case 'z':
			zflag = 1;
			break;
		case '?':
		default:
			usage();
		}
	argv += optind;
	argc -= optind;

#define	BACKWARD_COMPATIBILITY
#ifdef	BACKWARD_COMPATIBILITY
	if (*argv) {
		if (isdigit(**argv)) {
			interval = atoi(*argv);
			if (interval <= 0)
				usage();
			++argv;
			iflag = 1;
		}
		if (*argv) {
			nlistf = *argv;
			if (*++argv)
				memf = *argv;
		}
	}
#endif

	/*
	 * Discard setgid privileges if not the running kernel so that bad
	 * guys can't print interesting stuff from kernel memory.
	 */
	live = (nlistf == NULL && memf == NULL);
	if (!live)
		setgid(getgid());

	if (xflag && Tflag)
		xo_errx(1, "-x and -T are incompatible, pick one.");

	if (Bflag) {
		if (!live)
			usage();
		bpf_stats(interface);
		xo_finish();
		exit(0);
	}
	if (mflag) {
		if (!live) {
			if (kread(0, NULL, 0) == 0)
				mbpr(kvmd, nl[N_SFSTAT].n_value);
		} else
			mbpr(NULL, 0);
		xo_finish();
		exit(0);
	}
	if (Qflag) {
		if (!live) {
			if (kread(0, NULL, 0) == 0)
				netisr_stats(kvmd);
		} else
			netisr_stats(NULL);
		xo_finish();
		exit(0);
	}
#if 0
	/*
	 * Keep file descriptors open to avoid overhead
	 * of open/close on each call to get* routines.
	 */
	sethostent(1);
	setnetent(1);
#else
	/*
	 * This does not make sense any more with DNS being default over
	 * the files.  Doing a setXXXXent(1) causes a tcp connection to be
	 * used for the queries, which is slower.
	 */
#endif
	if (iflag && !sflag) {
		xo_open_container("statistics");
		intpr(interval, NULL, af);
		xo_close_container("statistics");
		xo_finish();
		exit(0);
	}
	if (rflag) {
		xo_open_container("statistics");
		if (sflag) {
			rt_stats();
			flowtable_stats();
		} else
			routepr(fib, af);
		xo_close_container("statistics");
		xo_finish();
		exit(0);
	}

	if (gflag) {
		xo_open_container("statistics");
		if (sflag) {
			if (af == AF_INET || af == AF_UNSPEC)
				mrt_stats();
#ifdef INET6
			if (af == AF_INET6 || af == AF_UNSPEC)
				mrt6_stats();
#endif
		} else {
			if (af == AF_INET || af == AF_UNSPEC)
				mroutepr();
#ifdef INET6
			if (af == AF_INET6 || af == AF_UNSPEC)
				mroute6pr();
#endif
		}
		xo_close_container("statistics");
		xo_finish();
		exit(0);
	}

	/* Load all necessary kvm symbols */
	kresolve_list(nl);

	if (tp) {
		xo_open_container("statistics");
		printproto(tp, tp->pr_name, &first);
		if (!first)
			xo_close_list("socket");
		xo_close_container("statistics");
		xo_finish();
		exit(0);
	}

	xo_open_container("statistics");
	if (af == AF_INET || af == AF_UNSPEC)
		for (tp = protox; tp->pr_name; tp++)
			printproto(tp, tp->pr_name, &first);
#ifdef INET6
	if (af == AF_INET6 || af == AF_UNSPEC)
		for (tp = ip6protox; tp->pr_name; tp++)
			printproto(tp, tp->pr_name, &first);
#endif /*INET6*/
#ifdef IPSEC
	if (af == PF_KEY || af == AF_UNSPEC)
		for (tp = pfkeyprotox; tp->pr_name; tp++)
			printproto(tp, tp->pr_name, &first);
#endif /*IPSEC*/
#ifdef NETGRAPH
	if (af == AF_NETGRAPH || af == AF_UNSPEC)
		for (tp = netgraphprotox; tp->pr_name; tp++)
			printproto(tp, tp->pr_name, &first);
#endif /* NETGRAPH */
	if ((af == AF_UNIX || af == AF_UNSPEC) && !sflag)
		unixpr(nl[N_UNP_COUNT].n_value, nl[N_UNP_GENCNT].n_value,
		    nl[N_UNP_DHEAD].n_value, nl[N_UNP_SHEAD].n_value,
		    nl[N_UNP_SPHEAD].n_value, &first);

	if (!first)
		xo_close_list("socket");
	xo_close_container("statistics");
	xo_finish();
	exit(0);
}

/*
 * Print out protocol statistics or control blocks (per sflag).
 * If the interface was not specifically requested, and the symbol
 * is not in the namelist, ignore this one.
 */
static void
printproto(struct protox *tp, const char *name, bool *first)
{
	void (*pr)(u_long, const char *, int, int);
	u_long off;
	bool doingdblocks = false;

	if (sflag) {
		if (iflag) {
			if (tp->pr_istats)
				intpr(interval, tp->pr_istats, af);
			else if (pflag)
				xo_message("%s: no per-interface stats routine",
				    tp->pr_name);
			return;
		} else {
			pr = tp->pr_stats;
			if (!pr) {
				if (pflag)
					xo_message("%s: no stats routine",
					    tp->pr_name);
				return;
			}
			if (tp->pr_usesysctl && live)
				off = 0;
			else if (tp->pr_sindex < 0) {
				if (pflag)
					xo_message("%s: stats routine doesn't "
					    "work on cores", tp->pr_name);
				return;
			} else
				off = nl[tp->pr_sindex].n_value;
		}
	} else {
		doingdblocks = true;
		pr = tp->pr_cblocks;
		if (!pr) {
			if (pflag)
				xo_message("%s: no PCB routine", tp->pr_name);
			return;
		}
		if (tp->pr_usesysctl && live)
			off = 0;
		else if (tp->pr_index < 0) {
			if (pflag)
				xo_message("%s: PCB routine doesn't work on "
				    "cores", tp->pr_name);
			return;
		} else
			off = nl[tp->pr_index].n_value;
	}
	if (pr != NULL && (off || (live && tp->pr_usesysctl) ||
	    af != AF_UNSPEC)) {
		if (doingdblocks && *first) {
			xo_open_list("socket");
			*first = false;
		}

		(*pr)(off, name, af, tp->pr_protocol);
	}
}

static int
kvmd_init(void)
{
	char errbuf[_POSIX2_LINE_MAX];

	if (kvmd != NULL)
		return (0);

	kvmd = kvm_openfiles(nlistf, memf, NULL, O_RDONLY, errbuf);
	setgid(getgid());

	if (kvmd == NULL) {
		xo_warnx("kvm not available: %s", errbuf);
		return (-1);
	}

	return (0);
}

/*
 * Resolve symbol list, return 0 on success.
 */
int
kresolve_list(struct nlist *_nl)
{

	if ((kvmd == NULL) && (kvmd_init() != 0))
		return (-1);

	if (_nl[0].n_type != 0)
		return (0);

	if (kvm_nlist(kvmd, _nl) < 0) {
		if (nlistf)
			xo_errx(1, "%s: kvm_nlist: %s", nlistf,
			    kvm_geterr(kvmd));
		else
			xo_errx(1, "kvm_nlist: %s", kvm_geterr(kvmd));
	}

	return (0);
}

/*
 * Read kernel memory, return 0 on success.
 */
int
kread(u_long addr, void *buf, size_t size)
{

	if (kvmd_init() < 0)
		return (-1);

	if (!buf)
		return (0);
	if (kvm_read(kvmd, addr, buf, size) != (ssize_t)size) {
		xo_warnx("%s", kvm_geterr(kvmd));
		return (-1);
	}
	return (0);
}

/*
 * Read single counter(9).
 */
uint64_t
kread_counter(u_long addr)
{

	if (kvmd_init() < 0)
		return (-1);

	return (kvm_counter_u64_fetch(kvmd, addr));
}

/*
 * Read an array of N counters in kernel memory into array of N uint64_t's.
 */
int
kread_counters(u_long addr, void *buf, size_t size)
{
	uint64_t *c = buf;

	if (kvmd_init() < 0)
		return (-1);

	if (kread(addr, buf, size) < 0)
		return (-1);

	while (size != 0) {
		*c = kvm_counter_u64_fetch(kvmd, *c);
		size -= sizeof(*c);
		c++;
	}
	return (0);
}

const char *
plural(uintmax_t n)
{
	return (n != 1 ? "s" : "");
}

const char *
plurales(uintmax_t n)
{
	return (n != 1 ? "es" : "");
}

const char *
pluralies(uintmax_t n)
{
	return (n != 1 ? "ies" : "y");
}

/*
 * Find the protox for the given "well-known" name.
 */
static struct protox *
knownname(const char *name)
{
	struct protox **tpp, *tp;

	for (tpp = protoprotox; *tpp; tpp++)
		for (tp = *tpp; tp->pr_name; tp++)
			if (strcmp(tp->pr_name, name) == 0)
				return (tp);
	return (NULL);
}

/*
 * Find the protox corresponding to name.
 */
static struct protox *
name2protox(const char *name)
{
	struct protox *tp;
	char **alias;			/* alias from p->aliases */
	struct protoent *p;

	/*
	 * Try to find the name in the list of "well-known" names. If that
	 * fails, check if name is an alias for an Internet protocol.
	 */
	if ((tp = knownname(name)) != NULL)
		return (tp);

	setprotoent(1);			/* make protocol lookup cheaper */
	while ((p = getprotoent()) != NULL) {
		/* assert: name not same as p->name */
		for (alias = p->p_aliases; *alias; alias++)
			if (strcmp(name, *alias) == 0) {
				endprotoent();
				return (knownname(p->p_name));
			}
	}
	endprotoent();
	return (NULL);
}

static void
usage(void)
{
	(void)xo_error("%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n",
"usage: netstat [-46AaLnRSTWx] [-f protocol_family | -p protocol]\n"
"               [-M core] [-N system]",
"       netstat -i | -I interface [-46abdhnW] [-f address_family]\n"
"               [-M core] [-N system]",
"       netstat -w wait [-I interface] [-46d] [-M core] [-N system]\n"
"               [-q howmany]",
"       netstat -s [-46sz] [-f protocol_family | -p protocol]\n"
"               [-M core] [-N system]",
"       netstat -i | -I interface -s [-46s]\n"
"               [-f protocol_family | -p protocol] [-M core] [-N system]",
"       netstat -m [-M core] [-N system]",
"       netstat -B [-z] [-I interface]",
"       netstat -r [-46AnW] [-F fibnum] [-f address_family]\n"
"               [-M core] [-N system]",
"       netstat -rs [-s] [-M core] [-N system]",
"       netstat -g [-46W] [-f address_family] [-M core] [-N system]",
"       netstat -gs [-46s] [-f address_family] [-M core] [-N system]",
"       netstat -Q");
	xo_finish();
	exit(1);
}