macro(tristate OPTION_NAME DESCRIPTION DEFAULT_VALUE)
  set(${OPTION_NAME} "${DEFAULT_VALUE}" CACHE STRING "${DESCRIPTION}")
  set_property(CACHE ${OPTION_NAME} PROPERTY STRINGS AUTO ON OFF)
endmacro()
