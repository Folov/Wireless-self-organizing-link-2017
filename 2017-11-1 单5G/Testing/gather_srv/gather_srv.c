#include "gather_srv.h"

int main(int argc, char **argv)
{
	FILE				*fp = NULL;
	int					listenfd, connfd;
	struct sockaddr_in	servaddr;
	char				recvline[MAXLINE + 1];
	pid_t				childpid;
	char *				buffer;
	int 				file_block_length = 0;
	int 				n = 0;
	int 				nf = 0;

	buffer = (char *)malloc(sizeof(char)*BUFFER_SIZE);	//for fread

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(9900);

	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);
	//listen函数将listenfd套接字转换为被动套接字，LISTENQ为队列最大值(1024即可)
	//允许Socket重用，防止出现 bind error: Address in use
	int Reuse = 1;
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &Reuse, sizeof(Reuse));
    signal(SIGCHLD,SIG_IGN);
    //避免出现子进程结束后的僵死进程的简单方法（忽略），缺点是不能对SIGCHILD信号进行处理。
	for ( ; ; ) 
	{
		connfd = Accept(listenfd, (SA *) NULL, NULL);	
		//第2,3个参数用于接收accept函数返回的对端客户协议地址。可以置空。我们不感兴趣。
		//accept函数返回已连接套接字(TCP三路握手成功后的)，它代表client端套接字
		if ( (childpid = Fork()) == 0) 		/* child process */
		{	
			Close(listenfd);				/* close listening socket */
			while ((n = Read(connfd, recvline, MAXLINE)) > 0)
			{
				recvline[n] = 0;	/* null terminate：数组结尾置0 */
				/*scan信息是固定的，只会传输一次，接收到scan信息后写入文件保存，等待需要时再读取*/
				if (strcmp(recvline, "scan") == 0)
				{
					printf("Prepare to get scan message!\n");
					Write(connfd, "Ready to get scan message!\n", strlen("Ready to get scan message!\n"));
					nf = Read(connfd, recvline, MAXLINE);

					Write(connfd, "Fin!\n", strlen("Fin!\n"));

					signal(SIGCHLD,SIG_DFL);
					

					if((system("cat /tmp/wsol/.txt >> All_Scan_Message.txt")) != 0)
						err_sys("system reboot error!");



				}
				/*else if (strcmp(recvline, "gps") == 0)
				{
					printf("Prepare to get gps message!\n");
					Write(connfd, "Ready to get gps message!\n", strlen("Ready to get gps message!\n"));
					nf = Read(connfd, recvline, MAXLINE);
					signal(SIGCHLD,SIG_DFL);
				}*/

				/*Heartbeats信息每3s接收一次，heartbeat信息写入变量，随时可供调用*/
				else if (strcmp(recvline, "heartbeat") == 0)
				{
					printf("Prepare to get heartbeat!\n");
					Write(connfd, "Ready to get gps message!\n", strlen("Ready to get gps message!\n"));
					nf = Read(connfd, recvline, MAXLINE);

					Write(connfd, "Fin!\n", strlen("Fin!\n"));

					strcat(recvline, "=1");
					if ((putenv(recvline)) != 0)
						err_sys("putenv error!");

					signal(SIGCHLD,SIG_DFL);




				}

				/*gather指令由pc发出，程序汇总所有信息后发回pc*/
				else if (strcmp(recvline, "gather") == 0)
				{
					printf("Prepare to send all data!\n");
					


					signal(SIGCHLD,SIG_DFL);




				}
				else
					continue;
			}
			if (n < 0)
				err_sys("Socket read error!");
			exit(0);
		}
		Close(connfd);			/* parent closes connected socket */
		// exit(0);
	}
}
