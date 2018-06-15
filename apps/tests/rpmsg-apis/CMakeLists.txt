
set (_app "rpmsg-apis-verify")
set (_cflags "${CMAKE_C_FLAGS} ${APP_EXTRA_C_FLAGS}")
set (_fw_dir "${APPS_SHARE_DIR}")

collector_list (_list PROJECT_INC_DIRS)
collector_list (_app_list APP_INC_DIRS)
include_directories (${_list} ${_app_list} ${CMAKE_CURRENT_SOURCE_DIR})

collector_list (_list PROJECT_LIB_DIRS)
collector_list (_app_list APP_LIB_DIRS)
link_directories (${_list} ${_app_list})

collector_list (_list APP_COMMON_SOURCES)
list (APPEND _list "${CMAKE_CURRENT_SOURCE_DIR}/rpmsg-apis-vdev-master.c")
list (APPEND _list "${CMAKE_CURRENT_SOURCE_DIR}/rpmsg-apis-vdev-slave.c")
add_executable (${_app}.out ${_list})

set_source_files_properties(${_list} PROPERTIES COMPILE_FLAGS "${_cflags}")

get_property (_linker_opt GLOBAL PROPERTY APP_LINKER_OPT)
get_property (_deps GLOBAL PROPERTY APP_LIB_DEPS)
set (_deps "${_deps} ${PLATFORM_LIB_DEPS}")
string (STRIP "${_deps}" _deps)

target_link_libraries(${_app}.out -Wl,-Map=${_app}.map -Wl,--gc-sections ${_linker_opt} -Wl,--start-group ${_deps} -Wl,--end-group)

install (TARGETS ${_app}.out RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

