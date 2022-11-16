cmake_minimum_required(VERSION 3.10)

project("baseload")

file(GLOB_RECURSE "BASELOAD_SRC" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable("baseload" ${BASELOAD_SRC})

target_include_directories("baseload" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries("baseload" PRIVATE "log4cpp")
