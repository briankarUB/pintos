#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "devices/shutdown.h"

typedef int pid_t;

#define call0_nr(NAME) NAME ()
#define call1_nr(NAME, T1) NAME ((T1) args[1])
#define call2_nr(NAME, T1, T2) NAME ((T1) args[1], (T2) args[2])
#define call3_nr(NAME, T1, T2, T3) NAME ((T1) args[1], (T2) args[2], (T3) args[3])

#define call0(NAME) (uint32_t) call0_nr (NAME)
#define call1(NAME, T1) (uint32_t) call1_nr (NAME, T1)
#define call2(NAME, T1, T2) (uint32_t) call2_nr (NAME, T1, T2)
#define call3(NAME, T1, T2, T3) (uint32_t) call3_nr (NAME, T1, T2, T3)

static void syscall_handler (struct intr_frame *);

/* Syscalls */
static void halt (void);
static void exit (int status);
static pid_t exec (const char *cmd_line);
static int wait (pid_t pid);
static bool create (const char *file, unsigned initial_size);
static bool remove (const char *file);
static int open (const char *file);
static int filesize (int fd);
static int read (int fd, void *buffer, unsigned size);
static int write (int fd, const void *buffer, unsigned size);
static void seek (int fd, unsigned position);
static unsigned tell (int fd);
static void close (int fd);

void
syscall_init (void)
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f)
{
  uint32_t *args = (uint32_t *) f->esp;

  /* Dispatch based on syscall number.
     TODO: Maybe there's a more modular(?) way to do this, but who cares? */
  switch (args[0])
  {
    case SYS_HALT:
      call0_nr (halt);
      return;
    case SYS_EXIT:
      call1_nr (exit, int);
      return;
    case SYS_EXEC:
      f->eax = call1 (exec, const char *);
      return;
    case SYS_WAIT:
      f->eax = call1 (wait, pid_t);
      return;
    case SYS_CREATE:
      f->eax = call2 (create, const char *, unsigned);
      return;
    case SYS_REMOVE:
      f->eax = call1 (remove, const char *);
      return;
    case SYS_OPEN:
      f->eax = call1 (open, const char *);
      return;
    case SYS_FILESIZE:
      f->eax = call1 (filesize, int);
      return;
    case SYS_READ:
      f->eax = call3 (read, int, void *, unsigned);
      return;
    case SYS_WRITE:
      f->eax = call3 (write, int, const void *, unsigned);
      return;
    case SYS_SEEK:
      call2_nr (seek, int, unsigned);
      return;
    case SYS_TELL:
      f->eax = call1 (tell, int);
      return;
    case SYS_CLOSE:
      call1_nr (close, int);
      return;
    default:
      PANIC ("Non-implemented syscall");
      return;
  }
}

static void
halt (void)
{
  shutdown_power_off ();
}

static void
exit (int status) /* TODO: Make sure this plays nicely with wait. */
{
  printf ("%s: exit(%d)\n", thread_current ()->name, status);
  thread_exit ();
}

static int
write (int fd, const void *buffer, unsigned size)
{
  if (fd == 1)
  {
    putbuf (buffer, size);
      return size;
  }
  else /* TODO: Implement for fd != 1 */
  {
    PANIC ("Unknown fd for SYS_WRITE call");
  }
}

static pid_t exec (const char *cmd_line)
{
  PANIC ("Not implemented"); /* TODO */
}

static int wait (pid_t pid)
{
  PANIC ("Not implemented"); /* TODO */
}

static bool create (const char *file, unsigned initial_size)
{
  PANIC ("Not implemented"); /* TODO */
}

static bool remove (const char *file)
{
  PANIC ("Not implemented"); /* TODO */
}

static int open (const char *file)
{
  PANIC ("Not implemented"); /* TODO */
}

static int filesize (int fd)
{
  PANIC ("Not implemented"); /* TODO */
}

static int read (int fd, void *buffer, unsigned size)
{
  PANIC ("Not implemented"); /* TODO */
}

static void seek (int fd, unsigned position)
{
  PANIC ("Not implemented"); /* TODO */
}

static unsigned tell (int fd)
{
  PANIC ("Not implemented"); /* TODO */
}

static void close (int fd)
{
  PANIC ("Not implemented"); /* TODO */
}
