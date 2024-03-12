cmake_minimum_required(VERSION 3.12)
cmake_policy(VERSION 3.12)

if(POLICY CMP0078)
  cmake_policy(SET CMP0078 NEW)   # UseSWIG: do not use legacy target names
endif(POLICY CMP0078)

if(POLICY CMP0086)
  cmake_policy(SET CMP0086 NEW)   # UseSWIG: pass -module option
endif(POLICY CMP0086)

# Add etc/cmake to CMake's search path so we can put our private stuff there
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/etc/cmake)

project(
  plfit
  VERSION 0.9.6
  DESCRIPTION "Library to fit power-law distributions to empirical data"
  HOMEPAGE_URL "https://github.com/ntamas/plfit"
  LANGUAGES C
)
enable_testing()

# Expose the BUILD_SHARED_LIBS option in the ccmake UI
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

# Add switches to use LTO and sanitizers if needed
include(lto)
include(sanitizers)
include(JoinPaths)

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
else()
    # On Linux and Mac we are okay with -Wall
    add_definitions(-Wall -DHAVE_CONFIG_H)
endif()

if(PLFIT_USE_SSE)
    message(STATUS "Using SSE/SSE2 optimizations if available")
    add_definitions(-DUSE_SSE)
else()
    message(STATUS "SSE/SSE2 optimizations disabled")
endif()

if(PLFIT_USE_OPENMP)
    find_package(OpenMP)
    if(OPENMP_FOUND)
        message(STATUS "Using OpenMP parallelization (experimental)")
    else()
        message(STATUS "OpenMP not supported by compiler; disabling OpenMP parallelization")
    endif()
else()
    message(STATUS "OpenMP parallelization disabled")
endif()

if(WIN32)
    # No need to link to the m library on Windows
    set(MATH_LIBRARY "")
else()
    FIND_LIBRARY(MATH_LIBRARY NAMES m)
endif()

# Add version information
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/src/plfit_version.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/src/plfit_version.h
)

add_subdirectory(src)
add_subdirectory(test)
