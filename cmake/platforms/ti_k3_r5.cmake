set (CMAKE_SYSTEM_PROCESSOR "arm" CACHE STRING "")
set (MACHINE                "ti_k3_r5" CACHE STRING "")
set (CROSS_PREFIX           "arm-none-eabi-" CACHE STRING "")

if (NOT LIBMETAL_PATH)
  set (LIBMETAL_PATH "../libmetal")
endif (NOT LIBMETAL_PATH)

set (CMAKE_INCLUDE_PATH     "${LIBMETAL_PATH}/build-libmetal/usr/local/include")
set (CMAKE_LIBRARY_PATH     "${LIBMETAL_PATH}/build-libmetal/usr/local/lib")

set (CMAKE_C_FLAGS          "-mcpu=cortex-r5 -g -O0" CACHE STRING "")

include (cross_generic_gcc)
