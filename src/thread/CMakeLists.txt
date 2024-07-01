cmake_minimum_required(VERSION 3.15)

project(thread)

aux_source_directory(. SRC)

set(LIBRARY_OUTPUT_PATH ${LIBPATH})

add_library(thread_a STATIC ${SRC})
add_library(thread_s SHARED ${SRC})