#include "gfx_shader_compiler.h"
#include "gfx_shader_driver.h"

enum {
	GFX_SHADER_SPV_MAGIC	 = 0x07230203,
	GFX_SHADER_SPV_VERSION	 = 0x00010000,
	GFX_SHADER_SPV_GENERATOR = 0,
	GFX_SHADER_SPV_SCHEMA	 = 0,
	GFX_SHADER_SPV_WORD_MAIN = 0x6e69616d,
	GFX_SHADER_SPV_WORD_ZERO = 0,
	GFX_SHADER_SPV_FLOAT_ONE = 0x3f800000,
};

enum {
	GFX_SHADER_SPV_OP_SOURCE	      = 3,
	GFX_SHADER_SPV_OP_MEMORY_MODEL	      = 14,
	GFX_SHADER_SPV_OP_ENTRY_POINT	      = 15,
	GFX_SHADER_SPV_OP_EXECUTION_MODE      = 16,
	GFX_SHADER_SPV_OP_CAPABILITY	      = 17,
	GFX_SHADER_SPV_OP_TYPE_VOID	      = 19,
	GFX_SHADER_SPV_OP_TYPE_INT	      = 21,
	GFX_SHADER_SPV_OP_TYPE_FLOAT	      = 22,
	GFX_SHADER_SPV_OP_TYPE_VECTOR	      = 23,
	GFX_SHADER_SPV_OP_TYPE_STRUCT	      = 30,
	GFX_SHADER_SPV_OP_TYPE_POINTER	      = 32,
	GFX_SHADER_SPV_OP_TYPE_FUNCTION	      = 33,
	GFX_SHADER_SPV_OP_CONSTANT	      = 43,
	GFX_SHADER_SPV_OP_FUNCTION	      = 54,
	GFX_SHADER_SPV_OP_FUNCTION_END	      = 56,
	GFX_SHADER_SPV_OP_VARIABLE	      = 59,
	GFX_SHADER_SPV_OP_LOAD		      = 61,
	GFX_SHADER_SPV_OP_STORE		      = 62,
	GFX_SHADER_SPV_OP_ACCESS_CHAIN	      = 65,
	GFX_SHADER_SPV_OP_DECORATE	      = 71,
	GFX_SHADER_SPV_OP_MEMBER_DECORATE     = 72,
	GFX_SHADER_SPV_OP_COMPOSITE_CONSTRUCT = 80,
	GFX_SHADER_SPV_OP_COMPOSITE_EXTRACT   = 81,
	GFX_SHADER_SPV_OP_F_NEGATE	      = 127,
	GFX_SHADER_SPV_OP_LABEL		      = 248,
	GFX_SHADER_SPV_OP_RETURN	      = 253,
};

enum {
	GFX_SHADER_SPV_CAPABILITY_SHADER	   = 1,
	GFX_SHADER_SPV_ADDRESSING_LOGICAL	   = 0,
	GFX_SHADER_SPV_MEMORY_GLSL450		   = 1,
	GFX_SHADER_SPV_EXECUTION_VERTEX		   = 0,
	GFX_SHADER_SPV_EXECUTION_FRAGMENT	   = 4,
	GFX_SHADER_SPV_EXECUTION_ORIGIN_UPPER_LEFT = 7,
	GFX_SHADER_SPV_SOURCE_GLSL		   = 2,
	GFX_SHADER_SPV_SOURCE_VERSION		   = 450,
	GFX_SHADER_SPV_DECORATION_BLOCK		   = 2,
	GFX_SHADER_SPV_DECORATION_BUILT_IN	   = 11,
	GFX_SHADER_SPV_DECORATION_LOCATION	   = 30,
	GFX_SHADER_SPV_BUILT_IN_POSITION	   = 0,
	GFX_SHADER_SPV_STORAGE_INPUT		   = 1,
	GFX_SHADER_SPV_STORAGE_OUTPUT		   = 3,
	GFX_SHADER_SPV_FUNCTION_CONTROL_NONE	   = 0,
	GFX_SHADER_SPV_WIDTH_32			   = 32,
	GFX_SHADER_SPV_SIGNED			   = 1,
	GFX_SHADER_SPV_VEC2			   = 2,
	GFX_SHADER_SPV_VEC4			   = 4,
};

static int gfx_shader_spv_write(buf_t *code, u32 word)
{
	return buf_write_u32le(code, word);
}

static int gfx_shader_spv_inst(buf_t *code, u32 op, u32 word_count, ...)
{
	if (gfx_shader_spv_write(code, (word_count << 16) | op)) {
		return 1; // LCOV_EXCL_LINE
	}

	va_list args;
	va_start(args, word_count);
	for (u32 i = 1; i < word_count; i++) {
		if (gfx_shader_spv_write(code, va_arg(args, u32))) {
			va_end(args); // LCOV_EXCL_LINE
			return 1;     // LCOV_EXCL_LINE
		}
	}
	va_end(args);
	return 0;
}

static int gfx_shader_spv_inst_begin(buf_t *code, u32 op, u32 word_count)
{
	return gfx_shader_spv_write(code, (word_count << 16) | op);
}

static int gfx_shader_spv_header(buf_t *code)
{
	return gfx_shader_spv_write(code, GFX_SHADER_SPV_MAGIC) || gfx_shader_spv_write(code, GFX_SHADER_SPV_VERSION) ||
	       gfx_shader_spv_write(code, GFX_SHADER_SPV_GENERATOR) || gfx_shader_spv_write(code, 0) ||
	       gfx_shader_spv_write(code, GFX_SHADER_SPV_SCHEMA);
}

typedef struct gfx_shader_spv_var_s {
	const gfx_shader_member_t *member;
	u32 type_id;
	u32 ptr_type_id;
	u32 var_id;
	u32 storage;
	u32 location;
} gfx_shader_spv_var_t;

typedef struct gfx_shader_spv_s {
	u32 next_id;
	u32 void_id;
	u32 function_type_id;
	u32 main_id;
	u32 label_id;
	u32 float_id;
	u32 vec2_id;
	u32 vec4_id;
	u32 int_id;
	u32 int_zero_id;
	u32 float_zero_id;
	u32 float_one_id;
	u32 gl_per_vertex_id;
	u32 ptr_output_gl_per_vertex_id;
	u32 gl_per_vertex_var_id;
	u32 ptr_output_position_id;
	gfx_shader_spv_var_t inputs[16];
	u32 input_count;
	gfx_shader_spv_var_t outputs[16];
	u32 output_count;
} gfx_shader_spv_t;

static u32 gfx_shader_spv_id(gfx_shader_spv_t *spv)
{
	return spv->next_id++;
}

static u32 gfx_shader_spv_location(strv_t semantic, u32 fallback)
{
	if (strv_eq(semantic, STRV("POSITION"))) {
		return 0;
	}
	if (strv_eq(semantic, STRV("COLOR0"))) {
		return 1;
	}
	return fallback;
}

static u32 gfx_shader_spv_type_id(const gfx_shader_spv_t *spv, strv_t type)
{
	if (strv_eq(type, STRV("vec2f"))) {
		return spv->vec2_id;
	}
	if (strv_eq(type, STRV("vec4f"))) {
		return spv->vec4_id;
	}
	return 0;
}

static const gfx_shader_member_t *gfx_shader_spv_lhs_member(const gfx_shader_struct_ir_t *ir, strv_t lhs)
{
	if (!gfx_shader_strv_prefix(lhs, STRV("output."))) {
		return NULL; // LCOV_EXCL_LINE
	}
	return gfx_shader_struct_member(ir, STRVN(lhs.data + STRV("output.").len, lhs.len - STRV("output.").len));
}

static const gfx_shader_spv_var_t *gfx_shader_spv_find_var(const gfx_shader_spv_var_t *vars, u32 count, strv_t semantic)
{
	for (u32 i = 0; i < count; i++) {
		if (vars[i].member != NULL && strv_eq(vars[i].member->semantic, semantic)) {
			return &vars[i];
		}
	}
	return NULL; // LCOV_EXCL_LINE
}

static int gfx_shader_spv_emit_header(buf_t *code, const gfx_shader_spv_t *spv, gfx_shader_stage_t stage)
{
	u32 interface_count = spv->input_count + spv->output_count + (stage == GFX_SHADER_STAGE_VERTEX ? 1 : 0);
	int ret		    = gfx_shader_spv_header(code);
	ret |= gfx_shader_spv_inst(code, GFX_SHADER_SPV_OP_CAPABILITY, 2, GFX_SHADER_SPV_CAPABILITY_SHADER);
	ret |= gfx_shader_spv_inst(
		code, GFX_SHADER_SPV_OP_MEMORY_MODEL, 3, GFX_SHADER_SPV_ADDRESSING_LOGICAL, GFX_SHADER_SPV_MEMORY_GLSL450);
	ret |= gfx_shader_spv_inst_begin(code, GFX_SHADER_SPV_OP_ENTRY_POINT, 5 + interface_count);
	ret |= gfx_shader_spv_write(code,
				    stage == GFX_SHADER_STAGE_VERTEX ? GFX_SHADER_SPV_EXECUTION_VERTEX : GFX_SHADER_SPV_EXECUTION_FRAGMENT);
	ret |= gfx_shader_spv_write(code, spv->main_id);
	ret |= gfx_shader_spv_write(code, GFX_SHADER_SPV_WORD_MAIN);
	ret |= gfx_shader_spv_write(code, GFX_SHADER_SPV_WORD_ZERO);
	if (stage == GFX_SHADER_STAGE_VERTEX) {
		ret |= gfx_shader_spv_write(code, spv->gl_per_vertex_var_id);
	}
	for (u32 i = 0; i < spv->output_count; i++) {
		ret |= gfx_shader_spv_write(code, spv->outputs[i].var_id);
	}
	for (u32 i = 0; i < spv->input_count; i++) {
		ret |= gfx_shader_spv_write(code, spv->inputs[i].var_id);
	}
	if (stage == GFX_SHADER_STAGE_FRAGMENT) {
		ret |= gfx_shader_spv_inst(
			code, GFX_SHADER_SPV_OP_EXECUTION_MODE, 3, spv->main_id, GFX_SHADER_SPV_EXECUTION_ORIGIN_UPPER_LEFT);
	}
	ret |= gfx_shader_spv_inst(code, GFX_SHADER_SPV_OP_SOURCE, 3, GFX_SHADER_SPV_SOURCE_GLSL, GFX_SHADER_SPV_SOURCE_VERSION);
	return ret;
}

static int gfx_shader_spv_emit_decorations(buf_t *code, const gfx_shader_spv_t *spv, gfx_shader_stage_t stage)
{
	int ret = 0;
	if (stage == GFX_SHADER_STAGE_VERTEX) {
		ret |= gfx_shader_spv_inst(code,
					   GFX_SHADER_SPV_OP_MEMBER_DECORATE,
					   5,
					   spv->gl_per_vertex_id,
					   0,
					   GFX_SHADER_SPV_DECORATION_BUILT_IN,
					   GFX_SHADER_SPV_BUILT_IN_POSITION);
		ret |= gfx_shader_spv_inst(code, GFX_SHADER_SPV_OP_DECORATE, 3, spv->gl_per_vertex_id, GFX_SHADER_SPV_DECORATION_BLOCK);
	}
	for (u32 i = 0; i < spv->output_count; i++) {
		ret |= gfx_shader_spv_inst(code,
					   GFX_SHADER_SPV_OP_DECORATE,
					   4,
					   spv->outputs[i].var_id,
					   GFX_SHADER_SPV_DECORATION_LOCATION,
					   spv->outputs[i].location);
	}
	for (u32 i = 0; i < spv->input_count; i++) {
		ret |= gfx_shader_spv_inst(code,
					   GFX_SHADER_SPV_OP_DECORATE,
					   4,
					   spv->inputs[i].var_id,
					   GFX_SHADER_SPV_DECORATION_LOCATION,
					   spv->inputs[i].location);
	}
	return ret;
}

static int gfx_shader_spv_emit_types(buf_t *code, const gfx_shader_spv_t *spv, gfx_shader_stage_t stage)
{
	int ret = 0;
	ret |= gfx_shader_spv_inst(code, GFX_SHADER_SPV_OP_TYPE_VOID, 2, spv->void_id);
	ret |= gfx_shader_spv_inst(code, GFX_SHADER_SPV_OP_TYPE_FUNCTION, 3, spv->function_type_id, spv->void_id);
	ret |= gfx_shader_spv_inst(code, GFX_SHADER_SPV_OP_TYPE_FLOAT, 3, spv->float_id, GFX_SHADER_SPV_WIDTH_32);
	ret |= gfx_shader_spv_inst(code, GFX_SHADER_SPV_OP_TYPE_VECTOR, 4, spv->vec2_id, spv->float_id, GFX_SHADER_SPV_VEC2);
	ret |= gfx_shader_spv_inst(code, GFX_SHADER_SPV_OP_TYPE_VECTOR, 4, spv->vec4_id, spv->float_id, GFX_SHADER_SPV_VEC4);
	if (stage == GFX_SHADER_STAGE_VERTEX) {
		ret |= gfx_shader_spv_inst(code, GFX_SHADER_SPV_OP_TYPE_STRUCT, 3, spv->gl_per_vertex_id, spv->vec4_id);
		ret |= gfx_shader_spv_inst(code,
					   GFX_SHADER_SPV_OP_TYPE_POINTER,
					   4,
					   spv->ptr_output_gl_per_vertex_id,
					   GFX_SHADER_SPV_STORAGE_OUTPUT,
					   spv->gl_per_vertex_id);
		ret |= gfx_shader_spv_inst(code,
					   GFX_SHADER_SPV_OP_VARIABLE,
					   4,
					   spv->ptr_output_gl_per_vertex_id,
					   spv->gl_per_vertex_var_id,
					   GFX_SHADER_SPV_STORAGE_OUTPUT);
		ret |= gfx_shader_spv_inst(
			code, GFX_SHADER_SPV_OP_TYPE_INT, 4, spv->int_id, GFX_SHADER_SPV_WIDTH_32, GFX_SHADER_SPV_SIGNED);
		ret |= gfx_shader_spv_inst(code, GFX_SHADER_SPV_OP_CONSTANT, 4, spv->int_id, spv->int_zero_id, GFX_SHADER_SPV_WORD_ZERO);
		ret |= gfx_shader_spv_inst(
			code, GFX_SHADER_SPV_OP_CONSTANT, 4, spv->float_id, spv->float_zero_id, GFX_SHADER_SPV_WORD_ZERO);
		ret |= gfx_shader_spv_inst(code, GFX_SHADER_SPV_OP_CONSTANT, 4, spv->float_id, spv->float_one_id, GFX_SHADER_SPV_FLOAT_ONE);
		ret |= gfx_shader_spv_inst(
			code, GFX_SHADER_SPV_OP_TYPE_POINTER, 4, spv->ptr_output_position_id, GFX_SHADER_SPV_STORAGE_OUTPUT, spv->vec4_id);
	}
	for (u32 i = 0; i < spv->input_count; i++) {
		ret |= gfx_shader_spv_inst(code,
					   GFX_SHADER_SPV_OP_TYPE_POINTER,
					   4,
					   spv->inputs[i].ptr_type_id,
					   spv->inputs[i].storage,
					   spv->inputs[i].type_id);
		ret |= gfx_shader_spv_inst(
			code, GFX_SHADER_SPV_OP_VARIABLE, 4, spv->inputs[i].ptr_type_id, spv->inputs[i].var_id, spv->inputs[i].storage);
	}
	for (u32 i = 0; i < spv->output_count; i++) {
		ret |= gfx_shader_spv_inst(code,
					   GFX_SHADER_SPV_OP_TYPE_POINTER,
					   4,
					   spv->outputs[i].ptr_type_id,
					   spv->outputs[i].storage,
					   spv->outputs[i].type_id);
		ret |= gfx_shader_spv_inst(
			code, GFX_SHADER_SPV_OP_VARIABLE, 4, spv->outputs[i].ptr_type_id, spv->outputs[i].var_id, spv->outputs[i].storage);
	}
	return ret;
}

static int gfx_shader_spv_emit_copy(buf_t *code, gfx_shader_spv_t *spv, const gfx_shader_spv_var_t *input,
				    const gfx_shader_spv_var_t *output)
{
	u32 loaded = gfx_shader_spv_id(spv);
	return gfx_shader_spv_inst(code, GFX_SHADER_SPV_OP_LOAD, 4, input->type_id, loaded, input->var_id) ||
	       gfx_shader_spv_inst(code, GFX_SHADER_SPV_OP_STORE, 3, output->var_id, loaded);
}

static int gfx_shader_spv_emit_position(buf_t *code, gfx_shader_spv_t *spv, const gfx_shader_spv_var_t *input)
{
	u32 loaded = gfx_shader_spv_id(spv);
	u32 x	   = gfx_shader_spv_id(spv);
	u32 y	   = gfx_shader_spv_id(spv);
	u32 neg_y  = gfx_shader_spv_id(spv);
	u32 pos	   = gfx_shader_spv_id(spv);
	u32 ptr	   = gfx_shader_spv_id(spv);
	return gfx_shader_spv_inst(code, GFX_SHADER_SPV_OP_LOAD, 4, input->type_id, loaded, input->var_id) ||
	       gfx_shader_spv_inst(code, GFX_SHADER_SPV_OP_COMPOSITE_EXTRACT, 5, spv->float_id, x, loaded, 0) ||
	       gfx_shader_spv_inst(code, GFX_SHADER_SPV_OP_COMPOSITE_EXTRACT, 5, spv->float_id, y, loaded, 1) ||
	       gfx_shader_spv_inst(code, GFX_SHADER_SPV_OP_F_NEGATE, 4, spv->float_id, neg_y, y) ||
	       gfx_shader_spv_inst(code,
				   GFX_SHADER_SPV_OP_COMPOSITE_CONSTRUCT,
				   7,
				   spv->vec4_id,
				   pos,
				   x,
				   neg_y,
				   spv->float_zero_id,
				   spv->float_one_id) ||
	       gfx_shader_spv_inst(code,
				   GFX_SHADER_SPV_OP_ACCESS_CHAIN,
				   5,
				   spv->ptr_output_position_id,
				   ptr,
				   spv->gl_per_vertex_var_id,
				   spv->int_zero_id) ||
	       gfx_shader_spv_inst(code, GFX_SHADER_SPV_OP_STORE, 3, ptr, pos);
}

static int gfx_shader_spv_emit_function(buf_t *code, gfx_shader_spv_t *spv, const gfx_shader_ir_t *ir, gfx_shader_stage_t stage)
{
	const gfx_shader_function_ir_t *fn = stage == GFX_SHADER_STAGE_VERTEX ? &ir->vertex : &ir->fragment;
	int ret				   = gfx_shader_spv_inst(code,
					 GFX_SHADER_SPV_OP_FUNCTION,
					 5,
					 spv->void_id,
					 spv->main_id,
					 GFX_SHADER_SPV_FUNCTION_CONTROL_NONE,
					 spv->function_type_id);
	ret |= gfx_shader_spv_inst(code, GFX_SHADER_SPV_OP_LABEL, 2, spv->label_id);
	for (u32 i = 0; i < fn->statement_count; i++) {
		const gfx_shader_statement_ir_t *stmt = &fn->statements[i];
		if (stmt->kind != GFX_SHADER_STMT_ASSIGN) {
			continue;
		}
		const gfx_shader_member_t *lhs =
			gfx_shader_spv_lhs_member(stage == GFX_SHADER_STAGE_VERTEX ? &ir->vs_out : &ir->fs_out, stmt->lhs);
		if (lhs == NULL) {
			return 1; // LCOV_EXCL_LINE
		}
		if (stage == GFX_SHADER_STAGE_VERTEX && strv_eq(lhs->semantic, STRV("POSITION"))) {
			const gfx_shader_spv_var_t *input = gfx_shader_spv_find_var(spv->inputs, spv->input_count, lhs->semantic);
			if (input == NULL || !strv_eq(input->member->type, STRV("vec2f"))) {
				return 1; // LCOV_EXCL_LINE
			}
			ret |= gfx_shader_spv_emit_position(code, spv, input);
		} else {
			const gfx_shader_spv_var_t *input  = gfx_shader_spv_find_var(spv->inputs, spv->input_count, lhs->semantic);
			const gfx_shader_spv_var_t *output = gfx_shader_spv_find_var(spv->outputs, spv->output_count, lhs->semantic);
			if (input == NULL || output == NULL || input->type_id != output->type_id) {
				return 1; // LCOV_EXCL_LINE
			}
			ret |= gfx_shader_spv_emit_copy(code, spv, input, output);
		}
	}
	ret |= gfx_shader_spv_inst(code, GFX_SHADER_SPV_OP_RETURN, 1);
	ret |= gfx_shader_spv_inst(code, GFX_SHADER_SPV_OP_FUNCTION_END, 1);
	return ret;
}

static int gfx_shader_spv_add_var(gfx_shader_spv_t *spv, gfx_shader_spv_var_t *vars, u32 *count, const gfx_shader_member_t *member,
				  u32 storage, u32 location)
{
	if (*count >= 16) {
		return 1; // LCOV_EXCL_LINE
	}
	gfx_shader_spv_var_t *var = &vars[*count];
	*var			  = (gfx_shader_spv_var_t){
				     .member	  = member,
				     .type_id	  = gfx_shader_spv_type_id(spv, member->type),
				     .ptr_type_id = gfx_shader_spv_id(spv),
				     .var_id	  = gfx_shader_spv_id(spv),
				     .storage	  = storage,
				     .location	  = location,
	     };
	if (var->type_id == 0) {
		return 1;
	}
	(*count)++;
	return 0;
}

static int gfx_shader_spirv_build(buf_t *code, gfx_shader_spv_t *spv, const gfx_shader_ir_t *ir, gfx_shader_stage_t stage)
{
	if (stage == GFX_SHADER_STAGE_VERTEX) {
		spv->int_id			 = gfx_shader_spv_id(spv);
		spv->int_zero_id		 = gfx_shader_spv_id(spv);
		spv->float_zero_id		 = gfx_shader_spv_id(spv);
		spv->float_one_id		 = gfx_shader_spv_id(spv);
		spv->gl_per_vertex_id		 = gfx_shader_spv_id(spv);
		spv->ptr_output_gl_per_vertex_id = gfx_shader_spv_id(spv);
		spv->gl_per_vertex_var_id	 = gfx_shader_spv_id(spv);
		spv->ptr_output_position_id	 = gfx_shader_spv_id(spv);
		for (u32 i = 0; i < ir->vs_in.member_count; i++) {
			if (gfx_shader_spv_add_var(spv,
						   spv->inputs,
						   &spv->input_count,
						   &ir->vs_in.members[i],
						   GFX_SHADER_SPV_STORAGE_INPUT,
						   gfx_shader_spv_location(ir->vs_in.members[i].semantic, i))) {
				return 1;
			}
		}
		u32 location = 0;
		for (u32 i = 0; i < ir->vs_out.member_count; i++) {
			if (strv_eq(ir->vs_out.members[i].semantic, STRV("POSITION"))) {
				continue;
			}
			if (gfx_shader_spv_add_var(spv,
						   spv->outputs,
						   &spv->output_count,
						   &ir->vs_out.members[i],
						   GFX_SHADER_SPV_STORAGE_OUTPUT,
						   location++)) {
				return 1; // LCOV_EXCL_LINE
			}
		}
	} else {
		for (u32 i = 0; i < ir->fs_in.member_count; i++) {
			if (gfx_shader_spv_add_var(
				    spv, spv->inputs, &spv->input_count, &ir->fs_in.members[i], GFX_SHADER_SPV_STORAGE_INPUT, i)) {
				return 1; // LCOV_EXCL_LINE
			}
		}
		for (u32 i = 0; i < ir->fs_out.member_count; i++) {
			if (gfx_shader_spv_add_var(
				    spv, spv->outputs, &spv->output_count, &ir->fs_out.members[i], GFX_SHADER_SPV_STORAGE_OUTPUT, i)) {
				return 1; // LCOV_EXCL_LINE
			}
		}
	}
	if (gfx_shader_spv_emit_header(code, spv, stage) || gfx_shader_spv_emit_decorations(code, spv, stage) ||
	    gfx_shader_spv_emit_types(code, spv, stage) || gfx_shader_spv_emit_function(code, spv, ir, stage)) {
		return 1; // LCOV_EXCL_LINE
	}
	return code->used < sizeof(u32) * 4;
}

static int gfx_shader_spirv_emit(const gfx_shader_ir_t *ir, gfx_shader_stage_t stage, gfx_shader_code_t *shader)
{
	buf_t code = {0};
	if (buf_init(&code, 1024, ALLOC_STD) == NULL) {
		return 1; // LCOV_EXCL_LINE
	}
	gfx_shader_spv_t spv = {.next_id = 1};
	spv.void_id	     = gfx_shader_spv_id(&spv);
	spv.function_type_id = gfx_shader_spv_id(&spv);
	spv.main_id	     = gfx_shader_spv_id(&spv);
	spv.label_id	     = gfx_shader_spv_id(&spv);
	spv.float_id	     = gfx_shader_spv_id(&spv);
	spv.vec2_id	     = gfx_shader_spv_id(&spv);
	spv.vec4_id	     = gfx_shader_spv_id(&spv);

	if (gfx_shader_spirv_build(&code, &spv, ir, stage)) {
		buf_free(&code);
		return 1;
	}
	((u32 *)code.data)[3] = spv.next_id;
	shader->code	      = code;
	return 0;
}

static gfx_shader_driver_t gfx_shader_spirv = {
	.name	  = "spirv",
	.language = GFX_SHADER_LANGUAGE_SPIRV,
	.emit	  = gfx_shader_spirv_emit,
};

GFX_SHADER_DRIVER(gfx_shader_spirv, &gfx_shader_spirv);
