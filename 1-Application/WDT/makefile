# 预定义变量 指令编译器和选项
CC = arm-fsl-linux-gnueabi-gcc			#C 语言编译程序
CFLAGS = -Wall -g	#传递给 CC 程序的标志
LDFLAGS = 	#传递给链接程序的标志

# 自定义变量，目标文件, 变量引用为$(变量)
TARGET = wdt			
SRC = wdt.c

#头文件路径
INC = -I ./ #-I ./testadd -I ./testsub


# makefile规则，第一个目标文件为make默认的执行目标
$(TARGET):$(SRC)
	$(CC) $(LDFLAGS) $(INC) -o $@ $^		# $@目标文件，$^包含所有的依赖文件

.PHONY:clean		#将目标文件clean设置为伪目标，该目标的规则被执行后不产生文件
clean:				#makefile规则
	rm -f $(TARGET) *.o	#*.o是指任意.o文件