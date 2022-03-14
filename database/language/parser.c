#include "parser.h"

static ToknType peek(const Parser* parser)
{
	return parser->current_token.type;
}

static void advance(Parser* parser)
{
	if (peek(parser) != TOKEN_EOF)
	{
		return;
	}
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

Result parser_parse(Parser* parser, Stmt* stmt, StringView text)
{
	scanner_init(&parser->scanner, text);

	if (match(parser, TOKEN_KEYWORD_INSERT))
	{
		stmt->type = STMT_GET;
	}
	else
	{
		return RESULT_ERROR;
	}

	return RESULT_OK;
}
