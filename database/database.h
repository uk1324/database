#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "error.h"
#include "str.h"
#include "string_view.h"
#include "data_type.h"

// TODO: write why
static_assert(_WIN64, "");

typedef struct
{
	String name;
	DataType data_type;
	size_t offset_in_entry;
} Column;

typedef struct
{
	String name;

	Column* columns;
	size_t column_count;

	size_t entry_size;

	HANDLE entry_auto_increment_file;
	HANDLE entry_auto_increment_mapping_object;
	// This is just the first 8 bytes of the info file.
	u64* entry_auto_increment;
	// mtx_t entry_auto_increment_lock;

	HANDLE data_file;
	HANDLE data_file_mapping_object;
	u8* data_file_map;

} Table;

Result table_create(Table* table, String string);
Result table_insert_entry(Table* table, const u8* entry);

Result table_read(Table* table, String name);


void table_free(Table* table);