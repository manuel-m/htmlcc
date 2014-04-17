cmake_minimum_required(VERSION 2.8)

#==============================================================================
# Additional package ressource finders
#==============================================================================
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/resources/CMake/Modules")

#==============================================================================
# The macro below forces the build directory to be different from source directory
#==============================================================================
include(MacroOutOfSourceBuild)
macro_ensure_out_of_source_build("${PROJECT_NAME} requires an out of source build.")

include(MacroUseUv)
macro_use_uv()

include(MacroLogAll)
macro_logall()

include(MacroDefineCFlags)
macro_define_cflags()

set(DIST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(GEN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/generated_sources)

file(MAKE_DIRECTORY ${DIST_DIR})
set(EXECUTABLE_OUTPUT_PATH ${DIST_DIR})
set(LIBRARY_OUTPUT_PATH ${DIST_DIR})

set(RSR_EMBEDER_EXE embed)

if (CMAKE_BUILD_TYPE MATCHES Debug)
  set(RSR_EMBEDER_EXE "${RSR_EMBEDER_EXE}_d")
endif (CMAKE_BUILD_TYPE MATCHES Debug)

set(LIBHC_SRC src/lib)
set(LIBHC_A htmlcc)
set(LIBRSR_A rsr)

include_directories (src ${GEN_DIR} src/lib)

add_library(${LIBHC_A} 
            src/lib/http_parser.c
            src/lib/mmtrace.c 
            src/lib/strcompat.c 
            src/lib/mmpool.c 
            src/lib/sub0.c
            src/lib/hashmap.c
            src/lib/diru.c
            src/lib/bagride2.c
            src/lib/br_http.c
            src/lib/br_alloc.c)

add_executable (${RSR_EMBEDER_EXE} src/tools/embed.c)
target_link_libraries(${RSR_EMBEDER_EXE} ${LIBHC_A})

add_subdirectory(src/samples)
add_subdirectory(src/tests)









