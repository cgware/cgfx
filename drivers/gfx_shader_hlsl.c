#include "gfx_shader_driver.h"

static const char *gfx_shader_hlsl_type(strv_t type)
{
	if (strv_eq(type, STRV("vec2f"))) {
		return "float2";
	}
	if (strv_eq(type, STRV("vec4f"))) {
		return "float4";
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
		return 1;
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

static int gfx_shader_is_ident_char(char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_';
}

static int gfx_shader_emit_hlsl_expr(buf_t *text, strv_t expr)
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
				mapped = "float2";
			} else if (strv_eq(tok, STRV("vec4f"))) {
				mapped = "float4";
			}
			if (mapped != NULL) {
				if (gfx_shader_text_put(text, strv_cstr(mapped))) {
					return 1;
				}
			} else if (gfx_shader_text_put(text, tok)) {
				return 1;
			}
		} else if (i > 0 && i + 1 < expr.len && expr.data[i] == 'f' &&
			   ((expr.data[i - 1] >= '0' && expr.data[i - 1] <= '9') || expr.data[i - 1] == '.')) {
			i++;
		} else {
			if (gfx_shader_text_put(text, STRVN(&expr.data[i], 1))) {
				return 1;
			}
			i++;
		}
	}
	return 0;
}

static int gfx_shader_emit_hlsl_statement(buf_t *text, const gfx_shader_function_ir_t *fn, const gfx_shader_statement_ir_t *stmt,
					  gfx_shader_stage_t stage)
{
	if (stmt->kind == GFX_SHADER_STMT_DECL) {
		const char *type = gfx_shader_hlsl_type(stmt->type);
		if (stage == GFX_SHADER_STAGE_FRAGMENT && strv_eq(stmt->type, fn->ret)) {
			return gfx_shader_text_put(text, STRV("    float4 output_color;\n"));
		}
		if (type == NULL && strv_eq(stmt->type, fn->ret)) {
			if (gfx_shader_text_putf(text, "    %.*s %.*s", stmt->type.len, stmt->type.data, stmt->name.len, stmt->name.data)) {
				return 1;
			}
		} else if (type != NULL) {
			if (gfx_shader_text_putf(text, "    %s %.*s", type, stmt->name.len, stmt->name.data)) {
				return 1;
			}
		} else {
			return 1;
		}
		if (stmt->has_init) {
			if (gfx_shader_text_put(text, STRV(" = ")) || gfx_shader_emit_hlsl_expr(text, stmt->expr)) {
				return 1;
			}
		}
		return gfx_shader_text_put(text, STRV(";\n"));
	}
	if (stmt->kind == GFX_SHADER_STMT_ASSIGN) {
		if (stage == GFX_SHADER_STAGE_FRAGMENT && strv_eq(stmt->lhs, STRV("output.color"))) {
			return gfx_shader_text_put(text, STRV("    output_color = ")) || gfx_shader_emit_hlsl_expr(text, stmt->expr) ||
			       gfx_shader_text_put(text, STRV(";\n"));
		}
		if (gfx_shader_text_putf(text, "    %.*s %.*s ", stmt->lhs.len, stmt->lhs.data, stmt->op.len, stmt->op.data) ||
		    gfx_shader_emit_hlsl_expr(text, stmt->expr) || gfx_shader_text_put(text, STRV(";\n"))) {
			return 1;
		}
		return 0;
	}
	if (stmt->kind == GFX_SHADER_STMT_RETURN) {
		if (stage == GFX_SHADER_STAGE_FRAGMENT && strv_eq(stmt->expr, STRV("output"))) {
			return gfx_shader_text_put(text, STRV("    return output_color;\n"));
		}
		return gfx_shader_text_put(text, STRV("    return ")) || gfx_shader_emit_hlsl_expr(text, stmt->expr) ||
		       gfx_shader_text_put(text, STRV(";\n"));
	}
	return 1;
}

static int gfx_shader_hlsl_emit(const gfx_shader_ir_t *ir, gfx_shader_stage_t stage, gfx_shader_code_t *shader)
{
	buf_t text = {0};
	if (gfx_shader_text_init(&text)) {
		return 1;
	}
	int ret				   = 1;
	const gfx_shader_function_ir_t *fn = stage == GFX_SHADER_STAGE_VERTEX ? &ir->vertex : &ir->fragment;
	if (stage == GFX_SHADER_STAGE_VERTEX) {
		if (gfx_shader_emit_hlsl_struct(&text, &ir->vs_in, 0) || gfx_shader_emit_hlsl_struct(&text, &ir->vs_out, 1) ||
		    gfx_shader_text_putf(&text,
					 "%.*s main(%.*s input) {\n",
					 ir->vs_out.name.len,
					 ir->vs_out.name.data,
					 ir->vs_in.name.len,
					 ir->vs_in.name.data)) {
			goto cleanup;
		}
	} else if (gfx_shader_emit_hlsl_struct(&text, &ir->vs_out, 1) ||
		   gfx_shader_text_putf(&text, "float4 main(%.*s input) : SV_TARGET {\n", ir->vs_out.name.len, ir->vs_out.name.data)) {
		goto cleanup;
	}
	for (u32 i = 0; i < fn->statement_count; i++) {
		if (gfx_shader_emit_hlsl_statement(&text, fn, &fn->statements[i], stage)) {
			goto cleanup;
		}
	}
	if (gfx_shader_text_put(&text, STRV("}\n")) || gfx_shader_text_finish(&text, shader)) {
		goto cleanup;
	}
	ret = 0;
cleanup:
	buf_free(&text);
	return ret;
}

static gfx_shader_driver_t gfx_shader_hlsl = {
	.name	  = "hlsl",
	.language = GFX_SHADER_LANGUAGE_HLSL,
	.emit	  = gfx_shader_hlsl_emit,
};

GFX_SHADER_DRIVER(gfx_shader_hlsl, &gfx_shader_hlsl);
