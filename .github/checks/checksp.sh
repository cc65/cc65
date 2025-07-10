#! /bin/bash
OD65_EXE=../bin/od65
CHECK_PATH=../../libwrk

cd "${CHECK_PATH}" || {
    echo "error: Directory ${CHECK_PATH} doesn't seem to exist" >&2
    exit 1
}

[ -x "${OD65_EXE}" ] || {
    echo "error: This check requires the od65 executable to be built" >&2
    exit 1
}

EXITCODE=0
find . -name \*.o -print | while read OBJ; do
    "${OD65_EXE}" --dump-imports "${OBJ}" | grep -q "\"sp\"" && {
        echo "error: Usage of symbol 'sp' found in module ${OBJ}" >&2
        EXITCODE=1
    }
done
exit ${EXITCODE}
