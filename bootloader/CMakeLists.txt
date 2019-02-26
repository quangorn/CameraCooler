cmake_minimum_required(VERSION 3.2)
project(UsbAspLoader C ASM)

SET(MCU "atmega328p")
SET(F_CPU "12000000")
SET(BOOTLOADER_ADDRESS "0x7000")

#-fpack-struct option is mandatory for compiler (pragma pack analog)
SET(CMAKE_C_FLAGS "-mmcu=${MCU} -DF_CPU=${F_CPU} -DBOOTLOADER_ADDRESS=${BOOTLOADER_ADDRESS} -DNDEBUG -DCONFIG_NO__CHIP_ERASE -DCONFIG_NO__ONDEMAND_PAGEERASE -DCONFIG_NO__PRESERVE_WATCHDOG -DCONFIG_NO__HAVE_READ_LOCK_FUSE -Os -funsigned-char -funsigned-bitfields -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -fno-move-loop-invariants -fno-tree-scev-cprop -fno-inline-small-functions -Wall -Wno-unknown-pragmas -std=gnu99")
SET(CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS} -x assembler-with-cpp" )
SET(CMAKE_C_LINK_FLAGS "-mmcu=${MCU} -Wl,--relax,--gc-sections -Wl,--section-start=.text=${BOOTLOADER_ADDRESS} -Wl,--defsym=nullVector=0")

include_directories(.)
include_directories(../firmware)

add_executable(UsbAspLoader ../firmware/usbdrv/usbdrvasm.S ../firmware/usbdrv/oddebug.c ../firmware/usbdrv/oddebug.h main.c bootloaderconfig.h spminterface.h usbconfig.h)