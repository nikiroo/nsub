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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>

#ifndef WIN32
    #include <fcntl.h>
#endif

#include "net.h"
#include "cstring.h"

#define bool int
#define true 1
#define false 0

/**
 * Get the sockaddr, IPv4 or IPv6.
 * @param sa the socket address is in this structure
 * @return the sockaddr_in or sockaddr_in6 inside this socketaddr,
 * 		   depending if it is IPv4 or IPv6
 */
void *get_in_addr(struct sockaddr *sa);

#ifndef WIN32
/**
 * Reap all zombie processes.
 * This function will be called when a child process terminates, and
 * will loop on all zombie processes to properly acknowledge them
 * so they can die.
 *
 * @param s
 */
void sigchld_handler(int pid);

void sigchld_handler(int pid) {
	if (pid > 0 || pid < 0) pid = 0;

	// Reap all zombie processes
	while (waitpid(-1, NULL, WNOHANG) > 0);
}
#endif

int net_init(){
#if defined (WIN32)
	WSADATA WSAData;
	return !WSAStartup(MAKEWORD(2,2), &WSAData);
#endif
	return 1;
}

void net_cleanup(){
#if defined (WIN32)
	WSACleanup();
#endif
}

int net_set_non_blocking(int fd) {
	return net_set_blocking(fd, 0);
}

int net_set_blocking(int fd, int block) {
	int flags;

/* If they have O_NONBLOCK, use the POSIX way to do it */
#if defined (O_NONBLOCK)
	/* O_NONBLOCK is defined but broken on SunOS 4.1.x and AIX 3.2.5. */
	if ((flags = fcntl(fd, F_GETFL, 0)) == -1) {
		flags = 0;
	}
	if (block) {
		return fcntl(fd, F_SETFL, flags ^ O_NONBLOCK);
	}
	else {
		return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
	}
#else
	flags = block?0:1;
	return ioctl(fd, FIONBIO, (int)(&flags));
#endif
}

int net_connect(const char server[], int port) {
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	cstring_t *str;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	// convert the port number to a string
	str = new_cstring();
	cstring_addp(str, "%i", port);
	rv = getaddrinfo(server, str->string, &hints, &servinfo);
	free_cstring(str);
	//

	if (rv != 0) {
		// DO NOT dirty the stderr
		//fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return -1;
	}

	// loop through all the results and connect to the first we can
	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol))
				== -1) {
			//perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			//perror("client: connect");
			continue;
		}

		break;
	}

	if (p == NULL) {
		//fprintf(stderr, "client: failed to connect\n");
		return -1;
	}
	
	freeaddrinfo(servinfo);

	return sockfd;
}

int net_accept(int ssocketd) {
	struct sockaddr_storage their_addr;
	socklen_t sin_size;

	sin_size = sizeof(their_addr);
	return accept(ssocketd, (struct sockaddr *) &their_addr, &sin_size);
}

int net_listen(int port, int backlog) {
#ifndef WIN32
    struct sigaction sa;
#endif
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	char yes = 1;
	int rv;
	cstring_t *str;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	// convert the port number to a string
	str = new_cstring();
	cstring_addp(str, "%d", port);
	rv = getaddrinfo(NULL, str->string, &hints, &servinfo);
	free_cstring(str);
	//

	if (rv != 0) {
		// DO NOT dirty the stderr
		//fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return -1;
	}

	// loop through all the results and bind to the first we can
	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			//perror("server: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
			//perror("setsockopt");
			return -1;
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			//perror("server: bind");
			continue;
		}

		break;
	}

	if (p == NULL) {
		//fprintf(stderr, "server: failed to bind\n");
		return -1;
	}

	// all done with this structure
	freeaddrinfo(servinfo);
	
	if (listen(sockfd, backlog) == -1) {
		//perror("listen");
		return -1;
	}

#ifndef WIN32
	sa.sa_handler = sigchld_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		//perror("sigaction");
		return -1;
	}
#endif

	return sockfd;
}

void net_close_socketd(int socketd) {
	close(socketd);
}

#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0
#endif
ssize_t net_write(int fd, const void *buf, size_t n) {
	// In UNIX: send() with flag set to '0' == write()
	// In WIN32: cannot write() to a socket
	return send(fd, (char *)buf, n, MSG_NOSIGNAL);
}

ssize_t net_read(int fd, void *buf, size_t nbytes) {
	// In UNIX: recv() with flag set to '0' == send()
	// In WIN32: cannot read() from a socket
	return recv(fd, (char *)buf, nbytes, 0);
}

void *get_in_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*) sa)->sin_addr);
	}
	else {
		return &(((struct sockaddr_in6*) sa)->sin6_addr);
	}
}

