cmake_minimum_required(VERSION 3.15)

project(cpp_reactor)

#设置c++标准
set(CMAKE_CXX_STANDARD 17)

#设置静态库输出路径
set(LIBPATH ${PROJECT_SOURCE_DIR}/lib)

#设置头文件路径
include_directories(${PROJECT_SOURCE_DIR}/include)

#输出静态库路径信息
message("library path: ${LIBPATH}")

# 设置全局库输出路径
# set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LIBPATH})  # 静态库 (.a)
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIBPATH})  # 共享库 (.so)

#设置子目录
add_subdirectory(src/buffer)
add_subdirectory(src/common)
