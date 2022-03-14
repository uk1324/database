#include "scanner.h"

void scanner_init(Scanner* scanner, StringView text)
{
	scanner->text = text;
	scanner->current_char_index = 0;
}

static char peek(const Scanner* scanner)
{
	return scanner->text.data[scanner->current_char_index];
}

static void advance(Scanner* scanner)
{
	if (scanner->current_char_index >= scanner->text.size)
	{
		return;
	}
	scanner->current_char_index++;
}

static Token make_token(Scanner* scanner, ToknType type)
{
	// Maybe also save position later.
	Token token;
	token.type = type;
	return token;
}

static bool is_alpha(char c)
{
	return ((c >= 'a') && (c <= 'z'))
		|| ((c >= 'A') && (c <= 'Z'));
}

static bool is_digit(char c)
{
	return ((c >= 0) && (c <= 9));
}

static Token keyword_or_identifier(Scanner* scanner)
{
	do
	{
		advance(scanner);
	} while (is_alpha(peek(scanner)) || (peek(scanner) == '_'));

	StringView text = string_view_substr(
		scanner->text,
		scanner->token_start_char_index,
		scanner->current_char_index - scanner->token_start_char_index);

#define S(text) string_view_from_cstring(text)

	struct
	{
		StringView text;
		ToknType token_type;
	} keywords[] = {
		{ S("get"), TOKEN_KEYWORD_GET },
		{ S("insert"), TOKEN_KEYWORD_INSERT },
	};

	for (size_t i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++)
	{
		if (string_view_compare(text, keywords[i].text))
		{
			return make_token(scanner, keywords[i].token_type);
		}
	}

	Token token = make_token(scanner, TOKEN_IDENTIFIER);
	token.value.identifier = text;
	return token;
}

Token scanner_next(Scanner* scanner)
{
	char c = peek(scanner);

	switch (peek(c))
	{
	default:
		if (is_alpha(c))
		{
			return keyword_or_identifier(scanner);
		}

		make_token(scanner, TOKEN_ERROR);
		break;
	}
}
