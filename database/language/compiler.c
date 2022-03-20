#include "compiler.h"
#include "../assertions.h"

static Result write_byte(Compiler* compiler, u8 value)
{
	if ((compiler->bytecode_bytes_written + 1) > compiler->bytecode_max_bytes)
	{
		return RESULT_ERROR;
	}
	compiler->bytecode[compiler->bytecode_bytes_written] = value;
	compiler->bytecode_bytes_written++;
	return RESULT_OK;
}

static Result write_word(Compiler* compiler, u16 word)
{
	CHECK_RESULT(write_byte(compiler, (u8)word));
	CHECK_RESULT(write_byte(compiler, (u8)(word >> 8)));
	return RESULT_OK;
}

static Result write_op(Compiler* compiler, Op op)
{
	return write_byte(compiler, op);
}

static Result expression(Compiler* compiler, const Expr* expr, DataType* result_data_type)
{
	switch (expr->type)
	{
	case EXPR_IDENTIFIER:
	{
		write_op(compiler, OP_LOAD_ENTRY_OFFSET_SIZE);
		for (size_t i = 0; i < compiler->table->column_count; i++)
		{
			const Column* column = &compiler->table->columns[i];
			if (string_view_compare(string_view_from_string(&compiler->table->columns[i].name), expr->identifier.identifier))
			{
				ASSERT(column->offset_in_entry <= UINT16_MAX);
				CHECK_RESULT(write_word(compiler, (u16)column->offset_in_entry));
				CHECK_RESULT(write_word(compiler, (u16)data_type_size(&column->data_type)));
				*result_data_type = column->data_type;
			}
			else
			{
				break;
			}
			return RESULT_OK;
		}
		log_error("table has no column named %s", expr->identifier.identifier);
		return RESULT_ERROR;
	}
	}

	ASSERT_NOT_REACHED();
	return RESULT_ERROR;
}

Result compiler_compile(
	Compiler* compiler,
	const Table* table,
	const Expr* expr,
	u8* bytecode,
	size_t bytecode_max_bytes,
	size_t* bytecode_bytes_written,
	DataType* result_data_type)
{
	compiler->bytecode = bytecode;
	compiler->bytecode_bytes_written = 0;
	compiler->bytecode_max_bytes = bytecode_max_bytes;
	compiler->table = table;

	CHECK_RESULT(expression(compiler, expr, result_data_type));
	write_op(compiler, OP_RETURN);

	*bytecode_bytes_written = compiler->bytecode_bytes_written;

	return RESULT_OK;
}