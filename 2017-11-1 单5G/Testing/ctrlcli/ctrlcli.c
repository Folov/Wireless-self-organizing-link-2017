#include "ctrl.h"

int main(int argc, char **argv)
{
	int					sockfd, i;
	struct sockaddr_in	servaddr;

	if (argc != 3)
		err_quit("usage: tcpcli <IPaddress> <ctrl message>[reboot/scan]");
	//AF_INET: IPv4 protocal	SOCK_STREAM: 字节流套接字, TCP专用
	sockfd = Socket(AF_INET, SOCK_STREAM, 0);

	//允许Socket重用，防止出现 bind error: Address in use
	int Reuse = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &Reuse, sizeof(Reuse));

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port   = htons(9900);
	Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);	/*点分十进制IP地址转二进制数值*/

	Connect(sockfd, (SA *) &servaddr, sizeof(servaddr));

	if ((i = ctrl_cli(argv[2], sockfd)) != 0)
	{
		Close(sockfd);
		err_quit("Ctrl message send error!"); //exit(1)
	}

	Close(sockfd);
	exit(0);
}

int ctrl_cli(char * mp, int sockfd)
{
	char recvline[BUFFER_SIZE];
	char *precv = recvline;
	int n = 0;
	int sum_nbyte = 0;

	if (strcmp(mp, "reboot") == 0)
	{
		Write(sockfd, mp, strlen(mp));

		n = Read(sockfd, recvline, MAXLINE);
		recvline[n] = 0;	/* null terminate */
		Fputs(recvline, stdout);
		return 0;
	}
	else if (strcmp(mp, "scan") == 0)
	{
		Write(sockfd, mp, strlen(mp));

		while((n = Read(sockfd, precv, MAXLINE)) > 0)
		{
			sum_nbyte += n;
			precv += n;
			if (recvline[sum_nbyte-1] == '#')	//ctrlsrv us '#' as the end of file
				break;
		}
		recvline[sum_nbyte-1] = 0;	/* null terminate */
		Fputs(recvline, stdout);
		return 0;
	}
	else
		return 1;
}

