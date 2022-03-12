#pragma once

#include "types.h"

typedef struct
{
	const char* data;
	size_t size;
} StringView;

StringView string_view_from_cstring(const char* string);