#include "gfx_shader_driver.h"

static int gfx_shader_emit_glsl_member_ref(buf_t *text, const char *prefix, const gfx_shader_member_t *member, strv_t suffix)
{
	if (member == NULL) {
		return 1; // LCOV_EXCL_LINE
	}
	return gfx_shader_text_putf(text, "%s%.*s%.*s", prefix, member->name.len, member->name.data, suffix.len, suffix.data);
}

static int gfx_shader_emit_glsl_lhs(buf_t *text, const gfx_shader_ir_t *ir, gfx_shader_stage_t stage, strv_t lhs)
{
	if (stage == GFX_SHADER_STAGE_VERTEX && gfx_shader_strv_prefix(lhs, STRV("output."))) {
		strv_t name			  = STRVN(lhs.data + STRV("output.").len, lhs.len - STRV("output.").len);
		const gfx_shader_member_t *member = gfx_shader_struct_member(&ir->vs_out, name);
		if (member != NULL && strv_eq(member->semantic, STRV("POSITION"))) {
			return gfx_shader_text_put(text, STRV("gl_Position"));
		}
		if (member != NULL) {
			return gfx_shader_emit_glsl_member_ref(text, "v_", member, STRV(""));
		}
	} else if (stage == GFX_SHADER_STAGE_FRAGMENT && gfx_shader_strv_prefix(lhs, STRV("output."))) {
		strv_t name			  = STRVN(lhs.data + STRV("output.").len, lhs.len - STRV("output.").len);
		const gfx_shader_member_t *member = gfx_shader_struct_member(&ir->fs_out, name);
		if (member != NULL) {
			return gfx_shader_emit_glsl_member_ref(text, "o_", member, STRV(""));
		}
	}
	return gfx_shader_text_put(text, lhs);
}

static const char *gfx_shader_glsl_type(strv_t type)
{
	if (strv_eq(type, STRV("vec2f"))) {
		return "vec2";
	}
	if (strv_eq(type, STRV("vec3f"))) {
		return "vec3";
	}
	if (strv_eq(type, STRV("vec4f"))) {
		return "vec4";
	}
	if (strv_eq(type, STRV("mat4f"))) {
		return "mat4";
	}
	return NULL;
}

static strv_t gfx_shader_expr_member_base(strv_t name)
{
	for (size_t i = 0; i < name.len; i++) {
		if (name.data[i] == '.') {
			return STRVN(name.data, i);
		}
	}
	return name;
}

static int gfx_shader_emit_glsl_expr(buf_t *text, const gfx_shader_ir_t *ir, const gfx_shader_statement_ir_t *stmt,
				     gfx_shader_stage_t stage, u32 node_id)
{
	if (stmt == NULL || node_id >= stmt->expr_count) {
		return 1; // LCOV_EXCL_LINE
	}
	const gfx_shader_expr_ir_t *node = &stmt->expr_nodes[node_id];
	if (node->kind == GFX_SHADER_EXPR_INT || node->kind == GFX_SHADER_EXPR_FLOAT) {
		return gfx_shader_text_put(text, node->text);
	}
	if (node->kind == GFX_SHADER_EXPR_LVALUE) {
		if (stage == GFX_SHADER_STAGE_VERTEX && gfx_shader_strv_prefix(node->text, STRV("input."))) {
			strv_t name	   = STRVN(node->text.data + STRV("input.").len, node->text.len - STRV("input.").len);
			strv_t member_name = gfx_shader_expr_member_base(name);
			const gfx_shader_member_t *member = gfx_shader_struct_member(&ir->vs_in, member_name);
			if (member != NULL) {
				return gfx_shader_emit_glsl_member_ref(
					text, "", member, STRVN(name.data + member_name.len, name.len - member_name.len));
			}
		}
		if (stage == GFX_SHADER_STAGE_FRAGMENT && gfx_shader_strv_prefix(node->text, STRV("input."))) {
			strv_t name	   = STRVN(node->text.data + STRV("input.").len, node->text.len - STRV("input.").len);
			strv_t member_name = gfx_shader_expr_member_base(name);
			const gfx_shader_member_t *member = gfx_shader_struct_member(&ir->fs_in, member_name);
			if (member != NULL) {
				return gfx_shader_emit_glsl_member_ref(
					text, "v_", member, STRVN(name.data + member_name.len, name.len - member_name.len));
			}
		}
		return gfx_shader_text_put(text, node->text);
	}
	if (node->kind == GFX_SHADER_EXPR_CALL) {
		const char *mapped = NULL;
		if (strv_eq(node->text, STRV("vec2f"))) {
			mapped = "vec2";
		} else if (strv_eq(node->text, STRV("vec4f"))) {
			mapped = "vec4";
		} else if (strv_eq(node->text, STRV("mat4f"))) {
			mapped = "mat4";
		}
		if (gfx_shader_text_put(text, mapped != NULL ? strv_cstr(mapped) : node->text) || gfx_shader_text_put(text, STRV("("))) {
			return 1; // LCOV_EXCL_LINE
		}
		for (u32 i = 0; i < node->arg_count; i++) {
			if ((i > 0 && gfx_shader_text_put(text, STRV(", "))) ||
			    gfx_shader_emit_glsl_expr(text, ir, stmt, stage, node->args[i])) {
				return 1; // LCOV_EXCL_LINE
			}
		}
		return gfx_shader_text_put(text, STRV(")"));
	}
	if (node->kind == GFX_SHADER_EXPR_BINARY) {
		return gfx_shader_text_put(text, STRV("(")) || gfx_shader_emit_glsl_expr(text, ir, stmt, stage, node->left) ||
		       gfx_shader_text_putf(text, " %.*s ", node->op.len, node->op.data) ||
		       gfx_shader_emit_glsl_expr(text, ir, stmt, stage, node->right) || gfx_shader_text_put(text, STRV(")"));
	}
	return 1; // LCOV_EXCL_LINE
}

static int gfx_shader_emit_glsl_buffers(buf_t *text, const gfx_shader_ir_t *ir)
{
	for (u32 i = 0; i < ir->buffer_count; i++) {
		const gfx_shader_struct_ir_t *buf = &ir->buffers[i];
		if (gfx_shader_text_putf(text, "layout(std140) uniform %.*s {\n", buf->name.len, buf->name.data)) {
			return 1; // LCOV_EXCL_LINE
		}
		for (u32 m = 0; m < buf->member_count; m++) {
			const char *type = gfx_shader_glsl_type(buf->members[m].type);
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

static int gfx_shader_emit_glsl_statement(buf_t *text, const gfx_shader_ir_t *ir, const gfx_shader_function_ir_t *fn,
					  const gfx_shader_statement_ir_t *stmt, gfx_shader_stage_t stage)
{
	if (stmt->kind == GFX_SHADER_STMT_DECL) {
		if (strv_eq(stmt->type, fn->ret)) {
			return 0;
		}
		const char *type = gfx_shader_glsl_type(stmt->type);
		if (type == NULL || gfx_shader_text_putf(text, "    %s %.*s", type, stmt->name.len, stmt->name.data)) {
			return 1; // LCOV_EXCL_LINE
		}
		if (stmt->has_init) {
			if (gfx_shader_text_put(text, STRV(" = ")) || gfx_shader_emit_glsl_expr(text, ir, stmt, stage, stmt->expr_root)) {
				return 1; // LCOV_EXCL_LINE
			}
		}
		return gfx_shader_text_put(text, STRV(";\n"));
	}
	if (stmt->kind == GFX_SHADER_STMT_ASSIGN) {
		if (gfx_shader_text_put(text, STRV("    ")) || gfx_shader_emit_glsl_lhs(text, ir, stage, stmt->lhs) ||
		    gfx_shader_text_putf(text, " %.*s ", stmt->op.len, stmt->op.data) ||
		    gfx_shader_emit_glsl_expr(text, ir, stmt, stage, stmt->expr_root) || gfx_shader_text_put(text, STRV(";\n"))) {
			return 1; // LCOV_EXCL_LINE
		}
		return 0;
	}
	if (stmt->kind == GFX_SHADER_STMT_RETURN) {
		return 0;
	}
	return 1; // LCOV_EXCL_LINE
}

static int gfx_shader_glsl_emit_text(buf_t *text, const gfx_shader_ir_t *ir, gfx_shader_stage_t stage)
{
	if (gfx_shader_text_put(text, STRV("#version 330 core\n"))) {
		return 1; // LCOV_EXCL_LINE
	}
	if (gfx_shader_emit_glsl_buffers(text, ir)) {
		return 1; // LCOV_EXCL_LINE
	}
	if (stage == GFX_SHADER_STAGE_VERTEX) {
		for (u32 i = 0; i < ir->vs_in.member_count; i++) {
			const char *type = gfx_shader_glsl_type(ir->vs_in.members[i].type);
			if (type == NULL || gfx_shader_text_putf(text,
								 "layout(location = %u) in %s %.*s;\n",
								 i,
								 type,
								 ir->vs_in.members[i].name.len,
								 ir->vs_in.members[i].name.data)) {
				return 1;
			}
		}
		for (u32 i = 0; i < ir->vs_out.member_count; i++) {
			if (strv_eq(ir->vs_out.members[i].semantic, STRV("POSITION"))) {
				continue;
			}
			const char *type = gfx_shader_glsl_type(ir->vs_out.members[i].type);
			if (type == NULL ||
			    gfx_shader_text_putf(
				    text, "out %s v_%.*s;\n", type, ir->vs_out.members[i].name.len, ir->vs_out.members[i].name.data)) {
				return 1; // LCOV_EXCL_LINE
			}
		}
		if (gfx_shader_text_put(text, STRV("void main(void) {\n"))) {
			return 1; // LCOV_EXCL_LINE
		}
		for (u32 i = 0; i < ir->vertex.statement_count; i++) {
			if (gfx_shader_emit_glsl_statement(text, ir, &ir->vertex, &ir->vertex.statements[i], stage)) {
				return 1; // LCOV_EXCL_LINE
			}
		}
	} else {
		for (u32 i = 0; i < ir->fs_in.member_count; i++) {
			const char *type = gfx_shader_glsl_type(ir->fs_in.members[i].type);
			if (type == NULL ||
			    gfx_shader_text_putf(
				    text, "in %s v_%.*s;\n", type, ir->fs_in.members[i].name.len, ir->fs_in.members[i].name.data)) {
				return 1; // LCOV_EXCL_LINE
			}
		}
		for (u32 i = 0; i < ir->fs_out.member_count; i++) {
			const char *type = gfx_shader_glsl_type(ir->fs_out.members[i].type);
			if (type == NULL || gfx_shader_text_putf(text,
								 "layout(location = %u) out %s o_%.*s;\n",
								 i,
								 type,
								 ir->fs_out.members[i].name.len,
								 ir->fs_out.members[i].name.data)) {
				return 1; // LCOV_EXCL_LINE
			}
		}
		if (gfx_shader_text_put(text, STRV("void main(void) {\n"))) {
			return 1; // LCOV_EXCL_LINE
		}
		for (u32 i = 0; i < ir->fragment.statement_count; i++) {
			if (gfx_shader_emit_glsl_statement(text, ir, &ir->fragment, &ir->fragment.statements[i], stage)) {
				return 1; // LCOV_EXCL_LINE
			}
		}
	}
	return gfx_shader_text_put(text, STRV("}\n"));
}

static int gfx_shader_glsl_emit(const gfx_shader_ir_t *ir, gfx_shader_stage_t stage, gfx_shader_code_t *shader)
{
	buf_t text = {0};
	if (gfx_shader_text_init(&text)) {
		return 1; // LCOV_EXCL_LINE
	}
	if (gfx_shader_glsl_emit_text(&text, ir, stage) || gfx_shader_text_finish(&text, shader)) {
		buf_free(&text);
		return 1;
	}
	buf_free(&text);
	return 0;
}

static gfx_shader_driver_t gfx_shader_glsl = {
	.name	  = "glsl",
	.language = GFX_SHADER_LANGUAGE_GLSL,
	.emit	  = gfx_shader_glsl_emit,
};

GFX_SHADER_DRIVER(gfx_shader_glsl, &gfx_shader_glsl);
