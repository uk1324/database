#include <stdio.h>
#include <winsock.h>
#include "vm.h"

Result execute_statement(int socket, StringView statement, Vm* vm, Table* table)
{
	printf("statement: %.*s\n", statement.size, statement.data);

	Result result = parser_parse(&vm->parser, &vm->current_statement, statement);
	if (result == RESULT_ERROR)
	{
		return RESULT_ERROR;
	}


	for (size_t i = 0; i < vm->current_statement.get.column_count; i++)
	{
		StringView name = vm->current_statement.get.columns[i].identifier.identifier;
		printf("%.*s\n", name.size, name.data);
	}
	
	return RESULT_OK;

	u32 status_code = 0;
	send(socket, (char*)&status_code, sizeof(status_code), 0);

	const char* message = "message";
	u32 message_length = strlen(message);
	send(socket, (char*)&message_length, sizeof(message_length), 0);
	send(socket, message, message_length, 0);

	u64 column_count = table->column_count;
	send(socket, &column_count, sizeof(column_count), 0);
	u64 entry_count = *table->entry_auto_increment;
	send(socket, &entry_count, sizeof(entry_count), 0);

	for (size_t i = 0; i < table->column_count; i++)
	{
		Column* column = &table->columns[i];
		u32 data_type = column->data_type.type;
		send(socket, &data_type, sizeof(data_type), 0);
		u32 name_size = column->name.size;
		send(socket, &name_size, sizeof(data_type), 0);
		send(socket, column->name.data, name_size, 0);
		//column->data_type
	}

	for (size_t i = 0; i < *table->entry_auto_increment; i++)
	{
		const u8* entry = table->data_file_map + table->entry_size * i;
		for (size_t i = 0; i < table->column_count; i++)
		{
			const Column* column = &table-> columns[i];
			send(socket, entry + column->offset_in_entry, data_type_size(&column->data_type), 0);
		}
	}

}