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
