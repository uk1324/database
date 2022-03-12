#pragma once

#include "types.h"

typedef enum
{
	DATA_TYPE_I32
} DataTypeType;

// Some data types like var char need to store additional information.
// The data type that would require making DataType recursive in most data bases is enum so I won't
// support it for now.
typedef struct
{
	DataTypeType type;

	// bool is_nullable
} DataType;

DataType data_type_from_type(DataTypeType type);
size_t data_type_size(const DataType* data_type);