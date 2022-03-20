#include <stdio.h>
#include <winsock.h>
#include "vm.h"
#include "../messages.h"
#include "../assertions.h"

// Could malloc a buffer entry_buffer that would hold the entry while it is written to
// this would make it so I don't have to send an Message every new column.

// Could make error reporting by sending error message length and then error message after MESSAGE_ERROR.

// Don't check if reading past OP_RETURN assumes compiler generated valid code.
static u8 read_byte(Vm* vm)
{
	u8 value = *vm->instruction_pointer;
	vm->instruction_pointer++;
	return value;
}

static u16 read_word(Vm* vm)
{
	return read_byte(vm) | (read_byte(vm) << 8);
}

static Result stack_push(Vm* vm, const u8* data, size_t data_byte_size)
{
	if ((vm->stack_top + data_byte_size) >= (vm->stack + sizeof(vm->stack)))
	{
		return RESULT_ERROR;
	}
	memcpy(vm->stack_top, data, data_byte_size);
	vm->stack_top += data_byte_size;
	return RESULT_OK;
}

static Result execute_bytecode(Vm* vm, const u8* entry, const u8* bytecode)
{
	vm->instruction_pointer = bytecode;
	vm->stack_top = vm->stack;

	for (;;)
	{
		switch (read_byte(vm))
		{
		case OP_LOAD_ENTRY_OFFSET_SIZE:
		{
			u16 offset = read_word(vm);
			u16 size = read_word(vm);
			CHECK_RESULT(stack_push(vm, entry + offset, size));
			break;
		}

		case OP_RETURN:
			return RESULT_OK;

		default:
			log_error("invalid bytecode instruction");
			return RESULT_ERROR;
		}
	}
}

Result execute_statement(int socket, StringView statement, Vm* vm, Table* table)
{
	printf("query: %.*s\n", (unsigned)statement.size, statement.data);

#define SEND_BYTES(data_ptr, bytes) \
	if (send(socket, (const char*)(data_ptr), (int)bytes, 0) == SOCKET_ERROR) \
	{ \
		return RESULT_ERROR; \
	}
#define SEND(data_ptr) SEND_BYTES(data_ptr, sizeof(*data_ptr))

	// Don't need to check send result when sending MESSAGE_ERROR because it will return anyway.
#define SEND_MESSAGE(msg) \
	{ \
		u8 message = (msg); \
		SEND(&message) \
	}

	if (parser_parse(&vm->parser, &vm->current_query, statement) == RESULT_ERROR)
	{
		SEND_MESSAGE(MESSAGE_ERROR);
		return RESULT_ERROR;
	}
	SEND_MESSAGE(MESSAGE_OK);

	u64 column_count = (u64)table->column_count;
	SEND(&column_count);

	switch (vm->current_query.type)
	{
	case QUERY_GET:
	{
		const GetQuery* query = &vm->current_query.get;
		u8* bytecode = vm->bytecode;
		for (size_t i = 0; i < query->column_count; i++)
		{
			size_t byte_code_bytes_written;
			if (compiler_compile(
				&vm->compiler, table,
				&query->columns[i],
				bytecode,
				vm->bytecode + sizeof(vm->bytecode) - bytecode,
				&byte_code_bytes_written,
				&vm->bytecode_result_data_types[i]) == RESULT_ERROR)
			{
				SEND_MESSAGE(MESSAGE_ERROR);
				return RESULT_ERROR;
			}
			vm->column_bytecodes[i] = bytecode;
			bytecode += byte_code_bytes_written;
			SEND_MESSAGE(MESSAGE_OK);

			u32 data_type = vm->bytecode_result_data_types[i].type;
			SEND(&data_type);
		}

		for (size_t i = 0; i < (size_t)(*table->entry_auto_increment); i++)
		{
			SEND_MESSAGE(MESSAGE_OK);
			const u8* entry = table->data_file_map + (table->entry_size * i);
			for (size_t i = 0; i < query->column_count; i++)
			{
				if (execute_bytecode(vm, entry, vm->column_bytecodes[i]) == RESULT_ERROR)
				{
					SEND_MESSAGE(MESSAGE_ERROR);
					return RESULT_ERROR;
				}
				SEND_MESSAGE(MESSAGE_OK);

				ASSERT(memcmp(entry, vm->stack, 4) == 0);
				printf("sending: %d\n", *((i32*)vm->stack));
				SEND_BYTES(vm->stack, data_type_size(&vm->bytecode_result_data_types[i]));
			}
		}
		SEND_MESSAGE(MESSAGE_END);
		break;
	}

	}

	return RESULT_OK;

	//u32 status_code = 0;
	//send(socket, (char*)&status_code, sizeof(status_code), 0);

	//const char* message = "message";
	//u32 message_length = strlen(message);
	//send(socket, (char*)&message_length, sizeof(message_length), 0);
	//send(socket, message, message_length, 0);

	//u64 column_count = table->column_count;
	//send(socket, &column_count, sizeof(column_count), 0);
	//u64 entry_count = *table->entry_auto_increment;
	//send(socket, &entry_count, sizeof(entry_count), 0);

	//for (size_t i = 0; i < table->column_count; i++)
	//{
	//	Column* column = &table->columns[i];
	//	u32 data_type = column->data_type.type;
	//	send(socket, &data_type, sizeof(data_type), 0);
	//	u32 name_size = column->name.size;
	//	send(socket, &name_size, sizeof(data_type), 0);
	//	send(socket, column->name.data, name_size, 0);
	//	//column->data_type
	//}


	//for (size_t i = 0; i < *table->entry_auto_increment; i++)
	//{
	//	const u8* entry = table->data_file_map + table->entry_size * i;
	//	for (size_t i = 0; i < table->column_count; i++)
	//	{
	//		const Column* column = &table-> columns[i];
	//		send(socket, entry + column->offset_in_entry, data_type_size(&column->data_type), 0);
	//	}
	//}

}