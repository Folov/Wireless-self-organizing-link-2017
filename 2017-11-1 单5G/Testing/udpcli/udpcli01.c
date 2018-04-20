#include "udp.h"

int main(int argc, char **argv)
{
	int					sockfd;
	struct sockaddr_in	servaddr;
	struct timeval		tv_o;

	if (argc != 3)
		err_quit("usage: udpcli <IPaddress> <message>");

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(9899);
	Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	sockfd = Socket(AF_INET, SOCK_DGRAM, 0);
	//设置SO_RCVTIMEO 超时3s
	tv_o.tv_sec = 3;
	tv_o.tv_usec = 0;
	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv_o, sizeof(tv_o));
 
	strcat(argv[2], "\n");	//没有\n udpsrv不会换行
	dg_cli(argv[2], sockfd, (SA *) &servaddr, sizeof(servaddr));

	exit(0);
}

void dg_cli(char *mp, int sockfd, const SA *pservaddr, socklen_t servlen)
{
	int		n;
	char	recvline[MAXLINE + 1];

	Connect(sockfd, (SA *) pservaddr, servlen);	//有助于快速得知服务端是否正常

	Write(sockfd, mp, strlen(mp));

	n = Read(sockfd, recvline, MAXLINE);

	recvline[n] = 0;	/* null terminate */
	Fputs(recvline, stdout);
}

