cmake_minimum_required(VERSION 3.2)
project(CameraCooler C ASM)

SET(MCU "atmega328p")
SET(F_CPU "12000000")

SET(CMAKE_C_FLAGS "-mmcu=${MCU} -DF_CPU=${F_CPU} -DNDEBUG -Os -funsigned-char -funsigned-bitfields -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -Wall -std=gnu99")
SET(CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS} -x assembler-with-cpp" )
SET(CMAKE_C_LINK_FLAGS "-mmcu=${MCU}")

include_directories(usbdrv)
include_directories(.)

add_executable(CameraCooler usbdrv/usbdrv.c usbdrv/usbdrvasm.S usbdrv/oddebug.c main.c i2c/i2c.c bme280/bme280.c bme280/bme280_user.c adc/adc.c)