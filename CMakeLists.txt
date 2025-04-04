cmake_minimum_required (VERSION 3.16)

set (OPENAMP_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set (OPENAMP_BIN_ROOT "${CMAKE_CURRENT_BINARY_DIR}")

list (APPEND CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake"
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules"
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/platforms")

include (syscheck)
project (open_amp C)

include (CheckIncludeFiles)
include (CheckCSourceCompiles)
include (collect)
include (options)
include (depends)
enable_testing ()


if (WITH_OBSOLETE)
  add_subdirectory (obsolete)
endif (WITH_OBSOLETE)

add_subdirectory (lib)

if (WITH_DOC)
  add_subdirectory (doc)
endif (WITH_DOC)
