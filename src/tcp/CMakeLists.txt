cmake_minimum_required(VERSION 3.15)

project(tcp)

aux_source_directory(. SRC)

set(LIBRARY_OUTPUT_PATH ${LIBPATH})

add_library(tcp_a STATIC ${SRC})
add_library(tcp_s SHARED ${SRC})