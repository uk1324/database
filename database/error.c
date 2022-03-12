#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
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