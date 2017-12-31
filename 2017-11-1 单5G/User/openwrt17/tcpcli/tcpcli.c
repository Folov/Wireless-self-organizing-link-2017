#include 	<sys/socket.h>
#include	<time.h>
#include 	<errno.h>
#include	<stdarg.h>
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#include	<netinet/in.h>	/* sockaddr_in{} and other Internet defns */

/* Following could be derived from SOMAXCONN in <sys/socket.h>, but many
   kernels still #define it as 5, while actually supporting many more */
#define	LISTENQ		1024	/* 2nd argument to listen() */

/* Miscellaneous constants */
#define	MAXLINE		4096	/* max text line length */
#define	BUFFSIZE	8192	/* buffer size for reads and writes */
#define	SA	struct sockaddr
#define MAXID 100
#define MAXMAC 20
#define MAXROUTER 20

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
extern int inet_pton(int af, const char *src, void *dst);

struct lpm
{
	char ssid[MAXID];
	// char mac_wan[MAXMAC];
	// char to_ssid[MAXID];
	// char separator[12];	//only the separator..no meaning
};
int read_LPM(char filename[], struct lpm lpm_data[], int max);

int
main(int argc, char **argv)
{
	int					sockfd;
	char				recvline[MAXLINE + 1];
	struct sockaddr_in	servaddr;

	if (argc != 3)
		err_quit("usage: tcpcli <IPaddress> <LPM file>");
//AF_INET: IPv4 protocal	SOCK_STREAM: 字节流套接字, TCP专用
	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		err_sys("socket error");
	int Reuse = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &Reuse, sizeof(Reuse));
	//允许Socket重用，防止出现 bind error: Address in use
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;		
	servaddr.sin_port   = htons(9898);	/*port13: daytime server */
	if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)	/*点分十进制IP地址转二进制数值*/
		err_quit("inet_pton error for %s", argv[1]);

	if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0)
		err_sys("connect error");

	// if ( (n = Read(sockfd, recvline, MAXLINE)) > 0) //read 返回0：对端关闭连接；	返回负值：发生错误	成功：返回读取的字节数
	// {	
	// 	recvline[n] = 0;	// null terminate：数组结尾置0 
	// 	if (fputs(recvline, stdout) == EOF)
	// 		err_sys("fputs error");
	// }
	// if (n < 0)
	// 	err_sys("read error");

	struct lpm lpm_to_send[MAXROUTER] = {0};
	int lpm_number = 0;
	lpm_number = read_LPM(argv[2], lpm_to_send, MAXROUTER);
	// printf("Read from LPM_file:\n%s\n%s\n%s\n%s\n", lpm_to_send->ssid, lpm_to_send->mac_wan, lpm_to_send->to_ssid, lpm_to_send->separator);
	// printf("%s\n", lpm_to_send[2].ssid);
	printf("%d ssid transmitted.\n", lpm_number);
	if ( lpm_number != 0 )
	{
		Write(sockfd, (char *)&lpm_to_send, sizeof(lpm_to_send));

		if (Read(sockfd, recvline, MAXLINE) > 0)	
			if (fputs(recvline, stdout) == EOF)
				err_sys("fputs error");
		Close(sockfd);
	}
	else
	{
		Close(sockfd);
		err_quit("Lpm file error!");
	}

	
	exit(0);
}

int read_LPM(char filename[], struct lpm lpm_data[], int max)
{
	FILE *fd;
	int i = 0;

	if ((fd = fopen(filename, "r")) == NULL)
	{
		err_quit("Can not open lpm file!");
	}

	while(fscanf(fd, "%s", lpm_data->ssid) != EOF)
	{
		strcat(lpm_data->ssid, "\n");
		// 注意：strcat会在结尾自动添加'\0' !!!		
		if (++i == max)
			break;
		lpm_data++;
	}
	fclose(fd);
	if (i == 0)
		err_quit("Lpm file empty!");
	return i;
}
