#pragma once

//#include <winsock.h>
//#pragma comment(lib, "Ws2_32.lib")
#include "../database.h"
#include "parser.h"
#include "compiler.h"

typedef struct
{
	Parser parser;
	Compiler compiler;
	Query current_query;

	u8 bytecode[1024];

	const u8* column_bytecodes[100];
	DataType bytecode_result_data_types[100];

	const u8* instruction_pointer;
	u8 stack[1024];
	u8* stack_top;
} Vm;

Result execute_statement(int socket, StringView statement, Vm* vm, Table* table);