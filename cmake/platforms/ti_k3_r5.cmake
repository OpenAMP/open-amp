set (CMAKE_SYSTEM_PROCESSOR "arm" CACHE STRING "")
set (MACHINE                "ti_k3_r5" CACHE STRING "")
set (CROSS_PREFIX           "arm-none-eabi-" CACHE STRING "")

set (CMAKE_C_FLAGS          "-mcpu=cortex-r5 -g -O0" CACHE STRING "")

include (cross_generic_gcc)
