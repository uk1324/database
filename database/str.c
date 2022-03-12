#include <string.h>
#include <stdlib.h>
#include "str.h"
#include "allocation.h"

String string_from_cstring(const char* data)
{
	String string;
	string.size = strlen(data);
	string.capacity = string.size + 1;
	string.data = (u8*)try_malloc(string.capacity);
	memcpy(string.data, data, string.capacity);
	return string;
}

String string_clone(const String* string)
{
	String str;
	str.capacity = string->size + 1;
	str.size = string->size;
	str.data = (u8*)try_malloc(string->capacity);
	memcpy(str.data, string->data, str.capacity);
	return str;
}

static size_t grow_capacity(size_t capacity)
{
	return capacity * 2;
}

String string_reserved(size_t bytes_to_reserve)
{
	String string;
	string.capacity = bytes_to_reserve;
	string.size = 0;
	string.data = try_malloc(bytes_to_reserve);
	return string;
}

String string_append(String* string, StringView to_append)
{
	if ((string->size + to_append.size + 1) > string->capacity)
	{
		size_t new_capacity = grow_capacity(string->capacity) >= (string->size + to_append.size + 1)
			? grow_capacity(string->capacity)
			: (string->size + to_append.size + 1);

		char* new_chars = try_malloc(new_capacity);

		memcpy(new_chars, string->data, string->size);
		free(string->data);
		string->data = new_chars;
		string->capacity = new_capacity;
	}
	memcpy(string->data + string->size, to_append.data, to_append.size);
	string->size = string->size + to_append.size;
	string->data[string->size] = '\0';
	return *string;
}

String string_concat(StringView a, StringView b)
{
	String string = string_reserved(a.size + b.size + 1);
	string_append(&string, a);
	string_append(&string, b);
	return string;
}

void string_free(String* string)
{
	free(string->data);
}

StringView string_view_from_string(const String* string)
{
	StringView str;
	str.data = string->data;
	str.size = string->size;
	return str;
}