#ifndef UDP_H_
#define UDP_H_

#include	<stdarg.h>		/* ANSI C header file */
#include 	<sys/socket.h>
#include 	<errno.h>
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#include	<netinet/in.h>	/* sockaddr_in{} and other Internet defns */
#include	<stdarg.h>
#include	<unistd.h>
#include	<syslog.h>

#define	LISTENQ		1024	/* 2nd argument to listen() */

#define	MAXLINE		4096	/* max text line length */
#define	BUFFSIZE	8192	/* buffer size for reads and writes */
#define MAXID 30
#define MAXMAC 20
#define MAXROUTER 255
#define	SA	struct sockaddr

int		Accept(int fd, struct sockaddr *sa, socklen_t *salenptr);
void	Bind(int fd, const struct sockaddr *sa, socklen_t salen);
void	Connect(int fd, const struct sockaddr *sa, socklen_t salen);
int 	Socket(int family, int type, int protocol);
void	Close(int fd);
void	Listen(int fd, int backlog);
void	Write(int fd, void *ptr, size_t nbytes);
ssize_t Read(int fd, void *buf, size_t count);
void	err_sys(const char *fmt, ...);
void	err_quit(const char *fmt, ...);
void	bzero(void *s, int n);
pid_t	Fork(void);
ssize_t Recvfrom(int, void *, size_t, int, SA *, socklen_t *);
void	Sendto(int, const void *, size_t, int, const SA *, socklen_t);
void	dg_echo(int, SA *, socklen_t);
void	dg_cli(char *, int, const SA *, socklen_t);



#endif