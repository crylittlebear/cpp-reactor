cmake_minimum_required(VERSION 3.15)

project(cpp_reactor)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif()

#设置宏
add_definitions(-DMDEBUG)
# add_definitions(-DMSG_SEND_AUTO)

#设置c++标准
set(CMAKE_CXX_STANDARD 17)

#设置静态库输出路径
set(LIBPATH ${PROJECT_SOURCE_DIR}/lib)

#设置可执行程序源文件
aux_source_directory(${PROJECT_SOURCE_DIR}/test SRC)

#设置头文件路径
include_directories(${PROJECT_SOURCE_DIR}/include)

# 设置全局库输出路径
# set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LIBPATH})  # 静态库 (.a)
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIBPATH})  # 共享库 (.so)

#设置子目录
add_subdirectory(src/common)
add_subdirectory(src/http)
add_subdirectory(src/reactor)
add_subdirectory(src/tcp)
add_subdirectory(src/thread)

#设置库目录
link_directories(${LIBPATH})

#链接库
link_libraries(tcp_a thread_a poller_a http_a common_a)

#生成可执行程序
add_executable(main ${SRC})
