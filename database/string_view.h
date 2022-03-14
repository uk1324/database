#pragma once

#include <stdbool.h>
#include "types.h"

typedef struct
{
	const char* data;
	size_t size;
} StringView;

StringView string_view_from_cstring(const char* string);
StringView string_view_substr(StringView string, size_t offset, size_t count);
bool string_view_compare(StringView a, StringView b);