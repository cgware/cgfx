#ifndef GFX_SHADER_DRIVER_H
#define GFX_SHADER_DRIVER_H

#include "driver.h"
#include "gfx_shader_compiler.h"

typedef struct gfx_shader_member_s {
	strv_t type;
	strv_t name;
	strv_t semantic;
} gfx_shader_member_t;

typedef struct gfx_shader_struct_ir_s {
	int present;
	strv_t name;
	gfx_shader_member_t members[16];
	u32 member_count;
} gfx_shader_struct_ir_t;

typedef enum gfx_shader_statement_kind_e {
	GFX_SHADER_STMT_DECL,
	GFX_SHADER_STMT_ASSIGN,
	GFX_SHADER_STMT_RETURN,
} gfx_shader_statement_kind_t;

typedef struct gfx_shader_statement_ir_s {
	gfx_shader_statement_kind_t kind;
	strv_t type;
	strv_t name;
	strv_t lhs;
	strv_t op;
	strv_t expr;
	int has_init;
} gfx_shader_statement_ir_t;

typedef struct gfx_shader_function_ir_s {
	int present;
	strv_t ret;
	strv_t name;
	gfx_shader_statement_ir_t statements[32];
	u32 statement_count;
} gfx_shader_function_ir_t;

typedef struct gfx_shader_ir_s {
	gfx_shader_struct_ir_t vs_in;
	gfx_shader_struct_ir_t vs_out;
	gfx_shader_struct_ir_t fs_in;
	gfx_shader_struct_ir_t fs_out;
	gfx_shader_function_ir_t vertex;
	gfx_shader_function_ir_t fragment;
} gfx_shader_ir_t;

typedef struct gfx_shader_driver_s {
	const char *name;
	gfx_shader_language_t language;
	int (*emit)(const gfx_shader_ir_t *ir, gfx_shader_stage_t stage, gfx_shader_code_t *shader);
} gfx_shader_driver_t;

int gfx_shader_text_init(buf_t *text);
int gfx_shader_text_put(buf_t *text, strv_t str);
int gfx_shader_text_putf(buf_t *text, const char *fmt, ...);
int gfx_shader_text_finish(buf_t *text, gfx_shader_code_t *shader);

const gfx_shader_member_t *gfx_shader_struct_member(const gfx_shader_struct_ir_t *ir, strv_t name);

int gfx_shader_strv_prefix(strv_t str, strv_t prefix);

#define GFX_SHADER_DRIVER_TYPE 0x534844

#define GFX_SHADER_DRIVER(_name, _data) DRIVER(_name, GFX_SHADER_DRIVER_TYPE, _data)

#endif
