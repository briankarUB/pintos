#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

static void syscall_handler (struct intr_frame *);

static int write (int fd, const char *buffer, unsigned size);
static void exit (int status);

void
syscall_init (void)
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f)
{
  uint32_t *args = (uint32_t *) f->esp;
  uint32_t ret = f->eax;

  // printf ("system call %d!\n", args[0]);

  switch (args[0])
  {
    case SYS_WRITE:
      ret = write ((int) args[1], (const char *) args[2], (unsigned) args[3]);
      break;
    case SYS_EXIT: /* TODO */
      exit ((int) args[1]);
      break;
    default: /* TODO */
      PANIC ("Unhandled system call");
      break;
  }

  f->eax = ret;
}

static int
write (int fd, const char *buffer, unsigned size)
{
  switch (fd)
  {
    case 1:
      putbuf (buffer, size);
      return size;
    default:
      PANIC ("Unknown fd for SYS_WRITE call");
  }
}

static void
exit (int status)
{
  printf ("%s: exit(%d)\n", thread_current ()->name, status);
  thread_exit ();
}
