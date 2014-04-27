
set(WEBSUMPUP websumup)

if (CMAKE_BUILD_TYPE MATCHES Debug)
  set(WEBSUMPUP "${WEBSUMPUP}_d")
endif (CMAKE_BUILD_TYPE MATCHES Debug)

set(RSR_H ${GEN_DIR}/hxd_websumup.h)
set(RSR_IN ${CMAKE_CURRENT_SOURCE_DIR}/public_html)

add_custom_target(gen_websumup
                  COMMAND ${RSR_EMBEDER_EXE} ${RSR_IN} websumup ${RSR_H}
                  DEPENDS ${RSR_EMBEDER_EXE})

add_executable (${WEBSUMPUP} main.c)
target_link_libraries (${WEBSUMPUP} ${LIBHC_A} ${UV_A} -lpthread)
add_dependencies(${WEBSUMPUP} gen_websumup)
