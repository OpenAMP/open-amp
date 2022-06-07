#
# Copyright (c) 2018 Linaro Limited
#
# SPDX-License-Identifier: Apache-2.0
#

if(CONFIG_OPENAMP)
set(WITH_ZEPHYR 1)
set(WITH_ZEPHYR_LIB 1)
if (CONFIG_OPENAMP_MASTER)
  set(WITH_VIRTIO_DRIVER 1)
else()
  set(WITH_VIRTIO_DRIVER OFF CACHE BOOL "" FORCE)
endif()
if (CONFIG_OPENAMP_SLAVE)
  set(WITH_VIRTIO_DEVICE 1)
else()
  set(WITH_VIRTIO_DEVICE OFF CACHE BOOL "" FORCE)
endif()
set(WITH_LIBMETAL_FIND OFF CACHE BOOL "" FORCE)
set(WITH_PROXY OFF CACHE BOOL "" FORCE)
set(LIBMETAL_INCLUDE_DIR ${ZEPHYR_BINARY_DIR}/ext/hal/libmetal/lib/include)
set(LIBMETAL_LIB ${ZEPHYR_BINARY_DIR}/ext/hal/libmetal/lib)

add_subdirectory(.. open-amp)
endif()
