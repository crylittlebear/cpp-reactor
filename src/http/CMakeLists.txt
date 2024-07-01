cmake_minimum_required(VERSION 3.15)

project(http)

aux_source_directory(. SRC)

set(LIBRARY_OUTPUT_PATH ${LIBPATH})

add_library(http_a STATIC ${SRC})
add_library(http_s SHARED ${SRC})