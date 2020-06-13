configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

set(PLFIT_CORE_SRCS error.c gss.c kolmogorov.c lbfgs.c mt.c platform.c plfit.c options.c rbinom.c sampling.c stats.c hzeta.c)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

add_library(plfit0 ${PLFIT_CORE_SRCS})

add_executable(plfit main.c getopt.c)
target_link_libraries(plfit plfit0 ${MATH_LIBRARY})
install(TARGETS plfit RUNTIME DESTINATION bin)

add_executable(plgen plgen.c getopt.c)
target_link_libraries(plgen plfit0 ${MATH_LIBRARY})
install(TARGETS plgen RUNTIME DESTINATION bin)

if(PLFIT_COMPILE_PYTHON_MODULE)
    # Check whether we have SWIG and the Python libraries installed
	find_package(SWIG REQUIRED)
	include(${SWIG_USE_FILE})
	find_package(PythonLibs REQUIRED)
	include_directories(${PYTHON_INCLUDE_PATH})

	set(CMAKE_SWIG_FLAGS "")
	set_property(SOURCE plfit.i PROPERTY SWIG_MODULE_NAME plfit)
	swig_add_library(plfit_python LANGUAGE python SOURCES plfit.i ${PLFIT_CORE_SRCS})
	swig_link_libraries(plfit_python m ${PYTHON_LIBRARIES})

endif(PLFIT_COMPILE_PYTHON_MODULE)
