/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License (the "License").
 * You may not use this file except in compliance with the License.
 *
 * You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
 * or http://www.opensolaris.org/os/licensing.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at usr/src/OPENSOLARIS.LICENSE.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 *
 * Portions Copyright 2006-2008 John Birrell jb@freebsd.org
 * Portions Copyright 2013 Justin Hibbits jhibbits@freebsd.org
 * Portions Copyright 2013 Howard Su howardsu@freebsd.org
 *
 * $FreeBSD: head/sys/cddl/dev/fbt/arm/fbt_isa.c 280039 2015-03-15 15:19:02Z rwatson $
 *
 */

/*
 * Copyright 2006 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

#include <sys/cdefs.h>
#include <sys/param.h>

#include <sys/dtrace.h>

#include "fbt.h"

#define	FBT_PATCHVAL		0xe7f000f0 /* Specified undefined instruction */

#define	FBT_PUSHM		0xe92d0000
#define	FBT_POPM		0xe8bd0000
#define	FBT_JUMP		0xea000000

#define	FBT_ENTRY	"entry"
#define	FBT_RETURN	"return"

int
fbt_invop(uintptr_t addr, uintptr_t *stack, uintptr_t rval)
{
	struct trapframe *frame = (struct trapframe *)stack;
	solaris_cpu_t *cpu = &solaris_cpu[curcpu];
	fbt_probe_t *fbt = fbt_probetab[FBT_ADDR2NDX(addr)];

	for (; fbt != NULL; fbt = fbt->fbtp_hashnext) {
		if ((uintptr_t)fbt->fbtp_patchpoint == addr) {
			fbt->fbtp_invop_cnt++;
			cpu->cpu_dtrace_caller = addr;

			/* TODO: Need 5th parameter from stack */
			dtrace_probe(fbt->fbtp_id, frame->tf_r0,
			    frame->tf_r1, frame->tf_r2,
			    frame->tf_r3, 0);

			cpu->cpu_dtrace_caller = 0;

			return (fbt->fbtp_rval | (fbt->fbtp_savedval << DTRACE_INVOP_SHIFT));
		}
	}

	return (0);
}

void
fbt_patch_tracepoint(fbt_probe_t *fbt, fbt_patchval_t val)
{

	*fbt->fbtp_patchpoint = val;
	cpu_icache_sync_range((vm_offset_t)fbt->fbtp_patchpoint, 4);
}

int
fbt_provide_module_function(linker_file_t lf, int symindx,
    linker_symval_t *symval, void *opaque)
{
	char *modname = opaque;
	const char *name = symval->name;
	fbt_probe_t *fbt, *retfbt;
	uint32_t *instr, *limit;
	int popm;

	if (strncmp(name, "dtrace_", 7) == 0 &&
	    strncmp(name, "dtrace_safe_", 12) != 0) {
		/*
		 * Anything beginning with "dtrace_" may be called
		 * from probe context unless it explicitly indicates
		 * that it won't be called from probe context by
		 * using the prefix "dtrace_safe_".
		 */
		return (0);
	}

	if (name[0] == '_' && name[1] == '_')
		return (0);

	/*
	 * Architecture-specific exclusion list, largely to do with FBT trap
	 * processing, to prevent reentrance.
	 */
	if (strcmp(name, "undefinedinstruction") == 0)
		return (0);

	instr = (uint32_t *)symval->value;
	limit = (uint32_t *)(symval->value + symval->size);

	for (; instr < limit; instr++)
		if ((*instr & 0xffff0000) == FBT_PUSHM &&
		    (*instr & 0x4000) != 0)
			break;

	if (instr >= limit)
		return (0);

	fbt = malloc(sizeof (fbt_probe_t), M_FBT, M_WAITOK | M_ZERO);
	fbt->fbtp_name = name;
	fbt->fbtp_id = dtrace_probe_create(fbt_id, modname,
	    name, FBT_ENTRY, 3, fbt);
	fbt->fbtp_patchpoint = instr;
	fbt->fbtp_ctl = lf;
	fbt->fbtp_loadcnt = lf->loadcnt;
	fbt->fbtp_savedval = *instr;
	fbt->fbtp_patchval = FBT_PATCHVAL;
	fbt->fbtp_rval = DTRACE_INVOP_PUSHM;
	fbt->fbtp_symindx = symindx;

	fbt->fbtp_hashnext = fbt_probetab[FBT_ADDR2NDX(instr)];
	fbt_probetab[FBT_ADDR2NDX(instr)] = fbt;

	lf->fbt_nentries++;

	popm = FBT_POPM | ((*instr) & 0x3FFF) | 0x8000;

	retfbt = NULL;
again:
	for (; instr < limit; instr++) {
		if (*instr == popm)
			break;
		else if ((*instr & 0xff000000) == FBT_JUMP) {
			uint32_t *target, *start;
			int offset;

			offset = (*instr & 0xffffff);
			offset <<= 8;
			offset /= 64;
			target = instr + (2 + offset);
			start = (uint32_t *)symval->value;
			if (target >= limit || target < start)
				break;
			instr++; /* skip delay slot */
		}
	}

	if (instr >= limit)
		return (0);

	/*
	 * We have a winner!
	 */
	fbt = malloc(sizeof (fbt_probe_t), M_FBT, M_WAITOK | M_ZERO);
	fbt->fbtp_name = name;
	if (retfbt == NULL) {
		fbt->fbtp_id = dtrace_probe_create(fbt_id, modname,
		    name, FBT_RETURN, 3, fbt);
	} else {
		retfbt->fbtp_next = fbt;
		fbt->fbtp_id = retfbt->fbtp_id;
	}
	retfbt = fbt;

	fbt->fbtp_patchpoint = instr;
	fbt->fbtp_ctl = lf;
	fbt->fbtp_loadcnt = lf->loadcnt;
	fbt->fbtp_symindx = symindx;
	if ((*instr & 0xff000000) == FBT_JUMP)
		fbt->fbtp_rval = DTRACE_INVOP_B;
	else
		fbt->fbtp_rval = DTRACE_INVOP_POPM;
	fbt->fbtp_savedval = *instr;
	fbt->fbtp_patchval = FBT_PATCHVAL;
	fbt->fbtp_hashnext = fbt_probetab[FBT_ADDR2NDX(instr)];
	fbt_probetab[FBT_ADDR2NDX(instr)] = fbt;

	lf->fbt_nentries++;

	instr++;
	goto again;
}
