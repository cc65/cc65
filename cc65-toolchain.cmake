# from https://github.com/fo-fo/ngin
# CMake toolchain file for cc65
# This is largely a result of experimentation, so some things may be done
# suboptimally/wrong. Some compilation options/CMake features may not work.
# Some generators will also not work (like MSVS). Ninja has been tested to work.
# What is supported: C, assembly, static libraries

set( CMAKE_SYSTEM_NAME Generic )

macro( __compilerCc65 lang )
    set( CMAKE_${lang}_COMPILER cl65 CACHE PATH "${lang} compiler" )
    set( CMAKE_${lang}_COMPILER_ID cc65 )

    # We cannot run tests for the cc65 compiler, because of cross-compilation,
    # so force the compiler tests to passed.
    set( CMAKE_${lang}_COMPILER_ID_RUN TRUE )
    # Don't know if these are necessary.
    set( CMAKE_${lang}_COMPILER_ID_WORKS TRUE )
    set( CMAKE_${lang}_COMPILER_ID_FORCED TRUE )

    set( CMAKE_DEPFILE_FLAGS_${lang} "--create-dep <DEPFILE>")
    set( CMAKE_${lang}_VERBOSE_FLAG "-v" )
    set( CMAKE_${lang}_FLAGS_DEBUG_INIT "-g -D DEBUG --asm-define DEBUG"  )
endmacro()

__compilerCc65( C )
__compilerCc65( ASM )

set( CMAKE_ASM_SOURCE_FILE_EXTENSIONS s;S;asm )

# Not sure why CMake by default looks for the compilers, but not the archiver.
# Force it to try to find the archiver.
find_program( CMAKE_AR ar65 )

# \note Need to delete the old file first because ar65 can only add files
#       into an archive (or remove named files, but we don't know the names).
set( CMAKE_C_CREATE_STATIC_LIBRARY
    "<CMAKE_COMMAND> -E remove <TARGET> "
    "<CMAKE_AR> a <TARGET> <LINK_FLAGS> <OBJECTS>"
)
set( CMAKE_ASM_CREATE_STATIC_LIBRARY ${CMAKE_C_CREATE_STATIC_LIBRARY} )
