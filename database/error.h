#pragma once

enum Status
{
	STATUS_OK,
	STATUS_ERROR
};

typedef enum
{
	RESULT_OK,
	RESULT_ERROR,
} Result;

void log_info(const char* format, ...);

void error_fatal(const char* format, ...);

void log_error(const char* format, ...);
void log_error_wsa_strerror(const char* format, ...);