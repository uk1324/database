#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#include "../error.h"
#include "../types.h"
#include "../data_type.h"

#define PORT "8080"

static Result buffered_read(SOCKET socket, void* buff, size_t bytes)
{
	u8* buffer = (u8*)buff;

	static u8 read_buffer[4096];

	int bytes_read = recv(socket, buffer, bytes, 0);
	if (bytes_read <= 0)
	{
		return RESULT_ERROR;
	}
	


	return RESULT_OK;
}

static void* get_in_addr(struct sockaddr* sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int client_thread()
{
	int sock, numbytes;
	struct addrinfo *servinfo;
	int rv;
	char s[INET6_ADDRSTRLEN];

	struct addrinfo hints;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	int result;
	if ((result = getaddrinfo("localhost", PORT, &hints, &servinfo)) != 0)
	{
		log_error("getaddrinfo: %s\n", gai_strerrorA(result));
		return 1;
	}

	struct addrinfo* p;
	for (p = servinfo; p != NULL; p = p->ai_next)
	{
		if ((sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == INVALID_SOCKET)
		{
			log_error_wsa_strerror("socket");
			continue;
		}

		if (connect(sock, p->ai_addr, p->ai_addrlen) == SOCKET_ERROR)
		{
			log_error_wsa_strerror("connect");
			closesocket(sock);
			continue;
		}

		break;
	}

	if (p == NULL)
	{
		log_error("failed to connect\n");
		return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr*)p->ai_addr), s, sizeof s);
	printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo);



	//if ((numbytes = recv(sockfd, read_buffer, sizeof(read_buffer), 0)) == SOCKET_ERROR)
	//{
	//	log_error_wsa_strerror("recv failed");
	//	//exit(1);
	//}

	char statement[] = "get col1, col1";
	send(sock, statement, sizeof(statement), 0);

#define READ_BYTES(data_ptr, bytes_to_read) \
	if (buffered_read(sock, data_ptr, bytes_to_read) == RESULT_ERROR) \
	{ \
		return -1; \
	}

#define READ(data_ptr) READ_BYTES(data_ptr, sizeof(*data_ptr))

	u32 status_code;
	READ(&status_code);
	u32 message_length;
	READ(&message_length);
	char message[299];
	READ_BYTES(message, message_length);
	u64 column_count;
	printf("status: %d, message: %.*s\n", status_code, message_length, message);
	READ(&column_count);
	u64 entry_count;
	READ(&entry_count);

	DataType data_types[299];
	for (size_t i = 0; i < column_count; i++)
	{
		u32 data_type;
		READ(&data_type);
		data_types[i].type = data_type;
		u32 name_size;
		READ(&name_size);
		READ_BYTES(message, name_size);
		printf("data_type: %d name: %.*s\n", data_type, name_size, message);
	}
	printf("entry count: %zu\n", entry_count);

	for (size_t i = 0; i < entry_count; i++)
	{
		printf("1 - ");
		for (size_t i = 0; i < column_count; i++)
		{
			switch (data_types->type)
			{
			case DATA_TYPE_I32:
			{
				i32 v;
				READ(&v);
				printf("%d ", v);
			}

			default:
				break;
			}
		}
		printf("\n");
	}

#undef READ_BYTES
#undef READ
	//u32* message_length = status_code + 1;
	//u8* message = message_length + 4;
	


	////buf[numbytes] = '\0';

	//printf("client: received '%s'\n", buf);

	closesocket(sock);

	return 0;
}

