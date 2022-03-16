#pragma once

#include "ast.h"
#include "../error.h"

#define EXPR_POOL_SIZE 1024

typedef struct
{
	Expr expr_pool[EXPR_POOL_SIZE];
	size_t exprs_allocated;

	Token current_token;
	Token previous_token;

	Scanner scanner;
} Parser;

Result parser_parse(Parser* parser, Stmt* statement, StringView text);