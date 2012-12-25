cmake_minimum_required(VERSION 2.6)

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

add_definitions(-Wall -DHAVE_CONFIG_H)

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

add_subdirectory(src)
add_subdirectory(test)
