// TCPClient.h: interface for the CTCPClient class.
//
//////////////////////////////////////////////////////////////////////
#ifndef _TCPCLIENT_H
#define _TCPCLIENT_H

#include <pthread.h>

class CTCPClient
{
private:
	pthread_t 	m_thread;      						//通讯线程标识符ID
	//数据通讯处理线程函数
	 static int SocketThreadFunc( void*lparam );

public:
	// TCP通讯Socket
	int			m_sockfd;
	int			m_sockclose;
	int			m_ExitThreadFlag;

	//远程主机IP地址
	char     m_remoteHost[255];  
	//远程主机端口
    int      m_port;
    
	char RecvBuf[1500];
	int  m_nRecvLen;

public:
	CTCPClient();
	virtual ~CTCPClient( );

	//打开客户端socket
	int Open( char* iface,char* ServerIP, int ServerPort );
	//关闭客户端socket
	int Close( );
	//与服务器端建立连接  并创建数据通讯处理线程
	int Connect();
	//向服务器端发送数据
	int SendData( char * buf , int len);
};

#endif
