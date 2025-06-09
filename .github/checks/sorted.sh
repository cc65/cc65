#! /bin/bash
OLDCWD=`pwd`
SCRIPT_PATH=`dirname $0`

SORT_OPT=-u

function checkarray_quoted_name
{
    CHECK_FILE="$1"

    awk '/'"$2"'/{flag=1;next}/};/{flag=0}flag' "$CHECK_FILE" | \
        sed -e 's:.*\"\(.*\)\".*:\1:g' | \
        sed '/^\s*$/d' > .a.tmp

    if [[ -z $(grep '[^[:space:]]' .a.tmp) ]] ; then
        echo "error: "$2" table is empty"
        exit -1
    fi

    LC_COLLATE=C sort $SORT_OPT .a.tmp > .b.tmp

    if cmp --silent -- .a.tmp .b.tmp; then
        echo ""$2" definitions OK"
    else
        echo "error: "$2" definitions are not sorted."
        diff -y .a.tmp .b.tmp
        exit -1
    fi
    rm -rf .a.tmp .b.tmp
}

function checkinstr_quoted_name
{
    CHECK_FILE="$1"

    awk '/'"$2"'/{flag=1;next}/};/{flag=0}flag' "$CHECK_FILE" | \
        sed -e 's:^ *{$::g' | \
        sed -e 's:^ *}$::g' | \
        sed -e 's:.*\"\(.*\)\".*:\1:g' | \
        sed '/^\s*$/d' > .a.tmp

    if [[ -z $(grep '[^[:space:]]' .a.tmp) ]] ; then
        echo "error: "$2" table is empty"
        exit -1
    fi

    LC_COLLATE=C sort $SORT_OPT .a.tmp > .b.tmp

    if cmp --silent -- .a.tmp .b.tmp; then
        echo ""$2" definitions OK"
    else
        echo "error: "$2" definitions are not sorted."
        diff -y .a.tmp .b.tmp
        exit -1
    fi
    rm -rf .a.tmp .b.tmp
}

function checkopcodes_quoted_name
{
    CHECK_FILE="$1"

    awk '/'"$2"'/{flag=1;next}/};/{flag=0}flag' "$CHECK_FILE" | \
        grep "^ *\".*\"," | \
        sed '/^\s*$/d' > .a.tmp

    if [[ -z $(grep '[^[:space:]]' .a.tmp) ]] ; then
        echo "error: "$2" table is empty"
        exit -1
    fi

    LC_COLLATE=C sort $SORT_OPT .a.tmp > .b.tmp

    if cmp --silent -- .a.tmp .b.tmp; then
        echo ""$2" definitions OK"
    else
        echo "error: "$2" definitions are not sorted."
        diff -y .a.tmp .b.tmp
        exit -1
    fi
    rm -rf .a.tmp .b.tmp
}

checkinstr_quoted_name ../../src/ca65/instr.c "sizeof \(InsTab6502\.Ins\) \/ sizeof \(InsTab6502\.Ins\[0\]\),"
checkinstr_quoted_name ../../src/ca65/instr.c "sizeof \(InsTab6502X\.Ins\) \/ sizeof \(InsTab6502X\.Ins\[0\]\),"
checkinstr_quoted_name ../../src/ca65/instr.c "sizeof \(InsTab6502DTV\.Ins\) \/ sizeof \(InsTab6502DTV\.Ins\[0\]\),"
checkinstr_quoted_name ../../src/ca65/instr.c "sizeof \(InsTab65C02\.Ins\) \/ sizeof \(InsTab65C02\.Ins\[0\]\),"
checkinstr_quoted_name ../../src/ca65/instr.c "sizeof \(InsTab4510\.Ins\) \/ sizeof \(InsTab4510\.Ins\[0\]\),"
checkinstr_quoted_name ../../src/ca65/instr.c "sizeof \(InsTab65816\.Ins\) \/ sizeof \(InsTab65816\.Ins\[0\]\),"
checkinstr_quoted_name ../../src/ca65/instr.c "sizeof \(InsTabSweet16\.Ins\) \/ sizeof \(InsTabSweet16\.Ins\[0\]\),"
checkinstr_quoted_name ../../src/ca65/instr.c "sizeof \(InsTabHuC6280\.Ins\) \/ sizeof \(InsTabHuC6280\.Ins\[0\]\),"

checkarray_quoted_name ../../src/ca65/scanner.c "} DotKeywords \[\] = {"

checkarray_quoted_name ../../src/cc65/codeinfo.c "static const FuncInfo FuncInfoTable\[\]"
checkarray_quoted_name ../../src/cc65/codeinfo.c "static const ZPInfo ZPInfoTable\[\]"
checkarray_quoted_name ../../src/cc65/codeoptutil.c "static const char\* const Tab\[\]"

checkarray_quoted_name ../../src/cc65/coptstop.c "static const OptFuncDesc FuncTable\[\]"
checkarray_quoted_name ../../src/cc65/coptstop.c "static const OptFuncDesc FuncRegATable\[\]"

checkopcodes_quoted_name ../../src/cc65/opcodes.c "const OPCDesc OPCTable\[OP65_COUNT\] = {"
checkarray_quoted_name ../../src/cc65/pragma.c "} Pragmas\[\] = {"
checkarray_quoted_name ../../src/cc65/preproc.c "} PPDTypes\[\] = {"
checkarray_quoted_name ../../src/cc65/scanner.c "} Keywords \[\] = {"
checkarray_quoted_name ../../src/cc65/stdfunc.c "} StdFuncs\[\] = {"

checkarray_quoted_name ../../src/common/filetype.c "static const FileId TypeTable\[\]"
checkarray_quoted_name ../../src/common/target.c "static const TargetEntry TargetMap\[\]"

checkarray_quoted_name ../../src/dbginfo/dbginfo.c "} KeywordTable\[\] = {"

checkarray_quoted_name ../../src/sp65/convert.c "static const ConverterMapEntry ConverterMap\[\]"
checkarray_quoted_name ../../src/sp65/input.c "static const FileId FormatTable\[\]"
checkarray_quoted_name ../../src/sp65/output.c "static const FileId FormatTable\[\]"
checkarray_quoted_name ../../src/sp65/palconv.c "static const PaletteMapEntry PaletteMap\[\]"

