#include "IMLM_S.h"
#define NUM_THREADS 3

int openwrt[MAXROUTER] = {0};// = {[0 ... (MAXROUTER-1)] = 1};
// memset(openwrt, 1, sizeof(openwrt));
int openwrt_med[MAXROUTER] = {0};

pthread_mutex_t mutex_med;

void *show();

int main(int argc, char const *argv[])
{
	pthread_t tid[NUM_THREADS];
	if (pthread_create(&tid[0], NULL, Heartbeats, NULL) != 0)
		err_sys("pthread_create Heartbeats error!");
	if (pthread_detach(tid[0]) != 0)
		err_sys("pthread_detach Heartbeats error!");
	// if (pthread_create(&tid[2], NULL, show, NULL) != 0)
	// 	err_sys("pthread_create show error!");
	// if (pthread_detach(tid[2]) != 0)
	// 	err_sys("pthread_detach show error!");


	if (pthread_create(&tid[1], NULL, Renew_openwrt, NULL) != 0)
		err_sys("pthread_create Renew_openwrt error!");
	if (pthread_detach(tid[1]) != 0)
		err_sys("pthread_detach Renew_openwrt error!");
	if (pthread_create(&tid[2], NULL, PC_server, NULL) != 0)
		err_sys("pthread_create PC_server error!");
	// if (pthread_detach(tid[2]) != 0)
	// 	err_sys("pthread_detach PC_server error!");
	pthread_join(tid[2],NULL);


	return 0;
}


/*****************************Heartbeats************************************************/
void *Heartbeats()
{
	int					sockfd;
	struct sockaddr_in	servaddr, cliaddr;

	sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(9899);

	Bind(sockfd, (SA *) &servaddr, sizeof(servaddr));

	dg_echo(sockfd, (SA *) &cliaddr, sizeof(cliaddr));

	return 0;
}


void dg_echo(int sockfd, SA *pcliaddr, socklen_t clilen)
{
	// int			n;
	socklen_t	len;
	char		mesg[MAXLINE];
	char *		p_num = mesg + 7; //point to the number of "openwrt17"
	int 		router_num = 0;

	for ( ; ; ) {
		len = clilen;
		Recvfrom(sockfd, mesg, MAXLINE, 0, pcliaddr, &len);
		// printf("Heartbeats from: %s", mesg);
		router_num = atoi(p_num);	// array to (int)number

		pthread_mutex_lock(&mutex_med);
		openwrt_med[router_num] = 1;
		pthread_mutex_unlock(&mutex_med);

		bzero(mesg, MAXLINE);
		Sendto(sockfd, "Heartbeats_Echo\n", sizeof("Heartbeats_Echo\n"), 0, pcliaddr, len);
	}
}

/*****************************Renew_openwrt************************************************/

void *Renew_openwrt()
{
	while(1)
	{
		//4s (1)
		sleep(4);								// 一倍去抖4s，二倍去抖7s
		pthread_mutex_lock(&mutex_med);
		memset(openwrt_med, 0, sizeof(openwrt_med));
		pthread_mutex_unlock(&mutex_med);

		//4s (2)
		sleep(4);
		pthread_mutex_lock(&mutex_med);			// initial value: 00
		for (int i = 0; i < MAXROUTER; ++i)		// 00 -> 00
		{										// 01 -> 00
			if (openwrt_med[i] == 0)			// 10 -> 11
				openwrt[i] = 0;					// 11 -> 11
			else
				openwrt[i] = 1;
		}
		pthread_mutex_unlock(&mutex_med);
	}
}
/*************************show****************************************/

void *show()
{
	while(1)
	{
		sleep(5);
		pthread_mutex_lock(&mutex_med);
		for (int i = 0; i < MAXROUTER; ++i)
		{
			printf("%d|%d ", openwrt_med[i], openwrt[i]);
		}
		printf("\n");
		pthread_mutex_unlock(&mutex_med);
	}
}

/*****************************PC_server************************************************/

void *PC_server()
{
	FILE				*fp = NULL;
	int					listenfd, connfd;
	struct sockaddr_in	servaddr;
	char				recvline_PC[MAXLINE + 1];
	char 				IP_str[20];
	char 				*p_static_info;
	char 				*buffer_static_info;
	char 				*buffer_all;
	int 				file_block_length = 0;
	int 				n = 0;
	int 				flag_static = 0;

	buffer_static_info = (char *)malloc(sizeof(char)*BUFFER_SIZE);
	buffer_all = (char *)malloc(sizeof(char)*(BUFFER_SIZE + MAXROUTER));
	memset(buffer_static_info, 0, sizeof(buffer_static_info));
	memset(buffer_all, 0, sizeof(buffer_all));

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

    while(1)
    {
	    connfd = Accept(listenfd, (SA *) NULL, NULL);	
	    //第2,3个参数用于接收accept函数返回的对端客户协议地址。可以置空。我们不感兴趣。
	    //accept函数返回已连接套接字(TCP三路握手成功后的)，它代表client端套接字
	    while ((n = Read(connfd, recvline_PC, MAXLINE)) > 0)
	    {
	    	recvline_PC[n] = 0;	/* null terminate：数组结尾置0 */
	    	if (strcmp(recvline_PC, "Please~") == 0)
	    	{
	    		printf("PC request message get!\n");
	    		signal(SIGCHLD,SIG_DFL);
	    		/* 取各节点信息保存 */
	    		if (flag_static == 0)
	    		{
	    			for (int i = 0; i < MAXROUTER; ++i)
		    		{
		    			if (openwrt[i] == 1)
		    			{
		    				sprintf(IP_str, "192.168.%d.%d", i, i);
		    				p_static_info = RU_cli(IP_str);
		    				strcat(buffer_static_info, p_static_info);
		    			}
		    		}
		    		flag_static = 1;
	    		}
	    		/* 汇总信息 */
	    		

	    		/* 发回PC */


	    	}
	    	else
	    		continue;
	    }
	    if (n < 0)
	    	err_sys("Socket read error!");
		Close(listenfd);				/* close listening socket */
		Close(connfd);			/* parent closes connected socket */
	}
}

char * RU_cli(char * addr)
{
	int					sockfd_RU;
	struct sockaddr_in	servaddr_RU;
	static char			recvline_RU_scan[BUFFER_SIZE];
	char 				*precv = recvline_RU_scan;
	int 				n_RU = 0;
	int 				sum_nbyte = 0;

	sockfd_RU = Socket(AF_INET, SOCK_STREAM, 0);

	//允许Socket重用，防止出现 bind error: Address in use
	int Reuse_RU = 1;
	setsockopt(sockfd_RU, SOL_SOCKET, SO_REUSEADDR, &Reuse_RU, sizeof(Reuse_RU));

	bzero(&servaddr_RU, sizeof(servaddr_RU));
	servaddr_RU.sin_family = AF_INET;
	servaddr_RU.sin_port   = htons(9900);
	Inet_pton(AF_INET, addr, &servaddr_RU.sin_addr);	/*点分十进制IP地址转二进制数值*/

	Connect(sockfd_RU, (SA *) &servaddr_RU, sizeof(servaddr_RU));

	Write(sockfd_RU, "scan", strlen("scan"));
	while((n_RU = Read(sockfd_RU, precv, MAXLINE)) > 0)
	{
		sum_nbyte += n_RU;
		precv += n_RU;
		if (recvline_RU_scan[sum_nbyte-1] == '#')	//ctrlsrv us '#' as the end of file
			break;
	}
	recvline_RU_scan[sum_nbyte-1] = 0;	/* null terminate */
	// Fputs(recvline_RU_scan, stdout);

	Close(sockfd_RU);
	return recvline_RU_scan;
}
