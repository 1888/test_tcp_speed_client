// TCPClient.cpp: implementation of the CTCPClient class.
//
//////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/un.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <net/if.h>
#include <linux/sockios.h>
#include <pthread.h>

#include "TCPClient.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//构造函数
CTCPClient::CTCPClient()
{
	m_ExitThreadFlag = 0;
	m_nRecvLen = 0;
	m_sockclose = 0;
}

//析构函数
CTCPClient::~CTCPClient()
{

}

/*--------------------------------------------------------------------
【函数介绍】:  此线程用于监听TCP客户端通讯的事件，例如当接收到数据、
			   连接断开和通讯过程发生错误等事件
【入口参数】:  lparam:无类型指针，可以通过此参数，向线程中传入需要用到的资源。
			   在这里我们将CTCPClient类实例指针传进来
【出口参数】:  (无)
【返回  值】:  返回值没有特别的意义，在此我们将返回值设为0。
---------------------------------------------------------------------*/
int CTCPClient::SocketThreadFunc( void* lparam )
{
	CTCPClient *pSocket;
	//得到CTCPClient实例指针
	pSocket = (CTCPClient*)lparam;

	//定义读事件集合
	fd_set fdRead;
	int ret;

	//定义事件等待时间
	struct timeval	aTime;

	while(1)
	{
        //收到退出事件，结束线程
		if ( pSocket->m_ExitThreadFlag )
		{
			printf("ExitThreadFlag=1\n");
			break;
		}
		//置空fdRead事件为空
		FD_ZERO(&fdRead);
		//给客户端socket设置读事件
		FD_SET(pSocket->m_sockfd,&fdRead);
		//调用select函数，判断是否有读事件发生
		aTime.tv_sec = 1;
		aTime.tv_usec = 0;
		ret = select(pSocket->m_sockfd+1,&fdRead,NULL,NULL,&aTime);
		//ret = select(0,&fdRead,NULL,NULL,&aTime);
		
		if (ret < 0 )
		{
			//关闭服务器套接字
			printf( "select = %d\n", ret);
			close(pSocket->m_sockfd );
			break;
		}

		if (ret > 0)
		{
			if (FD_ISSET(pSocket->m_sockfd,&fdRead))
			{
				//发生读事件
				//接收数据
				memset(pSocket->RecvBuf,0,1024);
				pSocket->m_nRecvLen = recv( pSocket->m_sockfd,  pSocket->RecvBuf, 1500,0);
				if (pSocket->m_nRecvLen <0 )
				{
					printf( "Socket error\n");
					close(pSocket->m_sockfd);
					break;
				}
				else if ( pSocket->m_nRecvLen == 0)
				{
					printf( "Close socket %d\n", pSocket->m_sockfd );
					//关闭socket
					close(pSocket->m_sockfd);
					pSocket->m_sockclose = 1;
					break;
				}
				else
				{
					//对接收的数据进行处理，这里为简单的数据回发
					pSocket->RecvBuf[pSocket->m_nRecvLen]=0;
					printf( "RCV:%s\n", pSocket->RecvBuf);
					//pSocket->SendData( pSocket->RecvBuf, pSocket->m_nRecvLen );
					//处理完毕
				}
			}
		}
	}

	printf( "SocketThreadFun finished\n");
	pthread_exit( NULL );
	return 0;
}

/*--------------------------------------------------------------------
【函数介绍】: 用于打开客户端socket
【入口参数】:
【出口参数】: (无)
【返回  值】:  0:打开成功;<0:打开失败
---------------------------------------------------------------------*/
int CTCPClient::Open( char* iface, char* ServerIP, int ServerPort )
{
	char localip[50] = {0};
	struct sockaddr_in * sin;
	//创建TCP套接字
	m_sockfd = socket(AF_INET,SOCK_STREAM, 0 );
   
	if (m_sockfd == -1)
	{
		printf( "Socket fail" );
		return -1;
	}

	strcpy( m_remoteHost, ServerIP );
	m_port = ServerPort;

	m_ExitThreadFlag = 0;

	struct ifreq interface;
	strncpy(interface.ifr_ifrn.ifrn_name, iface, sizeof(interface.ifr_ifrn.ifrn_name)-1);
    printf("ifrn_name %s\n",interface.ifr_ifrn.ifrn_name);
	if (setsockopt(m_sockfd, SOL_SOCKET, SO_BINDTODEVICE, (char *)&interface, sizeof(interface)) < 0)
	{
	    perror("SO_BINDTODEVICE failed");
        return -1;
	}

	if(ioctl (m_sockfd,SIOCGIFADDR,&interface) == -1){
        perror("SIOCGIFADDR failed");
        return -1;
	}
    
	sin = (struct sockaddr_in *)&interface.ifr_addr;
	strcpy(localip,inet_ntoa(sin->sin_addr));
	printf("socket sucess!, sockfd=%d, bind %s, localip %s\n",m_sockfd,iface,localip);

	return 1;
}


/*--------------------------------------------------------------------
【函数介绍】: 用于关闭客户端socket
【入口参数】:  (无)
【出口参数】:  (无)
【返回  值】: TRUE:关闭成功;FALSE:关闭失败
---------------------------------------------------------------------*/
int CTCPClient::Close( )
{
	m_ExitThreadFlag = 1;

	m_sockclose = 1;
	//关闭Socket，释放资源
	int err = close(m_sockfd);
	if( err<0 )
	{
		return 0;
	}
	return 1;
}


/*--------------------------------------------------------------------
【函数介绍】: 用于建立与TCP服务器连接
【入口参数】: (无)
【出口参数】: (无)
【返回  值】: 1:建立连接成功;0:建立连接失败
---------------------------------------------------------------------*/
int CTCPClient::Connect( )
{
    struct	sockaddr_in addr;
	int					ret;
	struct timeval 		timeVal;
	fd_set  			fdSet;
	pthread_attr_t 		attr;
	int					res, flags;


    addr.sin_family = AF_INET;
    addr.sin_port = htons(m_port);
    addr.sin_addr.s_addr=inet_addr(m_remoteHost);

    //设置通讯模式为异步模式
	//
	// Make socket non-blocking
	//
	flags = fcntl(m_sockfd, F_GETFL);
	if( (flags<0)||((fcntl(m_sockfd, F_SETFL, flags |O_ASYNC))<0) )
	{
		perror("fcntl");
	}

	//
	// Connect
	//
	ret = connect(m_sockfd, (struct sockaddr *) &addr, sizeof(addr) );
	if( ret<0 )
	{
		printf( "Connect failed \n");
		return 0;
	}

	m_sockclose = 0;
	timeVal.tv_sec = 1;
	timeVal.tv_usec = 1;

	FD_ZERO(&fdSet);
	FD_SET(m_sockfd, &fdSet);

	ret = select((int) m_sockfd + 1, NULL, &fdSet, NULL, &timeVal);
	if( ret == 0 )
	{
		printf("Connect select timeout with result=%d\n",ret);
		return 0;
	}
	if( ret != 1 )
	{
		printf("Connect select failed with result=%d\n",ret);
		return 0;
	}

	//创建通讯线程
	res = pthread_attr_init(&attr);
	if( res!=0 )
	{
		printf("Create attribute failed\n" );
	}
	// 设置线程绑定属性
	res = pthread_attr_setscope( &attr, PTHREAD_SCOPE_SYSTEM );
	// 设置线程分离属性
	res += pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED );
	if( res!=0 )
	{
		printf( "Setting attribute failed\n" );
	}

	res = pthread_create( &m_thread, &attr, (void *(*) (void *))&SocketThreadFunc, (void*)this );
	if( res!=0 )
	{
		close(m_sockfd );
		return 0;
	}

	pthread_attr_destroy( &attr );

    return 1;
}

/*--------------------------------------------------------------------
【函数介绍】: 向服务器端发送数据
【入口参数】: buf:待发送的数据
              len:待发送的数据长度
【出口参数】: (无)
【返回  值】: 1:发送数据成功;0:发送数据失败
---------------------------------------------------------------------*/
int CTCPClient::SendData( char * buf , int len)
{
	int nBytes = 0;
	int nSendBytes=0;
			
	while (nSendBytes < len)
	{
	    nBytes = send(m_sockfd,buf+nSendBytes,len-nSendBytes,0);
		if (nBytes<0 )
		{
			return nBytes;
		}

		nSendBytes = nSendBytes + nBytes;
		
//		if (nSendBytes < len)
//		{
//		    sleep(1);
//		}
	} 
	return nSendBytes;
}
