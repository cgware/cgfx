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
			return gfx_shader_text_put(text, STRV("gl_Position")); // LCOV_EXCL_LINE
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
	if (strv_eq(type, STRV("vec4f"))) {
		return "vec4";
	}
	return NULL; // LCOV_EXCL_LINE
}

static int gfx_shader_is_ident_char(char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_';
}

static int gfx_shader_emit_glsl_expr(buf_t *text, const gfx_shader_ir_t *ir, gfx_shader_stage_t stage, strv_t expr)
{
	for (size_t i = 0; i < expr.len;) {
		if ((expr.data[i] >= 'a' && expr.data[i] <= 'z') || (expr.data[i] >= 'A' && expr.data[i] <= 'Z') || expr.data[i] == '_') {
			size_t start = i++;
			while (i < expr.len && (gfx_shader_is_ident_char(expr.data[i]) || expr.data[i] == '.')) {
				i++;
			}
			strv_t tok	   = STRVN(&expr.data[start], i - start);
			const char *mapped = NULL;
			if (strv_eq(tok, STRV("vec2f"))) {
				mapped = "vec2";
			} else if (strv_eq(tok, STRV("vec4f"))) {
				mapped = "vec4";
			} else if (stage == GFX_SHADER_STAGE_VERTEX && gfx_shader_strv_prefix(tok, STRV("input."))) {
				strv_t name	   = STRVN(tok.data + STRV("input.").len, tok.len - STRV("input.").len);
				strv_t member_name = name;
				for (size_t n = 0; n < name.len; n++) {
					if (name.data[n] == '.') {
						member_name = STRVN(name.data, n);
						break;
					}
				}
				const gfx_shader_member_t *member = gfx_shader_struct_member(&ir->vs_in, member_name);
				if (member != NULL) {
					if (gfx_shader_emit_glsl_member_ref(
						    text, "", member, STRVN(name.data + member_name.len, name.len - member_name.len))) {
						return 1; // LCOV_EXCL_LINE
					}
					continue;
				}
			} else if (stage == GFX_SHADER_STAGE_FRAGMENT && gfx_shader_strv_prefix(tok, STRV("input."))) {
				strv_t name	   = STRVN(tok.data + STRV("input.").len, tok.len - STRV("input.").len);
				strv_t member_name = name;
				for (size_t n = 0; n < name.len; n++) {
					if (name.data[n] == '.') {
						member_name = STRVN(name.data, n);
						break;
					}
				}
				const gfx_shader_member_t *member = gfx_shader_struct_member(&ir->fs_in, member_name);
				if (member != NULL) {
					if (gfx_shader_emit_glsl_member_ref(
						    text, "v_", member, STRVN(name.data + member_name.len, name.len - member_name.len))) {
						return 1; // LCOV_EXCL_LINE
					}
					continue;
				}
			}
			if (mapped != NULL) {
				if (gfx_shader_text_put(text, strv_cstr(mapped))) {
					return 1; // LCOV_EXCL_LINE
				}
			} else if (gfx_shader_text_put(text, tok)) {
				return 1; // LCOV_EXCL_LINE
			}
		} else if (i > 0 && i + 1 < expr.len && expr.data[i] == 'f' &&
			   ((expr.data[i - 1] >= '0' && expr.data[i - 1] <= '9') || expr.data[i - 1] == '.')) { // LCOV_EXCL_LINE
			i++;											// LCOV_EXCL_LINE
		} else {
			if (gfx_shader_text_put(text, STRVN(&expr.data[i], 1))) {
				return 1; // LCOV_EXCL_LINE
			}
			i++;
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
			if (gfx_shader_text_put(text, STRV(" = ")) || gfx_shader_emit_glsl_expr(text, ir, stage, stmt->expr)) {
				return 1; // LCOV_EXCL_LINE
			}
		}
		return gfx_shader_text_put(text, STRV(";\n"));
	}
	if (stmt->kind == GFX_SHADER_STMT_ASSIGN) {
		if (gfx_shader_text_put(text, STRV("    ")) || gfx_shader_emit_glsl_lhs(text, ir, stage, stmt->lhs) ||
		    gfx_shader_text_putf(text, " %.*s ", stmt->op.len, stmt->op.data) ||
		    gfx_shader_emit_glsl_expr(text, ir, stage, stmt->expr) || gfx_shader_text_put(text, STRV(";\n"))) {
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
	if (stage == GFX_SHADER_STAGE_VERTEX) {
		for (u32 i = 0; i < ir->vs_in.member_count; i++) {
			const char *type = gfx_shader_glsl_type(ir->vs_in.members[i].type);
			if (type == NULL || gfx_shader_text_putf(text,
								 "layout(location = %u) in %s %.*s;\n",
								 i,
								 type,
								 ir->vs_in.members[i].name.len,
								 ir->vs_in.members[i].name.data)) {
				return 1; // LCOV_EXCL_LINE
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
		buf_free(&text); // LCOV_EXCL_LINE
		return 1;	 // LCOV_EXCL_LINE
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
