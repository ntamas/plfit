cmake_minimum_required(VERSION 2.6)

project(plfit C CXX)

option(PLFIT_COMPILE_PYTHON_MODULE
	   "Whether we want to compile the Python module (requires SWIG)"
	   OFF)
option(PLFIT_USE_SSE
       "Use SSE/SSE2 optimizations if available"
	   ON)

# Check for required headers
include(CheckIncludeFiles)
check_include_files(emmintrin.h HAVE_EMMINTRIN_H)
check_include_files(malloc.h HAVE_MALLOC_H)

add_definitions(-Wall -DHAVE_CONFIG_H)
if(PLFIT_USE_SSE)
	add_definitions(-DUSE_SSE)
endif(PLFIT_USE_SSE)

add_subdirectory(src)
