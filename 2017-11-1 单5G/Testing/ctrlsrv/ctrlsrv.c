#include "ctrl.h"

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
				if (strcmp(recvline, "reboot") == 0)
				{
					printf("Ctrl_reboot get!\n");
					Write(connfd, "He get reboot message!\n", strlen("He get reboot message!\n"));
					signal(SIGCHLD,SIG_DFL);
					if((system("reboot")) != 0)
						err_sys("system reboot error!");
				}
				else if (strcmp(recvline, "scan") == 0)
				{
					printf("Ctrl_scan get!\n");
					signal(SIGCHLD,SIG_DFL);
					if((system("/root/findrouter.sh")) != 0)
						err_sys("system findrouter.sh error!");
					else
					{
						if ((fp = fopen("/tmp/wsol/iwscan.txt", "r")) == NULL)
						{
							err_quit("Can not open iwscan file!");
						}
						bzero(buffer, BUFFER_SIZE);
						while((file_block_length = fread(buffer, sizeof(char), BUFFER_SIZE, fp)) > 0)
						{
							printf("file_block_length = %d\n", file_block_length);
							strcat(buffer, "#");
							file_block_length++;	// Use '#' as the end of file. For ctrlcli to read.
							/*
							 *Send the string to the buffer  
							 */
							Write(connfd, buffer, file_block_length);
							bzero(buffer, sizeof(buffer));
						}
						fclose(fp);
					}
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
