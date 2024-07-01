cmake_minimum_required(VERSION 3.15)

project(poller)

aux_source_directory(./ SRC)

set(LIBRARY_OUTPUT_PATH ${LIBPATH})

add_library(poller_a STATIC ${SRC})
add_library(poller_s SHARED ${SRC})