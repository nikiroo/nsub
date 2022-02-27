/*
 * CUtils: some small C utilities
 *
 * Copyright (C) 2012 Niki Roo
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

/**
 * @file
 * @author niki
 * @date November 2011
 *
 * Allows you to make connections to/from a server, and to send/receive data.
 */

#ifndef NET_H
#define NET_H

#ifdef __cplusplus
extern "C" {
#endif

// WHY ??
#ifdef _WIN32
	#ifndef WIN32
		#define WIN32
	#endif
#endif

#ifdef WIN32
    #ifndef _WIN32_WINNT
        #define _WIN32_WINNT 0x501
    #endif
	#include <winsock2.h>
	#include <ws2tcpip.h>
	typedef SSIZE_T ssize_t;
	#define close(s) closesocket(s)
	#define ioctl(a,b,c) ioctlsocket(a,b,c)
	#pragma comment(lib, "wsock32.lib");
	#pragma comment(lib, "ws2_32.lib");
#else
	#include <unistd.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <netdb.h>
	#include <arpa/inet.h>
	#include <sys/wait.h>
#endif

/* for ssize_t */
#include <sys/types.h>

/**
 * Obsolete, see net_set_blocking(..)
 */
int net_set_non_blocking(int fd);

/**
 * You must call this function before doing any network related operation,
 * because of some legacy WIN32 rule.
 * (It is a noop on all other platforms).
 *
 * @return 0 when the Windows host does not support WinSock and thus,
 * 		no network for you (you still need to call net_cleanup)
 * 
 */
int net_init();

/**
 * You must call this function after you are done using network related
 * operations within this DLL, because of some legacy WIN32 rule.
 * (It is a noop on all other platforms).
 */
void net_cleanup();

/**
 * Set the given socket to (non-)blocking I/O mode.
 * This function can work with file sockets, too.
 *
 * @param fd the file descriptor or socket to change
 * @param block 1 to block, 0 not to block
 * 
 * @return 1 if success
 */
int net_set_blocking(int fd, int block);

/**
 * Connect to this server on this port, and return a socket descriptor
 * to write to or read from it.
 *
 * @param server the server to connect to
 * @param port the port to connect on
 *
 * @return the server socket or -1 if error
 */
int net_connect(const char server[], int port);

/**
 * Open a port and returns a (server) socket descriptor from which you can
 * 		  accept connection.
 *
 * @param port the port to connect on
 * @param backlog the maximum number of client connections we will queue for
 *  	this socket until you handle them
 *
 * @return the server socket, or a negative value on error
 */
int net_listen(int port, int backlog);

/**
 * Block (or not) and wait for a client to connect on this socket. 
 * When this is done, return a socket to this specific client/server
 * connection. It can takes the connections from a queue,
 * as defined in net_listen.
 *
 * @param ssocketd the server socket on which to accept a connection
 *
 * @return the socket, or a negative value on error
 */
int net_accept(int ssocketd);

/**
 * Close a socket (or a server socket).
 *
 * @param socketd the (server) socket to close
 */
void net_close_socketd(int socketd);

/**
 * Write to this socket, as you would with a file.
 *
 * @param fd the socket to write to
 * @param buf the buffer to read from
 * @param n the number of bytes to write
 *
 * @return the actual number of bytes written or a
 * 		   negative number if error
 */
ssize_t net_write(int fd, const void *buf, size_t n);

/**
 * Read from this socket, as you would with a file.
 *
 * @param fd the socket to read from
 * @param buf the buffer to write to
 * @param nbytes the number of bytes to read
 *
 * @return the actual number of bytes read
 */
ssize_t net_read(int fd, void *buf, size_t nbytes);

#endif

#ifdef __cplusplus
extern }
#endif
