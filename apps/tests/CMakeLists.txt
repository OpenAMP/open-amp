
option (WITH_FUNCTION_TEST_SUITE "Include function test suite" OFF)

add_subdirectory (msg)
if (WITH_FUNCTION_TEST_SUITE)
  add_subdirectory (func_test_suite)
endif (WITH_FUNCTION_TEST_SUITE)
