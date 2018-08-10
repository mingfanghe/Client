// Client.cpp : 定义控制台应用程序的入口点。
#include "stdafx.h"
#include <WINSOCK2.H>
#include <iostream>
#include <WS2tcpip.h>
#include <ctime>
#include <cstddef>
#include <string>
#include <cassert>
using namespace std;
#pragma comment(lib,"ws2_32.lib")
#define  CLOCKS_PER_SEC ((clock_t)1000)

namespace bytes_helper {
	template <class T> struct type {};

	template <typename T, class iter>
	T read(iter it, type<T>) {
		T i = T();
		//[01][02][03][04]
		int T_len = sizeof(T);
		for (int idx = 0; idx < T_len; ++idx) {
			i |= *(it + idx) << (3 - idx) * 8;
		}
		return  i;
	}

	template <typename T, class iter>
	int write(T v, iter it, int size) {
		int i = 0;
		int T_len = sizeof(T);
		for (int idx = 0; idx < T_len; ++idx) {
			*(it + idx) = v >> (3 - idx) * 8;
			++i;
		}
		return i;
	}
}

enum packet_receive_state {
	S_READ_LEN,
	S_READ_CONTENT
};

int main()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET ClientConnetServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN ServerAddr;
	memset(&ServerAddr, 0, sizeof(ServerAddr));
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(8888);
	ServerAddr.sin_addr.s_addr = inet_addr("100.64.15.20");

	int connectrt = connect(ClientConnetServer, (SOCKADDR *)&ServerAddr, sizeof(ServerAddr));
	if (connectrt == SOCKET_ERROR)
	{
		int rt = ::WSAGetLastError();
		cout << "connect error: " << rt << endl;
		return rt;
	}
	else cout << "connect success !" << endl;
	

	//客户端要发送的数据包

	int recvmessage_len;
	const int buf_len = 1024;

	packet_receive_state s = S_READ_LEN;

	//send buffer
	static const char* package_content = "Hello Server !";
	static const int host_len = strlen(package_content);
	char wbuffer[buf_len] = { 0 };
	int wbuffer_write_idx = 0;
	assert(1024 >= sizeof(int));
	int wlen = bytes_helper::write<int>(host_len, wbuffer, 1024);
	assert(sizeof(int) == wlen);
	wbuffer_write_idx += wlen;
	memcpy(wbuffer + wbuffer_write_idx, package_content, host_len);
	wbuffer_write_idx += host_len;

	//received buffer
	char rbuffer[buf_len] = { 0 };
	int rbuffer_read_idx = 0;
	int rbuffer_write_idx = 0;

	int nreceived = 0;
	//在1秒内循环发送消息
	clock_t starttime, endtime;
	double time = 0;
	starttime = clock();
	//
	while (time < 56)
	{
		nreceived++;
		//由客户端发送数据“Hello Server !”到客户端
		int sndrt = send(ClientConnetServer, wbuffer, wbuffer_write_idx, 0);

		//接受服务器回的消息
	read_again:
		int ret = recv(ClientConnetServer, rbuffer + rbuffer_write_idx, buf_len - rbuffer_write_idx, 0);
		if (ret == SOCKET_ERROR)
		{
			//TODO
			assert(!"todo");
		}
		rbuffer_write_idx += ret;
	read_content:
		switch (s)
		{
			case S_READ_LEN:
			{
				if (rbuffer_write_idx < sizeof(int) - 1) {
					goto read_again;
				}
				recvmessage_len = bytes_helper::read<int>(rbuffer, bytes_helper::type<int>());
				rbuffer_read_idx += sizeof(int);
				s = S_READ_CONTENT;
				goto read_content;
			}
			case S_READ_CONTENT:
			{
				int wrlen = rbuffer_write_idx - rbuffer_read_idx;
				if (wrlen >= recvmessage_len)
				{
					char recv_content[buf_len] = { 0 };
					memcpy(recv_content, rbuffer + rbuffer_read_idx, recvmessage_len);
					rbuffer_read_idx += recvmessage_len;
					wrlen = rbuffer_write_idx - rbuffer_read_idx;
					//cout << "received bytes: " << recv_content << endl;

					//将缓存里的第一个消息删除，把offset之前的挪到前面
					memcpy(rbuffer, rbuffer + rbuffer_read_idx, wrlen);
					rbuffer_read_idx = 0;
					rbuffer_write_idx = wrlen;
					s = S_READ_LEN;
					break;
				}
				else
				{
					goto read_again;
				}
			}	
		}
		endtime = clock();
		time = (double)(endtime - starttime) / CLOCKS_PER_SEC;
	}
	cout << "the total send packet: " << nreceived << endl;
	closesocket(ClientConnetServer);
	system("pause");
	WSACleanup();
	return 0;
}