#include "gfx_shader_driver.h"

#include "mem.h"
#include "print.h"

int gfx_shader_text_init(buf_t *text)
{
	return buf_init(text, 1024, ALLOC_STD) == NULL;
}

int gfx_shader_text_put(buf_t *text, strv_t str)
{
	return buf_add_str(text, str, NULL);
}

int gfx_shader_text_putf(buf_t *text, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	va_list count_args;
	va_copy(count_args, args);
	int len = c_sprintv(NULL, 0, 0, fmt, count_args);
	va_end(count_args);
	if (len < 0) {
		va_end(args);
		return 1;
	}
	size_t need = text->used + (size_t)len + 1;
	if (need < text->used || buf_resize(text, need)) {
		va_end(args);
		return 1;
	}
	c_sprintv((char *)text->data, text->size - text->used, text->used, fmt, args);
	va_end(args);
	text->used += (size_t)len;
	return 0;
}

int gfx_shader_text_finish(buf_t *text, gfx_shader_code_t *shader)
{
	char zero = 0;
	if (buf_add(text, 1, &zero, NULL)) {
		return 1;
	}
	shader->code = *text;
	shader->text = shader->code.data;
	*text	     = (buf_t){0};
	return 0;
}

const gfx_shader_member_t *gfx_shader_struct_member(const gfx_shader_struct_ir_t *ir, strv_t name)
{
	if (ir == NULL) {
		return NULL;
	}
	for (u32 i = 0; i < ir->member_count; i++) {
		if (strv_eq(ir->members[i].name, name)) {
			return &ir->members[i];
		}
	}
	return NULL;
}

int gfx_shader_strv_prefix(strv_t str, strv_t prefix)
{
	return str.len >= prefix.len && mem_cmp(str.data, prefix.data, prefix.len) == 0;
}
