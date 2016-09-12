collect (APP_COMMON_SOURCES helper.c)
collect (APP_COMMON_SOURCES rsc_table.c)
collect (APP_COMMON_SOURCES platform_info.c)

collect (APP_INC_DIRS "${CMAKE_CURRENT_SOURCE_DIR}")

if (WITH_REMOTEPROC_MASTER)
  message(FATAL_ERROR "Remoteproc master is not currently supported on ${MACHINE}/${SYSTEM}.")
endif (WITH_REMOTEPROC_MASTER)

