#pragma once

#include "scanner.h"

typedef enum
{
	EXPR_IDENTIFIER
} ExprType;

typedef struct
{
	StringView identifier;
} IdentifierExpr;

typedef struct
{
	ExprType type;
	union
	{
		IdentifierExpr identifier;
	};
} Expr;

typedef enum
{
	STMT_GET,
	STMT_INSERT,
} StmtType;

#define STMT_MAX_COLUMNS 4096

typedef struct
{
	Expr columns[STMT_MAX_COLUMNS];
	size_t column_count;

	StringView table_name;
} GetStmt;

typedef struct
{
	StmtType type;
	union
	{
		GetStmt get;
	};
} Stmt;
