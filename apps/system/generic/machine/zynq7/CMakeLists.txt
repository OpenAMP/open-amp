collect (APP_INC_DIRS "${CMAKE_CURRENT_SOURCE_DIR}")
collect (APP_COMMON_SOURCES helper.c)

if (WITH_REMOTEPROC_MASTER)
  set (_linker_script "${CMAKE_CURRENT_SOURCE_DIR}/linker_master.ld")
else (WITH_REMOTEPROC_MASTER)
  set (_linker_script "${CMAKE_CURRENT_SOURCE_DIR}/linker_remote.ld")
endif (WITH_REMOTEPROC_MASTER)

set_property (GLOBAL PROPERTY APP_LINKER_SCRIPT "${_linker_script}")
