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
// 静态库 libpkg.a  //库都是在链接这一步使用的
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

// C++编译时，头文件查找方式
// g++ -lfolder
// 当程序编译时，头文件该如何查找呢：
// g++ -lfolder表示配置一个头文件查找目录

// 当#include "path/name.hpp" 使用双引号时：
// 编译器会在当前文件的目录下查找path/name.hpp,例如我们在/data/a.cpp中写了
// #include "path/name.hpp"，最终确认的路径是/data/path/name.hpp

// 当#include <path/name.hpp>使用尖括号时：
// 第一顺序：以g++ -I配置的路径查找，例如g++ -I/data/folder,确认路径是：
// /data/folder/path/name.hpp,对所有路径都进行测试，找到为止
// 第二顺序：g++内置的系统路径，一般是usr/include...等等,g++ -print-search-dirs可以打印出来
// 第三顺序：系统环境变量配置的路径，例如：C_INCLUDE_PATH,CPP_INCLUDE_PATH

// 三、Makefile基本语法依赖关系定义
// 1、make指令执行时，默认会查找当前目录下的makefile、Makefile文件
// 作为代码文件，当然也可以make -f abc的方式，指定make运行的Makefile代码
// 2、Makefile主要解决的问题是，描述生成依赖关系，根据生成和依赖文件修改时间新旧决定是否执行command。可手动调用g++进行编译，
// 重点是，如果每次编译，都是全体代码参与，对于没有修改部分的代码编译，是浪费时间。项目文件越多，这个问题越严重。
// 而Makefile帮我们解决这个问题
// 3、Makefile的重点有：描述依赖关系、command(生成文件的指令)
// 4、我们只需要学习Makefile的基本操作足以应付项目需求即可，并不需要学习全部语法

// 定义变量var:=folder
// 生成main.o依赖main.cpp
// 若main.cpp的修改时间比main.o更早更新
// 则触发执行shell指令g++ -c main.cpp -o main.o
// main.o : main.cpp    依赖定义域   main.o是生成项、main.cpp是依赖项
//     g++ -c main.cpp -o main.o    command需要跟依赖定义具有空格隔开，表示下级，一般是4个空格（即tab）
// 当执行make main.o时，会检查main.o和main.cpp的修改时间，决定是否执行生成指令：g++ -c main.cpp -o main.o

// 生成项可以没有依赖项，那么如果该生成项文件不存在,command将永远执行
// debug:
//     @echo a = $(a) 打印出a=...之类的内容，  @echo是打印出后面的内容

// Makefile基础-makefile语法
// 1、数据类型，字符串和字符串数组
// 2、定义变量，var:=folder,定义变量var，为string类型，值是folder
// 3、定义数组，var:=hello world folder,定义变量var，为数组类型，值是["hello","world","folder"]
// 4、定义的方式有多种
// = 赋值   var = folder 基本赋值,Makefile全部执行后决定取值（不常用）
// :=赋值   var:=folder  基本赋值，当前所在位置决定取值（常用）
// ？=赋值  var?=folder  如果没有赋值，则赋值为folder
// +=赋值   var+=folder  添加值，在var后面添加值。可以认为数组后面增加元素
// append,var:=hello,var+=world,结果是hello world,中间有空格
// 5、$(var),${var}，在这个位置解释为var的值，例如: var2 := $(var)
// 6、$(func param),调用Make提供的内置函数
// 例如：var:=$(shell echo hello),定义var值为执行shell echo hello后输出
// 例如：$(info $(var)),直接打印var变量内容
// 例如：var:=$(subst a,x,aabbcc),替换aabbcc中的a为x后赋值给var，结果xxbbcc
// 7、逻辑语法：ifeq、ifneq、ifdef、ifndef
// ifeq($(var),depends)
//     name:=hello
// endif

// #定义变量a，赋值为folder
// a:=folder

// #为变量a，增加内容directory,结果是：folder directory
// a+=directory

// #定义变量c，为执行ls指令后的输出字符串
// c:=$(shell ls)

// #定义变量d，为把xiao中的x，替换为a。结果是：aiao
// d:=$(subst x,a,xiao)

// #定义变量e，为在a每个元素前面增加-L符号，结果是：-Lfolder -Ldirectory
// e:=$(patsubst%,-L%,$(a))

// #打印变量e的内容
// $(info e=$(e))
// @echo  e=$(e)   这个好像也可以

// Makefile基础-总结
// 1、变量赋值有4种方式var=123、var:=123、var?=123、var+=123
// 其中var:=123常用
// var+=123常用
// 2、取变量值有两种,$(var),${var}。小括号大括号均可以
// 3、数据类型只有字符串和字符串数组，空格隔开表示多个元素
// 4、$(function arguments)是调用make内置函数的方法，具体可以参考：
// http://www.gnu.org/software/make/manual/make.html#Syntax-of-Functions
// 有函数大全。但是常用的其实只有少数两个即可
// 5、依赖关系定义中，如果代码修改时间比生成的更新/生成不存在时，command会执行。
// 否则只会打印main.o is up to date这是makefile解决的核心问题。
// 6、依赖关系可以链式的定义，即b依赖a，c依赖b，而make会自动链式的查找并根据时间执行command
// 7、command时shell命令，可以使用$(var)来将变量用到其中。
// 前面加@表示执行 执行时不打印原指令内容。否则默认打印指令后再执行指令
// 8、make不写具体生成名称，则会选择依赖关系中的第一项生成

// 4、基于Makefile的标准工程结构
// 一个标准工程，我们做如下定义：
// 1、具有src目录，存放我们的代码，可能有多级，例如main.cpp,foo/foo.cpp等
// 2、具有workspace目录，存放我们编译后的可执行程序、资源、数据
// 3、具有objs目录，存放由cpp编译后得到的o文件等中间文件
// 4、.vscode目录，存放vscode的cpp配置，用于语法解析器。vscode的c++插件所使用。ctrl+shift+p后搜索c++，找到JSON那一项就是
// 5、Makefile文件，当前工程的Makefile代码

// 实现例子在Makefile里面

// 5、基于Makefile实现的完整功能项目
// 实现的目的：
// 1、具有两个依赖，openssl、libcurl
// 2、存在include、libs依赖
// 3、可以锻炼一个完整的相对完善的工程案例。还可以锻炼到代码调试

// 实现的效果：
// 实现一个程序，可以从任何网站上下载东西

// 准备：
// 1、下载openssl:https://www.openssl.org/source/old/1.1.1/openssl-1.1.1j.tar.gz
// 2、下载libcurl:https://curl.se/download/curl-7.78.0.tar.gz

// 创建build目录，用于储存下载后的文件，准备用来编译
// 创建lean目录，用于存放编译后的结果，作为依赖项目录
// 将下载后的.tar.gz放到build目录下，并解压出来

// 6、分析程序依赖项、readelf、ldd
// 例如使用readelf -d指令，我们可以查看到程序依赖的so文件以及run path路径
// ldd指令可以查看该程序依赖的so，查找到的具体路径。看看是否符合预期
// 因为 如果不符合预期，会发生莫名其妙的错误，这个千万要不得。我们一定要让他符合我们的预期

// 7、配置C++的调试功能
// 使用vscode调试功能

// 8、头文件修改cpp自动编译的处理方法
// 问题:如果增加a.o : a.hpp。但是a.hpp来自于a.cpp的#include语法。我们可能随时修改a.cpp依赖不同的hpp文件，如何自动完成？
// 可以每次make clean，但不是很好的办法
// 使用下面的命令
// g++ -MM a.cpp -MF a.mk -MT prefix/a.o
// 该指令产生a.mk文件，里面写了a.cpp依赖的头文件
// a.mk本身就是makefile语法。他解析了a.cpp的全部头文件后分析
// 所有依赖的头文件，并写成makefile语法
// -MM分析 #include "a.hpp" ,引号类型的包含
// -M 分析全部包含，尖括号和引号
// -MF 指定储存路径
// -MT 指定依赖项名称

// foreach 函数使用
// foreach 函数和别的函数非常的不一样。因为这个函数是用来做循环用的，
// Makefile中的foreach函数几乎是仿照于Unix标准Shell （/bin/sh）中的for语句，或是C-Shell（/bin/csh）中的foreach语句而构建的。
// 它的语法是：$(foreach <var>,<list>,<text>)
// 这个函数的意思是，把参数<list>;中的单词逐一取出放到参数<var>;所指定的变量中，
// 然后再执行< text>;所包含的表达式。每一次<text>;会返回一个字符串，循环过程中，<text>;
// 的所返回的每个字符串会以空格分隔，最后当整个循环结束时，<text>;
// 所返回的每个字符串所组成的整个字符串（以空格分隔）将会是foreach函数的返回值。
// 所以，<var>;最好是一个变量名，<list>;可以是一个表达式，而<text>;中一般会使用<var>;
// 这个参数来依次枚举<list>;中的单词。举个例子：
//     names := a b c d
//     files := $(foreach n,$(names),$(n).o)
// 上面的例子中，$(name)中的单词会被挨个取出，并存到变量“n”中，“$(n).o”每次根据“$(n)”计算出一个值，
// 这些值以空格分隔，最后作为foreach函数的返回，所以，$(files)的值是“a.o b.o c.o d.o”。
// 注意，foreach中的<var>;参数是一个临时的局部变量，foreach函数执行完后，参数<var>;的变量将不在作用，
// 其作用域只在foreach函数当中。