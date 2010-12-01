set(PLFIT_CORE_SRCS lbfgs.c error.c kolmogorov.c plfit.c zeta.c)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(plfit ${PLFIT_CORE_SRCS} main.c test.c)
target_link_libraries(plfit m)

if(PLFIT_COMPILE_PYTHON_MODULE)
    # Check whether we have SWIG and the Python libraries installed
	find_package(SWIG REQUIRED)
	include(${SWIG_USE_FILE})
	find_package(PythonLibs REQUIRED)
	include_directories(${PYTHON_INCLUDE_PATH})

	set(CMAKE_SWIG_FLAGS "")
	swig_add_module(plfit python plfit.i ${PLFIT_CORE_SRCS})
	swig_link_libraries(plfit m)
endif(PLFIT_COMPILE_PYTHON_MODULE)
