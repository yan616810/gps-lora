CC = gcc
CFLAGS = -Wall

Include := . inc Include src lib 
Iflag := $(Include:%=-I%)

Source_files := $(notdir $(shell dir /s /b *.c))#/b 选项：简洁输出格式   /s 递归搜索子目录
# 正确替换：添加 .o 扩展名，保持路径结构
Object_files := $(patsubst %.c,%.o,$(Source_files))
# 添加 objs 前缀（使用正斜杠）
Object_files := $(addprefix objs\,$(Object_files))


exe:=gps.exe
all : $(exe)
$(exe) : $(Source_files)
	$(CC) $(CFLAGS) $(Iflag) $^ -o $@ 


debug:
	@echo $(Include)
	@echo $(Iflag)
	@echo $(Source_files)
	@echo $(Object_files)
	@echo $(CC) $(CFLAGS) $(Iflag) $(Source_files) -o $(exe)

clean:
	@del /Q $(exe) *.o 2>nul
	@echo Project cleaned.