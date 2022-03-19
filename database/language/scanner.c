#include "scanner.h"

void scanner_init(Scanner* scanner, StringView text)
{
	scanner->text = text;
	scanner->current_char_index = 0;
	scanner->token_start_char_index = 0;
}

static char peek(const Scanner* scanner)
{
	return scanner->text.data[scanner->current_char_index];
}

static bool is_at_end(Scanner* scanner)
{
	return scanner->current_char_index >= scanner->text.size;
}

static void advance(Scanner* scanner)
{
	if (is_at_end(scanner))
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
	return ((c >=  '0') && (c <= '9'));
}

void skip_whitespace(Scanner* scanner)
{
	while (is_at_end(scanner) == false)
	{
		switch (peek(scanner))
		{
		case ' ':
			advance(scanner);
			break;

		default:
			scanner->token_start_char_index = scanner->current_char_index;
			return;
		}
	}
}

static Token keyword_or_identifier(Scanner* scanner)
{
	do
	{
		advance(scanner);
	} while (is_alpha(peek(scanner)) || (peek(scanner) == '_') || is_digit(peek(scanner)));

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
	if (is_at_end(scanner))
	{
		return make_token(scanner, TOKEN_EOF);
	}

	skip_whitespace(scanner);

	char c = peek(scanner);
	advance(scanner);

	switch (c)
	{
	case ',': return make_token(scanner, TOKEN_COMMA);

		
	default:
		if (is_alpha(c))
		{
			return keyword_or_identifier(scanner);
		}

		return make_token(scanner, TOKEN_ERROR);
		break;
	}
}
