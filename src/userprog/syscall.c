#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "devices/shutdown.h"

#define SYSCALL(NUM, RETURNS) \
  static uint32_t syscall_##NUM##__ (uint32_t *args UNUSED, size_t idx__ UNUSED)

#define REGISTER(NUM, RETURNS) \
  DISPATCH_TABLE[NUM].fn = syscall_##NUM##__; \
  DISPATCH_TABLE[NUM].returns = RETURNS

#define ARG(TYPE, NAME) TYPE NAME = (TYPE) args[++idx__]

struct strategy
  {
    uint32_t (*fn) (uint32_t *, size_t);
    bool returns;
  };

static struct strategy DISPATCH_TABLE[20];

static void syscall_handler (struct intr_frame *);

SYSCALL (SYS_HALT, false)
{
  shutdown_power_off ();
}

SYSCALL (SYS_EXIT, false) /* TODO: Wake up waiting threads? */
{
  ARG (int, status);

  printf ("%s: exit(%d)\n", thread_current ()->name, status);
  thread_exit ();
}

SYSCALL (SYS_WRITE, true) /* TODO: Handle other fd besides 1 */
{
  ARG (int, fd);
  ARG (const char *, buffer);
  ARG (unsigned, size);

  switch (fd)
  {
    case 1:
      putbuf (buffer, size);
      return size;
    default:
      PANIC ("Unknown fd for SYS_WRITE call");
  }
}

void
syscall_init (void)
{
  REGISTER (SYS_HALT, false);
  REGISTER (SYS_EXIT, false);
  REGISTER (SYS_WRITE, true);

  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f)
{
  uint32_t *args = (uint32_t *) f->esp;
  struct strategy strat = DISPATCH_TABLE[args[0]];

  if (strat.returns)
    f->eax = strat.fn (args, 0);
  else
    strat.fn (args, 0);
}
