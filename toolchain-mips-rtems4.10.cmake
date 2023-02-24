# this one is important
SET(CMAKE_SYSTEM_NAME Generic)
# this one not so much
SET(CMAKE_SYSTEM_VERSION 1)

SET(CMAKE_FIND_ROOT_PATH  c:/Compilers/mingw+msys+mips-rtems4.10/opt/mips-rtems4.10/bin)

SET(CMAKE_MIPS_PREFIX     mips-rtems4.10)

# specify the cross compiler
SET(CMAKE_ASM_COMPILER    ${CMAKE_FIND_ROOT_PATH}/${CMAKE_MIPS_PREFIX}-gcc.exe)
SET(CMAKE_C_COMPILER      ${CMAKE_FIND_ROOT_PATH}/${CMAKE_MIPS_PREFIX}-gcc.exe)
SET(CMAKE_SIZE            ${CMAKE_FIND_ROOT_PATH}/${CMAKE_MIPS_PREFIX}-size.exe)

# set the linker - mind the CACHE ... FORCE, since we need to overwrite existing cache variable
SET(CMAKE_LINKER          ${CMAKE_FIND_ROOT_PATH}/${CMAKE_MIPS_PREFIX}-gcc.exe CACHE FILEPATH "" FORCE)
SET(CMAKE_OBJCOPY         ${CMAKE_FIND_ROOT_PATH}/${CMAKE_MIPS_PREFIX}-objcopy.exe CACHE FILEPATH "" FORCE)
SET(CMAKE_OBJDUMP         ${CMAKE_FIND_ROOT_PATH}/${CMAKE_MIPS_PREFIX}-objdump.exe CACHE FILEPATH "" FORCE)