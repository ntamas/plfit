include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)
add_definitions(-DDATADIR=\"${CMAKE_CURRENT_SOURCE_DIR}/../data\")

set(TEST_CASES kolmogorov discrete continuous gss real sampling
	underflow_handling)

foreach(test ${TEST_CASES})
	add_executable(test_${test} test_${test}.c test_common.c)
	target_link_libraries(test_${test} plfit ${MATH_LIBRARY})
	add_test(test_${test} test_${test})
endforeach(test)
