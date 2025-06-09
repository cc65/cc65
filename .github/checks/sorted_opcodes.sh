#! /bin/bash
OLDCWD=`pwd`
SCRIPT_PATH=`dirname $0`

CHECK_DIR=../../src

SORT_OPT=-u

function checkarray_quoted_name
{
    CHECK_FILE="$1"
    START="\\/\\* BEGIN SORTED_OPCODES.SH \\*\\/"
    END="\\/\\* END SORTED_OPCODES.SH \\*\\/"

    awk '/'"$START"'/{flag=1; count++; next} /'"$END"'/{flag=0;} flag {print count,"##",$0}' "$CHECK_FILE" | \
        sed 's:/\*.*::g' | \
        grep '".*",' | \
        sed -e 's:\(.*\) ##.*\"\(.*\)\".*:\1##\2:g' > .a.tmp

    if [[ -z $(grep '[^[:space:]]' .a.tmp) ]] ; then
        echo "error: "$1" table is empty"
        rm -rf .a.tmp
        exit -1
    fi

    LC_COLLATE=C sort $SORT_OPT .a.tmp > .b.tmp

    if cmp --silent -- .a.tmp .b.tmp; then
        echo ""$1" tables OK"
    else
        echo "error: "$1" tables are not sorted."
        diff -y .a.tmp .b.tmp
        rm -rf .a.tmp .b.tmp
        exit -1
    fi
    rm -rf .a.tmp .b.tmp
}

for N in `grep -rl "BEGIN SORTED_OPCODES.SH" "$CHECK_DIR"`; do
    checkarray_quoted_name $N
done

