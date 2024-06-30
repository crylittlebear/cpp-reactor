cmake_minimum_required(VERSION 3.15)

project(channel)

aux_source_directory(./ SRC)

set(LIBRARY_OUTPUT_PATH ${LIBPATH})

add_library(channel_a STATIC ${SRC})
add_library(channel_s SHARED ${SRC})