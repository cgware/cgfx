#ifndef GFX_SHADER_COMPILER_H
#define GFX_SHADER_COMPILER_H

#include "buf.h"
#include "estx.h"

typedef enum gfx_shader_stage_e {
	GFX_SHADER_STAGE_VERTEX,
	GFX_SHADER_STAGE_FRAGMENT,
} gfx_shader_stage_t;

typedef enum gfx_shader_language_e {
	GFX_SHADER_LANGUAGE_GLSL,
	GFX_SHADER_LANGUAGE_HLSL,
	GFX_SHADER_LANGUAGE_SPIRV,
} gfx_shader_language_t;

typedef struct gfx_shader_code_s {
	gfx_shader_stage_t stage;
	gfx_shader_language_t language;
	const char *text;
	buf_t code;
} gfx_shader_code_t;

typedef struct gfx_shader_rules_s {
	estx_node_t program;
	estx_node_t item;
	estx_node_t vs_in_struct;
	estx_node_t vs_out_struct;
	estx_node_t fs_in_struct;
	estx_node_t fs_out_struct;
	estx_node_t struct_mem;
	estx_node_t function_definition;
	estx_node_t function_header;
	estx_node_t statement;
	estx_node_t return_statement;
	estx_node_t declaration_expression;
	estx_node_t initialization;
	estx_node_t assignment_expression;
	estx_node_t assignment_operator;
	estx_node_t expression;
	estx_node_t lvalue;
	estx_node_t type_name;
	estx_node_t identifier;
	estx_node_t semantic;
} gfx_shader_rules_t;

typedef struct gfx_shader_compiler_s {
	alloc_t alloc;
	estx_t estx;
	gfx_shader_rules_t rules;
} gfx_shader_compiler_t;

gfx_shader_compiler_t *gfx_shader_compiler_init(gfx_shader_compiler_t *compiler, alloc_t alloc);
void gfx_shader_compiler_free(gfx_shader_compiler_t *compiler);

int gfx_shader_compiler_transpile(gfx_shader_compiler_t *compiler, strv_t source, gfx_shader_stage_t stage, gfx_shader_language_t language,
				  gfx_shader_code_t *shader);
void gfx_shader_code_free(gfx_shader_code_t *shader);

#endif
