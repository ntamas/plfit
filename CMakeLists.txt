cmake_minimum_required(VERSION 3.14)
cmake_policy(VERSION 3.14)

project(plfit C CXX)
enable_testing()

option(PLFIT_COMPILE_PYTHON_MODULE
	   "Whether we want to compile the Python module (requires SWIG)"
	   OFF)
option(PLFIT_USE_SSE
       "Use SSE/SSE2 optimizations if available"
	   ON)
option(PLFIT_USE_OPENMP
       "Use OpenMP parallelization if available (experimental)"
	   OFF)

# Check for required headers
include(CheckIncludeFiles)
check_include_files(emmintrin.h HAVE_EMMINTRIN_H)
check_include_files(malloc.h HAVE_MALLOC_H)

if(MSVC)
	# /Wall is too much for MSVC; use /W4 instead.
	# Don't produce C4100 warnings (unused formal parameter)
	add_definitions(-W4 -wd4100 -DHAVE_CONFIG_H -D_CRT_SECURE_NO_DEPRECATE)
else(MSVC)
	# On Linux and Mac we are okay with -Wall
	add_definitions(-Wall -DHAVE_CONFIG_H)
endif(MSVC)

if(PLFIT_USE_SSE)
	message(STATUS "Using SSE/SSE2 optimizations if available")
	add_definitions(-DUSE_SSE)
else(PLFIT_USE_SSE)
	message(STATUS "SSE/SSE2 optimizations disabled")
endif(PLFIT_USE_SSE)

if(PLFIT_USE_OPENMP)
    find_package(OpenMP)
	if(OPENMP_FOUND)
		message(STATUS "Using OpenMP parallelization (experimental)")
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
	else(OPENMP_FOUND)
		message(STATUS "OpenMP not supported by compiler; disabling OpenMP parallelization")
	endif(OPENMP_FOUND)
else(PLFIT_USE_OPENMP)
	message(STATUS "OpenMP parallelization disabled")
endif(PLFIT_USE_OPENMP)

if(WIN32)
	# No need to link to the m library on Windows
	set(MATH_LIBRARY "")
else(WIN32)
	FIND_LIBRARY(MATH_LIBRARY NAMES m)
endif(WIN32)

add_subdirectory(src)
add_subdirectory(test)
