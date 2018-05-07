#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "devices/shutdown.h"
#include "filesys/filesys.h"
#include "userprog/process.h"
#include "threads/vaddr.h"
#include "userprog/pagedir.h"
#include <string.h>

#define VERIFY(PTR) \
  if ((PTR) == NULL) exit (-1); \
  if (!is_user_vaddr ((PTR))) exit (-1); \
  if (pagedir_get_page (thread_current ()->pagedir, (PTR)) == NULL) exit (-1)

static void syscall_handler (struct intr_frame *);

/* Syscalls */
static void halt (void);
static tid_t exec (const char *cmd_line);
static int wait (tid_t pid);
static bool create (const char *file, unsigned initial_size);
static bool remove (const char *file);
static int open (const char *file);
static int filesize (int fd);
static int read (int fd, void *buffer, unsigned size);
static int write (int fd, const void *buffer, unsigned size);
static void seek (int fd, unsigned position);
static unsigned tell (int fd);
static void close (int fd);

struct lock filesys_lock;

struct p_file {
  struct file *file;
  int fd;
  struct list_elem elem;
};

void
syscall_init (void)
{
  lock_init(&filesys_lock);
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f)
{
  uint32_t *args = (uint32_t *) f->esp;

  /* Dispatch based on syscall number. */
  switch (args[0])
    {
      case SYS_HALT:
        halt ();
        return;
      case SYS_EXIT:
        exit((int) args[1]);
        return;
      case SYS_EXEC:
        f->eax = exec ((const char *) args[1]);
        return;
      case SYS_WAIT:
        f->eax = wait ((tid_t) args[1]);
        return;
      case SYS_CREATE:
        f->eax = create ((const char *) args[1], (unsigned) args[2]);
        return;
      case SYS_REMOVE:
        f->eax = remove ((const char *) args[1]);
        return;
      case SYS_OPEN:
        f->eax = open ((const char *) args[1]);
        return;
      case SYS_FILESIZE:
        f->eax = filesize ((int) args[1]);
        return;
      case SYS_READ:
        f->eax = read ((int) args[1], (void *) args[2], (unsigned) args[3]);
        return;
      case SYS_WRITE:
        f->eax = write ((int) args[1], (const void *) args[2], (unsigned) args[3]);
        return;
      case SYS_SEEK:
        seek ((int) args[1], (unsigned) args[2]);
        return;
      case SYS_TELL:
        f->eax = tell ((int) args[1]);
        return;
      case SYS_CLOSE:
        close ((int) args[1]);
        return;
      default:
        PANIC ("Unknown syscall");
        return;
    }
}

static void
halt (void)
{
  shutdown_power_off ();
}

void
exit (int status) /* TODO: Make sure this plays nicely with wait. */
{
  struct thread *cur = thread_current ();
  struct thread_exit_block *block = thread_get_exit_block (cur->tid);
  block->status = status;

  printf ("%s: exit(%d)\n", cur->name, status);
  thread_exit ();
}

static int
write (int fd, const void *buffer, unsigned size)
{
  VERIFY (buffer);

  if (fd == 0)
    {
      exit (-1);
    }
  else if (fd == 1)
    {
      putbuf (buffer, size);
      return size;
    }
  else /* TODO: Implement write for fd != 1 */
    {
      PANIC ("write - Not implemented for fd != 1");
    }
}

static tid_t exec (const char *cmd_line)
{
  VERIFY (cmd_line);

  return process_execute (cmd_line);
}

static int wait (tid_t pid)
{
  return process_wait (pid);
}

static bool create (const char *file, unsigned initial_size)
{
  VERIFY (file);

  return filesys_create (file, initial_size);
}

static bool remove (const char *file)
{
  VERIFY (file);

  return filesys_remove (file);
}

static int open (const char *file)
{
  VERIFY (file);

  if (strcmp (file, "") == 0)
    exit (1);
    lock_acquire(&filesys_lock);
  struct file *f = filesys_open (file);
  struct p_file *pf = malloc(sizeof(struct p_file));
  pf->file = f;
  pf->fd = thread_current()->fd;
  thread_current()->fd++;
  list_push_back(&thread_current()->file_list, &pf->elem);
  lock_release(&filesys_lock);
  return pf->fd;

}

static int filesize (int fd)
{
  PANIC ("filesize - Not implemented"); /* TODO */
}

static int read (int fd, void *buffer, unsigned size)
{
  VERIFY (buffer);

  /* Can't read from STDOUT. */
  if (fd == 1)
    exit (-1);

  PANIC ("read - Not implemented"); /* TODO */
}

static void seek (int fd, unsigned position)
{
  PANIC ("seek - Not implemented"); /* TODO */
}

static unsigned tell (int fd)
{
  PANIC ("tell - Not implemented"); /* TODO */
}

static void close (int fd)
{
  /* Can't close STDIN or STDOUT. */
  if (fd == 0 || fd == 1)
    exit (-1);

  PANIC ("close - Not implemented"); /* TODO */
}
