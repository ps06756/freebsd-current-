#!/bin/sh
#
# Copyright (c) 2005 Poul-Henning Kamp.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
#
# $FreeBSD: head/tools/tools/nanobsd/defaults.sh 275832 2014-12-16 17:59:05Z will $
#

set -e

#######################################################################
#
# Setup default values for all controlling variables.
# These values can be overridden from the config file(s)
#
#######################################################################

# Name of this NanoBSD build.  (Used to construct workdir names)
NANO_NAME=full

# Source tree directory
NANO_SRC=/usr/src

# Where nanobsd additional files live under the source tree
NANO_TOOLS=tools/tools/nanobsd

# Where cust_pkg() finds packages to install
NANO_PACKAGE_DIR=${NANO_SRC}/${NANO_TOOLS}/Pkg
NANO_PACKAGE_LIST="*"

# where package metadata gets placed
NANO_PKG_META_BASE=/var/db

# Object tree directory
# default is subdir of /usr/obj
#NANO_OBJ=""

# The directory to put the final images
# default is ${NANO_OBJ}
#NANO_DISKIMGDIR=""

# Make & parallel Make
NANO_MAKE="make"
NANO_PMAKE="make -j 3"

# The default name for any image we create.
NANO_IMGNAME="_.disk.full"

# Options to put in make.conf during buildworld only
CONF_BUILD=' '

# Options to put in make.conf during installworld only
CONF_INSTALL=' '

# Options to put in make.conf during both build- & installworld.
CONF_WORLD=' '

# Kernel config file to use
NANO_KERNEL=GENERIC

# Kernel modules to install. If empty, no modules are installed.
# Use "default" to install all built modules.
NANO_MODULES=

# Customize commands.
NANO_CUSTOMIZE=""

# Late customize commands.
NANO_LATE_CUSTOMIZE=""

# Newfs paramters to use
NANO_NEWFS="-b 4096 -f 512 -i 8192 -U"

# The drive name of the media at runtime
NANO_DRIVE=ad0

# Target media size in 512 bytes sectors
NANO_MEDIASIZE=2000000

# Number of code images on media (1 or 2)
NANO_IMAGES=2

# 0 -> Leave second image all zeroes so it compresses better.
# 1 -> Initialize second image with a copy of the first
NANO_INIT_IMG2=1

# Size of code file system in 512 bytes sectors
# If zero, size will be as large as possible.
NANO_CODESIZE=0

# Size of configuration file system in 512 bytes sectors
# Cannot be zero.
NANO_CONFSIZE=2048

# Size of data file system in 512 bytes sectors
# If zero: no partition configured.
# If negative: max size possible
NANO_DATASIZE=0

# Size of the /etc ramdisk in 512 bytes sectors
NANO_RAM_ETCSIZE=10240

# Size of the /tmp+/var ramdisk in 512 bytes sectors
NANO_RAM_TMPVARSIZE=10240

# Media geometry, only relevant if bios doesn't understand LBA.
NANO_SECTS=63
NANO_HEADS=16

# boot0 flags/options and configuration
NANO_BOOT0CFG="-o packet -s 1 -m 3"
NANO_BOOTLOADER="boot/boot0sio"

# boot2 flags/options
# default force serial console
NANO_BOOT2CFG="-h"

# Backing type of md(4) device
# Can be "file" or "swap"
NANO_MD_BACKING="file"

# for swap type md(4) backing, write out the mbr only
NANO_IMAGE_MBRONLY=true

# Progress Print level
PPLEVEL=3

# Set NANO_LABEL to non-blank to form the basis for using /dev/ufs/label
# in preference to /dev/${NANO_DRIVE}
# Root partition will be ${NANO_LABEL}s{1,2}
# /cfg partition will be ${NANO_LABEL}s3
# /data partition will be ${NANO_LABEL}s4
NANO_LABEL=""

#######################################################################
# Architecture to build.  Corresponds to TARGET_ARCH in a buildworld.
# Unfortunately, there's no way to set TARGET at this time, and it
# conflates the two, so architectures where TARGET != TARGET_ARCH do
# not work.  This defaults to the arch of the current machine.

NANO_ARCH=`uname -p`

# Directory to populate /cfg from
NANO_CFGDIR=""

# Directory to populate /data from
NANO_DATADIR=""

# src.conf to use when building the image. Defaults to /dev/null for the sake
# of determinism.
SRCCONF=${SRCCONF:=/dev/null}
 
#######################################################################
#
# The functions which do the real work.
# Can be overridden from the config file(s)
#
#######################################################################

# rm doesn't know -x prior to FreeBSD 10, so cope with a variety of build
# hosts for now.
nano_rm ( ) {
	case $(uname -r) in
	7*|8*|9*) rm $* ;;
	*) rm -x $* ;;
	esac
}

# run in the world chroot, errors fatal
CR()
{
	chroot ${NANO_WORLDDIR} /bin/sh -exc "$*"
}

# run in the world chroot, errors not fatal
CR0()
{
	chroot ${NANO_WORLDDIR} /bin/sh -c "$*" || true
}

nano_cleanup ( ) (
	if [ $? -ne 0 ]; then
		echo "Error encountered.  Check for errors in last log file." 1>&2
	fi
	exit $?
)

clean_build ( ) (
	pprint 2 "Clean and create object directory (${MAKEOBJDIRPREFIX})"

	if ! nano_rm -rf ${MAKEOBJDIRPREFIX}/ > /dev/null 2>&1 ; then
		chflags -R noschg ${MAKEOBJDIRPREFIX}/
		nano_rm -r ${MAKEOBJDIRPREFIX}/
	fi
)

make_conf_build ( ) (
	pprint 2 "Construct build make.conf ($NANO_MAKE_CONF_BUILD)"

	mkdir -p ${MAKEOBJDIRPREFIX}
	printenv > ${MAKEOBJDIRPREFIX}/_.env

	echo "${CONF_WORLD}" > ${NANO_MAKE_CONF_BUILD}
	echo "${CONF_BUILD}" >> ${NANO_MAKE_CONF_BUILD}
)

build_world ( ) (
	pprint 2 "run buildworld"
	pprint 3 "log: ${MAKEOBJDIRPREFIX}/_.bw"

	cd ${NANO_SRC}
	env TARGET_ARCH=${NANO_ARCH} ${NANO_PMAKE} \
		SRCCONF=${SRCCONF} \
		__MAKE_CONF=${NANO_MAKE_CONF_BUILD} buildworld \
		> ${MAKEOBJDIRPREFIX}/_.bw 2>&1
)

build_kernel ( ) (
	local extra

	pprint 2 "build kernel ($NANO_KERNEL)"
	pprint 3 "log: ${MAKEOBJDIRPREFIX}/_.bk"

	(
	if [ -f ${NANO_KERNEL} ] ; then
		kernconfdir_arg="KERNCONFDIR='$(realpath $(dirname ${NANO_KERNEL}))'"
		kernconf=$(basename ${NANO_KERNEL})
	else
		kernconf=${NANO_KERNEL}
	fi

	cd ${NANO_SRC};
	# unset these just in case to avoid compiler complaints
	# when cross-building
	unset TARGET_CPUTYPE
	# Note: We intentionally build all modules, not only the ones in
	# NANO_MODULES so the built world can be reused by multiple images.
	eval "TARGET_ARCH=${NANO_ARCH} ${NANO_PMAKE} buildkernel \
		SRCCONF='${SRCCONF}' \
		__MAKE_CONF='${NANO_MAKE_CONF_BUILD}' \
		${kernconfdir_arg} KERNCONF=${kernconf}"
	) > ${MAKEOBJDIRPREFIX}/_.bk 2>&1
)

clean_world ( ) (
	if [ "${NANO_OBJ}" != "${MAKEOBJDIRPREFIX}" ]; then
		pprint 2 "Clean and create object directory (${NANO_OBJ})"
		if ! nano_rm -rf ${NANO_OBJ}/ > /dev/null 2>&1 ; then
			chflags -R noschg ${NANO_OBJ}
			nano_rm -r ${NANO_OBJ}/
		fi
		mkdir -p ${NANO_OBJ} ${NANO_WORLDDIR}
		printenv > ${NANO_OBJ}/_.env
	else
		pprint 2 "Clean and create world directory (${NANO_WORLDDIR})"
		if ! nano_rm -rf ${NANO_WORLDDIR}/ > /dev/null 2>&1 ; then
			chflags -R noschg ${NANO_WORLDDIR}
			nano_rm -rf ${NANO_WORLDDIR}/
		fi
		mkdir -p ${NANO_WORLDDIR}
	fi
)

make_conf_install ( ) (
	pprint 2 "Construct install make.conf ($NANO_MAKE_CONF_INSTALL)"

	echo "${CONF_WORLD}" > ${NANO_MAKE_CONF_INSTALL}
	echo "${CONF_INSTALL}" >> ${NANO_MAKE_CONF_INSTALL}
)

install_world ( ) (
	pprint 2 "installworld"
	pprint 3 "log: ${NANO_OBJ}/_.iw"

	cd ${NANO_SRC}
	env TARGET_ARCH=${NANO_ARCH} \
	${NANO_MAKE} SRCCONF=${SRCCONF} \
		__MAKE_CONF=${NANO_MAKE_CONF_INSTALL} installworld \
		DESTDIR=${NANO_WORLDDIR} \
		> ${NANO_OBJ}/_.iw 2>&1
	chflags -R noschg ${NANO_WORLDDIR}
)

install_etc ( ) (

	pprint 2 "install /etc"
	pprint 3 "log: ${NANO_OBJ}/_.etc"

	cd ${NANO_SRC}
	env TARGET_ARCH=${NANO_ARCH} \
	${NANO_MAKE} SRCCONF=${SRCCONF} \
		__MAKE_CONF=${NANO_MAKE_CONF_INSTALL} distribution \
		DESTDIR=${NANO_WORLDDIR} \
		> ${NANO_OBJ}/_.etc 2>&1
	# make.conf doesn't get created by default, but some ports need it
	# so they can spam it.
	cp /dev/null ${NANO_WORLDDIR}/etc/make.conf
)

install_kernel ( ) (
	local extra

	pprint 2 "install kernel ($NANO_KERNEL)"
	pprint 3 "log: ${NANO_OBJ}/_.ik"

	(
	if [ -f ${NANO_KERNEL} ] ; then
		kernconfdir_arg="KERNCONFDIR='$(realpath $(dirname ${NANO_KERNEL}))'"
		kernconf=$(basename ${NANO_KERNEL})
	else
		kernconf=${NANO_KERNEL}
	fi

	# Install all built modules if NANO_MODULES=default,
	# else install only listed modules (none if NANO_MODULES is empty).
	if [ "${NANO_MODULES}" != "default" ]; then
		modules_override_arg="MODULES_OVERRIDE='${NANO_MODULES}'"
	fi

	cd ${NANO_SRC}
	eval "TARGET_ARCH=${NANO_ARCH} ${NANO_MAKE} installkernel \
		DESTDIR='${NANO_WORLDDIR}' \
		SRCCONF='${SRCCONF}' \
		__MAKE_CONF='${NANO_MAKE_CONF_INSTALL}' \
		${kernconfdir_arg} KERNCONF=${kernconf} \
		${modules_override_arg}"
	) > ${NANO_OBJ}/_.ik 2>&1
)

native_xtools ( ) (
	print 2 "Installing the optimized native build tools for cross env"
	pprint 3 "log: ${NANO_OBJ}/_.native_xtools"

	cd ${NANO_SRC}
	env TARGET_ARCH=${NANO_ARCH} \
	${NANO_MAKE} SRCCONF=${SRCCONF} \
		__MAKE_CONF=${NANO_MAKE_CONF_INSTALL} native-xtools \
		DESTDIR=${NANO_WORLDDIR} \
		> ${NANO_OBJ}/_.native_xtools 2>&1
)

run_customize() (

	pprint 2 "run customize scripts"
	for c in $NANO_CUSTOMIZE
	do
		pprint 2 "customize \"$c\""
		pprint 3 "log: ${NANO_OBJ}/_.cust.$c"
		pprint 4 "`type $c`"
		( set -x ; $c ) > ${NANO_OBJ}/_.cust.$c 2>&1
	done
)

run_late_customize() (

	pprint 2 "run late customize scripts"
	for c in $NANO_LATE_CUSTOMIZE
	do
		pprint 2 "late customize \"$c\""
		pprint 3 "log: ${NANO_OBJ}/_.late_cust.$c"
		pprint 4 "`type $c`"
		( set -x ; $c ) > ${NANO_OBJ}/_.late_cust.$c 2>&1
	done
)

setup_nanobsd ( ) (
	pprint 2 "configure nanobsd setup"
	pprint 3 "log: ${NANO_OBJ}/_.dl"

	(
	cd ${NANO_WORLDDIR}

	# Move /usr/local/etc to /etc/local so that the /cfg stuff
	# can stomp on it.  Otherwise packages like ipsec-tools which
	# have hardcoded paths under ${prefix}/etc are not tweakable.
	if [ -d usr/local/etc ] ; then
		(
		mkdir -p etc/local
		cd usr/local/etc
		find . -print | cpio -dumpl ../../../etc/local
		cd ..
		nano_rm -rf etc
		ln -s ../../etc/local etc
		)
	fi

	for d in var etc
	do
		# link /$d under /conf
		# we use hard links so we have them both places.
		# the files in /$d will be hidden by the mount.
		# XXX: configure /$d ramdisk size
		mkdir -p conf/base/$d conf/default/$d
		find $d -print | cpio -dumpl conf/base/
	done

	echo "$NANO_RAM_ETCSIZE" > conf/base/etc/md_size
	echo "$NANO_RAM_TMPVARSIZE" > conf/base/var/md_size

	# pick up config files from the special partition
	echo "mount -o ro /dev/${NANO_DRIVE}s3" > conf/default/etc/remount

	# Put /tmp on the /var ramdisk (could be symlink already)
	nano_rm -rf tmp
	ln -s var/tmp tmp

	) > ${NANO_OBJ}/_.dl 2>&1
)

setup_nanobsd_etc ( ) (
	pprint 2 "configure nanobsd /etc"

	(
	cd ${NANO_WORLDDIR}

	# create diskless marker file
	touch etc/diskless

	# Make root filesystem R/O by default
	echo "root_rw_mount=NO" >> etc/defaults/rc.conf

	# save config file for scripts
	echo "NANO_DRIVE=${NANO_DRIVE}" > etc/nanobsd.conf

	echo "/dev/${NANO_DRIVE}s1a / ufs ro 1 1" > etc/fstab
	echo "/dev/${NANO_DRIVE}s3 /cfg ufs rw,noauto 2 2" >> etc/fstab
	mkdir -p cfg
	)
)

prune_usr() (

	# Remove all empty directories in /usr 
	find ${NANO_WORLDDIR}/usr -type d -depth -print |
		while read d
		do
			rmdir $d > /dev/null 2>&1 || true 
		done
)

newfs_part ( ) (
	local dev mnt lbl
	dev=$1
	mnt=$2
	lbl=$3
	echo newfs ${NANO_NEWFS} ${NANO_LABEL:+-L${NANO_LABEL}${lbl}} ${dev}
	newfs ${NANO_NEWFS} ${NANO_LABEL:+-L${NANO_LABEL}${lbl}} ${dev}
	mount -o async ${dev} ${mnt}
)

# Convenient spot to work around any umount issues that your build environment
# hits by overriding this method.
nano_umount () (
	umount ${1}
)

populate_slice ( ) (
	local dev dir mnt lbl
	dev=$1
	dir=$2
	mnt=$3
	lbl=$4
	echo "Creating ${dev} (mounting on ${mnt})"
	newfs_part ${dev} ${mnt} ${lbl}
	if [ -n "${dir}" -a -d "${dir}" ]; then
		echo "Populating ${lbl} from ${dir}"
		cd ${dir}
		find . -print | grep -Ev '/(CVS|\.svn|\.hg|\.git)' | cpio -dumpv ${mnt}
	fi
	df -i ${mnt}
	nano_umount ${mnt}
)

populate_cfg_slice ( ) (
	populate_slice "$1" "$2" "$3" "$4"
)

populate_data_slice ( ) (
	populate_slice "$1" "$2" "$3" "$4"
)

create_diskimage ( ) (
	pprint 2 "build diskimage"
	pprint 3 "log: ${NANO_OBJ}/_.di"

	(
	echo $NANO_MEDIASIZE $NANO_IMAGES \
		$NANO_SECTS $NANO_HEADS \
		$NANO_CODESIZE $NANO_CONFSIZE $NANO_DATASIZE |
	awk '
	{
		printf "# %s\n", $0

		# size of cylinder in sectors
		cs = $3 * $4

		# number of full cylinders on media
		cyl = int ($1 / cs)

		# output fdisk geometry spec, truncate cyls to 1023
		if (cyl <= 1023)
			print "g c" cyl " h" $4 " s" $3
		else
			print "g c" 1023 " h" $4 " s" $3

		if ($7 > 0) { 
			# size of data partition in full cylinders
			dsl = int (($7 + cs - 1) / cs)
		} else {
			dsl = 0;
		}

		# size of config partition in full cylinders
		csl = int (($6 + cs - 1) / cs)

		if ($5 == 0) {
			# size of image partition(s) in full cylinders
			isl = int ((cyl - dsl - csl) / $2)
		} else {
			isl = int (($5 + cs - 1) / cs)
		}

		# First image partition start at second track
		print "p 1 165 " $3, isl * cs - $3
		c = isl * cs;

		# Second image partition (if any) also starts offset one 
		# track to keep them identical.
		if ($2 > 1) {
			print "p 2 165 " $3 + c, isl * cs - $3
			c += isl * cs;
		}

		# Config partition starts at cylinder boundary.
		print "p 3 165 " c, csl * cs
		c += csl * cs

		# Data partition (if any) starts at cylinder boundary.
		if ($7 > 0) {
			print "p 4 165 " c, dsl * cs
		} else if ($7 < 0 && $1 > c) {
			print "p 4 165 " c, $1 - c
		} else if ($1 < c) {
			print "Disk space overcommitted by", \
			    c - $1, "sectors" > "/dev/stderr"
			exit 2
		}

		# Force slice 1 to be marked active. This is necessary
		# for booting the image from a USB device to work.
		print "a 1"
	}
	' > ${NANO_OBJ}/_.fdisk

	IMG=${NANO_DISKIMGDIR}/${NANO_IMGNAME}
	MNT=${NANO_OBJ}/_.mnt
	mkdir -p ${MNT}

	if [ "${NANO_MD_BACKING}" = "swap" ] ; then
		MD=`mdconfig -a -t swap -s ${NANO_MEDIASIZE} -x ${NANO_SECTS} \
			-y ${NANO_HEADS}`
	else
		echo "Creating md backing file..."
		nano_rm -f ${IMG}
		dd if=/dev/zero of=${IMG} seek=${NANO_MEDIASIZE} count=0
		MD=`mdconfig -a -t vnode -f ${IMG} -x ${NANO_SECTS} \
			-y ${NANO_HEADS}`
	fi

	trap "echo 'Running exit trap code' ; df -i ${MNT} ; nano_umount ${MNT} || true ; mdconfig -d -u $MD" 1 2 15 EXIT

	fdisk -i -f ${NANO_OBJ}/_.fdisk ${MD}
	fdisk ${MD}
	# XXX: params
	# XXX: pick up cached boot* files, they may not be in image anymore.
	if [ -f ${NANO_WORLDDIR}/${NANO_BOOTLOADER} ]; then
		boot0cfg -B -b ${NANO_WORLDDIR}/${NANO_BOOTLOADER} ${NANO_BOOT0CFG} ${MD}
	fi
	if [ -f ${NANO_WORLDDIR}/boot/boot ]; then
		bsdlabel -w -B -b ${NANO_WORLDDIR}/boot/boot ${MD}s1
	else
		bsdlabel -w ${MD}s1
	fi
	bsdlabel ${MD}s1

	# Create first image
	populate_slice /dev/${MD}s1a ${NANO_WORLDDIR} ${MNT} "s1a"
	mount /dev/${MD}s1a ${MNT}
	echo "Generating mtree..."
	( cd ${MNT} && mtree -c ) > ${NANO_OBJ}/_.mtree
	( cd ${MNT} && du -k ) > ${NANO_OBJ}/_.du
	nano_umount ${MNT}

	if [ $NANO_IMAGES -gt 1 -a $NANO_INIT_IMG2 -gt 0 ] ; then
		# Duplicate to second image (if present)
		echo "Duplicating to second image..."
		dd conv=sparse if=/dev/${MD}s1 of=/dev/${MD}s2 bs=64k
		mount /dev/${MD}s2a ${MNT}
		for f in ${MNT}/etc/fstab ${MNT}/conf/base/etc/fstab
		do
			sed -i "" "s=${NANO_DRIVE}s1=${NANO_DRIVE}s2=g" $f
		done
		nano_umount ${MNT}
		# Override the label from the first partition so we
		# don't confuse glabel with duplicates.
		if [ ! -z ${NANO_LABEL} ]; then
			tunefs -L ${NANO_LABEL}"s2a" /dev/${MD}s2a
		fi
	fi
	
	# Create Config slice
	populate_cfg_slice /dev/${MD}s3 "${NANO_CFGDIR}" ${MNT} "s3"

	# Create Data slice, if any.
	if [ $NANO_DATASIZE -ne 0 ] ; then
		populate_data_slice /dev/${MD}s4 "${NANO_DATADIR}" ${MNT} "s4"
	fi

	if [ "${NANO_MD_BACKING}" = "swap" ] ; then
		if [ ${NANO_IMAGE_MBRONLY} ]; then
			echo "Writing out _.disk.mbr..."
			dd if=/dev/${MD} of=${NANO_DISKIMGDIR}/_.disk.mbr bs=512 count=1
		else
			echo "Writing out ${NANO_IMGNAME}..."
			dd if=/dev/${MD} of=${IMG} bs=64k
		fi

		echo "Writing out ${NANO_IMGNAME}..."
		dd conv=sparse if=/dev/${MD} of=${IMG} bs=64k
	fi

	if ${do_copyout_partition} ; then
		echo "Writing out _.disk.image..."
		dd conv=sparse if=/dev/${MD}s1 of=${NANO_DISKIMGDIR}/_.disk.image bs=64k
	fi
	mdconfig -d -u $MD

	trap - 1 2 15
	trap nano_cleanup EXIT

	) > ${NANO_OBJ}/_.di 2>&1
)

last_orders () (
	# Redefine this function with any last orders you may have
	# after the build completed, for instance to copy the finished
	# image to a more convenient place:
	# cp ${NANO_DISKIMGDIR}/_.disk.image /home/ftp/pub/nanobsd.disk
	true
)

#######################################################################
#
# Optional convenience functions.
#
#######################################################################

#######################################################################
# Common Flash device geometries
#

FlashDevice () {
	if [ -d ${NANO_TOOLS} ] ; then
		. ${NANO_TOOLS}/FlashDevice.sub
	else
		. ${NANO_SRC}/${NANO_TOOLS}/FlashDevice.sub
	fi
	sub_FlashDevice $1 $2
}

#######################################################################
# USB device geometries
#
# Usage:
#	UsbDevice Generic 1000	# a generic flash key sold as having 1GB
#
# This function will set NANO_MEDIASIZE, NANO_HEADS and NANO_SECTS for you.
#
# Note that the capacity of a flash key is usually advertised in MB or
# GB, *not* MiB/GiB. As such, the precise number of cylinders available
# for C/H/S geometry may vary depending on the actual flash geometry.
#
# The following generic device layouts are understood:
#  generic           An alias for generic-hdd.
#  generic-hdd       255H 63S/T xxxxC with no MBR restrictions.
#  generic-fdd       64H 32S/T xxxxC with no MBR restrictions.
#
# The generic-hdd device is preferred for flash devices larger than 1GB.
#

UsbDevice () {
	a1=`echo $1 | tr '[:upper:]' '[:lower:]'`
	case $a1 in
	generic-fdd)
		NANO_HEADS=64
		NANO_SECTS=32
		NANO_MEDIASIZE=$(( $2 * 1000 * 1000 / 512 ))
		;;
	generic|generic-hdd)
		NANO_HEADS=255
		NANO_SECTS=63
		NANO_MEDIASIZE=$(( $2 * 1000 * 1000 / 512 ))
		;;
	*)
		echo "Unknown USB flash device"
		exit 2
		;;
	esac
}

#######################################################################
# Setup serial console

cust_comconsole () (
	# Enable getty on console
	sed -i "" -e /tty[du]0/s/off/on/ ${NANO_WORLDDIR}/etc/ttys

	# Disable getty on syscons devices
	sed -i "" -e '/^ttyv[0-8]/s/	on/	off/' ${NANO_WORLDDIR}/etc/ttys

	# Tell loader to use serial console early.
	echo "${NANO_BOOT2CFG}" > ${NANO_WORLDDIR}/boot.config
)

#######################################################################
# Allow root login via ssh

cust_allow_ssh_root () (
	sed -i "" -e '/PermitRootLogin/s/.*/PermitRootLogin yes/' \
	    ${NANO_WORLDDIR}/etc/ssh/sshd_config
)

#######################################################################
# Install the stuff under ./Files

cust_install_files () (
	cd ${NANO_TOOLS}/Files
	find . -print | grep -Ev '/(CVS|\.svn|\.hg|\.git)' | cpio -Ldumpv ${NANO_WORLDDIR}
)

#######################################################################
# Install packages from ${NANO_PACKAGE_DIR}

cust_pkg () (

	# If the package directory doesn't exist, we're done.
	if [ ! -d ${NANO_PACKAGE_DIR} ]; then
		echo "DONE 0 packages"
		return 0
	fi

	# Copy packages into chroot
	mkdir -p ${NANO_WORLDDIR}/Pkg ${NANO_WORLDDIR}/${NANO_PKG_META_BASE}/pkg
	(
		cd ${NANO_PACKAGE_DIR}
		find ${NANO_PACKAGE_LIST} -print |
		    cpio -Ldumpv ${NANO_WORLDDIR}/Pkg
	)

	# Count & report how many we have to install
	todo=`ls ${NANO_WORLDDIR}/Pkg | wc -l`
	echo "=== TODO: $todo"
	ls ${NANO_WORLDDIR}/Pkg
	echo "==="
	while true
	do
		# Record how many we have now
		have=`ls ${NANO_WORLDDIR}/${NANO_PKG_META_BASE}/pkg | wc -l`

		# Attempt to install more packages
		# ...but no more than 200 at a time due to pkg_add's internal
		# limitations.
		CR0 'ls Pkg/*tbz | xargs -n 200 env PKG_DBDIR='${NANO_PKG_META_BASE}'/pkg pkg_add -v -F'

		# See what that got us
		now=`ls ${NANO_WORLDDIR}/${NANO_PKG_META_BASE}/pkg | wc -l`
		echo "=== NOW $now"
		ls ${NANO_WORLDDIR}/${NANO_PKG_META_BASE}/pkg
		echo "==="


		if [ $now -eq $todo ] ; then
			echo "DONE $now packages"
			break
		elif [ $now -eq $have ] ; then
			echo "FAILED: Nothing happened on this pass"
			exit 2
		fi
	done
	nano_rm -rf ${NANO_WORLDDIR}/Pkg
)

cust_pkgng () (

	# If the package directory doesn't exist, we're done.
	if [ ! -d ${NANO_PACKAGE_DIR} ]; then
		echo "DONE 0 packages"
		return 0
	fi

	# Find a pkg-* package
	for x in `find -s ${NANO_PACKAGE_DIR} -iname 'pkg-*'`; do
		_NANO_PKG_PACKAGE=`basename "$x"`
	done
	if [ -z "${_NANO_PKG_PACKAGE}" -o ! -f "${NANO_PACKAGE_DIR}/${_NANO_PKG_PACKAGE}" ]; then
		echo "FAILED: need a pkg/ package for bootstrapping"
		exit 2
	fi

	# Copy packages into chroot
	mkdir -p ${NANO_WORLDDIR}/Pkg
	(
		cd ${NANO_PACKAGE_DIR}
		find ${NANO_PACKAGE_LIST} -print |
		cpio -Ldumpv ${NANO_WORLDDIR}/Pkg
	)

	#Bootstrap pkg
	CR env ASSUME_ALWAYS_YES=YES SIGNATURE_TYPE=none /usr/sbin/pkg add /Pkg/${_NANO_PKG_PACKAGE}
	CR pkg -N >/dev/null 2>&1
	if [ "$?" -ne "0" ]; then
		echo "FAILED: pkg bootstrapping faied"
		exit 2
	fi
	nano_rm -f ${NANO_WORLDDIR}/Pkg/pkg-*

	# Count & report how many we have to install
	todo=`ls ${NANO_WORLDDIR}/Pkg | /usr/bin/wc -l`
	todo=$(expr $todo + 1) # add one for pkg since it is installed already
	echo "=== TODO: $todo"
	ls ${NANO_WORLDDIR}/Pkg
	echo "==="
	while true
	do
		# Record how many we have now
 		have=$(CR env ASSUME_ALWAYS_YES=YES /usr/sbin/pkg info | /usr/bin/wc -l)

		# Attempt to install more packages
		CR0 'ls 'Pkg/*txz' | xargs env ASSUME_ALWAYS_YES=YES /usr/sbin/pkg add'

		# See what that got us
 		now=$(CR env ASSUME_ALWAYS_YES=YES /usr/sbin/pkg info | /usr/bin/wc -l)
		echo "=== NOW $now"
		CR env ASSUME_ALWAYS_YES=YES /usr/sbin/pkg info
		echo "==="
		if [ $now -eq $todo ] ; then
			echo "DONE $now packages"
			break
		elif [ $now -eq $have ] ; then
			echo "FAILED: Nothing happened on this pass"
			exit 2
		fi
	done
	nano_rm -rf ${NANO_WORLDDIR}/Pkg
)

#######################################################################
# Convenience function:
# 	Register all args as customize function.

customize_cmd () {
	NANO_CUSTOMIZE="$NANO_CUSTOMIZE $*"
}

#######################################################################
# Convenience function:
# 	Register all args as late customize function to run just before
#	image creation.

late_customize_cmd () {
	NANO_LATE_CUSTOMIZE="$NANO_LATE_CUSTOMIZE $*"
}

#######################################################################
#
# All set up to go...
#
#######################################################################

# Progress Print
#	Print $2 at level $1.
pprint() (
    if [ "$1" -le $PPLEVEL ]; then
	runtime=$(( `date +%s` - $NANO_STARTTIME ))
	printf "%s %.${1}s %s\n" "`date -u -r $runtime +%H:%M:%S`" "#####" "$2" 1>&3
    fi
)

usage () {
	(
	echo "Usage: $0 [-bfiKknqvw] [-c config_file]"
	echo "	-b	suppress builds (both kernel and world)"
	echo "	-c	specify config file"
	echo "	-f	suppress code slice extraction"
	echo "	-i	suppress disk image build"
	echo "	-K	suppress installkernel"
	echo "	-k	suppress buildkernel"
	echo "	-n	add -DNO_CLEAN to buildworld, buildkernel, etc"
	echo "	-q	make output more quiet"
	echo "	-v	make output more verbose"
	echo "	-w	suppress buildworld"
	) 1>&2
	exit 2
}

#######################################################################
# Setup and Export Internal variables
#

export_var() {
	var=$1
	# Lookup value of the variable.
	eval val=\$$var
	pprint 3 "Setting variable: $var=\"$val\""
	export $1
}

# Call this function to set defaults _after_ parsing options.
set_defaults_and_export() {
	test -n "${NANO_OBJ}" || NANO_OBJ=/usr/obj/nanobsd.${NANO_NAME}
	test -n "${MAKEOBJDIRPREFIX}" || MAKEOBJDIRPREFIX=${NANO_OBJ}
	test -n "${NANO_DISKIMGDIR}" || NANO_DISKIMGDIR=${NANO_OBJ}
	NANO_WORLDDIR=${NANO_OBJ}/_.w
	NANO_MAKE_CONF_BUILD=${MAKEOBJDIRPREFIX}/make.conf.build
	NANO_MAKE_CONF_INSTALL=${NANO_OBJ}/make.conf.install

	# Override user's NANO_DRIVE if they specified a NANO_LABEL
	[ ! -z "${NANO_LABEL}" ] && NANO_DRIVE="ufs/${NANO_LABEL}"

	# Set a default NANO_TOOLS to NANO_SRC/NANO_TOOLS if it exists.
	[ ! -d "${NANO_TOOLS}" ] && [ -d "${NANO_SRC}/${NANO_TOOLS}" ] && \
		NANO_TOOLS="${NANO_SRC}/${NANO_TOOLS}"

	NANO_STARTTIME=`date +%s`
	pprint 3 "Exporting NanoBSD variables"
	export_var MAKEOBJDIRPREFIX
	export_var NANO_ARCH
	export_var NANO_CODESIZE
	export_var NANO_CONFSIZE
	export_var NANO_CUSTOMIZE
	export_var NANO_DATASIZE
	export_var NANO_DRIVE
	export_var NANO_HEADS
	export_var NANO_IMAGES
	export_var NANO_IMGNAME
	export_var NANO_MAKE
	export_var NANO_MAKE_CONF_BUILD
	export_var NANO_MAKE_CONF_INSTALL
	export_var NANO_MEDIASIZE
	export_var NANO_NAME
	export_var NANO_NEWFS
	export_var NANO_OBJ
	export_var NANO_PMAKE
	export_var NANO_SECTS
	export_var NANO_SRC
	export_var NANO_TOOLS
	export_var NANO_WORLDDIR
	export_var NANO_BOOT0CFG
	export_var NANO_BOOTLOADER
	export_var NANO_LABEL
	export_var NANO_MODULES
}
