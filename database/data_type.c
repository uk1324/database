#include "data_type.h"
#include "assertions.h"

DataType data_type_from_type(DataTypeType type)
{
	DataType data_type;
	data_type.type = type;
	return data_type;
}

size_t data_type_size(const DataType* data_type)
{
	switch (data_type->type)
	{
	case DATA_TYPE_I32: return 4;

	default:
		ASSERT_NOT_REACHED();
	}

	return 0;
}
