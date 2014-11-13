/* Test of ttyname_r(3).
   Copyright (C) 2010-2011 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include <config.h>

#include <unistd.h>

#include "signature.h"
SIGNATURE_CHECK (ttyname_r, int, (int, char *, size_t));

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#include "macros.h"

int
main (void)
{
  int fd;
  char buf[100];

  /* Open the controlling tty of the current process.  */
  fd = open ("/dev/tty", O_RDONLY);
  if (fd < 0)
    {
      fprintf (stderr, "Skipping test: cannot open controlling tty\n");
      return 77;
    }

  ASSERT (ttyname_r (fd, buf, 1) == ERANGE);

  ASSERT (ttyname_r (fd, buf, sizeof (buf)) == 0);
  ASSERT (memcmp (buf, "/dev/", 5) == 0);

  return 0;
}
