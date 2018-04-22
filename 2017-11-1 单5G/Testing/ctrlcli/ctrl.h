#ifndef CTRL_H_
#define CTRL_H_

#include 	<sys/socket.h>
#include	<time.h>
#include 	<errno.h>
#include	<stdarg.h>
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#include	<netinet/in.h>	/* sockaddr_in{} and other Internet defns */
#include	<arpa/inet.h>
#include	<unistd.h>
#include	<syslog.h>		/* for syslog() */

#define	LISTENQ		1024	/* 2nd argument to listen() */
#define	MAXLINE		4096	/* max text line length */
#define	BUFFSIZE	8192	/* buffer size for reads and writes */
#define BUFFER_SIZE	1*1024*1024	/* buffer size for fread and fwrite of iwscan.txt*/
#define	SA	struct sockaddr
#define MAXID 100
#define MAXMAC 20
#define MAXROUTER 20
#define	MAXLINE		4096	/* max text line length */

int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr);
void Bind(int fd, const struct sockaddr *sa, socklen_t salen);
void Connect(int fd, const struct sockaddr *sa, socklen_t salen);
int Socket(int family, int type, int protocol);
void Close(int fd);
void Listen(int fd, int backlog);
void Write(int fd, void *ptr, size_t nbytes);
ssize_t Read(int fd, void *buf, size_t count);
void err_sys(const char *fmt, ...);
void err_quit(const char *fmt, ...);
void bzero(void *s, int n);
void Inet_pton(int family, const char *strptr, void *addrptr);
void err_sys(const char *fmt, ...);
int ctrl_cli(char *, int fd);
char * Fgets(char *ptr, int n, FILE *stream);
void Fputs(const char *ptr, FILE *stream);
pid_t Fork(void);

#endif
