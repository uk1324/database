#include <stdio.h>
#include "vm.h"

Result execute_statement(SOCKET socket, StringView statement, Vm* vm, Table* table)
{
	printf("statement: %.*s\n", statement.size, statement.data);

	Result result = parser_parse(&vm->parser, &vm->current_statement, statement);
	if (result == RESULT_ERROR)
	{
		return RESULT_ERROR;
	}

	
	

}