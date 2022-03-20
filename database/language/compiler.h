#pragma once

#include "ast.h"
#include "../database.h"
#include "../data_type.h"
#include "../error.h"

typedef enum
{
	// Offset and size as a u16.
	OP_LOAD_ENTRY_OFFSET_SIZE,

	OP_RETURN
} Op;

typedef struct
{
	u8* bytecode;
	size_t bytecode_max_bytes;

	size_t bytecode_bytes_written;

	const Table* table;
} Compiler;

Result compiler_compile(
	Compiler* compiler,
	const Table* table,
	const Expr* expr,
	u8* bytecode,
	size_t bytecode_max_bytes,
	size_t* bytecode_bytes_written,
	DataType* result_data_type);