#! /bin/bash
OLDCWD=`pwd`
SCRIPT_PATH=`dirname $0`

CHECK_DIR=../../src

SORT_OPT="-u -c"

# $1: filename
function checkarray
{
    CHECK_FILE="$1"
    START="\\/\\* BEGIN DECL SORTED_CODEOPT.SH \\*\\/"
    END="\\/\\* END DECL SORTED_CODEOPT.SH \\*\\/"

    awk '/'"$START"'/{flag=1; count++; next} /'"$END"'/{flag=0;} flag {printf("%04d##%s\n", count, $0)}' "$CHECK_FILE" | \
        sed -e 's:\(.*##\).*"\(.*\)",.*:\1\2:g' > .a.tmp

    if [[ -z $(grep '[^[:space:]]' .a.tmp) ]] ; then
        echo "error: "$1" table is empty"
        rm -rf .a.tmp
        exit -1
    fi

    if `LC_COLLATE=C sort $SORT_OPT .a.tmp`; then
        echo ""$1" decls sorted."
    else
        echo "error: "$1" decls are not sorted."
        rm -rf .a.tmp
        exit -1
    fi

    START="\\/\\* BEGIN SORTED_CODEOPT.SH \\*\\/"
    END="\\/\\* END SORTED_CODEOPT.SH \\*\\/"

    awk '/'"$START"'/{flag=1; count++; next} /'"$END"'/{flag=0;} flag {printf("%04d##%s\n", count, $0)}' "$CHECK_FILE" | \
        sed -e 's:\(.*##\).*&D\(.*\),.*:\1\2:g' > .b.tmp

    if [[ -z $(grep '[^[:space:]]' .b.tmp) ]] ; then
        echo "error: "$1" table is empty"
        rm -rf .a.tmp .b.tmp
        exit -1
    fi

    if `LC_COLLATE=C sort $SORT_OPT .b.tmp`; then
        echo ""$1" tables sorted."
    else
        echo "error: "$1" tables are not sorted."
        rm -rf .a.tmp .b.tmp
        exit -1
    fi

    if cmp --silent -- .a.tmp .b.tmp; then
        echo ""$1" tables OK"
    else
        echo "error: "$1" tables are different."
        diff -y .a.tmp .b.tmp
        rm -rf .a.tmp .b.tmp
        exit -1
    fi

    rm -rf .a.tmp .b.tmp
}


for N in `grep -rl "BEGIN DECL SORTED_CODEOPT.SH" "$CHECK_DIR"`; do
    checkarray $N
done
