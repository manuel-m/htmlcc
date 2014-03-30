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

if (CMAKE_BUILD_TYPE MATCHES Debug)
  set(RSR_EMBEDER_EXE "${RSR_EMBEDER_EXE}_d")
  set(TEST_RSR_EMBEDER_EXE "${TEST_RSR_EMBEDER_EXE}_d")
endif (CMAKE_BUILD_TYPE MATCHES Debug)

include_directories (src tests ${GEN_DIR})

            
add_executable (${RSR_EMBEDER_EXE} src/tools/embedfile.c)

add_custom_command(
  OUTPUT ${GEN_DIR}/test_html.c
  COMMAND ${RSR_EMBEDER_EXE} sym_test ${CMAKE_CURRENT_SOURCE_DIR}/resources/test_html.rsr ${GEN_DIR}/test_html.c
  DEPENDS resources/test_html.rsr src/tools/embedfile.c)

add_executable (${TEST_RSR_EMBEDER_EXE} ${GEN_DIR}/test_html.c src/tests/test_embedfile.c)  





