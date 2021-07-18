configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

include(GNUInstallDirs)

if(WIN32)
  set(PKGCONFIG_LIBS_PRIVATE "")
else()
  set(PKGCONFIG_LIBS_PRIVATE "-lm")
endif()

set(PLFIT_CORE_SRCS error.c gss.c kolmogorov.c lbfgs.c mt.c platform.c plfit.c options.c rbinom.c sampling.c stats.c hzeta.c)

add_library(plfit ${PLFIT_CORE_SRCS})
target_include_directories(plfit PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(plfit PROPERTIES SOVERSION 0)
if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
  set_target_properties(plfit PROPERTIES LINK_FLAGS "-Wl,--version-script=${PROJECT_SOURCE_DIR}/src/plfit.map")
  set_target_properties(plfit PROPERTIES LINK_DEPENDS ${PROJECT_SOURCE_DIR}/src/plfit.map)
endif()

configure_file(${PROJECT_SOURCE_DIR}/libplfit.pc.in ${PROJECT_BINARY_DIR}/libplfit.pc @ONLY)

install(TARGETS plfit LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(
	FILES
	${PROJECT_SOURCE_DIR}/src/plfit.h
	${PROJECT_SOURCE_DIR}/src/plfit_error.h
	${PROJECT_SOURCE_DIR}/src/plfit_mt.h
	${PROJECT_SOURCE_DIR}/src/plfit_sampling.h
	${PROJECT_BINARY_DIR}/src/plfit_version.h
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(FILES ${PROJECT_BINARY_DIR}/libplfit.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

add_executable(plfit-bin main.c getopt.c)
target_link_libraries(plfit-bin plfit ${MATH_LIBRARY})
set_target_properties(plfit-bin PROPERTIES OUTPUT_NAME plfit)
install(TARGETS plfit-bin RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

add_executable(plgen plgen.c getopt.c)
target_link_libraries(plgen plfit ${MATH_LIBRARY})
install(TARGETS plgen RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

if(PLFIT_COMPILE_PYTHON_MODULE)
    # Check whether we have SWIG and the Python libraries installed
	find_package(SWIG REQUIRED)
	include(${SWIG_USE_FILE})
	find_package(PythonInterp REQUIRED)
	find_package(PythonLibs REQUIRED)
	include_directories(${PYTHON_INCLUDE_PATH})

	# using distutils.
	execute_process(
		COMMAND
		${PYTHON_EXECUTABLE} -c "from distutils import sysconfig; print(sysconfig.get_python_lib(1,0,prefix='${CMAKE_INSTALL_PREFIX}'))"
 		OUTPUT_VARIABLE _python_instdir
		OUTPUT_STRIP_TRAILING_WHITESPACE
		)
	set(PYTHON_INSTDIR "${_python_instdir}")

	set(CMAKE_SWIG_FLAGS "")
	if (NOT BUILD_SHARED_LIBS)
		swig_add_library(plfit LANGUAGE python SOURCES plfit.i ${PLFIT_CORE_SRCS})
		swig_link_libraries(plfit ${PYTHON_LIBRARIES} m)
	else()
		swig_add_library(plfit LANGUAGE python SOURCES plfit.i)
		swig_link_libraries(plfit ${PYTHON_LIBRARIES} plfit m)
	endif()

	install(TARGETS _plfit DESTINATION ${PYTHON_INSTDIR})
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/plfit.py DESTINATION ${PYTHON_INSTDIR})

endif(PLFIT_COMPILE_PYTHON_MODULE)
