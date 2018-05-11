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
#include "filesys/file.h"

#define VERIFY(PTR) \
  if ((PTR) == NULL) exit (-1); \
  if (!is_user_vaddr ((PTR))) exit (-1); \
  if (pagedir_get_page (thread_current ()->pagedir, (PTR)) == NULL) exit (-1)

#define VERIFY_VALUE(ARG) \
  if (!is_user_vaddr (&(ARG))) exit (-1)

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

/* Utils */
static struct p_file *get_pf (int fd);
static bool is_pf_valid (const struct p_file *pf);

static struct lock filesys_lock;

struct p_file {
  struct file *file;
  bool valid;
  tid_t owner;
};

#define MAX_FD_COUNT 256
static struct p_file openfiles[MAX_FD_COUNT];

void
syscall_init (void)
{
  lock_init(&filesys_lock);
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f)
{
  VERIFY (f->esp);

  uint32_t *args = (uint32_t *) f->esp;

  /* Dispatch based on syscall number. */
  switch (args[0])
    {
      case SYS_HALT:
        halt ();
        return;
      case SYS_EXIT:
        VERIFY_VALUE (args[1]);
        exit((int) args[1]);
        return;
      case SYS_EXEC:
        f->eax = exec ((const char *) args[1]);
        return;
      case SYS_WAIT:
        VERIFY_VALUE (args[1]);
        f->eax = wait (args[1]);
        return;
      case SYS_CREATE:
        VERIFY_VALUE (args[2]);
        f->eax = create ((const char *) args[1], (unsigned) args[2]);
        return;
      case SYS_REMOVE:
        f->eax = remove ((const char *) args[1]);
        return;
      case SYS_OPEN:
        f->eax = open ((const char *) args[1]);
        return;
      case SYS_FILESIZE:
        VERIFY_VALUE (args[1]);
        f->eax = filesize ((int) args[1]);
        return;
      case SYS_READ:
        VERIFY_VALUE (args[1]);
        VERIFY_VALUE (args[3]);
        f->eax = read ((int) args[1], (void *) args[2], (unsigned) args[3]);
        return;
      case SYS_WRITE:
        VERIFY_VALUE (args[1]);
        VERIFY_VALUE (args[3]);
        f->eax = write ((int) args[1], (const void *) args[2], (unsigned) args[3]);
        return;
      case SYS_SEEK:
        VERIFY_VALUE (args[1]);
        VERIFY_VALUE (args[2]);
        seek ((int) args[1], (unsigned) args[2]);
        return;
      case SYS_TELL:
        VERIFY_VALUE (args[1]);
        f->eax = tell ((int) args[1]);
        return;
      case SYS_CLOSE:
        VERIFY_VALUE (args[1]);
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

      //PANIC ("write - Not implemented for fd != 1");
      lock_acquire (&filesys_lock);

      struct p_file *pf = get_pf (fd);
      off_t count = 0;

      if (is_pf_valid (pf))
        count = file_write (pf->file, buffer, size);
      else
        count = -1;

      lock_release (&filesys_lock);
      return count;

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

  struct file *f;
  static int fd = 2;

  /* Can't open empty string filename. */
  if (strcmp (file, "") == 0)
    return -1;

  lock_acquire(&filesys_lock);
  f = filesys_open (file);

  if (f == NULL)
    {
      lock_release (&filesys_lock);
      return -1;
    }

  openfiles[fd].file = f;
  openfiles[fd].valid = true;
  openfiles[fd].owner = thread_current ()->tid;

  lock_release(&filesys_lock);
  return fd++;
}

static int filesize (int fd)
{
  struct p_file *pf = get_pf (fd);
  off_t len = 0;

  lock_acquire (&filesys_lock);

  if (is_pf_valid (pf))
    {
      len = file_length (pf->file);
    }

  lock_release (&filesys_lock);
  return len;
}

static int read (int fd, void *buffer, unsigned size)
{
  VERIFY (buffer);

  /* Can't read from STDOUT. */
  if (fd == 1)
    exit (-1);

  lock_acquire (&filesys_lock);

  struct p_file *pf = get_pf (fd);
  off_t count = 0;

  if (is_pf_valid (pf))
    count = file_read (pf->file, buffer, size);
  else
    count = -1;

  lock_release (&filesys_lock);
  return count;
  // PANIC ("read - Not implemented"); /* TODO */
}

static void seek (int fd, unsigned position)
{
  //PANIC ("seek - Not implemented"); /* TODO */
  struct p_file *pf = get_pf (fd);

  lock_acquire (&filesys_lock);

  if (is_pf_valid (pf))
    {
      file_seek (pf->file,position);
    }

  lock_release (&filesys_lock);
}

static unsigned tell (int fd)
{
  //PANIC ("tell - Not implemented"); /* TODO */
  struct p_file *pf = get_pf (fd);
  off_t tell = 0;

  lock_acquire (&filesys_lock);

  if (is_pf_valid (pf))
    {
      tell = file_tell (pf->file);
    }

  lock_release (&filesys_lock);
  return tell;

}

static void close (int fd)
{
  struct p_file *pf = get_pf (fd);

  lock_acquire (&filesys_lock);

  if (is_pf_valid (pf))
    {
      file_close (pf->file);
      pf->file = NULL;
      pf->valid = false;
    }

  lock_release (&filesys_lock);
}

static struct p_file *
get_pf (int fd)
{
  if (fd < 2 || fd >= MAX_FD_COUNT)
    return NULL;

  return &openfiles[fd];
}

static bool
is_pf_valid (const struct p_file *pf)
{
  return (pf != NULL
            && pf->file != NULL
            && pf->valid
            && thread_current ()->tid == pf->owner);
}
