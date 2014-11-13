/* Sequential list data type backed by another list, with out-of-memory
   checking.
   Copyright (C) 2009-2011 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2009.

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

#ifndef _GL_XSUBLIST_H
#define _GL_XSUBLIST_H

#include "gl_sublist.h"
#include "xalloc.h"

#ifdef __cplusplus
extern "C" {
#endif

/* These functions are thin wrappers around the corresponding functions with
   _nx_ infix from gl_sublist.h.  Upon out-of-memory, they invoke
   xalloc_die (), instead of returning an error indicator.  */
extern gl_list_t gl_sublist_create (gl_list_t whole_list,
                                    size_t start_index, size_t end_index);

#if HAVE_INLINE

# define gl_sublist_create gl_sublist_create_inline
static inline gl_list_t
gl_sublist_create (gl_list_t whole_list, size_t start_index, size_t end_index)
{
  gl_list_t result = gl_sublist_nx_create (whole_list, start_index, end_index);
  if (result == NULL)
    xalloc_die ();
  return result;
}

#endif

#ifdef __cplusplus
}
#endif

#endif /* _GL_XSUBLIST_H */
