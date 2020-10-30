# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)

# 项目信息
project (Demo)

# 引用OpenCV
set(CMAKE_INSTALL_LIB_DIR ${PROJECT_SOURCE_DIR}/lib)
set(OpenCV_DIR "${CMAKE_INSTALL_LIB_DIR}/opencv/lib/cmake/opencv4")
find_package(OpenCV REQUIRED)

# 指定生成目标
add_executable(Demo src/main.cpp)

# 将第三方的库链接到可执行文件
target_link_libraries(Demo ${OpenCV_LIBS})
    