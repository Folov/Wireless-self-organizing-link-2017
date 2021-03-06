#include "IMLM_S.h"
#define NUM_THREADS 3

int openwrt[MAXROUTER] = {0};
int openwrt_med[MAXROUTER] = {0};
int	flag_static = 0;

pthread_mutex_t mutex_med;
pthread_mutex_t mutex_static_flag;


int main(int argc, char const *argv[])
{
	pthread_t tid[NUM_THREADS];
	if (pthread_create(&tid[0], NULL, Heartbeats, NULL) != 0)
		err_sys("pthread_create Heartbeats error!");
	if (pthread_detach(tid[0]) != 0)
		err_sys("pthread_detach Heartbeats error!");

	if (pthread_create(&tid[1], NULL, Renew_openwrt, NULL) != 0)
		err_sys("pthread_create Renew_openwrt error!");
	if (pthread_detach(tid[1]) != 0)
		err_sys("pthread_detach Renew_openwrt error!");

	if (pthread_create(&tid[2], NULL, PC_server, NULL) != 0)
		err_sys("pthread_create PC_server error!");
	pthread_join(tid[2],NULL);


	return 0;
}

/*****************************Heartbeats************************************************/
void *Heartbeats()
{
	int					sockfd;
	struct sockaddr_in	servaddr, cliaddr;
	// struct timeval		tv_o;

	sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(9899);	// port: 9899 for Heartbeat

	Bind(sockfd, (SA *) &servaddr, sizeof(servaddr));

	// //设置SO_RCVTIMEO 接收超时7s 必要性：更新recvfrom
	// tv_o.tv_sec = 7;
	// tv_o.tv_usec = 0;
	// setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv_o, sizeof(tv_o));

	dg_echo(sockfd, (SA *) &cliaddr, sizeof(cliaddr));

	return 0;
}


void dg_echo(int sockfd, SA *pcliaddr, socklen_t clilen)
{
	socklen_t	len;
	char		mesg[MAXLINE];
	char *		p_num = mesg + 7; //point to the number of "openwrt17"(17)
	int 		router_num = 0;
	int 		n_sendto = 0;
	int 		n_recvfrom = 0;

	for ( ; ; ) {
		len = clilen;
		if ((n_recvfrom = recvfrom(sockfd, mesg, MAXLINE, 0, pcliaddr, &len)) < 0)
		{
			fprintf(stderr, "recvfrom err!\n");
			continue;
		}
		router_num = atoi(p_num);	// array to (int)number

		pthread_mutex_lock(&mutex_med);
		openwrt_med[router_num] = 1;
		pthread_mutex_unlock(&mutex_med);

		bzero(mesg, MAXLINE);
		if ((n_sendto = sendto(sockfd, "Heartbeats_Echo\n", sizeof("Heartbeats_Echo\n"), 0, pcliaddr, len)) < 0)
		{
			fprintf(stderr, "sendto err!\n");
			continue;
		}
	}
}

/*****************************Renew_openwrt************************************************/

void *Renew_openwrt()
{
	char command[40] = "/root/link_del.sh ";
	char errip[20];
	pid_t status;
	while(1)
	{
		//7s (1)
		sleep(7);								// 一倍去抖4s，二倍去抖7s
		pthread_mutex_lock(&mutex_med);
		memset(openwrt_med, 0, sizeof(openwrt_med));
		pthread_mutex_unlock(&mutex_med);

		//7s (2)
		sleep(7);
		pthread_mutex_lock(&mutex_med);			// initial value: 00
		for (int i = 0; i < MAXROUTER; ++i)		// 00 -> 00
		{										// 01 -> 00
			if (openwrt_med[i] == 0)			// 10 -> 11
			{									// 11 -> 11
				if (openwrt[i] == 1)			// if (0|1) -> link err
				{
					sprintf(errip, "192.168.%d.0", i);
					strcat(command, errip);
					status = system(command);

					bzero(command, sizeof(command));
					strcpy(command, "/root/link_del.sh ");
					
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
				}
				openwrt[i] = 0;
			}
			else	// 1X -> 11
			{
				pthread_mutex_lock(&mutex_static_flag);
				if (openwrt[i] == 0)			// if(1|0) -> link connect
					flag_static = 0;
				pthread_mutex_unlock(&mutex_static_flag);
				openwrt[i] = 1;
			}
		}
		pthread_mutex_unlock(&mutex_med);
	}
}

/*****************************PC_server************************************************/

void *PC_server()
{
	FILE 				*fd;
	int 				flen = 0;
	int					listenfd, connfd;
	struct sockaddr_in	servaddr;
	char				recvline_PC[MAXLINE + 1];
	char 				IP_str[20];
	char 				*p_static_info;
	char 				*buffer_static_info;
	char				*buffer_link_info;
	char 				*buffer_all;
	char				temp_opemwrt[MAXROUTER] = {0};
	int 				n = 0;
	int 				link_num = 0;
	char				buffer_link_num[5];

	buffer_static_info = (char *)malloc(sizeof(char)*BUFFER_SIZE);
	buffer_link_info = (char *)malloc(sizeof(char)*MAXLINE);
	buffer_all = (char *)malloc(sizeof(char)*(BUFFER_SIZE + MAXROUTER + MAXLINE));
	memset(buffer_static_info, 0, sizeof(char)*BUFFER_SIZE);
	memset(buffer_link_info, 0, sizeof(char)*MAXLINE);
	memset(buffer_all, 0, sizeof(char)*(BUFFER_SIZE + MAXROUTER + MAXLINE));

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(9901);	// port: 9901 for S to PC

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
				printf("PC Please message get!\n");
				signal(SIGCHLD,SIG_DFL);
				/* 取各节点信息保存 */
				pthread_mutex_lock(&mutex_static_flag);
				if (flag_static == 0)
				{
					memset(buffer_static_info, 0, sizeof(char)*BUFFER_SIZE);
					for (int i = 0; i < MAXROUTER; ++i)					//static message
					{
						if (openwrt[i] == 1)
						{
							strcat(buffer_static_info, "------\n");
							sprintf(IP_str, "192.168.%d.%d", i, i);
							printf("Getting data from %s\n", IP_str);
							strcat(buffer_static_info, IP_str);
							strcat(buffer_static_info, "\n");
							p_static_info = RU_cli(IP_str);
							strcat(buffer_static_info, p_static_info);
						}
					}
					flag_static = 1;
				}
				pthread_mutex_unlock(&mutex_static_flag);
				/* 汇总信息 */
				pthread_mutex_lock(&mutex_med);							//Heartbeats message
				for (int j = 0; j < MAXROUTER; ++j)
				{
					temp_opemwrt[j] = (char)(48 + openwrt[j]);
				}
				pthread_mutex_unlock(&mutex_med);

				if ((fd = fopen("/tmp/wsol/wsol_link.txt", "r")) == NULL)
					printf("wsol_link.txt open wrong!\n");				//link message
				else
				{
					memset(buffer_link_info, 0, sizeof(char)*MAXLINE);
					bzero(buffer_link_num, sizeof(buffer_link_num));
					link_num = 0;
					fseek(fd, 0L, SEEK_END);
					flen = ftell(fd);
					if (flen == 0)
						printf("wsol_link.txt is empty!\n");
					fseek(fd, 0L, SEEK_SET);
					fread(buffer_link_info, flen, 1, fd);
					strcat(buffer_link_info, "\0");
					fclose(fd);
				}

				strcpy(buffer_all, temp_opemwrt);
				strcat(buffer_all, "\n");
				for (int i = 0; i < flen; ++i)
				{
					if (buffer_link_info[i] == '\n')
						link_num++;
				}
				sprintf(buffer_link_num, "%d\n", link_num);
				strcat(buffer_all, buffer_link_num);
				strcat(buffer_all, buffer_link_info);
				strcat(buffer_all, buffer_static_info);
				strcat(buffer_all, "#");
				/* 发回PC */
				Write(connfd, buffer_all, strlen(buffer_all));
				printf("PC transmition success!\n");
				bzero(buffer_all, sizeof(buffer_all));
				bzero(recvline_PC, sizeof(recvline_PC));
			}
			else if (strcmp(recvline_PC, "Renew~") == 0)
			{
				printf("PC Renew message get!\n");
				signal(SIGCHLD,SIG_DFL);
				/* 取各节点信息保存 */
				pthread_mutex_lock(&mutex_static_flag);
				memset(buffer_static_info, 0, sizeof(char)*BUFFER_SIZE);
				for (int i = 0; i < MAXROUTER; ++i)						//static message
				{
					if (openwrt[i] == 1)
					{
						strcat(buffer_static_info, "------\n");
						sprintf(IP_str, "192.168.%d.%d", i, i);
						printf("Getting data from %s\n", IP_str);
						strcat(buffer_static_info, IP_str);
						strcat(buffer_static_info, "\n");
						p_static_info = RU_cli(IP_str);
						strcat(buffer_static_info, p_static_info);
					}
				}
				pthread_mutex_unlock(&mutex_static_flag);
				/* 汇总信息 */
				pthread_mutex_lock(&mutex_med);							//Heartbeats message
				for (int j = 0; j < MAXROUTER; ++j)
				{
					temp_opemwrt[j] = (char)(48 + openwrt[j]);
				}
				pthread_mutex_unlock(&mutex_med);

				if ((fd = fopen("/tmp/wsol/wsol_link.txt", "r")) == NULL)
					printf("wsol_link.txt open wrong!\n");				//link message
				else
				{
					memset(buffer_link_info, 0, sizeof(char)*MAXLINE);
					bzero(buffer_link_num, sizeof(buffer_link_num));
					link_num = 0;
					fseek(fd, 0L, SEEK_END);
					flen = ftell(fd);
					if (flen == 0)
						printf("wsol_link.txt is empty!\n");
					fseek(fd, 0L, SEEK_SET);
					fread(buffer_link_info, flen, 1, fd);
					strcat(buffer_link_info, "\0");
					fclose(fd);
				}

				strcpy(buffer_all, temp_opemwrt);
				strcat(buffer_all, "\n");
				for (int i = 0; i < flen; ++i)
				{
					if (buffer_link_info[i] == '\n')
						link_num++;
				}
				sprintf(buffer_link_num, "%d\n", link_num);
				strcat(buffer_all, buffer_link_num);
				strcat(buffer_all, buffer_link_info);
				strcat(buffer_all, buffer_static_info);
				strcat(buffer_all, "#");
				/* 发回PC */
				Write(connfd, buffer_all, strlen(buffer_all));
				printf("PC transmition success!\n");
				bzero(buffer_all, sizeof(buffer_all));
				bzero(recvline_PC, sizeof(recvline_PC));
			}
			else
				continue;
		}
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

	memset(recvline_RU_scan, 0, sizeof(char)*BUFFER_SIZE);

	sockfd_RU = Socket(AF_INET, SOCK_STREAM, 0);

	//允许Socket重用，防止出现 bind error: Address in use
	int Reuse_RU = 1;
	setsockopt(sockfd_RU, SOL_SOCKET, SO_REUSEADDR, &Reuse_RU, sizeof(Reuse_RU));

	bzero(&servaddr_RU, sizeof(servaddr_RU));
	servaddr_RU.sin_family = AF_INET;
	servaddr_RU.sin_port   = htons(9900);	// port: 9900 for RU to S
	Inet_pton(AF_INET, addr, &servaddr_RU.sin_addr);	/*点分十进制IP地址转二进制数值*/

	Connect(sockfd_RU, (SA *) &servaddr_RU, sizeof(servaddr_RU));	

	Write(sockfd_RU, "scan", strlen("scan"));

	while((n_RU = Read(sockfd_RU, precv, MAXLINE)) > 0)
	{
		sum_nbyte += n_RU;
		precv += n_RU;
		if (recvline_RU_scan[sum_nbyte-1] == '#')	//IMLM_RU use '#' as the end of file
			break;
	}
	recvline_RU_scan[sum_nbyte-1] = 0;	/* null terminate */
	// Fputs(recvline_RU_scan, stdout);

	Close(sockfd_RU);
	return recvline_RU_scan;
}
