include(helpers)

tristate(PLFIT_ENABLE_LTO "Enable link-time optimization" OFF)

include(CheckIPOSupported)

if(PLFIT_ENABLE_LTO)
  # this matches both ON and AUTO
  check_ipo_supported(RESULT IPO_SUPPORTED OUTPUT IPO_NOT_SUPPORTED_REASON)
  if(PLFIT_ENABLE_LTO STREQUAL "AUTO")
    # autodetection
	set(PLFIT_ENABLE_LTO ${IPO_SUPPORTED})
    if(IPO_SUPPORTED)
      set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    endif()
  elseif(IPO_SUPPORTED)
    # user wanted LTO and the compiler supports it
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
  else()
    # user wanted LTO and the compiler does not support it
    message(FATAL_ERROR "Link-time optimization not supported on this compiler")
  endif()
endif()
