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

set(RSR_EMBEDER_EXE embedfile)
set(TEST_RSR_EMBEDER_EXE test_embedfile)
set(TEST_ASYNC_HTTP_EXE test_async_http)
set(TEST_AHE_EXE test_ahe)
set(TEST_AHE2_EXE test_ahe2)
set(TEST_HASHMAP_EXE test_hashmap)
set(TEST_RECURSE_DIR_EXE test_recurse_dir)

if (CMAKE_BUILD_TYPE MATCHES Debug)
  set(RSR_EMBEDER_EXE "${RSR_EMBEDER_EXE}_d")
  set(TEST_RSR_EMBEDER_EXE "${TEST_RSR_EMBEDER_EXE}_d")
  set(TEST_ASYNC_HTTP_EXE "${TEST_ASYNC_HTTP_EXE}_d")
  set(TEST_AHE_EXE "${TEST_AHE_EXE}_d")
  set(TEST_AHE2_EXE "${TEST_AHE2_EXE}_d")
  set(TEST_HASHMAP_EXE "${TEST_HASHMAP_EXE}_d")
  set(TEST_RECURSE_DIR_EXE "${TEST_RECURSE_DIR_EXE}_d")
endif (CMAKE_BUILD_TYPE MATCHES Debug)

set(LIBHC_SRC src/lib)
set(LIBHC_A htmlcc)
set(LIBRSR_A rsr)

include_directories (src ${GEN_DIR} src/lib)

add_library(${LIBHC_A} 
            src/lib/http_parser.c
            src/lib/mmtrace.c 
            src/lib/mmpool.c 
            src/lib/sub0.c
            src/lib/hashmap.c
            src/lib/diru.c
            src/lib/bagride2.c)
            

add_executable (${TEST_ASYNC_HTTP_EXE} src/tests/test_async_http.c)  
target_link_libraries (${TEST_ASYNC_HTTP_EXE} ${LIBHC_A} ${UV_A} -lpthread)


add_executable (${TEST_HASHMAP_EXE} src/lib/hashmap.c src/tests/test_hashmap.c)

add_executable (${TEST_RECURSE_DIR_EXE} src/lib/diru.c src/tests/test_recurse_dir.c)

#
#add_executable (${TEST_AHE_EXE} src/tests/test_ahe.c)
#target_link_libraries (${TEST_AHE_EXE} ${LIBHC_A} ${UV_A} -lpthread  ${GEN_DIR}/remark.min.js.o ${GEN_DIR}/htmlcc.html.o ${GEN_DIR}/not_found.html.o)

add_executable (test_ahe_xxd src/tests/test_ahe_xxd.c)
target_link_libraries (test_ahe_xxd ${LIBHC_A} ${UV_A} -lpthread )


add_executable (embed src/tools/embed.c)
target_link_libraries(embed ${LIBHC_A})

#add_library(${LIBRSR_A}   
#            ${GEN_DIR}/htmlcc.html.c 
#            ${GEN_DIR}/not_found.html.c 
#            ${GEN_DIR}/remark.min.js.c)


#add_executable (${TEST_AHE2_EXE} src/tests/test_ahe2.c)
#target_link_libraries (${TEST2_AHE_EXE} ${LIBHC_A} ${UV_A} -lpthread)