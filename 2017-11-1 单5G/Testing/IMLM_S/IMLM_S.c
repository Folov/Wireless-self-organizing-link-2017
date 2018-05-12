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
	char				recvline[MAXLINE + 1];
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

    while(1)
    {
	    connfd = Accept(listenfd, (SA *) NULL, NULL);	
	    //第2,3个参数用于接收accept函数返回的对端客户协议地址。可以置空。我们不感兴趣。
	    //accept函数返回已连接套接字(TCP三路握手成功后的)，它代表client端套接字
	    while ((n = Read(connfd, recvline, MAXLINE)) > 0)
	    {
	    	recvline[n] = 0;	/* null terminate：数组结尾置0 */
	    	if (strcmp(recvline, "Please~") == 0)
	    	{
	    		printf("PC request message get!\n");
	    		signal(SIGCHLD,SIG_DFL);
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


