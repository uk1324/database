#pragma once

#include "types.h"
#include "string_view.h"

typedef struct
{
	u8* data;
	size_t size;
	size_t capacity;
} String;

String string_from_cstring(const char* data);
String string_clone(const String* string);

String string_reserved(size_t bytes_to_reserve);
String string_append(String* string, StringView to_append);
String string_concat(StringView a, StringView b);

StringView string_view_from_string(const String* string);

void string_free(String* string);