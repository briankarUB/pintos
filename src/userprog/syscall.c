#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f) 
{
  uint32_t *args = ((uint32_t *) f->esp); /* Args are 32-bit words on stack. */

  printf ("system call %d!\n", args[0]);

  switch (args[0])
  {
    case SYS_WRITE: /* TODO */
      printf ("write (%d, \"%s\", %u)\n", (int) args[3], (const char *) args[2], (unsigned) args[1]);
      break;
    case SYS_EXIT: /* TODO */
      thread_exit ();
      break;
    default: /* TODO */
      PANIC ("Unhandled system call!");
      break;
  }
}
