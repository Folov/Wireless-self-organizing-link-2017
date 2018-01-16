#include 	<sys/socket.h>
#include	<time.h>
#include 	<errno.h>
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#include	<netinet/in.h>	/* sockaddr_in{} and other Internet defns */
#include	<signal.h>

#define	LISTENQ		1024	/* 2nd argument to listen() */

#define	MAXLINE		4096	/* max text line length */
#define	BUFFSIZE	8192	/* buffer size for reads and writes */
#define MAXID 30
#define MAXMAC 20
#define MAXROUTER 20
#define	SA	struct sockaddr

extern int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr);
extern void Bind(int fd, const struct sockaddr *sa, socklen_t salen);
extern void Connect(int fd, const struct sockaddr *sa, socklen_t salen);
extern int Socket(int family, int type, int protocol);
extern void Close(int fd);
extern void Listen(int fd, int backlog);
extern void Write(int fd, void *ptr, size_t nbytes);
extern ssize_t Read(int fd, void *buf, size_t count);
extern void err_sys(const char *fmt, ...);
extern void err_quit(const char *fmt, ...);
extern void bzero(void *s, int n);
extern pid_t Fork(void);

struct lpm
{
	char ssid[MAXID];
	// char mac_wan[MAXMAC];
	// char to_ssid[MAXID];
	// char separator[12];	//only the separator..no meaning
};


int
main(int argc, char **argv)
{
	FILE				*fp = NULL;
	int					listenfd, connfd;
	struct sockaddr_in	servaddr;
	char				recvline[MAXLINE + 1];
	char				recv_save[MAXLINE+1];
	pid_t				childpid;
	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(9898);

	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);
	//listen函数将listenfd套接字转换为被动套接字，LISTENQ为队列最大值(1024即可)
	//允许Socket重用，防止出现 bind error: Address in use
    int Reuse = 1;
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &Reuse, sizeof(Reuse));
    signal(SIGCHLD,SIG_IGN);
    //避免出现子进程结束后的僵死进程的简单方法（忽略），缺点是不能对SIGCHILD信号进行处理。
	for ( ; ; ) {
		connfd = Accept(listenfd, (SA *) NULL, NULL);	
		//第2,3个参数用于接收accept函数返回的对端客户协议地址。可以置空。我们不感兴趣。
		//accept函数返回已连接套接字(TCP三路握手成功后的)，它代表client端套接字
		if ( (childpid = Fork()) == 0) {	/* child process */
			Close(listenfd);				/* close listening socket */
			int j = 0, n = 0;
			//read 返回0：对端关闭连接；返回负值：发生错误	成功：返回读取的字节数
			while ((n = Read(connfd, recvline, MAXLINE)) > 0)	
			{
				recvline[n] = 0;	/* null terminate：数组结尾置0 */
				if ((fp = fopen("/tmp/wsol/LPM_TOP.txt", "w+")) == NULL)
				{
					printf("Can not open file LPM_TOP.txt\n");
					exit(3);
				}
				// 接收到的lpm存放在recvline数组中，其中含有'\0'，需要将其去掉，否则fprintf会识别\0从而无法完全写入文件。
				for (int i = 0; i < n; ++i)
				{
					if (recvline[i] != '\0')
					{
						recv_save[j] = recvline[i];
						j++;
					}
				}
				recv_save[j] = 0; //数组结尾置0
				if (fprintf(fp, "%s", recv_save) < 0)
				{
					printf("Can not fprintf.\n");
					exit(3);
				}
				fclose(fp);
				Write(connfd, "Serv get it!\n", strlen("Serv Get it!\n"));
				signal(SIGCHLD,SIG_DFL);
				if((system("/root/link_build_up.sh")) != 0)
					err_sys("link_build_up.sh error!");
				else
					exit(0);
			}
			if (n < 0)
				err_sys("read error");

			exit(0);
		}
		Close(connfd);			/* parent closes connected socket */
		exit(0);
	}

}
