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