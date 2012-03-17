include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

set(TEST_CASES kolmogorov discrete continuous real sampling)

foreach(test ${TEST_CASES})
	add_executable(test_${test} test_${test}.c test_common.c)
	target_link_libraries(test_${test} plfit0 m)
	add_test(test_${test} test_${test})
endforeach(test)