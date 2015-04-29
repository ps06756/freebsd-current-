/*-
 * Copyright (c) 2014 Sandvine Inc.
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $FreeBSD: head/sys/cddl/compat/opensolaris/sys/nvpair.h 279437 2015-03-01 00:22:45Z rstone $
 */

#ifndef _OPENSOLARIS_SYS_NVPAIR_H_
#define _OPENSOLARIS_SYS_NVPAIR_H_

/*
 * Some of the symbols in the Illumos nvpair library conflict with symbols
 * provided by nv(9), so we use this preprocessor hack to avoid the conflict.
 *
 * This list was generated by:
 *   cat nv.h nv_impl.h nvlist_* nvpair_impl.h | \
 *     sed -nE 's/^[[:alnum:]_][[:alnum:]_ ]*[[:space:]]+[*]*([[:alnum:]_]+)\(.*$/#define \1 illumos_\1/p' | \
 *     sort -u
 */
#define nvlist_add_binary illumos_nvlist_add_binary
#define nvlist_add_bool illumos_nvlist_add_bool
#define nvlist_add_descriptor illumos_nvlist_add_descriptor
#define nvlist_add_null illumos_nvlist_add_null
#define nvlist_add_number illumos_nvlist_add_number
#define nvlist_add_nvlist illumos_nvlist_add_nvlist
#define nvlist_add_nvpair illumos_nvlist_add_nvpair
#define nvlist_add_string illumos_nvlist_add_string
#define nvlist_add_stringf illumos_nvlist_add_stringf
#define nvlist_add_stringv illumos_nvlist_add_stringv
#define nvlist_addf_binary illumos_nvlist_addf_binary
#define nvlist_addf_bool illumos_nvlist_addf_bool
#define nvlist_addf_descriptor illumos_nvlist_addf_descriptor
#define nvlist_addf_null illumos_nvlist_addf_null
#define nvlist_addf_number illumos_nvlist_addf_number
#define nvlist_addf_nvlist illumos_nvlist_addf_nvlist
#define nvlist_addf_string illumos_nvlist_addf_string
#define nvlist_addv_binary illumos_nvlist_addv_binary
#define nvlist_addv_bool illumos_nvlist_addv_bool
#define nvlist_addv_descriptor illumos_nvlist_addv_descriptor
#define nvlist_addv_null illumos_nvlist_addv_null
#define nvlist_addv_number illumos_nvlist_addv_number
#define nvlist_addv_nvlist illumos_nvlist_addv_nvlist
#define nvlist_addv_string illumos_nvlist_addv_string
#define nvlist_check_header illumos_nvlist_check_header
#define nvlist_clone illumos_nvlist_clone
#define nvlist_create illumos_nvlist_create
#define nvlist_descriptors illumos_nvlist_descriptors
#define nvlist_destroy illumos_nvlist_destroy
#define nvlist_dump illumos_nvlist_dump
#define nvlist_empty illumos_nvlist_empty
#define nvlist_error illumos_nvlist_error
#define nvlist_exists illumos_nvlist_exists
#define nvlist_exists_binary illumos_nvlist_exists_binary
#define nvlist_exists_bool illumos_nvlist_exists_bool
#define nvlist_exists_descriptor illumos_nvlist_exists_descriptor
#define nvlist_exists_null illumos_nvlist_exists_null
#define nvlist_exists_number illumos_nvlist_exists_number
#define nvlist_exists_nvlist illumos_nvlist_exists_nvlist
#define nvlist_exists_string illumos_nvlist_exists_string
#define nvlist_exists_type illumos_nvlist_exists_type
#define nvlist_existsf illumos_nvlist_existsf
#define nvlist_existsf_binary illumos_nvlist_existsf_binary
#define nvlist_existsf_bool illumos_nvlist_existsf_bool
#define nvlist_existsf_descriptor illumos_nvlist_existsf_descriptor
#define nvlist_existsf_null illumos_nvlist_existsf_null
#define nvlist_existsf_number illumos_nvlist_existsf_number
#define nvlist_existsf_nvlist illumos_nvlist_existsf_nvlist
#define nvlist_existsf_string illumos_nvlist_existsf_string
#define nvlist_existsf_type illumos_nvlist_existsf_type
#define nvlist_existsv illumos_nvlist_existsv
#define nvlist_existsv_binary illumos_nvlist_existsv_binary
#define nvlist_existsv_bool illumos_nvlist_existsv_bool
#define nvlist_existsv_descriptor illumos_nvlist_existsv_descriptor
#define nvlist_existsv_null illumos_nvlist_existsv_null
#define nvlist_existsv_number illumos_nvlist_existsv_number
#define nvlist_existsv_nvlist illumos_nvlist_existsv_nvlist
#define nvlist_existsv_string illumos_nvlist_existsv_string
#define nvlist_existsv_type illumos_nvlist_existsv_type
#define nvlist_fdump illumos_nvlist_fdump
#define nvlist_first_nvpair illumos_nvlist_first_nvpair
#define nvlist_free illumos_nvlist_free
#define nvlist_free_binary illumos_nvlist_free_binary
#define nvlist_free_bool illumos_nvlist_free_bool
#define nvlist_free_descriptor illumos_nvlist_free_descriptor
#define nvlist_free_null illumos_nvlist_free_null
#define nvlist_free_number illumos_nvlist_free_number
#define nvlist_free_nvlist illumos_nvlist_free_nvlist
#define nvlist_free_nvpair illumos_nvlist_free_nvpair
#define nvlist_free_string illumos_nvlist_free_string
#define nvlist_free_type illumos_nvlist_free_type
#define nvlist_freef illumos_nvlist_freef
#define nvlist_freef_binary illumos_nvlist_freef_binary
#define nvlist_freef_bool illumos_nvlist_freef_bool
#define nvlist_freef_descriptor illumos_nvlist_freef_descriptor
#define nvlist_freef_null illumos_nvlist_freef_null
#define nvlist_freef_number illumos_nvlist_freef_number
#define nvlist_freef_nvlist illumos_nvlist_freef_nvlist
#define nvlist_freef_string illumos_nvlist_freef_string
#define nvlist_freef_type illumos_nvlist_freef_type
#define nvlist_freev illumos_nvlist_freev
#define nvlist_freev_binary illumos_nvlist_freev_binary
#define nvlist_freev_bool illumos_nvlist_freev_bool
#define nvlist_freev_descriptor illumos_nvlist_freev_descriptor
#define nvlist_freev_null illumos_nvlist_freev_null
#define nvlist_freev_number illumos_nvlist_freev_number
#define nvlist_freev_nvlist illumos_nvlist_freev_nvlist
#define nvlist_freev_string illumos_nvlist_freev_string
#define nvlist_freev_type illumos_nvlist_freev_type
#define nvlist_get_binary illumos_nvlist_get_binary
#define nvlist_get_bool illumos_nvlist_get_bool
#define nvlist_get_descriptor illumos_nvlist_get_descriptor
#define nvlist_get_number illumos_nvlist_get_number
#define nvlist_get_nvlist illumos_nvlist_get_nvlist
#define nvlist_get_nvpair illumos_nvlist_get_nvpair
#define nvlist_get_string illumos_nvlist_get_string
#define nvlist_getf_binary illumos_nvlist_getf_binary
#define nvlist_getf_bool illumos_nvlist_getf_bool
#define nvlist_getf_descriptor illumos_nvlist_getf_descriptor
#define nvlist_getf_number illumos_nvlist_getf_number
#define nvlist_getf_nvlist illumos_nvlist_getf_nvlist
#define nvlist_getf_string illumos_nvlist_getf_string
#define nvlist_getv_binary illumos_nvlist_getv_binary
#define nvlist_getv_bool illumos_nvlist_getv_bool
#define nvlist_getv_descriptor illumos_nvlist_getv_descriptor
#define nvlist_getv_number illumos_nvlist_getv_number
#define nvlist_getv_nvlist illumos_nvlist_getv_nvlist
#define nvlist_getv_string illumos_nvlist_getv_string
#define nvlist_move_binary illumos_nvlist_move_binary
#define nvlist_move_descriptor illumos_nvlist_move_descriptor
#define nvlist_move_nvlist illumos_nvlist_move_nvlist
#define nvlist_move_nvpair illumos_nvlist_move_nvpair
#define nvlist_move_string illumos_nvlist_move_string
#define nvlist_movef_binary illumos_nvlist_movef_binary
#define nvlist_movef_descriptor illumos_nvlist_movef_descriptor
#define nvlist_movef_nvlist illumos_nvlist_movef_nvlist
#define nvlist_movef_string illumos_nvlist_movef_string
#define nvlist_movev_binary illumos_nvlist_movev_binary
#define nvlist_movev_descriptor illumos_nvlist_movev_descriptor
#define nvlist_movev_nvlist illumos_nvlist_movev_nvlist
#define nvlist_movev_string illumos_nvlist_movev_string
#define nvlist_ndescriptors illumos_nvlist_ndescriptors
#define nvlist_next illumos_nvlist_next
#define nvlist_next_nvpair illumos_nvlist_next_nvpair
#define nvlist_pack illumos_nvlist_pack
#define nvlist_prev_nvpair illumos_nvlist_prev_nvpair
#define nvlist_recv illumos_nvlist_recv
#define nvlist_remove_nvpair illumos_nvlist_remove_nvpair
#define nvlist_report_missing illumos_nvlist_report_missing
#define nvlist_send illumos_nvlist_send
#define nvlist_set_error illumos_nvlist_set_error
#define nvlist_size illumos_nvlist_size
#define nvlist_take_binary illumos_nvlist_take_binary
#define nvlist_take_bool illumos_nvlist_take_bool
#define nvlist_take_descriptor illumos_nvlist_take_descriptor
#define nvlist_take_number illumos_nvlist_take_number
#define nvlist_take_nvlist illumos_nvlist_take_nvlist
#define nvlist_take_nvpair illumos_nvlist_take_nvpair
#define nvlist_take_string illumos_nvlist_take_string
#define nvlist_takef_binary illumos_nvlist_takef_binary
#define nvlist_takef_bool illumos_nvlist_takef_bool
#define nvlist_takef_descriptor illumos_nvlist_takef_descriptor
#define nvlist_takef_number illumos_nvlist_takef_number
#define nvlist_takef_nvlist illumos_nvlist_takef_nvlist
#define nvlist_takef_string illumos_nvlist_takef_string
#define nvlist_takev_binary illumos_nvlist_takev_binary
#define nvlist_takev_bool illumos_nvlist_takev_bool
#define nvlist_takev_descriptor illumos_nvlist_takev_descriptor
#define nvlist_takev_number illumos_nvlist_takev_number
#define nvlist_takev_nvlist illumos_nvlist_takev_nvlist
#define nvlist_takev_string illumos_nvlist_takev_string
#define nvlist_unpack illumos_nvlist_unpack
#define nvlist_xfer illumos_nvlist_xfer
#define nvlist_xpack illumos_nvlist_xpack
#define nvlist_xunpack illumos_nvlist_xunpack
#define nvpair_allocv illumos_nvpair_allocv
#define nvpair_assert illumos_nvpair_assert
#define nvpair_clone illumos_nvpair_clone
#define nvpair_create_binary illumos_nvpair_create_binary
#define nvpair_create_bool illumos_nvpair_create_bool
#define nvpair_create_descriptor illumos_nvpair_create_descriptor
#define nvpair_create_null illumos_nvpair_create_null
#define nvpair_create_number illumos_nvpair_create_number
#define nvpair_create_nvlist illumos_nvpair_create_nvlist
#define nvpair_create_string illumos_nvpair_create_string
#define nvpair_create_stringf illumos_nvpair_create_stringf
#define nvpair_create_stringv illumos_nvpair_create_stringv
#define nvpair_createf_binary illumos_nvpair_createf_binary
#define nvpair_createf_bool illumos_nvpair_createf_bool
#define nvpair_createf_descriptor illumos_nvpair_createf_descriptor
#define nvpair_createf_null illumos_nvpair_createf_null
#define nvpair_createf_number illumos_nvpair_createf_number
#define nvpair_createf_nvlist illumos_nvpair_createf_nvlist
#define nvpair_createf_string illumos_nvpair_createf_string
#define nvpair_createv_binary illumos_nvpair_createv_binary
#define nvpair_createv_bool illumos_nvpair_createv_bool
#define nvpair_createv_descriptor illumos_nvpair_createv_descriptor
#define nvpair_createv_null illumos_nvpair_createv_null
#define nvpair_createv_number illumos_nvpair_createv_number
#define nvpair_createv_nvlist illumos_nvpair_createv_nvlist
#define nvpair_createv_string illumos_nvpair_createv_string
#define nvpair_free illumos_nvpair_free
#define nvpair_free_structure illumos_nvpair_free_structure
#define nvpair_get_binary illumos_nvpair_get_binary
#define nvpair_get_bool illumos_nvpair_get_bool
#define nvpair_get_descriptor illumos_nvpair_get_descriptor
#define nvpair_get_number illumos_nvpair_get_number
#define nvpair_get_nvlist illumos_nvpair_get_nvlist
#define nvpair_get_string illumos_nvpair_get_string
#define nvpair_header_size illumos_nvpair_header_size
#define nvpair_insert illumos_nvpair_insert
#define nvpair_move_binary illumos_nvpair_move_binary
#define nvpair_move_descriptor illumos_nvpair_move_descriptor
#define nvpair_move_nvlist illumos_nvpair_move_nvlist
#define nvpair_move_string illumos_nvpair_move_string
#define nvpair_movef_binary illumos_nvpair_movef_binary
#define nvpair_movef_descriptor illumos_nvpair_movef_descriptor
#define nvpair_movef_nvlist illumos_nvpair_movef_nvlist
#define nvpair_movef_string illumos_nvpair_movef_string
#define nvpair_movev_binary illumos_nvpair_movev_binary
#define nvpair_movev_descriptor illumos_nvpair_movev_descriptor
#define nvpair_movev_nvlist illumos_nvpair_movev_nvlist
#define nvpair_movev_string illumos_nvpair_movev_string
#define nvpair_name illumos_nvpair_name
#define nvpair_next illumos_nvpair_next
#define nvpair_nvlist illumos_nvpair_nvlist
#define nvpair_pack illumos_nvpair_pack
#define nvpair_pack_descriptor illumos_nvpair_pack_descriptor
#define nvpair_prev illumos_nvpair_prev
#define nvpair_remove illumos_nvpair_remove
#define nvpair_size illumos_nvpair_size
#define nvpair_type illumos_nvpair_type
#define nvpair_type_string illumos_nvpair_type_string
#define nvpair_unpack illumos_nvpair_unpack
#define nvpair_unpack_descriptor illumos_nvpair_unpack_descriptor

#include_next <sys/nvpair.h>

#endif
