#include <string.h>
#include "string_view.h"
#include "str.h"

StringView string_view_from_cstring(const char* string)
{
	StringView str;
	str.size = strlen(string);
	str.data = string;
	return str;
}

StringView string_view_substr(StringView string, size_t offset, size_t count)
{
	string.data += offset;
	string.size = count;
	return string;
}

bool string_view_compare(StringView a, StringView b)
{
	if (a.size != b.size)
		return false;
	return memcmp(a.data, b.data, a.size) == 0;
}
