#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include "TCPClient.h"
#include <sys/time.h>

#define MAX_ETH_CNT 4

int main(int argc, char** argv){

	char serverip[20] = "192.168.201.92";
	int serverport = 50000;
	char eth[MAX_ETH_CNT][20]={0};
	int i=0;
	int ethcnt = 0;
	char sendbuf[1024] ;
	class CTCPClient tcpClient[MAX_ETH_CNT];

	printf("=====test_tcp_speed_client V1.0=====\n");

	if(argc < 4 || argc > 7){
		printf("format Error, Please use as :\n");
		printf("Client Usage:\n");
		printf("\ttest_tcp_speed_client serverip serverport iface...\n");
		printf("\tonly test eth0: test_tcp_speed_client 192.168.201.93 50000 eth0 \n");
		printf("\ttest 4 eth: test_tcp_speed_client 192.168.201.93 50000 eth0 eth1 eth2 eth3\n");
		exit(1);
	}


	strncpy(serverip,argv[1],strlen(argv[1]));
	memset(serverip+strlen(argv[1]),0,sizeof(serverip) - strlen(argv[1]));

	serverport = atoi(argv[2]);

	for(i=3;i<argc;i++){
        memset(eth[i-3],0,20);
		memcpy(eth[i-3] , argv[i],strlen(argv[i]));
	}
	ethcnt = argc -3;
	printf("your cmdline:\n");
	printf("%s %s %d ",argv[0],serverip,serverport);
	for(i=0;i<ethcnt;i++){
		printf("%s ",eth[i]);
	}
	printf("\n");

    memset(sendbuf,'A',1024);
	tcpClient[0].Open(eth[0],serverip,serverport);
    tcpClient[0].Connect();

	long total_send[MAX_ETH_CNT] = {0};
	int usec = 0;
	int len=0;
	struct   timeval   start,stop,diff;
    printf("------start test -----\n");
	gettimeofday(&start,0);
	while(true){
        //printf("total_send[0] = %ld\n",total_send[0]);
		len = tcpClient[0].SendData(sendbuf,1024);
		if(len > 0){
			total_send[0] += len;
		}
		if(total_send[0] > 50000000){
			//if total_send is more than 50MB,break;
            printf("while loop break\n");
			break;
		}
	}
	gettimeofday(&stop,0);
	usec = (stop.tv_sec - start.tv_sec)*1000000 + (stop.tv_usec - start.tv_usec);

	printf("total_send[0] %dByte, time %d us, speed %fMBytes/s\n",total_send[0],usec,(total_send[0]/1000000)/(usec/1000000.0));
	return 0;
}
