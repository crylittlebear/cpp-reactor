cmake_minimum_required(VERSION 3.15)

project(buffer)

aux_source_directory(./ SRC)

#设置库输出路径
set(LIBRARY_OUTPUT_PATH ${LIBPATH})

#添加静态库文件
add_library(buffer STATIC ${SRC})
add_library(buff SHARED ${SRC}) 



