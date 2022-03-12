#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "database.h"
#include "error.h"
#include "assertions.h"

#define PAGE_SIZE 4096

static void free_table_columns(Table* table)
{
	for (size_t i = 0; i < table->column_count; i++)
	{
		string_free(&table->columns[i].name);
	}
	free(table->columns);
}

#define ALIGN_UP_TO(num, alignment) \
	((num % alignment == 0) ? num : num + (alignment - num % alignment))

static void calculate_columns_offset_in_entry_and_entry_size(Table* table)
{
	table->entry_size = 0;
	for (size_t i = 0; i < table->column_count; i++)
	{
		Column* column = &table->columns[i];
		column->offset_in_entry = table->entry_size;
		table->entry_size += ALIGN_UP_TO(data_type_size(&column->data_type), 8);
	}
}

static Result create_read_write_file_map(
	HANDLE file,
	HANDLE* file_mapping_object,
	void** file_map, 
	size_t size)
{
	*file_mapping_object = CreateFileMappingW(
		file, /* if a initial size is bigger than 2^16 it has to be split in half */
		NULL, PAGE_READWRITE, 0, (DWORD)size, NULL);
	if (*file_mapping_object == NULL)
	{
		log_error("failed to create file mapping object ec: %d", GetLastError());
		return RESULT_ERROR;
	}

	*file_map = MapViewOfFile(
		*file_mapping_object,
		FILE_MAP_ALL_ACCESS, 0, 0, size);
	if (*file_map == NULL)
	{
		log_error("failed to create file map ec: %d", GetLastError());
		return RESULT_ERROR;
	}

	return RESULT_OK;
}

Result table_create(Table* table, String string)
{
	table->name = string;
	table->entry_auto_increment = 0;
	table->column_count = 2;
	table->columns = malloc(table->column_count * sizeof(Column));
	if (table->columns == NULL)
	{
		log_error("failed to allocate memory");
		return RESULT_ERROR;
	}
	table->columns[0].data_type = data_type_from_type(DATA_TYPE_I32);
	table->columns[0].name = string_from_cstring("col1");
	table->columns[1].data_type = data_type_from_type(DATA_TYPE_I32);
	table->columns[1].name = string_from_cstring("col2");
	calculate_columns_offset_in_entry_and_entry_size(table);

	FILE* info_file = fopen(table->name.data, "wb");
	if (info_file == NULL)
	{
		string_free(&table->name);
		log_error("failed to create file '%s'", table->name.data);
		return RESULT_ERROR;
	}

#define WRITE_TO_INFO_FILE_BYTES(data_ptr, bytes_to_write) \
	if (fwrite(data_ptr, 1, bytes_to_write, info_file) != bytes_to_write) \
	{ \
		log_error("failed to write to file '%s'", table->name.data); \
		ASSERT(fclose(info_file) != EOF); \
		string_free(&table->name); \
		free_table_columns(table); \
		return RESULT_ERROR; \
	}
#define WRITE_TO_INFO_FILE(data_ptr) WRITE_TO_INFO_FILE_BYTES(data_ptr, sizeof(*data_ptr))

	u64 entry_count = (u64)table->entry_auto_increment;
	WRITE_TO_INFO_FILE(&entry_count);
	u64 column_count = (u64)table->column_count;
	WRITE_TO_INFO_FILE(&column_count);

	for (size_t i = 0; i < column_count; i++)
	{
		const Column* column = &table->columns[i];

		i32 data_type = column->data_type.type;
		WRITE_TO_INFO_FILE(&data_type);

		i32 column_name_size = (u32)column->name.size;
		WRITE_TO_INFO_FILE(&column_name_size);
		WRITE_TO_INFO_FILE_BYTES(column->name.data, column_name_size);
	}

#undef WRITE_TO_INFO_FILE
	ASSERT(fclose(info_file) != EOF);

	String filename = string_concat(
		string_view_from_string(&table->name),
		string_view_from_cstring("_data"));

	table->data_file = CreateFileA(
		filename.data,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	string_free(&filename);
	if (table->data_file == INVALID_HANDLE_VALUE)
	{
		string_free(&table->name);
		free_table_columns(table);

		log_error("failed to create data file ec: %d", GetLastError());
		return RESULT_ERROR;
	}

	// Could use GetSystemInfo to get page size
	size_t initial_size = PAGE_SIZE;

	if (create_read_write_file_map(
		table->data_file,
		&table->file_mapping_object,
		&table->data_file, initial_size) == RESULT_ERROR)
	{
		string_free(&table->name);
		free_table_columns(table);
		ASSERT(CloseHandle(table->data_file));
		return RESULT_ERROR;
	}

	return RESULT_OK;
}

Result table_insert_entry(Table* table, const u8* entry)
{
	// TODO: make a get file size that returns u64 and maybe cache the result.
	if ((table->entry_auto_increment + 1) * table->entry_size > GetFileSize(table->data_file, NULL))
	{
		// Resize
	}

	u8* entry_pos = table->data_file_map + table->entry_auto_increment * table->entry_size;
	memcpy(entry_pos, entry, table->entry_size);
	table->entry_auto_increment++;
	if (FlushViewOfFile(entry_pos, table->entry_size) == false)
	{
		log_error("failed to flush file map ec: %d", GetLastError());
		return RESULT_ERROR;
	}

	return RESULT_OK;
}

Result table_read(Table* table, String name)
{
	table->name = name;
	FILE* info_file = fopen(table->name.data, "rb");
	if (info_file == NULL)
	{
		string_free(&table->name);
		log_error("failed to open file '%s'", table->name.data);
		return RESULT_ERROR;
	}
#define READ_FROM_INFO_FILE_BYTES(data_ptr, bytes_to_read, additional_things_to_close) \
	if (fread(data_ptr, 1, bytes_to_read, info_file) != bytes_to_read) \
	{ \
		log_error("failed to read from file '%s'", table->name.data); \
		string_free(&table->name); \
		fclose(info_file); \
		additional_things_to_close \
		return RESULT_ERROR; \
	}
	
#define READ_FROM_INFO_FILE(data_ptr, additional_things_to_close) \
	READ_FROM_INFO_FILE_BYTES(data_ptr, sizeof(*data_ptr), additional_things_to_close)

	i64 entry_count;
	READ_FROM_INFO_FILE(&entry_count, {});
	table->entry_auto_increment = entry_count; // no cast
	i64 column_count;
	READ_FROM_INFO_FILE(&column_count, {});
	table->column_count = column_count;
	// Not checking if the size is huge.
	table->columns = malloc(table->column_count * sizeof(Column));
	if (table->columns == NULL)
	{
		string_free(&table->name);
		fclose(info_file);
		log_error("failed to allocate memory");
		return RESULT_ERROR;
	}

	for (size_t i = 0; i < table->column_count; i++)
	{
		Column* column = &table->columns[i];

		i32 data_type;
		READ_FROM_INFO_FILE(&data_type, {});
		column->data_type = data_type_from_type((DataTypeType)data_type);
		i32 column_name_size;
		READ_FROM_INFO_FILE(&column_name_size, {});
		column->name = string_reserved((size_t)column_name_size + 1); // No check for size
		column->name.size = column_name_size;
		READ_FROM_INFO_FILE_BYTES(column->name.data, column_name_size, {});
		column->name.data[column_name_size] = '\0';
	}
	fclose(info_file);

	calculate_columns_offset_in_entry_and_entry_size(table);

	String data_file_name = string_concat(
		string_view_from_string(&table->name),
		string_view_from_cstring("_data"));

	table->data_file = CreateFileA(
		data_file_name.data,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	string_free(&data_file_name);
	if (table->data_file == INVALID_HANDLE_VALUE)
	{
		string_free(&table->name);
		free_table_columns(table);

		log_error("failed to open data file ec: %d", GetLastError());
		return RESULT_ERROR;
	}

	string_free(&data_file_name);
	if (create_read_write_file_map(
		table->data_file,
		&table->file_mapping_object,
		&table->data_file, 0) == RESULT_ERROR)
	{
		string_free(&table->name);
		free_table_columns(table);
		ASSERT(CloseHandle(table->data_file));
		return RESULT_ERROR;
	}

	return RESULT_OK;
}

void table_free(Table* table)
{
	string_free(&table->name);
	free_table_columns(table);
	ASSERT(CloseHandle(table->file_mapping_object));
	ASSERT(CloseHandle(table->data_file));
	ASSERT(UnmapViewOfFile(table->data_file_map));
}

void column_free(Column* column)
{
	string_free(&column->name);
}
