#pragma once

//#include <winsock.h>
//#pragma comment(lib, "Ws2_32.lib")
#include "../database.h"
#include "parser.h"
#include "compiler.h"

typedef struct
{
	Parser parser;
	Stmt current_statement;


} Vm;

Result execute_statement(int socket, StringView statement, Vm* vm, Table* table);