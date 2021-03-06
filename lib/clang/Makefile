# $FreeBSD: head/lib/clang/Makefile 280031 2015-03-15 13:31:13Z dim $

.include <src.opts.mk>

.if !make(install)
.if ${MK_CLANG_FULL} != "no"
_libclangstaticanalyzer= \
	libclangstaticanalyzercheckers \
	libclangstaticanalyzercore \
	libclangstaticanalyzerfrontend
_libclangarcmigrate= \
	libclangarcmigrate
.endif # MK_CLANG_FULL

SUBDIR=	libclanganalysis \
	${_libclangarcmigrate} \
	libclangast \
	libclangbasic \
	libclangcodegen \
	libclangdriver \
	libclangedit \
	libclangfrontend \
	libclangfrontendtool \
	libclanglex \
	libclangparse \
	libclangrewritefrontend \
	libclangrewrite \
	libclangsema \
	libclangserialization \
	${_libclangstaticanalyzer} \
	\
	libllvmanalysis \
	libllvmasmparser \
	libllvmasmprinter \
	libllvmbitreader \
	libllvmbitwriter \
	libllvmcodegen \
	libllvmcore \
	libllvminstcombine \
	libllvminstrumentation \
	libllvmipa \
	libllvmipo \
	libllvmirreader \
	libllvmlinker \
	libllvmmc \
	libllvmmcdisassembler \
	libllvmmcparser \
	libllvmobjcarcopts \
	libllvmobject \
	libllvmoption \
	libllvmprofiledata \
	libllvmscalaropts \
	libllvmselectiondag \
	libllvmsupport \
	libllvmtablegen \
	libllvmtarget \
	libllvmtransformutils \
	libllvmvectorize \
	\
	libllvmaarch64asmparser \
	libllvmaarch64codegen \
	libllvmaarch64desc \
	libllvmaarch64disassembler \
	libllvmaarch64info \
	libllvmaarch64instprinter \
	libllvmaarch64utils \
	libllvmarmasmparser \
	libllvmarmcodegen \
	libllvmarmdesc \
	libllvmarmdisassembler \
	libllvmarminfo \
	libllvmarminstprinter \
	libllvmmipsasmparser \
	libllvmmipscodegen \
	libllvmmipsdesc \
	libllvmmipsdisassembler \
	libllvmmipsinfo \
	libllvmmipsinstprinter \
	libllvmpowerpcasmparser \
	libllvmpowerpccodegen \
	libllvmpowerpcdesc \
	libllvmpowerpcdisassembler \
	libllvmpowerpcinfo \
	libllvmpowerpcinstprinter \
	libllvmsparcasmparser \
	libllvmsparccodegen \
	libllvmsparcdesc \
	libllvmsparcdisassembler \
	libllvmsparcinfo \
	libllvmsparcinstprinter \
	libllvmx86asmparser \
	libllvmx86codegen \
	libllvmx86desc \
	libllvmx86disassembler \
	libllvmx86info \
	libllvmx86instprinter \
	libllvmx86utils

.if ${MK_CLANG_EXTRAS} != "no"
SUBDIR+=libllvmdebuginfo
.endif # MK_CLANG_EXTRAS
.if ${MK_CLANG_EXTRAS} != "no" || ${MK_LLDB} != "no"
SUBDIR+=libllvmexecutionengine \
	libllvminterpreter \
	libllvmmcjit \
	libllvmruntimedyld
.endif # MK_CLANG_EXTRAS | LLDB

.if ${MK_LLDB} != "no"
SUBDIR+=liblldb \
	\
	liblldbAPI \
	liblldbBreakpoint \
	liblldbCommands \
	liblldbCore \
	liblldbDataFormatters \
	liblldbExpression \
	liblldbHostCommon \
	liblldbHostFreeBSD \
	liblldbHostPOSIX \
	liblldbInterpreter \
	liblldbSymbol \
	liblldbTarget \
	liblldbUtility \
	\
	liblldbPluginABISysV_ppc \
	liblldbPluginABISysV_ppc64 \
	liblldbPluginABISysV_x86_64 \
	liblldbPluginCXXItaniumABI \
	liblldbPluginDisassemblerLLVM \
	liblldbPluginDynamicLoaderStatic \
	liblldbPluginDynamicLoaderPosixDYLD \
	liblldbPluginInstructionARM \
	liblldbPluginInstructionARM64 \
	liblldbPluginInstrumentationRuntimeAddressSanitizer \
	liblldbPluginJITLoaderGDB \
	liblldbPluginMemoryHistoryASan \
	liblldbPluginObjectContainerBSDArchive \
	liblldbPluginObjectFileELF \
	liblldbPluginObjectFileJIT \
	liblldbPluginPlatformFreeBSD \
	liblldbPluginPlatformGDB \
	liblldbPluginProcessElfCore \
	liblldbPluginProcessFreeBSD \
	liblldbPluginProcessGDBRemote \
	liblldbPluginProcessPOSIX \
	liblldbPluginProcessUtility \
	liblldbPluginSymbolFileDWARF \
	liblldbPluginSymbolFileSymtab \
	liblldbPluginSymbolVendorELF \
	liblldbPluginUnwindAssemblyInstEmulation \
	liblldbPluginUnwindAssemblyX86
.endif # MK_LLDB

.endif # !make(install)

SUBDIR+= include

SUBDIR_PARALLEL=

.include <bsd.subdir.mk>
