#!/bin/sh
#
# Copyright 2014 EMC Corp.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#
# * Redistributions of source code must retain the above copyright
#   notice, this list of conditions and the following disclaimer.
# * Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimer in the
#   documentation and/or other materials provided with the distribution.
# * Neither the name of Google Inc. nor the names of its contributors
#   may be used to endorse or promote products derived from this software
#   without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# $FreeBSD: head/tests/sys/file/flock_test.sh 281432 2015-04-11 10:14:59Z ngie $

# Testcase # 11 is racy; uses an undocumented kernel interface for testing
#            locking
# Testcase # 16 is racy/doesn't handle EINTR properly
last_testcase=16

echo "1..$last_testcase"

for n in `seq 1 $last_testcase`; do
	todomsg=""

	if [ $n -eq 11 ]; then
		todomsg=" # TODO: racy testcase"
	# Test 16 fails:
	# F_SETLKW on locked region by two threads: FAIL ((uintptr_t)res != 0)
	elif [ $n -eq 16 ]; then
		todomsg=" # TODO: racy testcase (doesn't handle EINTR properly)"
	fi

	$(dirname $0)/flock_helper . $n | grep -q SUCCEED
	if [ $? -eq 0 ]; then
		echo "ok $n$todomsg"
	else
		echo "not ok $n$todomsg"
	fi
done
