#include "gfx_shader_driver.h"

#include "log.h"
#include "test.h"

#include <string.h>

static int t_gfx_shader_compiler_alloc_count;
static int t_gfx_shader_compiler_alloc_fail_at;

enum {
	T_GFX_SHADER_SPV_OP_F_NEGATE = 127,
};

static const char *t_gfx_shader_compiler_source = "vs_in 0 VertexIn {\n"
						  "\tvec2f position : POSITION;\n"
						  "\tvec4f color : COLOR0;\n"
						  "}\n"
						  "vs_out VertexOut {\n"
						  "\tvec4f position : POSITION;\n"
						  "\tvec4f color : COLOR0;\n"
						  "}\n"
						  "fs_in FragmentIn {\n"
						  "\tvec4f color : COLOR0;\n"
						  "}\n"
						  "fs_out FragmentOut {\n"
						  "\tvec4f color : COLOR0;\n"
						  "}\n"
						  "VertexOut vertex(VertexIn input) {\n"
						  "\tVertexOut output;\n"
						  "\tvec2f local = vec2f(1.0f, 2.0f);\n"
						  "\tvec2f pos = input.position.xy;\n"
						  "\toutput.position = vec4f(local.x, local.y, 0.0f, 1.0f);\n"
						  "\toutput.color = input.color;\n"
						  "\treturn output;\n"
						  "}\n"
						  "FragmentOut fragment(FragmentIn input) {\n"
						  "\tFragmentOut output;\n"
						  "\tvec4f color = input.color.rgba;\n"
						  "\toutput.color = color;\n"
						  "\treturn output;\n"
						  "}\n";

static const char *t_gfx_shader_compiler_expr_source = "vs_in 0 VertexIn {\n"
						       "\tvec2f position : POSITION;\n"
						       "\tvec4f color : COLOR0;\n"
						       "}\n"
						       "vs_out VertexOut {\n"
						       "\tvec4f position : POSITION;\n"
						       "\tvec4f color : COLOR0;\n"
						       "}\n"
						       "fs_in FragmentIn {\n"
						       "\tvec4f color : COLOR0;\n"
						       "}\n"
						       "fs_out FragmentOut {\n"
						       "\tvec4f color : COLOR0;\n"
						       "}\n"
						       "VertexOut vertex(VertexIn input) {\n"
						       "\tVertexOut output;\n"
						       "\tvec4f tint = vec4f(1.0f, 0.5f, 0.25f, 1.0f);\n"
						       "\toutput.color = tint;\n"
						       "\toutput.unknown = tint;\n"
						       "\treturn output;\n"
						       "}\n"
						       "FragmentOut fragment(FragmentIn input) {\n"
						       "\tFragmentOut output;\n"
						       "\tvec4f color = vec4f(1.0f, 0.5f, 0.25f, 1.0f);\n"
						       "\toutput.color = color;\n"
						       "\treturn output;\n"
						       "}\n";

static void t_gfx_shader_compiler_reset(void)
{
	t_gfx_shader_compiler_alloc_count   = 0;
	t_gfx_shader_compiler_alloc_fail_at = 0;
}

static int t_gfx_shader_spv_has_op(const gfx_shader_code_t *shader, u32 op)
{
	const u32 *words = shader->code.data;
	u32 count	 = (u32)(shader->code.used / sizeof(u32));
	for (u32 i = 5; i < count;) {
		u32 word_count = words[i] >> 16;
		u32 word_op    = words[i] & 0xffffu;
		if (word_op == op) {
			return 1;
		}
		if (word_count == 0) {
			return 0;
		}
		i += word_count;
	}
	return 0;
}

static void *t_gfx_shader_compiler_alloc_fail_n(alloc_t *alloc, size_t size)
{
	t_gfx_shader_compiler_alloc_count++;
	if (t_gfx_shader_compiler_alloc_count == t_gfx_shader_compiler_alloc_fail_at) {
		return NULL;
	}
	return alloc_alloc_std(alloc, size);
}

static int t_gfx_shader_compiler_realloc_fail(alloc_t *alloc, void **ptr, size_t *old_size, size_t new_size)
{
	(void)alloc;
	(void)ptr;
	(void)old_size;
	(void)new_size;
	return 1;
}

TEST(gfx_shader_compiler_init_null_compiler)
{
	START;

	EXPECT_NULL(gfx_shader_compiler_init(NULL, ALLOC_STD));

	END;
}

TEST(gfx_shader_compiler_init_alloc_failure)
{
	START;

	for (int i = 1; i <= 1; i++) {
		gfx_shader_compiler_t compiler = {0};
		t_gfx_shader_compiler_reset();
		t_gfx_shader_compiler_alloc_fail_at = i;

		log_set_quiet(0, 1);
		gfx_shader_compiler_t *ret = gfx_shader_compiler_init(
			&compiler,
			(alloc_t){.alloc = t_gfx_shader_compiler_alloc_fail_n, .realloc = alloc_realloc_std, .free = alloc_free_std});
		log_set_quiet(0, 0);
		if (ret != NULL) {
			gfx_shader_compiler_free(&compiler);
		}
	}

	END;
}

TEST(gfx_shader_compiler_free_null_compiler)
{
	START;

	gfx_shader_compiler_free(NULL);

	END;
}

TEST(gfx_shader_compiler_transpile_null_shader)
{
	START;

	log_set_quiet(0, 1);
	EXPECT_EQ(gfx_shader_compiler_transpile(NULL, STRV(""), GFX_SHADER_STAGE_VERTEX, GFX_SHADER_LANGUAGE_GLSL, NULL), 1);
	log_set_quiet(0, 0);

	END;
}

TEST(gfx_shader_compiler_transpile_null_compiler)
{
	START;

	gfx_shader_code_t shader = {0};

	log_set_quiet(0, 1);
	EXPECT_EQ(gfx_shader_compiler_transpile(NULL, STRV(""), (gfx_shader_stage_t)99, (gfx_shader_language_t)99, &shader), 1);
	log_set_quiet(0, 0);

	END;
}

TEST(gfx_shader_compiler_transpile_failures)
{
	START;

	gfx_shader_compiler_t compiler = {0};
	EXPECT_PTR(gfx_shader_compiler_init(&compiler, ALLOC_STD), &compiler);
	gfx_shader_code_t shader = {0};

	log_set_quiet(0, 1);
	EXPECT_EQ(gfx_shader_compiler_transpile(
			  &compiler, STRV("not shader source\n"), GFX_SHADER_STAGE_VERTEX, GFX_SHADER_LANGUAGE_GLSL, &shader),
		  1);
	log_set_quiet(0, 0);

	const char *source = "vs_in 0 VertexIn {\n"
			     "\tvec2f position : POSITION;\n"
			     "}\n"
			     "vs_out VertexOut {\n"
			     "\tvec4f position : POSITION;\n"
			     "}\n"
			     "fs_in FragmentIn {\n"
			     "\tvec4f color : COLOR0;\n"
			     "}\n"
			     "fs_out FragmentOut {\n"
			     "\tvec4f color : COLOR0;\n"
			     "}\n"
			     "VertexOut vertex(VertexIn input) {\n"
			     "\tVertexOut output;\n"
			     "\treturn output;\n"
			     "}\n"
			     "FragmentOut fragment(FragmentIn input) {\n"
			     "\tFragmentOut output;\n"
			     "\treturn output;\n"
			     "}\n";
	log_set_quiet(0, 1);
	EXPECT_EQ(gfx_shader_compiler_transpile(&compiler, strv_cstr(source), GFX_SHADER_STAGE_VERTEX, GFX_SHADER_LANGUAGE_GLSL, &shader),
		  1);
	log_set_quiet(0, 0);

	log_set_quiet(0, 1);
	EXPECT_EQ(gfx_shader_compiler_transpile(
			  &compiler, strv_cstr(t_gfx_shader_compiler_source), GFX_SHADER_STAGE_VERTEX, (gfx_shader_language_t)99, &shader),
		  1);
	log_set_quiet(0, 0);

	gfx_shader_compiler_free(&compiler);
	END;
}

TEST(gfx_shader_compiler_transpile_alloc_failure)
{
	START;

	gfx_shader_compiler_t compiler = {0};
	EXPECT_PTR(gfx_shader_compiler_init(&compiler, ALLOC_STD), &compiler);
	for (int i = 1; i <= 2; i++) {
		gfx_shader_code_t shader = {0};
		t_gfx_shader_compiler_reset();
		t_gfx_shader_compiler_alloc_fail_at = i;
		compiler.alloc =
			(alloc_t){.alloc = t_gfx_shader_compiler_alloc_fail_n, .realloc = alloc_realloc_std, .free = alloc_free_std};

		log_set_quiet(0, 1);
		EXPECT_EQ(gfx_shader_compiler_transpile(&compiler,
							strv_cstr(t_gfx_shader_compiler_source),
							GFX_SHADER_STAGE_VERTEX,
							GFX_SHADER_LANGUAGE_GLSL,
							&shader),
			  1);
		log_set_quiet(0, 0);
		gfx_shader_code_free(&shader);
	}

	gfx_shader_compiler_free(&compiler);
	END;
}

TEST(gfx_shader_compiler_transpile_outputs)
{
	START;

	gfx_shader_compiler_t compiler = {0};
	EXPECT_PTR(gfx_shader_compiler_init(&compiler, ALLOC_STD), &compiler);

	gfx_shader_code_t shader = {0};
	int ret			 = gfx_shader_compiler_transpile(
		&compiler, strv_cstr(t_gfx_shader_compiler_source), GFX_SHADER_STAGE_VERTEX, GFX_SHADER_LANGUAGE_GLSL, &shader);

	EXPECT_EQ(ret, 0);
	if (ret == 0) {
		EXPECT_STRN(shader.text, "#version 330 core", 17);
		EXPECT_NOT_NULL(strstr(shader.text, "layout(location = 0) in vec2 position;"));
		EXPECT_NOT_NULL(strstr(shader.text, "layout(location = 1) in vec4 color;"));
		EXPECT_NOT_NULL(strstr(shader.text, "vec2 local = vec2"));
		EXPECT_NOT_NULL(strstr(shader.text, "vec2 pos = position.xy;"));
		EXPECT_NOT_NULL(strstr(shader.text, "gl_Position = vec4("));
		EXPECT_NOT_NULL(strstr(shader.text, "local.x"));
		EXPECT_NOT_NULL(strstr(shader.text, "local.y"));
		EXPECT_NULL(strstr(shader.text, "u_target_size"));
	}
	gfx_shader_code_free(&shader);

	shader = (gfx_shader_code_t){0};
	ret    = gfx_shader_compiler_transpile(
		&compiler, strv_cstr(t_gfx_shader_compiler_source), GFX_SHADER_STAGE_FRAGMENT, GFX_SHADER_LANGUAGE_GLSL, &shader);

	EXPECT_EQ(ret, 0);
	if (ret == 0) {
		EXPECT_NOT_NULL(strstr(shader.text, "in vec4 v_color;"));
		EXPECT_NOT_NULL(strstr(shader.text, "layout(location = 0) out vec4 o_color;"));
		EXPECT_NOT_NULL(strstr(shader.text, "vec4 color = v_color.rgba;"));
		EXPECT_NOT_NULL(strstr(shader.text, "o_color = color;"));
	}
	gfx_shader_code_free(&shader);

	shader = (gfx_shader_code_t){0};
	ret    = gfx_shader_compiler_transpile(
		&compiler, strv_cstr(t_gfx_shader_compiler_source), GFX_SHADER_STAGE_VERTEX, GFX_SHADER_LANGUAGE_HLSL, &shader);

	EXPECT_EQ(ret, 0);
	if (ret == 0) {
		EXPECT_NOT_NULL(strstr(shader.text, "float2 position : POSITION;"));
		EXPECT_NOT_NULL(strstr(shader.text, "float4 position : SV_POSITION;"));
		EXPECT_NOT_NULL(strstr(shader.text, "float2 local = float2"));
		EXPECT_NOT_NULL(strstr(shader.text, "return output;"));
	}
	gfx_shader_code_free(&shader);

	shader = (gfx_shader_code_t){0};
	ret    = gfx_shader_compiler_transpile(
		&compiler, strv_cstr(t_gfx_shader_compiler_source), GFX_SHADER_STAGE_FRAGMENT, GFX_SHADER_LANGUAGE_HLSL, &shader);

	EXPECT_EQ(ret, 0);
	if (ret == 0) {
		EXPECT_NOT_NULL(strstr(shader.text, "float4 main(VertexOut input) : SV_TARGET"));
		EXPECT_NOT_NULL(strstr(shader.text, "float4 output_color;"));
		EXPECT_NOT_NULL(strstr(shader.text, "output_color = color;"));
		EXPECT_NOT_NULL(strstr(shader.text, "return output_color;"));
	}
	gfx_shader_code_free(&shader);

	shader = (gfx_shader_code_t){0};
	ret    = gfx_shader_compiler_transpile(
		&compiler, strv_cstr(t_gfx_shader_compiler_source), GFX_SHADER_STAGE_VERTEX, GFX_SHADER_LANGUAGE_SPIRV, &shader);

	EXPECT_EQ(ret, 0);
	if (ret == 0) {
		EXPECT_EQ(shader.language, GFX_SHADER_LANGUAGE_SPIRV);
		EXPECT_EQ(shader.stage, GFX_SHADER_STAGE_VERTEX);
		EXPECT_NE(shader.code.used, 0);
		EXPECT_EQ(t_gfx_shader_spv_has_op(&shader, T_GFX_SHADER_SPV_OP_F_NEGATE), 1);
	}

	gfx_shader_code_free(&shader);

	shader = (gfx_shader_code_t){0};
	ret    = gfx_shader_compiler_transpile(
		&compiler, strv_cstr(t_gfx_shader_compiler_expr_source), GFX_SHADER_STAGE_VERTEX, GFX_SHADER_LANGUAGE_GLSL, &shader);

	EXPECT_EQ(ret, 0);
	if (ret == 0) {
		EXPECT_NOT_NULL(strstr(shader.text, "vec4 tint = vec4"));
		EXPECT_NOT_NULL(strstr(shader.text, "v_color = tint;"));
		EXPECT_NOT_NULL(strstr(shader.text, "output.unknown = tint;"));
	}
	gfx_shader_code_free(&shader);

	shader = (gfx_shader_code_t){0};
	ret    = gfx_shader_compiler_transpile(
		&compiler, strv_cstr(t_gfx_shader_compiler_expr_source), GFX_SHADER_STAGE_FRAGMENT, GFX_SHADER_LANGUAGE_HLSL, &shader);

	EXPECT_EQ(ret, 0);
	if (ret == 0) {
		EXPECT_NOT_NULL(strstr(shader.text, "float4 color = float4"));
	}
	gfx_shader_code_free(&shader);

	gfx_shader_compiler_free(&compiler);
	END;
}

TEST(gfx_shader_code_free_null_shader)
{
	START;

	gfx_shader_code_free(NULL);

	END;
}

TEST(gfx_shader_text_putf_format_failure)
{
	START;

	buf_t text = {0};
	EXPECT_EQ(gfx_shader_text_init(&text), 0);

	EXPECT_EQ(gfx_shader_text_putf(&text, "%"), 1);

	buf_free(&text);
	END;
}

TEST(gfx_shader_text_putf_resize_failure)
{
	START;

	buf_t text = {0};
	EXPECT_PTR(buf_init(&text,
			    1,
			    (alloc_t){.alloc = alloc_alloc_std, .realloc = t_gfx_shader_compiler_realloc_fail, .free = alloc_free_std}),
		   &text);

	log_set_quiet(0, 1);
	EXPECT_EQ(gfx_shader_text_putf(&text, "ab"), 1);
	log_set_quiet(0, 0);

	buf_free(&text);
	END;
}

TEST(gfx_shader_text_putf_rejects_n_conversion)
{
	START;

	int count  = 0;
	buf_t text = {0};
	EXPECT_EQ(gfx_shader_text_init(&text), 0);

	EXPECT_EQ(gfx_shader_text_putf(&text, "%n", &count), 1);
	EXPECT_EQ(count, 0);
	EXPECT_EQ(text.used, 0);

	buf_free(&text);
	END;
}

TEST(gfx_shader_text_putf_append)
{
	START;

	buf_t text		 = {0};
	gfx_shader_code_t shader = {0};
	EXPECT_EQ(gfx_shader_text_init(&text), 0);

	EXPECT_EQ(gfx_shader_text_putf(&text, "a"), 0);
	EXPECT_EQ(gfx_shader_text_putf(&text, "b"), 0);
	EXPECT_EQ(gfx_shader_text_finish(&text, &shader), 0);
	EXPECT_STR(shader.text, "ab");

	gfx_shader_code_free(&shader);
	buf_free(&text);
	END;
}

TEST(gfx_shader_text_finish_failure)
{
	START;

	buf_t text = {0};
	EXPECT_PTR(buf_init(&text,
			    1,
			    (alloc_t){.alloc = alloc_alloc_std, .realloc = t_gfx_shader_compiler_realloc_fail, .free = alloc_free_std}),
		   &text);
	gfx_shader_code_t shader = {0};
	text.used		 = 1;

	log_set_quiet(0, 1);
	EXPECT_EQ(gfx_shader_text_finish(&text, &shader), 1);
	log_set_quiet(0, 0);

	buf_free(&text);
	END;
}

TEST(gfx_shader_struct_member_missing)
{
	START;

	gfx_shader_struct_ir_t ir = {
		.members      = {{.name = STRV("color")}},
		.member_count = 1,
	};

	EXPECT_NULL(gfx_shader_struct_member(NULL, STRV("color")));
	EXPECT_NULL(gfx_shader_struct_member(&ir, STRV("position")));

	END;
}

TEST(gfx_shader_strv_prefix_false)
{
	START;

	EXPECT_EQ(gfx_shader_strv_prefix(STRV("ab"), STRV("abc")), 0);
	EXPECT_EQ(gfx_shader_strv_prefix(STRV("abc"), STRV("abd")), 0);

	END;
}

STEST(gfx_shader_compiler)
{
	SSTART;

	RUN(gfx_shader_compiler_init_null_compiler);
	RUN(gfx_shader_compiler_init_alloc_failure);
	RUN(gfx_shader_compiler_free_null_compiler);
	RUN(gfx_shader_compiler_transpile_null_shader);
	RUN(gfx_shader_compiler_transpile_null_compiler);
	RUN(gfx_shader_compiler_transpile_failures);
	RUN(gfx_shader_compiler_transpile_alloc_failure);
	RUN(gfx_shader_compiler_transpile_outputs);
	RUN(gfx_shader_code_free_null_shader);
	RUN(gfx_shader_text_putf_format_failure);
	RUN(gfx_shader_text_putf_resize_failure);
	RUN(gfx_shader_text_putf_rejects_n_conversion);
	RUN(gfx_shader_text_putf_append);
	RUN(gfx_shader_text_finish_failure);
	RUN(gfx_shader_struct_member_missing);
	RUN(gfx_shader_strv_prefix_false);

	SEND;
}
