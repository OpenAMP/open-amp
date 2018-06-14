
option (WITH_LOAD_FW "Include loading firmware example" OFF)

add_subdirectory (echo)
add_subdirectory (matrix_multiply)
if (WITH_LOAD_FW)
  add_subdirectory (load_fw)
endif (WITH_LOAD_FW)
if (WITH_PROXY_APPS)
if (NOT WITH_REMOTEPROC_MASTER)
add_subdirectory (rpc_demo)
endif (NOT WITH_REMOTEPROC_MASTER)
endif (WITH_PROXY_APPS)
