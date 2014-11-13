/* pselect - synchronous I/O multiplexing

   Copyright 2011 Free Software Foundation, Inc.

   This file is part of gnulib.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

/* written by Paul Eggert */

#include <config.h>

#include <sys/select.h>

#include <errno.h>
#include <signal.h>

/* Examine the size-NFDS file descriptor sets in RFDS, WFDS, and XFDS
   to see whether some of their descriptors are ready for reading,
   ready for writing, or have exceptions pending.  Wait for at most
   TIMEOUT seconds, and use signal mask SIGMASK while waiting.  A null
   pointer parameter stands for no descriptors, an infinite timeout,
   or an unaffected signal mask.  */

int
pselect (int nfds, fd_set *restrict rfds,
         fd_set *restrict wfds, fd_set *restrict xfds,
         struct timespec const *restrict timeout,
         sigset_t const *restrict sigmask)
{
  int select_result;
  sigset_t origmask;
  struct timeval tv, *tvp;

  if (timeout)
    {
      if (! (0 <= timeout->tv_nsec && timeout->tv_nsec < 1000000000))
        {
          errno = EINVAL;
          return -1;
        }

      tv.tv_sec = timeout->tv_sec;
      tv.tv_usec = (timeout->tv_nsec + 999) / 1000;
      tvp = &tv;
    }
  else
    tvp = NULL;

  /* Signal mask munging should be atomic, but this is the best we can
     do in this emulation.  */
  if (sigmask)
    pthread_sigmask (SIG_SETMASK, sigmask, &origmask);

  select_result = select (nfds, rfds, wfds, xfds, tvp);

  if (sigmask)
    {
      int select_errno = errno;
      pthread_sigmask (SIG_SETMASK, &origmask, NULL);
      errno = select_errno;
    }

  return select_result;
}
