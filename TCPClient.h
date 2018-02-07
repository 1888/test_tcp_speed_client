// TCPClient.h: interface for the CTCPClient class.
//
//////////////////////////////////////////////////////////////////////
#ifndef _TCPCLIENT_H
#define _TCPCLIENT_H

#include <pthread.h>

class CTCPClient
{
private:
	pthread_t 	m_thread;      						//ͨѶ�̱߳�ʶ��ID
	//����ͨѶ�����̺߳���
	 static int SocketThreadFunc( void*lparam );

public:
	// TCPͨѶSocket
	int			m_sockfd;
	int			m_sockclose;
	int			m_ExitThreadFlag;

	//Զ������IP��ַ
	char     m_remoteHost[255];  
	//Զ�������˿�
    int      m_port;
    
	char RecvBuf[1500];
	int  m_nRecvLen;

public:
	CTCPClient();
	virtual ~CTCPClient( );

	//�򿪿ͻ���socket
	int Open( char* iface,char* ServerIP, int ServerPort );
	//�رտͻ���socket
	int Close( );
	//��������˽�������  ����������ͨѶ�����߳�
	int Connect();
	//��������˷�������
	int SendData( char * buf , int len);
};

#endif