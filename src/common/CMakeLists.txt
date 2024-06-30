cmake_minimum_required(VERSION 3.15)

project(common)

aux_source_directory(./ SRC)

#设置库输出路径
set(LIBRARY_OUTPUT_PATH ${LIBPATH})

#添加库文件
add_library(common STATIC ${SRC})
add_library(comm SHARED ${SRC})



