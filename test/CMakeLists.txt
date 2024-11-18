include_directories(
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/src
)
add_definitions(-DDATADIR=\"${CMAKE_CURRENT_SOURCE_DIR}/../data\")

set(TEST_CASES discrete continuous real sampling underflow_handling xmin_too_low)
set(TEST_CASES_INTERNAL hzeta kolmogorov gss)

# Borrowed from igraph
function(correct_test_environment TEST_NAME)
	if(WIN32 AND BUILD_SHARED_LIBS)
		# On Windows the built plfit.dll is not automatically found by the tests. We therefore
		# add the dir that contains the built igraph.dll to the path environment variable
		# so that igraph.dll is found when running the tests.
		set(PLFIT_LIBDIR $<TARGET_FILE_DIR:plfit>)

		# The next line is necessitated by MinGW on Windows. MinGW uses forward slashes in
		# PLFIT_LIBDIR, but we need to supply CTest with backslashes because CTest is executed
		# in a cmd.exe shell. We therefore explicitly ensure that that path is transformed to a
		# native path.
		file(TO_NATIVE_PATH "${PLFIT_LIBDIR}" PLFIT_LIBDIR)

		# Semicolons are used as list separators in CMake so we need to escape them in the PATH,
		# otherwise the PATH envvar gets split by CMake before it passes the PATH on to CTest.
		# We process each path separately to ensure it is a proper path. In particular, we need
		# to ensure that a trailing backslash is not incorrectly interpreted as an escape
		# character. Presumably, with cmake 3.20, this can be changed to using TO_NATIVE_PATH_LIST.
		set(TEST_PATHS)
		foreach (PATH $ENV{PATH})
			file(TO_NATIVE_PATH "${PATH}" CORRECT_PATH)
			# Remove trailing backslash
			string(REGEX REPLACE "\\$" "" CORRECT_PATH ${CORRECT_PATH})
			list(APPEND TEST_PATHS ${CORRECT_PATH})
		endforeach()

		# Join all paths in a single string, separated by an escaped semi-colon.
		string(JOIN "\;" CORRECT_PATHS ${TEST_PATHS})
		set_tests_properties(${TEST_NAME} PROPERTIES ENVIRONMENT "PATH=${PLFIT_LIBDIR}\;${CORRECT_PATHS}" )
	endif()
endfunction()

foreach(test ${TEST_CASES})
	add_executable(test_${test} test_${test}.c test_common.c)
	target_link_libraries(test_${test} plfit ${MATH_LIBRARY})
	add_test(NAME test_${test} COMMAND test_${test})
	correct_test_environment(test_${test})
endforeach(test)

foreach(test ${TEST_CASES_INTERNAL})
	add_executable(test_${test} test_${test}.c test_common.c ${PROJECT_SOURCE_DIR}/src/${test}.c)
	target_link_libraries(test_${test} plfit ${MATH_LIBRARY})
	add_test(NAME test_${test} COMMAND test_${test})
	correct_test_environment(test_${test})
endforeach(test)
