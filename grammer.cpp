// 1、相比cmake，Makefile更加轻量简洁，侵入性低，掌控力强。语法更少更简单
// 2、使用Makefile你可以进行更细粒度的掌控，虽然cmake简化了这些，但是对于大型项目，细节的掌控是必须的
// 3、 对于算法落地，我们会面临各种库包，理清楚非常有利于降低问题的发生
// 4、g++和gcc都是gnu推出的c++编译器，时代不同
// 5、使用g++我们可以编译c++代码为程序
// 6、g++和gcc都可以进行c++编译
// 7、g++和gcc一样，都属于driver，即驱动编译，他会调用cclplus/ccl/ld/as等指令实现编译链接等工作，他们俩只是默认选项上的处理不同。这里采用g++而不是gcc
// 8、g++ 等价于 gcc -xc++ -lstdc++ -shared-libgcc

// 一、g++指令介绍
// 4种情况
// 预处理：g++ -E main.cpp -o main.i
// 预处理后，宏定义，包含的头文件都会被展开
// 汇编：g++ -S main.i -o main.s
// 转为汇编代码
// 编译：g++ -c main.s -o main.o
// 转为二进制代码，ELF文件格式
// 链接：g++ main.o -o main.bin
//把二进制文件代码链接为可执行程序

// g++可以允许跨过中间步骤，例如：
// g++ -S main.cpp -o main.s
// g++ -c main.cpp -o main.bin
// g++ main.cpp -o main.bin
// 比较常用的是编译-链接：
// g++ -c main.cpp -o  main.o
// g++ main.o -o main.bin

// 二、C++编译链接、编译时和运行时
// C++编译链接流程
// g++ -c a.cpp -o a.o
// g++ -c b.cpp -o b.o
// 动态库 lib3rd.so
// 静态库 libpkg.a
// g++ a.o b.o -l3rd -lpkg -o out.bin

// C++编译过程
// 关于编码的结论是：
// 1、c++中的函数/符号/变量会被编码
// 2、函数的编码关心的是：函数名称、所在命名空间、参数类型
// 3、函数的编码不关心的是：返回值类型
// 关于编译的结论是：
// 1、调用一个函数，会生成call函数名称的代码，函数的具体实现不会搬过来

//动态库链接
// 如果链接的符号在so中，out.bin会引用这个符号的so文件，在运行时动态加载lib3rd.so,再调用add函数。
// 在out.bin中会出现.reference_library:lib3rd.so

//静态库链接
// 如果链接的符号在a中，out.bin会将add的实现代码完整复制到out.bin中，运行out.bin时，不需要libpkg.a

// C++链接过程-so文件、a文件查找方式   (链接时)
// g++ -lpkg -l3rd main.o -o out.bin
// 当程序链接时，如何决定链接的是哪个so、a文件呢，是按照如下依据来的：
// 1、g++ -lname,表示链接一个动态库或者静态库(.so/.a)，名字是libname.so/libname.a
// 2、g++ -Lfolder,表示配置一个动态、静态库的查找目录
// 3、g++ 查找so/a文件的地方有3个，按照下列优先顺序查找
// 第一顺序：-L配置的目录
// 第二顺序：g++内置的系统目录，例如/usr/lib... 巴拉巴拉
// 第三顺序：系统环境变量（例如LIBRARY_PATH）指定的目录

// C++的so文件查找方式   （运行时）
// ./out.bin
// 当程序运行时，此时进入运行时状态，跟编译链接完全不同。此时的so文件的查找则也不同，是按照如下进行：
// 第一顺序：应用程序的当前目录（当前目录不同于程序所在目录）
// 第二顺序：out.bin中储存的rpath(run path)。readelf -d out.bin指令可以查看文件的runpath信息。
// 如果该选项指定了依旧失效，说明依赖的so文件还存在更多依赖在其他目录没有明确（常用）
// 第三顺序：环境变量指定的目录（例如LD_LIBRARY_PATH）