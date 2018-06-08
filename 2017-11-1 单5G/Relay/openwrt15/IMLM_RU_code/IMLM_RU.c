#include "IMLM_RU.h"
#define NUM_THREADS 2

int openwrt[MAXROUTER] = {0};
int openwrt_med[MAXROUTER] = {0};

pthread_mutex_t mutex_med;

struct Argument
{
	char IP_str[20];
	char SSID[MAXID];
};

int main(int argc, char const *argv[])
{
	char tmp_ssid[MAXID] = {0};
	const char * pnum = argv[2] + 7;

	if (argc != 3)
		err_quit("usage: IMLM_RU <IPaddress> <SSID>");
	strcpy(tmp_ssid, argv[2]);
	tmp_ssid[7] = 0;
	if (strcmp(tmp_ssid, "openwrt") != 0)
	{
		printf("SSID WRONG!\n");
		err_quit("usage: IMLM_RU <IPaddress> <SSID>");
	}
	if ((atoi(pnum) > 255 || atoi(pnum) <= 0))
	{
		printf("SSID WRONG!\n");
		err_quit("usage: IMLM_RU <IPaddress> <SSID>");
	}

	struct Argument arg_heartbeat;
	strcpy(arg_heartbeat.IP_str, argv[1]);
	strcpy(arg_heartbeat.SSID, argv[2]);

	pthread_t tid[NUM_THREADS];
	
	if (pthread_create(&tid[0], NULL, S_server, NULL) != 0)
		err_sys("pthread_create S_server error!");
	if (pthread_detach(tid[0]) != 0)
		err_sys("pthread_detach S_server error!");

	if (pthread_create(&tid[1], NULL, Heartbeat, (void *)&arg_heartbeat) != 0)
		err_sys("pthread_create Heartbeat error!");

	pthread_join(tid[1],NULL);

	return 0;
}


/*****************************Heartbeat************************************************/
void *Heartbeat(void *arg)
{
	int					sockfd, n, n_write;
	struct sockaddr_in	servaddr;
	struct timeval		tv_o;
	struct Argument		*parg_in;
	char				recvline[MAXLINE + 1];
	int 				readerr_count = 0;
	int 				connecterr_count = 0;
	int 				status = 0;

	parg_in = (struct Argument *)arg;

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(9899);	// port: 9899 for Heartbeat
	Inet_pton(AF_INET, parg_in->IP_str, &servaddr.sin_addr);

	sockfd = Socket(AF_INET, SOCK_DGRAM, 0);
	//设置SO_RCVTIMEO 接收超时3s 必要性：防止服务端端宕机后阻塞于Read调用
	tv_o.tv_sec = 3;
	tv_o.tv_usec = 0;
	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv_o, sizeof(tv_o));
 
	strcat(parg_in->SSID, "\n");	//没有\n udpsrv不会换行
	while(1)
	{
		sleep(3);	// Every 3s send heartbeat message
		bzero(recvline, sizeof(recvline));

		if(connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0)		//有助于快速得知服务端是否正常
		{
			connecterr_count++;
			printf("Connect error! Time: %d\n", connecterr_count);
			if (connecterr_count == 4)
			{
				printf("Ready to reboot!\n");
				break;
			}
			continue;
		}

		if ((n_write = write(sockfd, parg_in->SSID, strlen(parg_in->SSID))) < 0)
		{
			fprintf(stderr, "Heartbeat write err!\n");
			continue;
		}

		if((n = read(sockfd, recvline, MAXLINE)) == -1 && errno == EAGAIN)
		{
			readerr_count++;
			printf("UDP read timeout! Time: %d\n", readerr_count);
			if (readerr_count == 4)
			{
				printf("Ready to reboot!\n");
				break;
			}								//3*4 = 12s. 重复4次3s超时，代表链路某节点宕机 break 重启
			continue;						//同时防止"read err: Resource temporarily unavailable" 原因：errno == EAGAIN(接收超时4s)
		}
		else if (n == -1)
		{
			readerr_count++;
			fprintf(stderr, "Heartbeat read Echo err! NOT timeout! Time: %d\n", readerr_count);
			if (readerr_count == 4)
			{
				printf("Ready to reboot!\n");
				break;
			}
			continue;
		}
		readerr_count = 0;
		connecterr_count = 0;
		recvline[n] = 0;	/* null terminate */
		Fputs(recvline, stdout);

	}

	status = system("reboot");
	/**** never print ****/
	if (-1 == status)
		printf("system error!");
	else
	{
		printf("exit status value = [0x%x]\n", status);
		if (WIFEXITED(status))
		{
			if (0 == WEXITSTATUS(status))
				printf("run shell script successfully.\n");
			else
				printf("run shell script fail, script exit code: %d\n", WEXITSTATUS(status));
		}
		else
			printf("exit status = [%d]\n", WEXITSTATUS(status));
	}
	exit(1);
}

/*****************************PC_server************************************************/

void *S_server()
{
	FILE				*fp = NULL;
	int					listenfd, connfd;
	struct sockaddr_in	servaddr;
	char				recvline_S[MAXLINE + 1];
	char 				*buffer_static_info;
	int 				n = 0;
	int 				fsize = 0;

	buffer_static_info = (char *)malloc(sizeof(char)*BUFFER_SIZE);
	memset(buffer_static_info, 0, sizeof(char)*BUFFER_SIZE);

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(9900);	// port: 9900 for RU to S

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
	    while ((n = Read(connfd, recvline_S, MAXLINE)) > 0)
	    {
	    	recvline_S[n] = 0;	/* null terminate：数组结尾置0 */
	    	if (strcmp(recvline_S, "scan") == 0)
	    	{
	    		printf("Source scan request message get!\n");
	    		signal(SIGCHLD,SIG_DFL);
	    		/* 读取文件 */
	    		if ((fp = fopen("/tmp/wsol/iwscan_trans.txt", "rb")) == NULL)
	    		{
	    			printf("Can not open file iwscan_trans.txt\n");
	    			exit(3);
	    		}
	    		//求得文件的大小  
				fseek(fp, 0, SEEK_END);  
				fsize = ftell(fp);  
				rewind(fp);

				fread(buffer_static_info, 1, fsize, fp);	//每次读一个，共读size次

				fclose(fp);
	    		/* 发回PC */
	    		strcat(buffer_static_info, "#");	// use '#' as the end of array
				Write(connfd, buffer_static_info, strlen(buffer_static_info));
				memset(buffer_static_info, 0, sizeof(char)*BUFFER_SIZE);
				bzero(recvline_S, sizeof(recvline_S));
	    	}
	    	else if (strcmp(recvline_S, "gps") == 0)
	    	{
	    		printf("Source gps request message get!\n");
	    		signal(SIGCHLD,SIG_DFL);

	    	}
	    	else
	    		continue;
	    }
		Close(connfd);			/* parent closes connected socket */
	}
}
