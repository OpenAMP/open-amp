
option (WITH_RPMSG_APIS_VERIFY "Include RPMsg APIs verification" OFF)

add_subdirectory (msg)
if (WITH_RPMSG_APIS_VERIFY)
  add_subdirectory (rpmsg-apis)
endif (WITH_RPMSG_APIS_VERIFY)
