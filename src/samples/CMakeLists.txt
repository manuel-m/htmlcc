add_subdirectory(presentation)
add_subdirectory(tinycontrol)
add_subdirectory(websumup)

if (CMAKE_BUILD_TYPE MATCHES Debug)
  add_executable (webhello_d webhello.c)
else(CMAKE_BUILD_TYPE MATCHES Debug)
  add_executable (webhello webhello.c)
endif (CMAKE_BUILD_TYPE MATCHES Debug)

