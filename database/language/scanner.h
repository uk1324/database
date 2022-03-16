#pragma once

#include "../types.h"
#include "../string_view.h"

// This isn't called TokenType because windows.h includes winnt.h which has an enum member TokenType
// Can't just not include winnt.h because it also defined HANDLE among other things.
typedef enum
{
	TOKEN_KEYWORD_GET,
	TOKEN_KEYWORD_INSERT,

	TOKEN_COMMA,

	TOKEN_IDENTIFIER,

	TOKEN_INTEGER,

	TOKEN_ERROR,
	TOKEN_EOF,
} ToknType;

typedef struct
{
	ToknType type;
	union
	{
		u64 integer;
		StringView identifier;
	} value;
} Token;

typedef struct
{
	StringView text;

	size_t token_start_char_index;
	size_t current_char_index;
} Scanner;

void scanner_init(Scanner* scanner, StringView text);
Token scanner_next(Scanner* scanner);