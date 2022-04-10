#vscode右下角更改space和Tab

# 检索src目录查找cpp为后缀文件，用shell指令的find
srcs := $(shell find src -name "*.cpp")

#将srcs的后缀为.cpp的替换为.o  这里srcs:.cpp=.o意思是srcs里面的所有.cpp结尾的全部变成.o结尾
objs := $(srcs:.cpp=.o) 

#将src/前缀替换为objs/前缀，让o文件放到objs目录下   objs:src/%=objs/%意思将objs里面src/开头的，全部变成objs/
objs:=$(objs:src/%=objs/%)

mks := $(objs:.o=.mk) #将所有objs文件的.o改为.mk，得到mks

include_paths := /data/sxai/makefile/make7/lean/curl7.78.0/include \
				/data/sxai/makefile/make7/lean/openssl-1.1.1j/include #1、增加include_paths选项，因为头文件需要他们

library_paths := /data/sxai/makefile/make7/lean/curl7.78.0/lib \
				/data/sxai/makefile/make7/lean/openssl-1.1.1j/lib #2、增加了ld_librarys选项，因为需要链接他们

ld_librarys := curl ssl crypto

#把每一个头文件路径前面增加-I,库文件路径前面增加-L，链接选项前面加-l
#3、使用$(var:source=dest)语法，将每个路径前面加上特定符号，变为g++的指令需要
#-I是配置头文件路径、-L配置库文件路径、-lname配置依赖的so
#4、增加compile_flags,增加编译选项，例如我们要使用C++11特性等，-w避免警告，-g生成调试信息,-O0优化级别关闭
#5、把编译选项增加到g++编译后面
#6、把链接选项增加到g++链接后面
run_paths     := $(library_paths:%=-Wl,rpath=%)
#增加run_paths变量，他的语法是：g++ main.o test.o -o out.bin -WL,-rpath=/xxx/openssl/lib
include_paths := $(include_paths:%=-I%)
library_paths := $(library_paths:%=-L%)
ld_librarys   := $(ld_librarys:%=-l%)
compile_flags := -std=c++11 -w -g -O0 $(include_paths)
link_flags := $(library_paths) $(ld_librarys) $(run_paths)
#runpath路径，运行时的so查找路径。通过runpath我们避免运行时提示找不到so，也避免把so复制到workspace目录

ifneq($(MAKECMDGOALS),clean)
include $(mks) #包含所有的mks,makefile文件
endif

#定义objs下的o文件，依赖src下对应的cpp文件
# $@ = 左边的生成项
# $< = 右边的依赖项第一个
objs/%.o : src/%.cpp         #代表了通配依赖关系，是模式匹配，%相当于是变量部分
	@echo 编译$< 生成 $@
	@mkdir -p $(dir $@)
	g++ -c $< -o $@ $(compile_flags)

# $^ = 右边依赖项全部
# 我们把pro放到workspace下面
workspace/pro : $(objs)
	@echo 这里的依赖项所有是[$^]
	@echo 链接$@
	g++ $^ -o $@ $(link_flags)

objs/%.mk : src/%.cpp  
# 定义.mk文件的依赖以及生成的方式
# $(@:.mk = .o) , $@是生成项，xxx.mk   修改为xxx.o，此时携带路径 例如objs/main.mk
	@echo 生成依赖项    
	@mkdir -p $(dir $@)
	@g++ -MM $< -MF $@ -MT $(@:.mk = .o)

#定义简洁指令，make pro即可生成程序
pro : workspace/pro
	@echo 编译完成

#定义make run,编译好pro后顺便给我执行一下呗
run : pro
	@cd workspace && ./pro

#定义make clean,清理掉编译留下的垃圾
clean:
	@rm -rf workspace/pro objs

#定义伪符号，这些符号不视为文件，视作指令
#也可以说，视作永远不存在的文件
.PHONY : pro run debug clean

debug:
	@echo objs is [$(srcs)]