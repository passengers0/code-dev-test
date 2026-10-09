/*
 * Minimal bare-metal POSIX syscall stubs for the end-to-end smoke test.
 * picolibc's stdio (libc_stdio_posixiob_stdout.c) references the
 * POSIX names read/write/lseek/close on a bare-metal target.
 */
#include <errno.h>

int read(int file, char *ptr, int len)
{
    (void)file; (void)ptr; (void)len;
    errno = ENOSYS;
    return -1;
}

int write(int file, const char *ptr, int len)
{
    (void)file; (void)ptr; (void)len;
    return len; /* swallow output */
}

int lseek(int file, int ptr, int dir)
{
    (void)file; (void)ptr; (void)dir;
    return 0;
}

int close(int file)
{
    (void)file;
    return -1;
}

void _exit(int status)
{
    (void)status;
    for (;;)
        ;
}

void _fstat(int file, void *st)
{
    (void)file; (void)st;
}

int _isatty(int file)
{
    (void)file;
    return 1;
}
