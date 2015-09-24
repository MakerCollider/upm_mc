set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples)

add_executable(geometry-example geometry.cxx)

include_directories (${PROJECT_SOURCE_DIR}/src/geometry)

target_link_libraries (geometry-example geometry ${CMAKE_THREAD_LIBS_INIT})
