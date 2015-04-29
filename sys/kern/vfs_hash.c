/*-
 * Copyright (c) 2005 Poul-Henning Kamp
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#include <sys/cdefs.h>
__FBSDID("$FreeBSD: head/sys/kern/vfs_hash.c 276424 2014-12-30 21:40:45Z mjg $");

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/malloc.h>
#include <sys/mount.h>
#include <sys/rwlock.h>
#include <sys/vnode.h>

static MALLOC_DEFINE(M_VFS_HASH, "vfs_hash", "VFS hash table");

static LIST_HEAD(vfs_hash_head, vnode)	*vfs_hash_tbl;
static LIST_HEAD(,vnode)		vfs_hash_side;
static u_long				vfs_hash_mask;
static struct rwlock			vfs_hash_lock;

static void
vfs_hashinit(void *dummy __unused)
{

	vfs_hash_tbl = hashinit(desiredvnodes, M_VFS_HASH, &vfs_hash_mask);
	rw_init(&vfs_hash_lock, "vfs hash");
	LIST_INIT(&vfs_hash_side);
}

/* Must be SI_ORDER_SECOND so desiredvnodes is available */
SYSINIT(vfs_hash, SI_SUB_VFS, SI_ORDER_SECOND, vfs_hashinit, NULL);

u_int
vfs_hash_index(struct vnode *vp)
{

	return (vp->v_hash + vp->v_mount->mnt_hashseed);
}

static struct vfs_hash_head *
vfs_hash_bucket(const struct mount *mp, u_int hash)
{

	return (&vfs_hash_tbl[(hash + mp->mnt_hashseed) & vfs_hash_mask]);
}

int
vfs_hash_get(const struct mount *mp, u_int hash, int flags, struct thread *td, struct vnode **vpp, vfs_hash_cmp_t *fn, void *arg)
{
	struct vnode *vp;
	int error;

	while (1) {
		rw_rlock(&vfs_hash_lock);
		LIST_FOREACH(vp, vfs_hash_bucket(mp, hash), v_hashlist) {
			if (vp->v_hash != hash)
				continue;
			if (vp->v_mount != mp)
				continue;
			if (fn != NULL && fn(vp, arg))
				continue;
			VI_LOCK(vp);
			rw_runlock(&vfs_hash_lock);
			error = vget(vp, flags | LK_INTERLOCK, td);
			if (error == ENOENT && (flags & LK_NOWAIT) == 0)
				break;
			if (error)
				return (error);
			*vpp = vp;
			return (0);
		}
		if (vp == NULL) {
			rw_runlock(&vfs_hash_lock);
			*vpp = NULL;
			return (0);
		}
	}
}

void
vfs_hash_remove(struct vnode *vp)
{

	rw_wlock(&vfs_hash_lock);
	LIST_REMOVE(vp, v_hashlist);
	rw_wunlock(&vfs_hash_lock);
}

int
vfs_hash_insert(struct vnode *vp, u_int hash, int flags, struct thread *td, struct vnode **vpp, vfs_hash_cmp_t *fn, void *arg)
{
	struct vnode *vp2;
	int error;

	*vpp = NULL;
	while (1) {
		rw_wlock(&vfs_hash_lock);
		LIST_FOREACH(vp2,
		    vfs_hash_bucket(vp->v_mount, hash), v_hashlist) {
			if (vp2->v_hash != hash)
				continue;
			if (vp2->v_mount != vp->v_mount)
				continue;
			if (fn != NULL && fn(vp2, arg))
				continue;
			VI_LOCK(vp2);
			rw_wunlock(&vfs_hash_lock);
			error = vget(vp2, flags | LK_INTERLOCK, td);
			if (error == ENOENT && (flags & LK_NOWAIT) == 0)
				break;
			rw_wlock(&vfs_hash_lock);
			LIST_INSERT_HEAD(&vfs_hash_side, vp, v_hashlist);
			rw_wunlock(&vfs_hash_lock);
			vput(vp);
			if (!error)
				*vpp = vp2;
			return (error);
		}
		if (vp2 == NULL)
			break;
			
	}
	vp->v_hash = hash;
	LIST_INSERT_HEAD(vfs_hash_bucket(vp->v_mount, hash), vp, v_hashlist);
	rw_wunlock(&vfs_hash_lock);
	return (0);
}

void
vfs_hash_rehash(struct vnode *vp, u_int hash)
{

	rw_wlock(&vfs_hash_lock);
	LIST_REMOVE(vp, v_hashlist);
	LIST_INSERT_HEAD(vfs_hash_bucket(vp->v_mount, hash), vp, v_hashlist);
	vp->v_hash = hash;
	rw_wunlock(&vfs_hash_lock);
}