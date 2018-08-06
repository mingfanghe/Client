// Client.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <WINSOCK2.H>
#include <iostream>
#include <WS2tcpip.h>
#include <ctime>
using namespace std;
#pragma comment(lib,"ws2_32.lib")
#define  CLOCKS_PER_SEC ((clock_t)1000)
int main()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	//��1����ѭ��������Ϣ
	int QPScount = 0;
	clock_t starttime, endtime;
	double time = 0;
	starttime = clock();
	while (time < 1)
	{
		SOCKET ClientConnetServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		SOCKADDR_IN ServerAddr;
		memset(&ServerAddr, 0, sizeof(ServerAddr));
		ServerAddr.sin_family = AF_INET;
		ServerAddr.sin_port = htons(8888);
		ServerAddr.sin_addr.s_addr = inet_addr("100.64.15.20");

		if (connect(ClientConnetServer, (SOCKADDR *)&ServerAddr, sizeof(ServerAddr)) == SOCKET_ERROR)
		{
			cout << "connect error !" << endl;
			closesocket(ClientConnetServer);
			return 0;
		}

		//�ɿͻ��˷������ݡ�Hello Server !�����ͻ���
		const char * sendData = "Hello Server !";
		send(ClientConnetServer, sendData, strlen(sendData), 0);

		//���ܿͻ�����Ϣ
		char recData[255];
		int ret = recv(ClientConnetServer, recData, 255, 0);
		if (ret > 0)
		{
			recData[ret] = '\0';
			cout << recData << endl;
		}
		
		endtime = clock();
		time = (double)(endtime - starttime)/ CLOCKS_PER_SEC;
		QPScount++;
		closesocket(ClientConnetServer);
	}
	
	cout << QPScount << endl;
	system("pause");
	WSACleanup();
    return 0;
}

