/* STM32F103VET6 Memory Definitions */

ENTRY(Reset_Handler)

MEMORY
{
  ROM  : org = 0x08000000, len = (512K)   /* Flash: 512k */
  RAM  : org = 0x20000000, len =   (64K)   /* SRAM: 64k */
}

_endof_sram = ORIGIN(RAM) + LENGTH(RAM);    /* End of available SRAM address + 1 */


/* Section Definitions */

SECTIONS
{
  /* .text section has program code, constants and read-only data. */
  /* It is located in ROM. */
  .text :
  {
    KEEP(*(.VECTOR))	/* Exception vector table placed at start of flash memory */
    . = ALIGN(4);
    *(.text .text.*)	/* Program code */
    *(.gnu.linkonce.t*)
    *(.gcc_except_table)
    . = ALIGN(4);
  /* 包含初始化和终止代码，通常是启动代码或 C 库提供的低级初始化/清理函数，包含的是实际的代码或数据
  /* 加上这两句之后__libc_init_array()竟然就可以使用了！！！，C library initializers */
    /* KEEP (*(.init))   
    KEEP (*(.fini))
    . = ALIGN(4); */
  } > ROM

  .rodata :
  {
    . = ALIGN(4);
    *(.rodata .rodata.*)	/*Read-only data (constants)*/
	  *(.gnu.linkonce.r*)
    . = ALIGN(4);
  } > ROM

  .ARM.excep :			/* Data table for library exception handling */
  {
    . = ALIGN(4);
    *(.ARM.exidx*)
    *(.ARM.extab*)
    . = ALIGN(4);
  } > ROM

/* 包含初始化和终止代码，通常是启动代码或 C 库提供的低级初始化/清理函数，包含的是实际的代码或数据
  /* 加上这两句之后__libc_init_array()竟然就可以使用了！！！，C library initializers */
  .init :
  {
    KEEP (*(.init))
  } > ROM
  .fini :
  {
    KEEP (*(.fini))
  } > ROM
  . = ALIGN(4);
/*包含 C/C++ 运行时初始化和终止函数的指针表，专门用于存储函数指针表，格式固定（每个条目是一个函数指针数组）*/
  .preinit_array     :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } >ROM
  .init_array :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
  } >ROM
  .fini_array :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } >ROM

  _etext = .;		/* End of .text section */
  . = ALIGN(4);
  _sidata = .;		/* Start of the initialization values of the .data section */

  /* .data section has static objects with initial value. It is located in RAM */
  /* but the content is placed to ROM area just below .text section. */
  /* This section will be initialized with the ROM data by startup code. */
  .data : AT (_sidata)
  {
	_sdata = .;	/* Start of .data section */
	*(.data .data.*)
	*(.gnu.linkonce.d*)
	. = ALIGN(4);
	_edata = .;	/* End of .data section */
  } > RAM


  /* .bss section has static oobjects with no initial value. */
  /* This section will be filled with zero by startup code to meet ANSI C standard. */
  .bss (NOLOAD) : ALIGN(4)
  {
	_sbss = .;	/* Start of .bss section */
	*(.bss .bss.*)
	*(.gnu.linkonce.b*)
	*(COMMON)
	. = ALIGN(4);
	_ebss = .;	/* End of .bss section */
  } > RAM

  .stack (NOLOAD) : ALIGN(8)/*此处使用8字节对齐为了支持浮点数*/
  {
	*(.stack)
	*(.STACK)
	. = ALIGN(4);
  } > RAM
  
  _end = .;/*看你自己实现的stubs桩函数_sbrk()中使用的是什么符号标记！作用是：标记内存区域的结束地址,常用于 C 库（如 newlib）或你的启动代码里，用来确定堆（heap）或其他动态分配区的起始位置*/
  /* end = .; */

  .heap (NOLOAD) : ALIGN(4)
  {
  _heap_start = .;
  /* . = . + 1K; */
  _heap_end = .;
  } > RAM



  /* Stabs debugging sections.  */
  .stab          0 : { *(.stab) }
  .stabstr       0 : { *(.stabstr) }
  .stab.excl     0 : { *(.stab.excl) }
  .stab.exclstr  0 : { *(.stab.exclstr) }
  .stab.index    0 : { *(.stab.index) }
  .stab.indexstr 0 : { *(.stab.indexstr) }
  .comment       0 : { *(.comment) }
  /* DWARF debug sections.
     Symbols in the DWARF debugging sections are relative to the beginning
     of the section so we begin them at 0.  */
  /* DWARF 1 */
  .debug          0 : { *(.debug) }
  .line           0 : { *(.line) }
  /* GNU DWARF 1 extensions */
  .debug_srcinfo  0 : { *(.debug_srcinfo) }
  .debug_sfnames  0 : { *(.debug_sfnames) }
  /* DWARF 1.1 and DWARF 2 */
  .debug_aranges  0 : { *(.debug_aranges) }
  .debug_pubnames 0 : { *(.debug_pubnames) }
  /* DWARF 2 */
  .debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
  .debug_abbrev   0 : { *(.debug_abbrev) }
  .debug_line     0 : { *(.debug_line) }
  .debug_frame    0 : { *(.debug_frame) }
  .debug_str      0 : { *(.debug_str) }
  .debug_loc      0 : { *(.debug_loc) }
  .debug_macinfo  0 : { *(.debug_macinfo) }
  /* SGI/MIPS DWARF 2 extensions */
  .debug_weaknames 0 : { *(.debug_weaknames) }
  .debug_funcnames 0 : { *(.debug_funcnames) }
  .debug_typenames 0 : { *(.debug_typenames) }
  .debug_varnames  0 : { *(.debug_varnames) }
}
