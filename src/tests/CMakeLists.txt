set(TEST_RSR_EMBEDER_EXE test_embedfile)
set(TEST_ASYNC_HTTP_EXE test_async_http)
set(TEST_HASHMAP_EXE test_hashmap)
set(TEST_RECURSE_DIR_EXE test_recurse_dir)
set(TEST_HTTP_PARSER_EXE test_http_parser)

if (CMAKE_BUILD_TYPE MATCHES Debug)
  set(TEST_RSR_EMBEDER_EXE "${TEST_RSR_EMBEDER_EXE}_d")
  set(TEST_ASYNC_HTTP_EXE "${TEST_ASYNC_HTTP_EXE}_d")
  set(TEST_HASHMAP_EXE "${TEST_HASHMAP_EXE}_d")
  set(TEST_RECURSE_DIR_EXE "${TEST_RECURSE_DIR_EXE}_d")
endif (CMAKE_BUILD_TYPE MATCHES Debug)


add_executable (${TEST_ASYNC_HTTP_EXE} test_async_http.c)  
target_link_libraries (${TEST_ASYNC_HTTP_EXE} ${LIBHC_A} ${UV_A} -lpthread)

add_executable (${TEST_HASHMAP_EXE} test_hashmap.c)
target_link_libraries (${TEST_HASHMAP_EXE} ${LIBHC_A})

add_executable (${TEST_RECURSE_DIR_EXE} test_recurse_dir.c)
target_link_libraries (${TEST_RECURSE_DIR_EXE} ${LIBHC_A})

add_executable (${TEST_HTTP_PARSER_EXE} test_http-parser.c)
target_link_libraries (${TEST_HTTP_PARSER_EXE} ${LIBHC_A})