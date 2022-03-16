#pragma once

#include <winsock.h>
#include "../database.h"
#include "parser.h"
#include "compiler.h"

typedef struct
{
	Parser parser;
	Stmt current_statement;


} Vm;

Result execute_statement(SOCKET socket, StringView statement, Vm* vm, Table* table);