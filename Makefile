# 1.发布时建议注释掉177行的生成单文件源码与汇编对照文件.lst，以加快编译速度并减少磁盘空间占用
# 2.第273行，连接器用不到编译器参数，即用不到$(ALL_CFLAGS)，只需要将$(ALL_CFLAGS)替换成-mcpu=$(CPU) $(THUMBIW)即可
# 3.Windows 下输出空行的正确方法echo.

### Project name (also used for output file name)
PROJECT	= GPS_LoRa

### C Standard level (c89, gnu89, c99 or gnu99)
CSTD = c11

### Optimization level (0, 1, 2, 3, g or s)
OPTIMIZE = s

### Processor type and Thumb(-2) mode for CSRC/ASRC files (YES or NO)
CPU   = cortex-m3
THUMB = YES

### Linker script for the target MCU
LINKSCRIPT = STM32F103VET6.ld

### Object output directory
OBJDIR = obj

### Warning controls
WARNINGS = all extra unused # error

### Output file type (hex, bin or both) and debugger type
OUTPUT	= hex
DEBUG	= dwarf-4 #dwarf-2 #dwarf-3
ASM_DEBUG = dwarf-2 #兼容性更好一些

# Default target.
all: version build size#在终端只执行make时，默认执行version build size；不要加 clean，破坏增量构建并影响并行-j


### Include dirs, library dirs and definitions
DEFS	=
ADEFS	=
MATHLIB	= -lm
LIBS	=
LIBDIRS	=
INCDIRS := \
			start/ \
			SPL/STM32F10x_StdPeriph_Driver/inc/ \
			SPL/CMSIS/CM3/ \
			sys_hardware \
			protocol \
			board_hardware \
			lib/u8g2/csrc \
			lib/lwrb/src/include/lwrb \
			lib/lwgps \
# 			lib/mpu6050 \


### Source files and search directories
#匹配$(csrc/*.c)文件
CSRC =	\
		$(wildcard *.c) \
    	$(wildcard start/*.c) \
    	$(wildcard SPL/STM32F10x_StdPeriph_Driver/src/*.c) \
    	$(wildcard SPL/CMSIS/CM3/*.c) \
		sys_hardware/Delay.c \
		protocol/iic.c \
		board_hardware/OLED.c \
		sys_hardware/USART.c \
		sys_hardware/hw_iic.c \
		board_hardware/key.c \
		sys_hardware/hw_spi.c \
		board_hardware/LCD.c \
		board_hardware/u8g2_monochrome_display.c \
		lib/u8g2/csrc/u8x8_d_ssd1306_128x64_noname.c \
		lib/u8g2/csrc/u8g2_arc.c \
		lib/u8g2/csrc/u8g2_bitmap.c \
		lib/u8g2/csrc/u8g2_box.c \
		lib/u8g2/csrc/u8g2_buffer.c \
		lib/u8g2/csrc/u8g2_button.c \
		lib/u8g2/csrc/u8g2_circle.c \
		lib/u8g2/csrc/u8g2_cleardisplay.c \
		lib/u8g2/csrc/u8g2_d_memory.c \
		lib/u8g2/csrc/u8g2_d_setup.c \
		lib/u8g2/csrc/u8g2_font.c \
		lib/u8g2/csrc/u8g2_fonts.c \
		lib/u8g2/csrc/u8g2_hvline.c \
		lib/u8g2/csrc/u8g2_input_value.c \
		lib/u8g2/csrc/u8g2_intersection.c \
		lib/u8g2/csrc/u8g2_kerning.c \
		lib/u8g2/csrc/u8g2_line.c \
		lib/u8g2/csrc/u8g2_ll_hvline.c \
		lib/u8g2/csrc/u8g2_message.c \
		lib/u8g2/csrc/u8g2_polygon.c \
		lib/u8g2/csrc/u8g2_selection_list.c \
		lib/u8g2/csrc/u8g2_setup.c \
		lib/u8g2/csrc/u8x8_8x8.c \
		lib/u8g2/csrc/u8x8_byte.c \
		lib/u8g2/csrc/u8x8_cad.c \
		lib/u8g2/csrc/u8x8_capture.c \
		lib/u8g2/csrc/u8x8_debounce.c \
		lib/u8g2/csrc/u8x8_display.c \
		lib/u8g2/csrc/u8x8_fonts.c \
		lib/u8g2/csrc/u8x8_gpio.c \
		lib/u8g2/csrc/u8x8_input_value.c \
		lib/u8g2/csrc/u8x8_message.c \
		lib/u8g2/csrc/u8x8_selection_list.c \
		lib/u8g2/csrc/u8x8_setup.c \
		lib/u8g2/csrc/u8x8_string.c \
		lib/u8g2/csrc/u8x8_u8toa.c \
		lib/u8g2/csrc/u8x8_u16toa.c \
		sys_hardware/DMA_USART.c \
		lib/lwrb/src/lwrb/lwrb.c \
		lib/lwrb/src/lwrb/lwrb_ex.c \
		lib/lwgps/lwgps.c \
		board_hardware/GPS.c \
		board_hardware/bmp280.c \
		lib/WMM_Tiny/Core/Src/wmm.c \
		lib/WMM_Tiny/Core/Src/WMM_COF.c \
# 		lib/mpu6050/mpu6050.c \
# 		lib/mpu6050/inv_mpu_dmp_motion_driver.c \
# 		lib/mpu6050/inv_mpu.c \
# 		lib/mpu6050/mpu6050_dmp.c \






#匹配$(asrc/*.S)文件
ASRC	=$(wildcard start/*.S)
CSRCARM	=
ASRCARM	=


### Search directories (make uses VPATH to search source files)
# VPATH   = start/ SPL/STM32F10x_StdPeriph_Driver/src/ SPL/CMSIS/CM3/ #linux只能使用:当分隔符
vpath %.c $(sort $(dir $(CSRC)))#提取目录，sort去重


### Programs to build porject
CC      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
OBJDUMP = arm-none-eabi-objdump
SIZE    = arm-none-eabi-size
NM      = arm-none-eabi-nm


# Define all object files
COBJ      = $(CSRC:.c=.o) 
AOBJ      = $(ASRC:.S=.o)
COBJARM   = $(CSRCARM:.c=.o)
AOBJARM   = $(ASRCARM:.S=.o)
# Define all object files with directory
COBJ_dir  := $(addprefix $(OBJDIR)/,$(notdir $(COBJ)))
AOBJ_dir  := $(addprefix $(OBJDIR)/,$(notdir $(AOBJ)))
COBJARM_dir   := $(addprefix $(OBJDIR)/,$(notdir $(COBJARM)))
AOBJARM_dir   := $(addprefix $(OBJDIR)/,$(notdir $(AOBJARM)))
PROJECT_dir   := $(OBJDIR)/$(PROJECT)


### Compiler flags
ifeq ($(THUMB),YES)
THUMBFLAG = -mthumb #指示编译器生成 Thumb 指令集的代码
THUMBIW = -mthumb-interwork #允许 ARM 和 Thumb 代码之间的相互调用
else
THUMBFLAG =
THUMBIW =
endif


# Flags for C files
CFLAGS += -std=$(CSTD)
CFLAGS += -g$(DEBUG)
CFLAGS += -O$(OPTIMIZE)
CFLAGS += $(addprefix -W,$(WARNINGS))
CFLAGS += $(addprefix -I,$(INCDIRS))
CFLAGS += $(addprefix -D,$(DEFS))
# CFLAGS += -fno-diagnostics-color #禁止错误有颜色
CFLAGS += -fdiagnostics-color=always #始终使用错误颜色
##将每个函数或数据放在单独的节中，以便链接器通过参数-wl,-gc-section可以删除未使用的函数或数据，从而减小最终可执行文件的大小
CFLAGS += -ffunction-sections -fdata-sections
##生成依赖文件，用于自动管理头文件依赖关系，方便增量编译，根据时间戳只重新编译修改过的文件及其依赖文件
CFLAGS += -Wp,-MM,-MP,-MT,$(OBJDIR)/$(*F).o,-MF,$(OBJDIR)/$(*F).d#-Wp:表示将后面的选项传递给预处理器 -MM:生成依赖关系 -MP:生成伪目标 -MT:指定目标文件 -MF:指定依赖文件的名称
##发布阶段（建议直接注释下面-Wa参数，禁用汇编器生成单文件源码与汇编对照文件.lst）
# CFLAGS += -Wa,-a,-ad,-alms=$(OBJDIR)/$(notdir $(<:.c=.lst))#-Wa:表示将后面的选项传递给汇编器 查看单文件源码与汇编对应
CFLAGS += -Wa,-ahls=$(OBJDIR)/$(notdir $(<:.c=.lst)) #h=C代码, l=汇编指令, m=将宏展开, s=符号表
##gnu arm默认禁止宽字符，从而确保 printf 输出中文字符时以单字节形式（例如 UTF-8 或 GBK 编码的原始字节）发送到串口，而不是被解析为多字节字符。
##vscode 里面的串口解析插件有bug，不能设置接收时间间隔，而导致中文字符乱码，使用vofa+设置接收时间间隔可以正确显示中文字符
CFLAGS += -Wno-missing-braces #这个警告 -Wmissing-braces 表示多维数组初始化时缺少了内层的大括号,加上这个标志后不报错

# Assembler flags
ASFLAGS += $(addprefix -D,$(ADEFS)) -g$(ASM_DEBUG)
ASFLAGS += -ffunction-sections -fdata-sections


# Linker flags
##GCC 前端选项：直接传递（-specs, -T, -L, -l, -nostartfiles）
LDFLAGS += -T$(LINKSCRIPT)		# 1. 指定链接脚本
# LDFLAGS += -nostartfiles -Wl,-Map=$(PROJECT_dir).map,--cref,--gc-sections #-nostartfiles 是 GCC 前端选项，不在 binutils/ld 手册中
# LDFLAGS += -nostartfiles        # 2. GCC前端选项，不使用标准启动文件,但是堆标准库函数化代码都在nano库中，因此必须注释掉此行
LDFLAGS += -specs=nano.specs # 使用类似微库的newlib-nano库  or 完整、标准的 newlib（所有功能都开）使用-specs=nosys.specs
LDFLAGS += $(MATHLIB)
LDFLAGS += $(patsubst %,-L%,$(LIBDIRS)) $(patsubst %,-l%,$(LIBS))
##链接器选项：通过 -Wl,option传递（--gc-sections, -Map, --cref）
LDFLAGS += -Wl,-Map=$(PROJECT_dir).map,--cref,--gc-sections#-Wl:将后面逗号分隔的选项传递给链接器


# Combine all necessary flags and optional flags.
# Add target processor to flags.
ALL_CFLAGS  = -mcpu=$(CPU) $(THUMBIW) -I. $(CFLAGS)
ALL_ASFLAGS = -mcpu=$(CPU) $(THUMBIW) -I. -x assembler-with-cpp $(ASFLAGS)


# Display compiler version information.
version :
	@$(CC) --version


# Display size of file.
size:$(PROJECT_dir).elf
	@echo "-----------------------------"
	$(SIZE) -A --radix=16 $<
	@echo "-----------------------------"
	$(SIZE) -A --radix=10 $<
	@echo "-----------------------------"
	$(SIZE) -B --radix=10 $<


ifeq ($(OUTPUT),hex)
build: elf hex lst sym
hex: $(PROJECT_dir).hex
else
ifeq ($(OUTPUT),bin)
build: elf bin lst sym
bin: $(PROJECT_dir).bin
else
ifeq ($(OUTPUT),both)
build: elf hex bin lst sym
hex: $(PROJECT_dir).hex
bin: $(PROJECT_dir).bin
else
$(error "Invalid format: $(OUTPUT)")
endif
endif
endif


elf: $(PROJECT_dir).elf
lst: $(PROJECT_dir).lst
sym: $(PROJECT_dir).sym


# Create final output file (.hex or .bin) from ELF output file.
%.hex: %.elf | $(OBJDIR)
# 	@echo.
	@echo
	$(OBJCOPY) -O ihex $< $@

%.bin: %.elf | $(OBJDIR)
# 	@echo.
	@echo
	$(OBJCOPY) -O binary $< $@

# Create extended listing file from ELF output file.
%.lst: %.elf | $(OBJDIR)
# 	@echo.
	@echo
	$(OBJDUMP) -h -S -C $< > $@

# Create a symbol table from ELF output file.
%.sym: %.elf | $(OBJDIR)
# 	@echo.
	@echo
	$(NM) -n $< > $@

# Link: create ELF output file from object files.
%.elf:  $(AOBJARM_dir) $(AOBJ_dir) $(COBJARM_dir) $(COBJ_dir) | $(OBJDIR)
# 	@echo.
	@echo
	@echo Linking...
	$(CC) $(THUMBFLAG) -mcpu=$(CPU) $(THUMBIW) $^ -o $@ $(LDFLAGS)

# Compile: create object files from C source files. ARM or Thumb(-2)
$(COBJ_dir) : $(OBJDIR)/%.o : %.c | $(OBJDIR)
# 	@echo.
	@echo
	@echo $< :
	$(CC) -c $(THUMBFLAG) $(ALL_CFLAGS) $< -o $@

# Compile: create object files from C source files. ARM-only
$(COBJARM_dir) : $(OBJDIR)/%.o : %.c | $(OBJDIR)
# 	@echo.
	@echo
	@echo $< :
	$(CC) -c $(ALL_CFLAGS) $< -o $@ 

# Assemble: create object files from assembler source files. ARM or Thumb(-2)
$(AOBJ_dir) : $(OBJDIR)/%.o : %.S | $(OBJDIR)
# 	@echo.
	@echo
	@echo $< :
	$(CC) -c $(THUMBFLAG) $(ALL_ASFLAGS) $< -o $@

# Assemble: create object files from assembler source files. ARM-only
$(AOBJARM_dir) : $(OBJDIR)/%.o : %.S | $(OBJDIR)
# 	@echo.
	@echo
	@echo $< :
	$(CC) -c $(ALL_ASFLAGS) $< -o $@


# Target: clean project.
clean:
# 	@echo.
	@echo
	@echo Cleaning project...
# 	rmdir /S /Q $(OBJDIR) | exit 0
	rm -f -r $(OBJDIR) | exit 0 
# 	等效于-rm -f -r $(OBJDIR)


# Include the dependency files.
# # 确保目录存在的目标
# $(OBJDIR):
# 	@if not exist $(OBJDIR) mkdir $(OBJDIR)
$(OBJDIR):
	@mkdir -p $(OBJDIR)
# 包含依赖文件
-include $(wildcard $(OBJDIR)/*.d)


### help
help : 
	@echo "***********************************************"
	@echo Available targets:
	@echo	'all      -    (Default) Build all targets,includes(version,build,size)'
	@echo	'clean    -    Remove all generated files'
	@echo	'version  -    Display compiler version'
	@echo	'build    -    Build elf and lst, sym and hex/bin files'
	@echo	'elf      -    Build ELF file'
	@echo	'hex      -    Build HEX file'
	@echo	'bin      -    Build BIN file'
	@echo	'lst      -    Generate listing file'
	@echo	'sym      -    Generate symbol file'
	@echo	'size     -    Display size of the output file'
	@echo	'flash    -    Flash the program to the device'
	@echo '***********************************************'
	@echo *************all .c file with dir**************
	@echo $(CSRC)
	@echo ****************all .c file********************
	@echo $(notdir $(CSRC))
	@echo *******************obj************************
	@echo $(COBJ_dir)
	@echo *******************vpath***********************
	@echo $(sort $(dir $(CSRC)))

#program $< verify exit->烧录后验证；program $< verify reset exit->烧录后验证并复位运行
flash : $(PROJECT_dir).elf
	@openocd -f interface/cmsis-dap.cfg -f target/stm32f1x.cfg -c "program $< verify reset exit"

flash_stm32flash : $(PROJECT_dir).hex
	@stm32flash -w $< -v -g 0x8000000 /dev/ttyUSB0

.PHONY: all version build size elf hex bin lst sym clean help flash