#include "gfx_shader_driver.h"

static const char *gfx_shader_hlsl_type(strv_t type)
{
	if (strv_eq(type, STRV("vec2f"))) {
		return "float2";
	}
	if (strv_eq(type, STRV("vec3f"))) {
		return "float3";
	}
	if (strv_eq(type, STRV("vec4f"))) {
		return "float4";
	}
	if (strv_eq(type, STRV("mat4f"))) {
		return "float4x4";
	}
	return NULL;
}

static const char *gfx_shader_hlsl_semantic(strv_t semantic, int output)
{
	if (output && strv_eq(semantic, STRV("POSITION"))) {
		return "SV_POSITION";
	}
	if (strv_eq(semantic, STRV("POSITION"))) {
		return "POSITION";
	}
	if (strv_eq(semantic, STRV("COLOR0"))) {
		return "COLOR0";
	}
	return NULL;
}

static int gfx_shader_emit_hlsl_struct(buf_t *text, const gfx_shader_struct_ir_t *ir, int output)
{
	if (gfx_shader_text_putf(text, "struct %.*s {\n", ir->name.len, ir->name.data)) {
		return 1; // LCOV_EXCL_LINE
	}
	for (u32 i = 0; i < ir->member_count; i++) {
		const char *type = gfx_shader_hlsl_type(ir->members[i].type);
		const char *sem	 = gfx_shader_hlsl_semantic(ir->members[i].semantic, output);
		if (type == NULL || sem == NULL ||
		    gfx_shader_text_putf(text, "    %s %.*s : %s;\n", type, ir->members[i].name.len, ir->members[i].name.data, sem)) {
			return 1;
		}
	}
	return gfx_shader_text_put(text, STRV("};\n"));
}

typedef enum gfx_shader_hlsl_expr_type_e {
	GFX_SHADER_HLSL_EXPR_UNKNOWN,
	GFX_SHADER_HLSL_EXPR_SCALAR,
	GFX_SHADER_HLSL_EXPR_VECTOR,
	GFX_SHADER_HLSL_EXPR_MATRIX,
} gfx_shader_hlsl_expr_type_t;

static gfx_shader_hlsl_expr_type_t gfx_shader_hlsl_type_class(strv_t type)
{
	if (strv_eq(type, STRV("mat4f"))) {
		return GFX_SHADER_HLSL_EXPR_MATRIX;
	}
	if (strv_eq(type, STRV("vec2f")) || strv_eq(type, STRV("vec3f")) || strv_eq(type, STRV("vec4f"))) {
		return GFX_SHADER_HLSL_EXPR_VECTOR;
	}
	return GFX_SHADER_HLSL_EXPR_UNKNOWN;
}

static gfx_shader_hlsl_expr_type_t gfx_shader_hlsl_lvalue_type(const gfx_shader_ir_t *ir, const gfx_shader_function_ir_t *fn, strv_t name)
{
	for (u32 i = 0; i < ir->buffer_count; i++) {
		for (u32 m = 0; m < ir->buffers[i].member_count; m++) {
			if (strv_eq(ir->buffers[i].members[m].name, name)) {
				return gfx_shader_hlsl_type_class(ir->buffers[i].members[m].type);
			}
		}
	}
	for (u32 i = 0; i < fn->statement_count; i++) {
		if (fn->statements[i].kind == GFX_SHADER_STMT_DECL && strv_eq(fn->statements[i].name, name)) {
			return gfx_shader_hlsl_type_class(fn->statements[i].type);
		}
	}
	return GFX_SHADER_HLSL_EXPR_UNKNOWN;
}

static gfx_shader_hlsl_expr_type_t gfx_shader_hlsl_expr_type(const gfx_shader_ir_t *ir, const gfx_shader_function_ir_t *fn,
							     const gfx_shader_statement_ir_t *stmt, u32 node_id)
{
	if (node_id >= stmt->expr_count) {
		return GFX_SHADER_HLSL_EXPR_UNKNOWN; // LCOV_EXCL_LINE
	}
	const gfx_shader_expr_ir_t *node = &stmt->expr_nodes[node_id];
	if (node->kind == GFX_SHADER_EXPR_INT || node->kind == GFX_SHADER_EXPR_FLOAT) {
		return GFX_SHADER_HLSL_EXPR_SCALAR;
	}
	if (node->kind == GFX_SHADER_EXPR_LVALUE) {
		return gfx_shader_hlsl_lvalue_type(ir, fn, node->text);
	}
	if (node->kind == GFX_SHADER_EXPR_CALL) {
		return gfx_shader_hlsl_type_class(node->text);
	}
	if (node->kind != GFX_SHADER_EXPR_BINARY) {
		return GFX_SHADER_HLSL_EXPR_UNKNOWN; // LCOV_EXCL_LINE
	}
	gfx_shader_hlsl_expr_type_t left  = gfx_shader_hlsl_expr_type(ir, fn, stmt, node->left);
	gfx_shader_hlsl_expr_type_t right = gfx_shader_hlsl_expr_type(ir, fn, stmt, node->right);
	if (left == GFX_SHADER_HLSL_EXPR_MATRIX && right == GFX_SHADER_HLSL_EXPR_MATRIX) {
		return GFX_SHADER_HLSL_EXPR_MATRIX;
	}
	if (left == GFX_SHADER_HLSL_EXPR_MATRIX || right == GFX_SHADER_HLSL_EXPR_MATRIX || // LCOV_EXCL_LINE
	    left == GFX_SHADER_HLSL_EXPR_VECTOR ||					   // LCOV_EXCL_LINE
	    right == GFX_SHADER_HLSL_EXPR_VECTOR) {
		return GFX_SHADER_HLSL_EXPR_VECTOR; // LCOV_EXCL_LINE
	}
	return left != GFX_SHADER_HLSL_EXPR_UNKNOWN ? left : right; // LCOV_EXCL_LINE
}

static int gfx_shader_emit_hlsl_expr(buf_t *text, const gfx_shader_ir_t *ir, const gfx_shader_function_ir_t *fn,
				     const gfx_shader_statement_ir_t *stmt, u32 node_id)
{
	if (stmt == NULL || node_id >= stmt->expr_count) {
		return 1; // LCOV_EXCL_LINE
	}
	const gfx_shader_expr_ir_t *node = &stmt->expr_nodes[node_id];
	if (node->kind == GFX_SHADER_EXPR_INT || node->kind == GFX_SHADER_EXPR_FLOAT || node->kind == GFX_SHADER_EXPR_LVALUE) {
		return gfx_shader_text_put(text, node->text);
	}
	if (node->kind == GFX_SHADER_EXPR_CALL) {
		const char *mapped = NULL;
		if (strv_eq(node->text, STRV("vec2f"))) {
			mapped = "float2";
		} else if (strv_eq(node->text, STRV("vec4f"))) {
			mapped = "float4";
		} else if (strv_eq(node->text, STRV("mat4f"))) {
			mapped = "float4x4";
		}
		if (gfx_shader_text_put(text, mapped != NULL ? strv_cstr(mapped) : node->text) || gfx_shader_text_put(text, STRV("("))) {
			return 1; // LCOV_EXCL_LINE
		}
		for (u32 i = 0; i < node->arg_count; i++) {
			if ((i > 0 && gfx_shader_text_put(text, STRV(", "))) ||
			    gfx_shader_emit_hlsl_expr(text, ir, fn, stmt, node->args[i])) {
				return 1; // LCOV_EXCL_LINE
			}
		}
		return gfx_shader_text_put(text, STRV(")"));
	}
	if (node->kind == GFX_SHADER_EXPR_BINARY) {
		gfx_shader_hlsl_expr_type_t left  = gfx_shader_hlsl_expr_type(ir, fn, stmt, node->left);
		gfx_shader_hlsl_expr_type_t right = gfx_shader_hlsl_expr_type(ir, fn, stmt, node->right);
		if (strv_eq(node->op, STRV("*")) && (left == GFX_SHADER_HLSL_EXPR_MATRIX || right == GFX_SHADER_HLSL_EXPR_MATRIX)) {
			return gfx_shader_text_put(text, STRV("mul(")) || gfx_shader_emit_hlsl_expr(text, ir, fn, stmt, node->left) ||
			       gfx_shader_text_put(text, STRV(", ")) || gfx_shader_emit_hlsl_expr(text, ir, fn, stmt, node->right) ||
			       gfx_shader_text_put(text, STRV(")"));
		}
		return gfx_shader_text_put(text, STRV("(")) || gfx_shader_emit_hlsl_expr(text, ir, fn, stmt, node->left) ||
		       gfx_shader_text_putf(text, " %.*s ", node->op.len, node->op.data) ||
		       gfx_shader_emit_hlsl_expr(text, ir, fn, stmt, node->right) || gfx_shader_text_put(text, STRV(")"));
	}
	return 1; // LCOV_EXCL_LINE
}

static int gfx_shader_emit_hlsl_buffers(buf_t *text, const gfx_shader_ir_t *ir)
{
	for (u32 i = 0; i < ir->buffer_count; i++) {
		const gfx_shader_struct_ir_t *buf = &ir->buffers[i];
		if (gfx_shader_text_putf(text, "cbuffer %.*s : register(b%u) {\n", buf->name.len, buf->name.data, buf->slot)) {
			return 1; // LCOV_EXCL_LINE
		}
		for (u32 m = 0; m < buf->member_count; m++) {
			const char *type = gfx_shader_hlsl_type(buf->members[m].type);
			if (type == NULL ||
			    gfx_shader_text_putf(text, "    %s %.*s;\n", type, buf->members[m].name.len, buf->members[m].name.data)) {
				return 1; // LCOV_EXCL_LINE
			}
		}
		if (gfx_shader_text_put(text, STRV("};\n"))) {
			return 1; // LCOV_EXCL_LINE
		}
	}
	return 0;
}

static int gfx_shader_hlsl_is_position_output(const gfx_shader_ir_t *ir, strv_t lhs)
{
	if (!gfx_shader_strv_prefix(lhs, STRV("output."))) {
		return 0;
	}
	strv_t name			  = STRVN(lhs.data + STRV("output.").len, lhs.len - STRV("output.").len);
	const gfx_shader_member_t *member = gfx_shader_struct_member(&ir->vs_out, name);
	return member != NULL && strv_eq(member->semantic, STRV("POSITION"));
}

static int gfx_shader_emit_hlsl_statement(buf_t *text, const gfx_shader_ir_t *ir, const gfx_shader_function_ir_t *fn,
					  const gfx_shader_statement_ir_t *stmt, gfx_shader_stage_t stage)
{
	if (stmt->kind == GFX_SHADER_STMT_DECL) {
		const char *type = gfx_shader_hlsl_type(stmt->type);
		if (stage == GFX_SHADER_STAGE_FRAGMENT && strv_eq(stmt->type, fn->ret)) {
			return gfx_shader_text_put(text, STRV("    float4 output_color;\n"));
		}
		if (type == NULL && strv_eq(stmt->type, fn->ret)) {
			if (gfx_shader_text_putf(text, "    %.*s %.*s", stmt->type.len, stmt->type.data, stmt->name.len, stmt->name.data)) {
				return 1; // LCOV_EXCL_LINE
			}
		} else if (type != NULL) {
			if (gfx_shader_text_putf(text, "    %s %.*s", type, stmt->name.len, stmt->name.data)) {
				return 1; // LCOV_EXCL_LINE
			}
		} else {
			return 1; // LCOV_EXCL_LINE
		}
		if (stmt->has_init) {
			if (gfx_shader_text_put(text, STRV(" = ")) || gfx_shader_emit_hlsl_expr(text, ir, fn, stmt, stmt->expr_root)) {
				return 1; // LCOV_EXCL_LINE
			}
		}
		return gfx_shader_text_put(text, STRV(";\n"));
	}
	if (stmt->kind == GFX_SHADER_STMT_ASSIGN) {
		if (stage == GFX_SHADER_STAGE_FRAGMENT && strv_eq(stmt->lhs, STRV("output.color"))) {
			return gfx_shader_text_put(text, STRV("    output_color = ")) ||
			       gfx_shader_emit_hlsl_expr(text, ir, fn, stmt, stmt->expr_root) || gfx_shader_text_put(text, STRV(";\n"));
		}
		if (stage == GFX_SHADER_STAGE_VERTEX && gfx_shader_hlsl_is_position_output(ir, stmt->lhs)) {
			return gfx_shader_text_putf(text, "    %.*s %.*s ", stmt->lhs.len, stmt->lhs.data, stmt->op.len, stmt->op.data) ||
			       gfx_shader_emit_hlsl_expr(text, ir, fn, stmt, stmt->expr_root) ||
			       gfx_shader_text_put(text,
						   STRV(";\n    output.position.z = (output.position.z + output.position.w) * 0.5f;\n"));
		}
		if (gfx_shader_text_putf(text, "    %.*s %.*s ", stmt->lhs.len, stmt->lhs.data, stmt->op.len, stmt->op.data) ||
		    gfx_shader_emit_hlsl_expr(text, ir, fn, stmt, stmt->expr_root) || gfx_shader_text_put(text, STRV(";\n"))) {
			return 1; // LCOV_EXCL_LINE
		}
		return 0;
	}
	if (stmt->kind == GFX_SHADER_STMT_RETURN) {
		if (stage == GFX_SHADER_STAGE_FRAGMENT && strv_eq(stmt->expr, STRV("output"))) {
			return gfx_shader_text_put(text, STRV("    return output_color;\n"));
		}
		return gfx_shader_text_put(text, STRV("    return ")) || gfx_shader_emit_hlsl_expr(text, ir, fn, stmt, stmt->expr_root) ||
		       gfx_shader_text_put(text, STRV(";\n"));
	}
	return 1; // LCOV_EXCL_LINE
}

static int gfx_shader_hlsl_emit_text(buf_t *text, const gfx_shader_ir_t *ir, gfx_shader_stage_t stage)
{
	const gfx_shader_function_ir_t *fn = stage == GFX_SHADER_STAGE_VERTEX ? &ir->vertex : &ir->fragment;
	if (gfx_shader_emit_hlsl_buffers(text, ir)) {
		return 1; // LCOV_EXCL_LINE
	}
	if (stage == GFX_SHADER_STAGE_VERTEX) {
		if (gfx_shader_emit_hlsl_struct(text, &ir->vs_in, 0) || gfx_shader_emit_hlsl_struct(text, &ir->vs_out, 1) ||
		    gfx_shader_text_putf(text,
					 "%.*s main(%.*s input) {\n",
					 ir->vs_out.name.len,
					 ir->vs_out.name.data,
					 ir->vs_in.name.len,
					 ir->vs_in.name.data)) {
			return 1;
		}
	} else if (gfx_shader_emit_hlsl_struct(text, &ir->vs_out, 1) ||
		   gfx_shader_text_putf(text, "float4 main(%.*s input) : SV_TARGET {\n", ir->vs_out.name.len, ir->vs_out.name.data)) {
		return 1; // LCOV_EXCL_LINE
	}
	for (u32 i = 0; i < fn->statement_count; i++) {
		if (gfx_shader_emit_hlsl_statement(text, ir, fn, &fn->statements[i], stage)) {
			return 1; // LCOV_EXCL_LINE
		}
	}
	return gfx_shader_text_put(text, STRV("}\n"));
}

static int gfx_shader_hlsl_emit(const gfx_shader_ir_t *ir, gfx_shader_stage_t stage, gfx_shader_code_t *shader)
{
	buf_t text = {0};
	if (gfx_shader_text_init(&text)) {
		return 1; // LCOV_EXCL_LINE
	}
	if (gfx_shader_hlsl_emit_text(&text, ir, stage) || gfx_shader_text_finish(&text, shader)) {
		buf_free(&text);
		return 1;
	}
	buf_free(&text);
	return 0;
}

static gfx_shader_driver_t gfx_shader_hlsl = {
	.name	  = "hlsl",
	.language = GFX_SHADER_LANGUAGE_HLSL,
	.emit	  = gfx_shader_hlsl_emit,
};

GFX_SHADER_DRIVER(gfx_shader_hlsl, &gfx_shader_hlsl);
