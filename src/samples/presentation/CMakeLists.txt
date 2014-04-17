
set(PRESENTATION htmlcc_doc)

if (CMAKE_BUILD_TYPE MATCHES Debug)
  set(PRESENTATION "${PRESENTATION}_d")
endif (CMAKE_BUILD_TYPE MATCHES Debug)

set(RSR_H ${GEN_DIR}/hxd_htmlcc_doc.h)
set(RSR_IN ${CMAKE_CURRENT_SOURCE_DIR}/public_html)

add_custom_target(gen_htmlcc_doc
                  COMMAND ${RSR_EMBEDER_EXE} ${RSR_IN} htmlcc_doc ${RSR_H}
                  DEPENDS ${RSR_EMBEDER_EXE})

add_executable (${PRESENTATION} main.c)
target_link_libraries (${PRESENTATION} ${LIBHC_A} ${UV_A} -lpthread)
add_dependencies(${PRESENTATION} gen_htmlcc_doc)
