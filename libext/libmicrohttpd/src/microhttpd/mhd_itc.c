/*
  This file is part of libmicrohttpd
  Copyright (C) 2016 Karlson2k (Evgeny Grin)

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*/

/**
 * @file microhttpd/mhd_sockets.c
 * @brief  Implementation of inter-thread communication functions
 * @author Karlson2k (Evgeny Grin)
 */

#include "mhd_itc.h"

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H */
#include <fcntl.h>

#if defined(_WIN32) && !defined(__CYGWIN__)
/**
 * Create pair of mutually connected TCP/IP sockets on loopback address
 * @param sockets_pair array to receive resulted sockets
 * @return zero on success, -1 otherwise
 */
int MHD_W32_pair_of_sockets_(SOCKET sockets_pair[2])
{
  int i;
  if (!sockets_pair)
    {
      errno = EINVAL;
      return -1;
    }

#define PAIRMAXTRYIES 800
  for (i = 0; i < PAIRMAXTRYIES; i++)
    {
      struct sockaddr_in listen_addr;
      SOCKET listen_s;
      static const int c_addinlen = sizeof(struct sockaddr_in); /* help compiler to optimize */
      int addr_len = c_addinlen;
      int opt = 1;

      listen_s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
      if (INVALID_SOCKET == listen_s)
        break; /* can't create even single socket */

      listen_addr.sin_family = AF_INET;
      listen_addr.sin_port = htons(0);
      listen_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
      if (0 == bind(listen_s, (struct sockaddr*) &listen_addr, c_addinlen)
          && 0 == listen(listen_s, 1)
          && 0 == getsockname(listen_s, (struct sockaddr*) &listen_addr,
                  &addr_len))
        {
          SOCKET client_s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
          if (INVALID_SOCKET != client_s)
            {
              if (0 == ioctlsocket(client_s, FIONBIO, (u_long*) &opt)
                  && (0 == connect(client_s, (struct sockaddr*) &listen_addr, c_addinlen)
                      || WSAGetLastError() == WSAEWOULDBLOCK))
                {
                  struct sockaddr_in accepted_from_addr;
                  SOCKET server_s;
                  addr_len = c_addinlen;
                  server_s = accept(listen_s,
                      (struct sockaddr*) &accepted_from_addr, &addr_len);
                  if (INVALID_SOCKET != server_s)
                    {
                      struct sockaddr_in client_addr;
                      addr_len = c_addinlen;
                      opt = 0;
                      if (0 == getsockname(client_s, (struct sockaddr*) &client_addr, &addr_len)
                          && accepted_from_addr.sin_family == client_addr.sin_family
                          && accepted_from_addr.sin_port == client_addr.sin_port
                          && accepted_from_addr.sin_addr.s_addr == client_addr.sin_addr.s_addr
                          && 0 == ioctlsocket(client_s, FIONBIO, (u_long*) &opt)
                          && 0 == ioctlsocket(server_s, FIONBIO, (u_long*) &opt))
                        {
                          closesocket(listen_s);
                          sockets_pair[0] = client_s;
                          sockets_pair[1] = server_s;
                          return 0;
                        }
                      closesocket(server_s);
                    }
                }
              closesocket(client_s);
            }
        }
      closesocket(listen_s);
    }

  sockets_pair[0] = INVALID_SOCKET;
  sockets_pair[1] = INVALID_SOCKET;
  return -1;
}

#endif /* _WIN32 && ! __CYGWIN__ */

#ifndef MHD_DONT_USE_PIPES
#if !defined(_WIN32) || defined(__CYGWIN__)


/**
 * Change itc FD options to be non-blocking.
 *
 * @param fd the FD to manipulate
 * @return non-zero if succeeded, zero otherwise
 */
int
MHD_itc_nonblocking_ (MHD_pipe fd)
{
  int flags;

  flags = fcntl (fd, F_GETFL);
  if (-1 == flags)
    return 0;

  if ( ((flags | O_NONBLOCK) != flags) &&
       (0 != fcntl (fd, F_SETFL, flags | O_NONBLOCK)) )
    return 0;

  return !0;
}
#endif /* _WIN32 && ! __CYGWIN__ */
#endif /* ! MHD_DONT_USE_PIPES */
