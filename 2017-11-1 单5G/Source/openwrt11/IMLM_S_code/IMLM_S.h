#ifndef CTRL_H_
#define CTRL_H_

#include 	<sys/socket.h>
#include	<time.h>
#include 	<errno.h>
#include	<stdarg.h>
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#include 	<signal.h>
#include	<netinet/in.h>	/* sockaddr_in{} and other Internet defns */
#include	<unistd.h>
#include	<syslog.h>		/* for syslog() */
#include	<pthread.h>		/* enable pthread */


#define	LISTENQ		1024	/* 2nd argument to listen() */
#define	MAXLINE		4096	/* max text line length */
#define	BUFFSIZE	8192	/* buffer size for reads and writes */
#define	SA	struct sockaddr
#define MAXID 100
#define MAXMAC 20
#define MAXROUTER 255
#define	MAXLINE		4096	/* max text line length */
#define BUFFER_SIZE	1*1024*1024	/* buffer size for fread and fwrite of iwscan.txt*/

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
int inet_pton(int af, const char *src, void *dst);
void Inet_pton(int family, const char *strptr, void *addrptr);
void err_sys(const char *fmt, ...);
char * Fgets(char *ptr, int n, FILE *stream);
void Fputs(const char *ptr, FILE *stream);
pid_t Fork(void);
ssize_t Recvfrom(int, void *, size_t, int, SA *, socklen_t *);
void	Sendto(int, const void *, size_t, int, const SA *, socklen_t);

void *Heartbeats();
void *Renew_openwrt();
void *PC_server();
void dg_echo(int sockfd, SA *pcliaddr, socklen_t clilen);
char *RU_cli(char * addr);


#endif
