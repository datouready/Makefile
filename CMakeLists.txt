# // 指定CMAKE的最小版本要求
cmake_minimum_required(VERSION 2.8.3)

# // 定义工程名称，并可指定工程支持的语言
project(MYPRO)

# // 显式的定义变量
set(SRC mythread.cpp main.cpp)

# // include_directories 向工程添加多个特定的头文件搜索路径--》相当于指定g++编译器的-I(大写的i)参数
include_directories(./include)

# // link_directories-向工程添加多个特定的库文件搜索路径--》相当于指定g++编译器的-L参数
# // 语法 link_directories(dir1 dir2...)
link_directories(./lib)

# // add_library-生成库文件
# // add_library(libname[SHARED|STATIC|MODULE][EXCLUDE_FROM_ALL]source1 source2...sourceN)
# // 通过变量SRC生成libhello.so共享库
add_library(hello SHARED ${SRC})

# // add_compile_options-添加编译参数
add_compile_options(-wall -std=c++11 -O2)

# // add_executable-生成可执行文件
# // 语法：add_executable(exename source1 source2...sourceN)
# // 编译main.cpp生成可执行文件main
add_executable(main ${SRC})
// 或者add_executable(main main.cpp)

# // target_link_libraries-为target添加需要链接的共享库--》相同于指定g++编译器-l参数（小写的L）
# // 静态库不需要在这里链接，因为静态库在编译的时候已经加载了
# // 语法：target_link_libraries(target library1 library2...)
# // 将hello动态库文件链接到可执行文件main
target_link_libraries(main hello)

# // add_subdirectory-向当前工程添加存放源文件的子目录，并可以指定中间二进制和目标二进制存放的位置
# // 语法：add_subdirectory(source_dir [binary_dir][EXCLUDE_FROM_ALL])
# // 添加src子目录，src中需要有一个CMakeLists.txt
add_subdirectory(src)

# aux_source_directory-发现一个目录下所有的源代码文件并将列表储存在一个变量中，这个指定临时被用来自动构建源文件列表
# 定义SRC变量，其值为当前目录下所有的源代码文件
aux_source_directory(.SRC)
# 编译SRC变量所代表的源代码文件，生成main可执行文件
add_executable(main ${SRC})

# CMAKE_C_COMPILER:指定C编译器
# CMAKE_CXX_COMPILER:指定C++编译器
# EXECUTABLE_OUTPUT_PATH:可执行文件输出的存放路径
# LIBRARY_OUTPUT_PATH:库文件输出的存放路径