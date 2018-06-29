#include "IMLM_RU.h"

int main(int argc, char **argv)
{
	int fd;                            //文件描述符
	int len;
	int i;
	char rcv_buf[MAXLINE];

	if(argc != 2)
	{
		printf("Usage: %s /dev/ttyXXX \n",argv[0]);
		return FALSE;
	}

	fd = UART0_Open(fd, argv[1]); //打开串口，返回文件描述符

	UART0_Init(fd, 115200, 0, 8, 1, 'N');
	printf("Set Port Exactly!\n");
	
	// if(0 == strcmp(argv[2], "0"))
	// {
	// 	for(i = 0; i < 10; i++)
	// 	{
	// 		len = UART0_Send(fd,send_buf,15);
	// 		if(len > 0)
	// 			printf(" %d time send %d data successful\n",i,len);
	// 		else
	// 			printf("send data failed!\n");

	// 		sleep(2);
	// 	}
	// 	UART0_Close(fd);
	// }
	while(1) //循环读取数据
	{
		len = UART0_Recv(fd, rcv_buf, MAXLINE);
		if(len > 0)
		{
			rcv_buf[len] = '\0';
			printf("receive data is %s\n",rcv_buf);
			printf("len = %d\n",len);
		}
		else
			printf("cannot receive data\n");
		bzero(rcv_buf, sizeof(rcv_buf));
		sleep(2);
	}
	UART0_Close(fd);
}