set(TINYCONTROL tinycontrol)

if (CMAKE_BUILD_TYPE MATCHES Debug)
  set(TINYCONTROL "${TINYCONTROL}_d")
endif (CMAKE_BUILD_TYPE MATCHES Debug)

set(RSR_H ${GEN_DIR}/hxd_tinycontrol.h)
set(RSR_IN ${CMAKE_CURRENT_SOURCE_DIR}/public_html)

add_custom_target(gen_tinycontrol
                  COMMAND ${RSR_EMBEDER_EXE} ${RSR_IN} tinycontrol ${RSR_H}
                  DEPENDS ${RSR_EMBEDER_EXE})

add_executable (${TINYCONTROL} main.c)
target_link_libraries (${TINYCONTROL} ${LIBHC_A} ${UV_A} -lpthread)
add_dependencies(${TINYCONTROL} gen_tinycontrol)
