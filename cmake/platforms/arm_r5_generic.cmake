set (CMAKE_SYSTEM_PROCESSOR "arm" CACHE STRING "")
set (CROSS_PREFIX           "armr5-none-eabi-" CACHE STRING "")

set (CMAKE_C_FLAGS          "-mfloat-abi=hard -mfpu=vfpv3-d16 -mcpu=cortex-r5" CACHE STRING "")

include (cross_generic_gcc)
