configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

set(PLFIT_CORE_SRCS lbfgs.c error.c kolmogorov.c mt.c plfit.c sampling.c zeta.c)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

add_library(plfit0 ${PLFIT_CORE_SRCS})

add_executable(plfit main.c)
target_link_libraries(plfit plfit0 m)
install(TARGETS plfit RUNTIME DESTINATION bin)

add_executable(plgen plgen.c)
target_link_libraries(plgen plfit0 m)
install(TARGETS plgen RUNTIME DESTINATION bin)

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
