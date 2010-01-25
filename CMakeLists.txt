cmake_minimum_required(VERSION 2.6)

project(plfit C CXX)

option(PLFIT_COMPILE_PYTHON_MODULE
	   "Whether we want to compile the Python module (requires SWIG)"
	   OFF)

add_definitions(-Wall)
add_subdirectory(src)
