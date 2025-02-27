
#include "cflare/buffer.h"

typedef struct cflare_buffer_part
{
	size_t length;
	const uint8_t* pointer;
} cflare_buffer_part;

typedef struct cflare_buffer
{
	bool needs_copy;
	bool reversed;
	bool nullchar;
	size_t length; // total bytes
	size_t count; // total parts
	size_t alloc_count; // allocated parts
	cflare_buffer_part* parts;
} cflare_buffer;

cflare_buffer* cflare_buffer_new(cflare_buffer_options opts)
{
	cflare_buffer* ret = malloc(sizeof(cflare_buffer));
	ret->needs_copy = (opts & CFLARE_BUFFER_NOCOPY) ? 0 : 1;
	ret->reversed = (opts & CFLARE_BUFFER_REVERSE) ? 1 : 0;
	ret->nullchar = (opts & CFLARE_BUFFER_NULLCHAR) ? 1 : 0;
	ret->length = 0;
	ret->count = 0;
	ret->alloc_count = 8;
	ret->parts = malloc(sizeof(cflare_buffer_part) * ret->alloc_count);
	
	if(ret->reversed)
		cflare_fatal("buffer_new(): buffer reversing not yet implimented.");
	
	return ret;
}

void cflare_buffer_delete(cflare_buffer* buff)
{
	if(buff->needs_copy)
	{
		for(size_t i = 0; i < buff->count; i++)
			free((uint8_t*)buff->parts[i].pointer);
	}
	
	free(buff->parts);
	free(buff);
}

size_t cflare_buffer_length(cflare_buffer* buff)
{
	return buff->length;
}

size_t cflare_buffer_parts(cflare_buffer* buff)
{
	return buff->count;
}

void cflare_buffer_append(cflare_buffer* buff, const uint8_t* data, size_t len)
{
	if(buff->needs_copy)
	{
		uint8_t* newdata = malloc(len);
		memcpy(newdata, data, len);
		data = newdata;
	}
	
	if(buff->count + 1 >= buff->alloc_count)
	{
		buff->alloc_count *= 2;
		buff->parts = realloc(buff->parts, sizeof(cflare_buffer_part) * buff->alloc_count);
	}
	
	cflare_buffer_part* part = buff->parts + buff->count;
	buff->count += 1;
	
	buff->length += len;
	part->length = len;
	part->pointer = data;
}

uint8_t* cflare_buffer_build(cflare_buffer* buff)
{
	size_t len = buff->length;
	if(buff->nullchar)
		len += 1;
	
	uint8_t* ret = malloc(len);
	uint8_t* position_ptr = ret;
	
	for(size_t i = 0; i < buff->count; i++)
	{
		cflare_buffer_part* part = buff->parts + i;
		memcpy(position_ptr, part->pointer, part->length);
		position_ptr += part->length;
	}
	
	if(buff->nullchar)
		*position_ptr = 0;
	
	return ret;
}
