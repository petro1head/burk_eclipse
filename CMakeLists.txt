cmake_minimum_required(VERSION 3.2)

project(burkProject C ASM)

#set(CMAKE_SYSTEM_NAME Generic)

message("CMAKE_C_COMPILER="${CMAKE_C_COMPILER})
message("CMAKE_C_IMPLICIT_LINK_LIBRARIES="${CMAKE_C_IMPLICIT_LINK_LIBRARIES})

message("PROJECT_SOURCE_DIR=" ${PROJECT_SOURCE_DIR})
message("CMAKE_LINKER=" ${CMAKE_LINKER})
message("CMAKE_OBJDUMP=" ${CMAKE_OBJDUMP})


#SET(PROGRAM_ADDR 0x80200000)#starterkit with cache
#SET(PROGRAM_ADDR 0xbfc40000) #VME cs0 chachable
#SET(PROGRAM_ADDR 0xa0000800) #VME cs0 chachable for mcp

#SET(PROGRAM_ADDR 0xBFC00000) #uncacheble CS0 start from 0 address for mpvv1


#SET(PROGRAM_STACK  0xBFC60000 ) #CS0 MRAM ROM0 end for mpvv1
SET(PROGRAM_ADDR 0xa0200000)#starterkit no cache
#SET(PROGRAM_ADDR 0xbfc40000) #VME cs0 no chachable
SET(PROGRAM_STACK  ${PROGRAM_ADDR} )
#SET(PROGRAM_STACK  0xa01ffff0) #for mcp


include_directories(LibNiisi/include)
include_directories(startup/include)
include_directories(LibVme/include)
include_directories(LibVme/include/Mcp)

SET(CMAKE_C_FLAGS "-std=c99 -DVM014 -Wall ")
SET(CMAKE_ASM_FLAGS "-g3 -W -Wall  -pedantic-errors -W -x assembler-with-cpp -fno-pic -D_MIPS_ARCH_MIPS1  -DPROGRAM_STACK=${PROGRAM_STACK}")  
SET(CMAKE_EXE_LINKER_FLAGS "-g3 -O2 -W -Wall -fno-pic -nostartfiles -fno-builtin -fno-exceptions -D_MIPS_ARCH_MIPS1 -Ttext=${PROGRAM_ADDR} -T ../burk/src/newlib_mips_mod.ld")

add_executable(
  burk

  startup/startup_auto.S
  startup/io.S
  startup/flushcache.S
 # LibNiisi/devices/sys_unit.c
 # LibNiisi/devices/gpio.c
  LibNiisi/devices/uart.c
 # LibNiisi/devices/mkio.c
 # LibNiisi/devices/timer.c
 # LibNiisi/devices/spi.c
  src/main.c
  src/io.c
  src/burk_function.c
# src/vmeInit.c
#  LibNiisi/devices/spw_lib.c
#  libVme/src/Mcp/mcpMemoryController.c
#  libVme/src/Mcp/mcpSpw.c
#  libVme/src/bistTest.c
  

)


add_custom_target(
  binary
  ALL
  DEPENDS burk
  COMMAND
  ${CMAKE_OBJCOPY} --strip-all -R .bss -O binary ${PROJECT_SOURCE_DIR}/../burkProject/bin/burk ${PROJECT_SOURCE_DIR}/../burkProject/bin/burk.bin
  COMMAND
  ${CMAKE_OBJDUMP} -dS ${PROJECT_SOURCE_DIR}/../burkProject/bin/burk > ${PROJECT_SOURCE_DIR}/../burkProject/bin/burk.dis
  COMMAND
  ${CMAKE_OBJDUMP} -h ${PROJECT_SOURCE_DIR}/../burkProject/bin/burk > ${PROJECT_SOURCE_DIR}/../burkProject/bin/burk.hdr
  COMMAND
  ${CMAKE_OBJDUMP} -t ${PROJECT_SOURCE_DIR}/../burkProject/bin/burk > ${PROJECT_SOURCE_DIR}/../burkProject/bin/burk.sym
#  COMMAND
#  d:/Soft/Net/PuTTY/psftp -pw 1 -b sftpput dmitry@172.20.54.221
)

#
set_target_properties(
  burk 
  PROPERTIES 
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../burkProject/bin
)
