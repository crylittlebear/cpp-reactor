cmake_minimum_required(VERSION 3.25)

project(cpp_reactor)

#设置c++标准
set(CMAKE_CXX_STANDARD 17)

#定义参与编译的源文件
aux_source_directory(. SRC_LIST)

add_executable(main ${SRC_LIST})   

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

