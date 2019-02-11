cmake_minimum_required(VERSION 3.2)
project(CameraCooler C ASM)

SET(MCU "atmega328p")
SET(F_CPU "12000000")

#-fpack-struct option is mandatory for compiler (pragma pack analog)
SET(CMAKE_C_FLAGS "-mmcu=${MCU} -DF_CPU=${F_CPU} -DNDEBUG -Os -funsigned-char -funsigned-bitfields -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -Wall -Wno-unknown-pragmas -std=gnu99")
SET(CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS} -x assembler-with-cpp" )
SET(CMAKE_C_LINK_FLAGS "-mmcu=${MCU}")

include_directories(usbdrv)
include_directories(.)

add_executable(CameraCooler usbdrv/usbdrv.c usbdrv/usbdrv.h usbdrv/usbdrvasm.S usbdrv/oddebug.c usbdrv/oddebug.h main.c i2c/i2c.c i2c/i2c.h bme280/bme280.c bme280/bme280.h bme280/bme280_defs.h bme280/bme280_user.c bme280/bme280_user.h adc/adc.c adc/adc.h common/definitions.h common/runtime_info.h common/settings.h cooler/cooler.h cooler/cooler.c)