#ifndef IRC_H
#define IRC_H

#include <string.h>
#include <sys/socket.h>
// For struct addrinfo
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

// Prints the error in errno to the standard error stream and exits with errno.
#define perror()\
  fprintf(stderr, "error: %s\n", strerror(errno));	\
  exit(errno)
#define NOSSLPORT "6667"

int irc_init(char * address);
size_t irc_send(int sockfd, char* str);
size_t irc_recv(int sockfd, char * buff, size_t bufflen);

#endif
