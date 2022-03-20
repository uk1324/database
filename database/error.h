#pragma once

typedef enum
{
	RESULT_OK,
	RESULT_ERROR,
} Result;

#define CHECK_RESULT(result) \
	do \
	{ \
		if ((result) == RESULT_ERROR) \
		{ \
			return RESULT_ERROR; \
		} \
	} while (0)

void log_info(const char* format, ...);

void error_fatal(const char* format, ...);

void log_error(const char* format, ...);
void log_error_wsa_strerror(const char* format, ...);