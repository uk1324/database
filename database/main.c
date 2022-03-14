#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "error.h"
#include "str.h"
#include "types.h"
#include "assertions.h"

// Memory mapped files cannot be resized on windows withingout closing it
// (shared memory which also uses CreateFileMapping can be). To synchronize the
// threads atomic variable could be created that count the number of threads
// a memory mapped file. Then if the file resize is needed a lock saying that
// a file is going to be resized would be set. This would prevent new files
// from incrementing the counter. Then the thread resizing the file would
// wait until only it uses the file and then it can resize it.
// Don't know if there are any problems with this approach.

// Compiled as c++ properites/advances/compile as

// If a function takes a pointer to a string it takes ownership to the deta in the string.

// Could use strerror.

#include "database.h"

#include "allocation.h"

static void table_print(const Table* table)
{
	printf("entry count: %zu\n", *table->entry_auto_increment);
	printf("entry size: %zu\n", table->entry_size);
	for (size_t i = 0; i < table->column_count; i++)
	{
		const Column* column = &table->columns[i];
		printf("column %s: offset = %zu\n", column->name.data, column->offset_in_entry);
	}

	for (size_t i = 0; i < *table->entry_auto_increment; i++)
	{
		const u8* entry = table->data_file_map + table->entry_size * i;

		printf("%zu ", i);
		for (size_t i = 0; i < table->column_count; i++)
		{
			const Column* column = &table->columns[i];
			if (column->data_type.type == DATA_TYPE_I32)
			{
				printf("%d ", entry[column->offset_in_entry]);
			}
			else
			{
				ASSERT_NOT_REACHED();
			}
		}
		printf("\n");
	}
}

#include "language/parser.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

typedef int Socket;

#define PORT "8080"  // the port users will be connecting to

void* get_in_addr(struct sockaddr* sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main()
{
	WSADATA wsa_data;
	int res = WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0;
	if (res != 0)
	{
		log_error("WSAStartup failed: %d\n", res);
		return EXIT_FAILURE;
	}

	struct addrinfo hints, *servinfo;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	int yes = 1;
	char s[INET6_ADDRSTRLEN];

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	int rv;
	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0)
	{
		log_error(stderr, "getaddrinfo: %s\n", gai_strerrorA(rv));
		return EXIT_FAILURE;
	}

	SOCKET sockfd;
	// loop through all the results and bind to the first we can
	struct addrinfo* p;
	for (p = servinfo; p != NULL; p = p->ai_next)
	{
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
		{
			perror("server: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
			sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}

	freeaddrinfo(servinfo); // all done with this structure

	if (p == NULL)
	{
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}

	if (listen(sockfd, 10) == -1) {
		perror("listen");
		exit(1);
	}

	printf("server: waiting for connections...\n");

	while (1) {  // main accept() loop
		sin_size = sizeof their_addr;
		SOCKET new_fd = accept(sockfd, (struct sockaddr*)&their_addr, &sin_size);
		if (new_fd == -1) {
			perror("accept");
			continue;
		}

		inet_ntop(their_addr.ss_family,
			get_in_addr((struct sockaddr*)&their_addr),
			s, sizeof s);
		printf("server: got connection from %s\n", s);

		send(new_fd, "Hello, world!", 13, 0);
		closesocket(new_fd);  // parent doesn't need this
	}

	return 0;

	WSACleanup();

	//return 0;
	//StringView text = string_view_from_cstring("get * from table");
	//Parser parser;
	//Stmt stmt;
	//if (parser_parse(&parser, &stmt, text) == RESULT_ERROR)
	//{
	//	return;
	//}

	//sizeof(Parser);

	return 0;
	//int abc = 3;
	//// Is this little endian only?
	//printf("%p\n", &((u8)abc));
	//printf("%p\n", &((u64)abc));
	//printf("%p\n", &(abc));
	//return 0;

	Table table;
	String name = string_from_cstring("table");
	String name2 = string_clone(&name);
	
	//Result result = table_read(&table, name2);
	Result result = table_create(&table, name2);
	if (result == RESULT_ERROR)
	{
		log_error("failed to read database %s", name);
		return EXIT_FAILURE;
	}

	table_print(&table);
	string_free(&name);

	//Result result = table_create(&table, &name);
	/*if (result == RESULT_ERROR)
	{
		log_error("failed to create database %s", name);
		return EXIT_FAILURE;
	}*/

	//u8* entry = try_malloc(table.entry_size);
	//for (size_t i = 0; i < 3; i++)
	//{
	//	for (size_t i = 0; i < table.column_count; i++)
	//	{
	//		const Column* column = &table.columns[i];
	//		switch (column->data_type.type)
	//		{
	//		case DATA_TYPE_I32:
	//			*(i32*)&entry[column->offset_in_entry] = rand();
	//			break;

	//		default:
	//			ASSERT_NOT_REACHED();
	//		}
	//	}
	//	table_insert_entry(&table, entry);
	//}
	//free(entry);

	table_free(&table);


	return 0;
	//WCHAR* filename = L"test.txt";

	//HANDLE file = CreateFileW(
	//	filename,
	//	GENERIC_READ | GENERIC_WRITE,
	//	0,
	//	NULL,
	//	OPEN_EXISTING,
	//	FILE_ATTRIBUTE_NORMAL,
	//	NULL);

	//if (file == INVALID_HANDLE_VALUE)
	//{
	//	log_error("failed to open file '%s' ec: %d", filename, GetLastError());
	//	return STATUS_ERROR;
	//}

	//HANDLE file_map = CreateFileMappingW(file, NULL, PAGE_READWRITE, 0, 4096, NULL);
	//if (file_map == NULL)
	//{
	//	log_error("failed to create file map ec: '%d'", GetLastError());
	//	return STATUS_ERROR;
	//}

	//u8* file_view = MapViewOfFile(file_map, FILE_MAP_ALL_ACCESS, 0, 0, 4096);
	//if (file_view == NULL)
	//{
	//	log_error("failed to map file view ec: '%d'", GetLastError());
	//	return STATUS_ERROR;
	//}

	////memcpy(file_view, "abcd", 4);

	//BOOL status = UnmapViewOfFile(file_view);
	//if (status == 0)
	//{
	//	log_error("failed to unmap file view ec: '%d'", GetLastError());
	//}
	//status = CloseHandle(file_map);
	//if (status == 0)
	//{
	//	log_error("failed to close file map ec: '%d'", GetLastError());
	//}
	//status = CloseHandle(file);
	//if (status == 0)
	//{
	//	log_error("failed to close file: '%d'", GetLastError());
	//}

	//String abc = string_from_cstring("abc");

	//FILE* file = fopen("test.txt", "a+b");

	//if (fseek(file, PAGE_SIZE, SEEK_SET) != 0)
	//{
	//	error_fatal("fseek failed");
	//}

	//u8 buffer[PAGE_SIZE];
	//if (fwrite(buffer, 1, sizeof(buffer), file) != sizeof(buffer))
	//{
	//	error_fatal("fwrite failed");
	//}

	//if (fclose(file) == EOF)
	//{
	//	error_fatal("fclose failed");
	//}
	//fflush()

	return EXIT_SUCCESS;
}