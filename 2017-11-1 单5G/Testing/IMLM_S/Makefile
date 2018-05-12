#定义了一个变量SRCS，它的值就是当前面目录下面所有的以.c结尾的源文件。
SRCS = $(wildcard *.c)
#变量OBJS的值就是将SRCS里面所有.c文件编译出的.o目标文件
OBJS = $(SRCS:.c = .o)
#变量CC代表我们要使用的编译器，CC是环境变量
CC = gcc
#这里指定除了编译器默认的头文件和库文件的路径之外需要额外引用的头文件路径以及库的路径，×××表示路径
INCLUDES = -I/×××
LIBS = -L/×××
#CCFLAGS变量存放的是编译（gcc）选项，CCFLAGES是环境变量，-O0表示没有优化code不加也行
CCFLAGS = -g -Wall -std=c99
#$^代表$(OBJS),所有依赖的集合，$@代表tcpsrv目标文件集合
IMLM_S : $(OBJS)
	$(CC) $^ -o $@ $(INCLUDES) $(LIBS) $(CCFLAGS)
#$<表示依赖中符合模式(%)的一系列文件集（一个一个取出来）
%.o : %.c
	$(CC) -c $< $(CCFLAGS)

.PHONY:clean
clean:
	-rm IMLM_S *.o

