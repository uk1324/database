#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock.h>
#include "error.h"

void log_info(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	fprintf(stderr, "info: ");
	vfprintf(stderr, format, args);
	fprintf(stderr, "\n");
	va_end(args);
}

void error_fatal(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	fprintf(stderr, "fatal: ");
	vfprintf(stderr, format, args);
	fprintf(stderr, "\n");
	va_end(args);
	exit(EXIT_FAILURE);
}

void log_error(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	fprintf(stderr, "error: ");
	vfprintf(stderr, format, args);
	fprintf(stderr, "\n");
	va_end(args);
}

void log_error_strerror(const char* format, ...)
{
	fprintf(stderr, "error: ");
	va_list args;
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
	char buffer[256];
	buffer[0] = '\0';    
	//strerror_s(buffer, sizeof(buffer), errno);
	int error = WSAGetLastError();
	FormatMessageA(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 
		NULL,
		error,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		buffer,
		sizeof(buffer),
		NULL);
	if (*buffer == '\0')
		sprintf(buffer, " %d", error);
	fprintf(stderr, ": %s\n", buffer);
}
