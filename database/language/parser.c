#include "parser.h"

static ToknType peek(const Parser* parser)
{
	return parser->current_token.type;
}

static ToknType peek_previous(const Parser* parser)
{
	return parser->previous_token.type;
}

static void advance(Parser* parser)
{
	if (peek(parser) == TOKEN_EOF)
	{
		return;
	}
	parser->previous_token = parser->current_token;
	parser->current_token = scanner_next(&parser->scanner);
}

static bool match(Parser* parser, ToknType type)
{
	if (peek(parser) == type)
	{
		advance(parser);
		return true;
	}
	return false;
}

Result expr(Parser* parser, Expr* expr)
{
	if (match(parser, TOKEN_IDENTIFIER))
	{
		expr->type = EXPR_IDENTIFIER;
		expr->identifier.identifier = parser->previous_token.value.identifier;
		return RESULT_OK;
	}

	return RESULT_ERROR;
}

Result parser_parse(Parser* parser, Stmt* statement, StringView text)
{
	scanner_init(&parser->scanner, text);
	parser->current_token = scanner_next(&parser->scanner);

	if (match(parser, TOKEN_KEYWORD_GET))
	{
		statement->type = STMT_GET;
		GetStmt* stmt = &statement->get;
		stmt->column_count = 0;

		do
		{
			if (expr(parser, &stmt->columns[stmt->column_count]) == RESULT_ERROR)
			{
				return RESULT_ERROR;
			}
			stmt->column_count++;
		} while (match(parser, TOKEN_COMMA));
	}
	else
	{
		return RESULT_ERROR;
	}

	return RESULT_OK;
}
