#include <stdlib.h>
#include "allocation.h"
#include "error.h"

void* try_malloc(size_t size)
{
	void* data = malloc(size);
	if (data == NULL)
	{
		error_fatal("failed to allocate memory");
	}
	return data;
}