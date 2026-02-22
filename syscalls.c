// /* Support files for GNU libc.  Files in the system namespace go here.
//    Files in the C namespace (ie those that do not start with an
//    underscore) go in .c.  */
// #include "USART.h"

#include <_ansi.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#include <errno.h>
#include <reent.h>
#include <unistd.h>
#include <sys/wait.h>

/* Register name faking - works in collusion with the linker.  */
register char * stack_ptr __asm__ ("sp");

// extern long *_endof_sram,*end;/* Defined by the linker.  */

// /* Forward prototypes.  */
// int	_system		(const char *);
// int	_rename		(const char *, const char *);
// int	_isatty		(int);
// clock_t _times		(struct tms *);
// int	_gettimeofday	(struct timeval *, void *);
// int	_unlink		(const char *);
// int	_link		(const char *, const char *);
// int	_stat		(const char *, struct stat *);
// int	_fstat		(int, struct stat *);
// int	_swistat	(int fd, struct stat * st);
void *	_sbrk		(ptrdiff_t);/* Heap management */
// pid_t	_getpid		(void);
// int	_close		(int);
// clock_t	_clock		(void);
// int	_swiclose	(int);
// int	_open		(const char *, int, ...);
// int	_swiopen	(const char *, int);
int	_write		(int, const void *, size_t);
// int	_swiwrite	(int, const void *, size_t);
// _off_t	_lseek		(int, _off_t, int);
// _off_t	_swilseek	(int, _off_t, int);
// int	_read		(int, void *, size_t);
// int	_swiread	(int, void *, size_t);
// void	initialise_monitor_handles (void);


void * _sbrk (ptrdiff_t incr)
{
  // extern char   end asm ("_end"); /* Defined by the linker. _end只是一个符号,此处任意定义一个变量来代表这个符号即可,取地址即表示符号地址;或定义一个任意数据类型的指针型变量,可直接表示该符号的地址  */
  extern char  end __asm__ ("_end"); /* Defined by the linker.    _end只是一个符号,此处任意定义一个变量来代表这个符号即可,取地址即表示符号地址;或定义一个任意数据类型的指针型变量,可直接表示该符号的地址  */
  static char * heap_end;
  char *        prev_heap_end;

  if(heap_end == NULL) heap_end = (char *)&end;

  prev_heap_end = heap_end;

  if (heap_end + incr > stack_ptr)
    {
      /* Some of the libstdc++-v3 tests rely upon detecting
         out of memory errors, so do not abort here.  */
#if 0
      extern void abort (void);

      _write (1, "_sbrk: Heap and stack collision\n", 32);

      abort ();
#else
      errno = ENOMEM;
      return (void *) -1;
#endif
    }

  heap_end += incr;

  return (void *) prev_heap_end;
}


// int _write(int fd, const void *ptr, size_t len) {
//     if (fd == STDOUT_FILENO || fd == STDERR_FILENO) { // stdout=1, stderr=2
//         // const char *buf = (const char *)ptr;
//         usart2_send_Hex((uint8_t *)ptr, len);
//         // for (size_t i = 0; i < len; i++) {
//         //     usart2_send_Char(buf[i]); // 你的 UART 发送函数
//         // }
//         return len; // 返回写入的字节数
//     }
//     errno = EBADF; // 不支持其他文件描述符
//     return -1;
// }



int _close(int file) {
    (void)file;
    errno = EBADF;
    return -1;
}

off_t _lseek(int file, off_t offset, int whence) {
    (void)file;(void)offset;(void)whence;
    errno = EBADF;
    return -1;
}

int _read(int file, char *ptr, int len) {
    (void)file;(void)ptr;(void)len;
    errno = EBADF;
    return 0;
}

int _fstat(int file, struct stat *st) {
    (void)file;
    st->st_mode = S_IFCHR; // 假装是字符设备
    return 0;
}

int _isatty(int file) {
    (void)file;
    return 1; // 假装是终端
}

void _exit(int status) {
    (void)status;
    while (1) { // 无限循环，模拟程序终止
        __asm__("nop");
    }
}

int _kill(int pid, int sig) {
    (void)pid;(void)sig;
    errno = EINVAL;
    return -1;
}

int _getpid(void) {
    return 1; // 返回一个默认PID
}