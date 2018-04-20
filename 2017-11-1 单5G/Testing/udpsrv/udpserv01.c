#include "udp.h"

int main(int argc, char **argv)
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
}


void dg_echo(int sockfd, SA *pcliaddr, socklen_t clilen)
{
	// int			n;
	socklen_t	len;
	char		mesg[MAXLINE];

	for ( ; ; ) {
		len = clilen;
		Recvfrom(sockfd, mesg, MAXLINE, 0, pcliaddr, &len);
		printf("Heartbeats from: %s", mesg);
		bzero(mesg, MAXLINE);
		Sendto(sockfd, "Heartbeats_Echo\n", sizeof("Heartbeats_Echo\n"), 0, pcliaddr, len);
	}
}

