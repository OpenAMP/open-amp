collect (APP_COMMON_SOURCES helper.c)

if (WITH_REMOTEPROC_MASTER)
  message(FATAL_ERROR "Remoteproc master is not currently supported on ${MACHINE}/${SYSTEM}.")
endif (WITH_REMOTEPROC_MASTER)

