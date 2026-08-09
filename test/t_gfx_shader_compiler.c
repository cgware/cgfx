#include "gfx_shader_driver.h"

#include "log.h"
#include "test.h"

#include <stdio.h>
#include <string.h>

static int t_gfx_shader_compiler_alloc_count;
static int t_gfx_shader_compiler_alloc_fail_at;

enum {
	T_GFX_SHADER_SPV_OP_F_NEGATE		= 127,
	T_GFX_SHADER_SPV_OP_MATRIX_TIMES_VECTOR = 145,
	T_GFX_SHADER_SPV_OP_MATRIX_TIMES_MATRIX = 146,
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

static const char *t_gfx_shader_compiler_extra_semantic_source =
	"vs_in 0 VertexIn {\n"
	"\tvec2f position : POSITION;\n"
	"\tvec4f color : COLOR0;\n"
	"\tvec4f extra : TEXCOORD0;\n"
	"}\n"
	"vs_out VertexOut {\n"
	"\tvec4f position : POSITION;\n"
	"\tvec4f color : COLOR0;\n"
	"\tvec4f extra : TEXCOORD0;\n"
	"}\n"
	"fs_in FragmentIn {\n"
	"\tvec4f color : COLOR0;\n"
	"}\n"
	"fs_out FragmentOut {\n"
	"\tvec4f color : COLOR0;\n"
	"}\n"
	"VertexOut vertex(VertexIn input) {\n"
	"\tVertexOut output;\n"
	"\toutput.position = vec4f(input.position.x, input.position.y, 0.0f, 1.0f);\n"
	"\toutput.color = input.color;\n"
	"\treturn output;\n"
	"}\n"
	"FragmentOut fragment(FragmentIn input) {\n"
	"\tFragmentOut output;\n"
	"\toutput.color = input.color;\n"
	"\treturn output;\n"
	"}\n";

static const char *t_gfx_shader_compiler_extra_type_source = "vs_in 0 VertexIn {\n"
							     "\tvec2f position : POSITION;\n"
							     "\tvec4f color : COLOR0;\n"
							     "\tvec5f extra : TEXCOORD0;\n"
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
							     "\toutput.position = vec4f(input.position.x, input.position.y, 0.0f, 1.0f);\n"
							     "\toutput.color = input.color;\n"
							     "\treturn output;\n"
							     "}\n"
							     "FragmentOut fragment(FragmentIn input) {\n"
							     "\tFragmentOut output;\n"
							     "\toutput.color = input.color;\n"
							     "\treturn output;\n"
							     "}\n";

static const char *t_gfx_shader_compiler_uniform_source =
	"vs_in 0 VertexIn {\n"
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
	"buffer 3 Camera {\n"
	"\tmat4f model;\n"
	"\tmat4f view;\n"
	"\tmat4f projection;\n"
	"}\n"
	"VertexOut vertex(VertexIn input) {\n"
	"\tVertexOut output;\n"
	"\toutput.position = projection * view * model * vec4f(input.position.x, input.position.y, 0.0f, 1.0f);\n"
	"\toutput.color = input.color;\n"
	"\treturn output;\n"
	"}\n"
	"FragmentOut fragment(FragmentIn input) {\n"
	"\tFragmentOut output;\n"
	"\toutput.color = input.color;\n"
	"\treturn output;\n"
	"}\n";

static const char *t_gfx_shader_compiler_invalid_buffer_source =
	"vs_in 0 VertexIn {\n"
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
	"buffer 0 BadBuffer {\n"
	"\tvec5f invalid;\n"
	"}\n"
	"VertexOut vertex(VertexIn input) {\n"
	"\tVertexOut output;\n"
	"\toutput.position = vec4f(input.position.x, input.position.y, 0.0f, 1.0f);\n"
	"\toutput.color = input.color;\n"
	"\treturn output;\n"
	"}\n"
	"FragmentOut fragment(FragmentIn input) {\n"
	"\tFragmentOut output;\n"
	"\toutput.color = input.color;\n"
	"\treturn output;\n"
	"}\n";

static const char *t_gfx_shader_compiler_vec3_mat4_source = "vs_in 0 VertexIn {\n"
							    "\tvec3f position : POSITION;\n"
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
							    "\tvec3f local = input.position;\n"
							    "\tmat4f transform = mat4f();\n"
							    "\toutput.position = transform * vec4f(local.x, local.y, local.z, 1.0f);\n"
							    "\toutput.color = input.color;\n"
							    "\treturn output;\n"
							    "}\n"
							    "FragmentOut fragment(FragmentIn input) {\n"
							    "\tFragmentOut output;\n"
							    "\toutput.color = input.color;\n"
							    "\treturn output;\n"
							    "}\n";

static const char *t_gfx_shader_compiler_spirv_extra_type_source =
	"vs_in 0 VertexIn {\n"
	"\tvec3f position : POSITION;\n"
	"\tvec4f color : COLOR0;\n"
	"\tmat4f transform : TEXCOORD0;\n"
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
	"\toutput.position = vec4f(input.position.x, input.position.y, input.position.z, 1.0f);\n"
	"\toutput.color = input.color;\n"
	"\treturn output;\n"
	"}\n"
	"FragmentOut fragment(FragmentIn input) {\n"
	"\tFragmentOut output;\n"
	"\toutput.color = input.color;\n"
	"\treturn output;\n"
	"}\n";

static const char *t_gfx_shader_compiler_overflow_buffer_slot_source =
	"vs_in 0 VertexIn {\n"
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
	"buffer 42949672960 Camera {\n"
	"\tmat4f mvp;\n"
	"}\n"
	"VertexOut vertex(VertexIn input) {\n"
	"\tVertexOut output;\n"
	"\toutput.position = vec4f(input.position.x, input.position.y, 0.0f, 1.0f);\n"
	"\toutput.color = input.color;\n"
	"\treturn output;\n"
	"}\n"
	"FragmentOut fragment(FragmentIn input) {\n"
	"\tFragmentOut output;\n"
	"\toutput.color = input.color;\n"
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

static int t_gfx_shader_compiler_transpile_expr(gfx_shader_compiler_t *compiler, const char *expr)
{
	char source[1024] = {0};
	int len		  = snprintf(source,
			     sizeof(source),
			     "vs_in 0 VertexIn {\n"
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
				     "\tvec2f pos = %s;\n"
				     "\toutput.position = vec4f(pos.x, pos.y, 0.0f, 1.0f);\n"
				     "\toutput.color = input.color;\n"
				     "\treturn output;\n"
				     "}\n"
				     "FragmentOut fragment(FragmentIn input) {\n"
				     "\tFragmentOut output;\n"
				     "\toutput.color = input.color;\n"
				     "\treturn output;\n"
				     "}\n",
			     expr);
	if (len < 0 || (size_t)len >= sizeof(source)) {
		return 1; // LCOV_EXCL_LINE
	}

	gfx_shader_code_t shader = {0};
	int ret = gfx_shader_compiler_transpile(compiler, strv_cstr(source), GFX_SHADER_STAGE_VERTEX, GFX_SHADER_LANGUAGE_GLSL, &shader);
	gfx_shader_code_free(&shader);
	return ret;
}

static int t_gfx_shader_compiler_transpile_spirv_position_expr(gfx_shader_compiler_t *compiler, const char *position_type, const char *expr,
							       int uniform_block)
{
	char source[2048] = {0};
	int len		  = snprintf(source,
			     sizeof(source),
			     "vs_in 0 VertexIn {\n"
				     "\t%s position : POSITION;\n"
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
				     "%s"
				     "VertexOut vertex(VertexIn input) {\n"
				     "\tVertexOut output;\n"
				     "\toutput.position = %s;\n"
				     "\toutput.color = input.color;\n"
				     "\treturn output;\n"
				     "}\n"
				     "FragmentOut fragment(FragmentIn input) {\n"
				     "\tFragmentOut output;\n"
				     "\toutput.color = input.color;\n"
				     "\treturn output;\n"
				     "}\n",
			     position_type,
			     uniform_block ? "buffer 0 Camera {\n\tmat4f mvp;\n}\n" : "",
			     expr);
	if (len < 0 || (size_t)len >= sizeof(source)) {
		return 1; // LCOV_EXCL_LINE
	}

	gfx_shader_code_t shader = {0};
	int ret = gfx_shader_compiler_transpile(compiler, strv_cstr(source), GFX_SHADER_STAGE_VERTEX, GFX_SHADER_LANGUAGE_SPIRV, &shader);
	gfx_shader_code_free(&shader);
	return ret;
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

TEST(gfx_shader_compiler_transpile_expression_operator_spacing)
{
	START;

	gfx_shader_compiler_t compiler = {0};
	EXPECT_PTR(gfx_shader_compiler_init(&compiler, ALLOC_STD), &compiler);

	EXPECT_EQ(t_gfx_shader_compiler_transpile_expr(&compiler, "input.position+input.position"), 0);
	EXPECT_EQ(t_gfx_shader_compiler_transpile_expr(&compiler, "input.position +input.position"), 0);
	EXPECT_EQ(t_gfx_shader_compiler_transpile_expr(&compiler, "input.position+ input.position"), 0);
	EXPECT_EQ(t_gfx_shader_compiler_transpile_expr(&compiler, "input.position + input.position"), 0);
	EXPECT_EQ(t_gfx_shader_compiler_transpile_expr(&compiler, "input.position  +input.position"), 0);
	EXPECT_EQ(t_gfx_shader_compiler_transpile_expr(&compiler, "input.position+  input.position"), 0);
	EXPECT_EQ(t_gfx_shader_compiler_transpile_expr(&compiler, "input.position  +  input.position"), 0);
	EXPECT_EQ(t_gfx_shader_compiler_transpile_expr(&compiler, "input.position    +    input.position"), 0);

	gfx_shader_compiler_free(&compiler);
	END;
}

TEST(gfx_shader_compiler_transpile_expression_parse_failures)
{
	START;

	gfx_shader_compiler_t compiler = {0};
	EXPECT_PTR(gfx_shader_compiler_init(&compiler, ALLOC_STD), &compiler);

	log_set_quiet(0, 1);
	EXPECT_EQ(t_gfx_shader_compiler_transpile_expr(&compiler, ""), 1);
	EXPECT_EQ(t_gfx_shader_compiler_transpile_expr(&compiler, "@"), 1);
	EXPECT_EQ(t_gfx_shader_compiler_transpile_expr(&compiler, "input.position +"), 1);
	EXPECT_EQ(t_gfx_shader_compiler_transpile_expr(&compiler, "vec2f(input.position,"), 1);
	EXPECT_EQ(t_gfx_shader_compiler_transpile_expr(&compiler, "vec2f(input.position input.position)"), 1);
	log_set_quiet(0, 0);
	EXPECT_EQ(t_gfx_shader_compiler_transpile_expr(&compiler, "vec2f()"), 0);
	char expr[1024] = {0};
	for (u32 i = 0; i < 34; i++) {
		if (i > 0) {
			strcat(expr, " + ");
		}
		strcat(expr, "input.position");
	}
	log_set_quiet(0, 1);
	EXPECT_EQ(t_gfx_shader_compiler_transpile_expr(&compiler, expr), 1);
	log_set_quiet(0, 0);

	gfx_shader_compiler_free(&compiler);
	END;
}

TEST(gfx_shader_compiler_transpile_expression_multi_char_operators)
{
	START;

	gfx_shader_compiler_t compiler = {0};
	EXPECT_PTR(gfx_shader_compiler_init(&compiler, ALLOC_STD), &compiler);

	EXPECT_EQ(t_gfx_shader_compiler_transpile_expr(&compiler, "input.position == input.position"), 0);

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

TEST(gfx_shader_compiler_transpile_uniform_block_outputs)
{
	START;

	gfx_shader_compiler_t compiler = {0};
	EXPECT_PTR(gfx_shader_compiler_init(&compiler, ALLOC_STD), &compiler);

	gfx_shader_code_t shader = {0};
	int ret			 = gfx_shader_compiler_transpile(
		 &compiler, strv_cstr(t_gfx_shader_compiler_uniform_source), GFX_SHADER_STAGE_VERTEX, GFX_SHADER_LANGUAGE_GLSL, &shader);

	EXPECT_EQ(ret, 0);
	if (ret == 0) {
		EXPECT_EQ(shader.buffer_count, 1);
		EXPECT_EQ(shader.buffers[0].slot, 3);
		EXPECT_EQ(strv_eq(shader.buffers[0].name, STRV("Camera")), 1);
		EXPECT_NOT_NULL(strstr(shader.text, "layout(std140) uniform Camera"));
		EXPECT_NOT_NULL(strstr(shader.text, "mat4 model;"));
		EXPECT_NOT_NULL(strstr(shader.text, "mat4 view;"));
		EXPECT_NOT_NULL(strstr(shader.text, "mat4 projection;"));
		EXPECT_NOT_NULL(strstr(shader.text, "projection"));
		EXPECT_NOT_NULL(strstr(shader.text, "view"));
		EXPECT_NOT_NULL(strstr(shader.text, "model"));
		EXPECT_NOT_NULL(strstr(shader.text, "vec4("));
	}
	gfx_shader_code_free(&shader);

	shader = (gfx_shader_code_t){0};
	ret    = gfx_shader_compiler_transpile(
		   &compiler, strv_cstr(t_gfx_shader_compiler_uniform_source), GFX_SHADER_STAGE_VERTEX, GFX_SHADER_LANGUAGE_HLSL, &shader);

	EXPECT_EQ(ret, 0);
	if (ret == 0) {
		EXPECT_EQ(shader.buffer_count, 1);
		EXPECT_EQ(shader.buffers[0].slot, 3);
		EXPECT_EQ(strv_eq(shader.buffers[0].name, STRV("Camera")), 1);
		EXPECT_NOT_NULL(strstr(shader.text, "cbuffer Camera : register(b3)"));
		EXPECT_NOT_NULL(strstr(shader.text, "float4x4 model;"));
		EXPECT_NOT_NULL(strstr(shader.text, "float4x4 view;"));
		EXPECT_NOT_NULL(strstr(shader.text, "float4x4 projection;"));
		EXPECT_NOT_NULL(strstr(shader.text, "projection"));
		EXPECT_NOT_NULL(strstr(shader.text, "view"));
		EXPECT_NOT_NULL(strstr(shader.text, "model"));
		EXPECT_NOT_NULL(strstr(shader.text, "float4("));
	}
	gfx_shader_code_free(&shader);

	shader = (gfx_shader_code_t){0};
	ret    = gfx_shader_compiler_transpile(
		   &compiler, strv_cstr(t_gfx_shader_compiler_uniform_source), GFX_SHADER_STAGE_VERTEX, GFX_SHADER_LANGUAGE_SPIRV, &shader);

	EXPECT_EQ(ret, 0);
	if (ret == 0) {
		EXPECT_EQ(shader.language, GFX_SHADER_LANGUAGE_SPIRV);
		EXPECT_EQ(shader.stage, GFX_SHADER_STAGE_VERTEX);
		EXPECT_EQ(shader.buffer_count, 1);
		EXPECT_EQ(shader.buffers[0].slot, 3);
		EXPECT_EQ(strv_eq(shader.buffers[0].name, STRV("Camera")), 1);
		EXPECT_EQ(t_gfx_shader_spv_has_op(&shader, T_GFX_SHADER_SPV_OP_MATRIX_TIMES_MATRIX), 1);
		EXPECT_EQ(t_gfx_shader_spv_has_op(&shader, T_GFX_SHADER_SPV_OP_MATRIX_TIMES_VECTOR), 1);
	}
	gfx_shader_code_free(&shader);

	gfx_shader_compiler_free(&compiler);
	END;
}

TEST(gfx_shader_compiler_transpile_rejects_invalid_buffer_member)
{
	START;

	gfx_shader_compiler_t compiler = {0};
	EXPECT_PTR(gfx_shader_compiler_init(&compiler, ALLOC_STD), &compiler);
	gfx_shader_code_t shader = {0};

	log_set_quiet(0, 1);
	EXPECT_EQ(gfx_shader_compiler_transpile(&compiler,
						strv_cstr(t_gfx_shader_compiler_invalid_buffer_source),
						GFX_SHADER_STAGE_VERTEX,
						GFX_SHADER_LANGUAGE_GLSL,
						&shader),
		  1);
	log_set_quiet(0, 0);
	gfx_shader_code_free(&shader);

	gfx_shader_compiler_free(&compiler);
	END;
}

TEST(gfx_shader_compiler_transpile_vec3_mat4_outputs)
{
	START;

	gfx_shader_compiler_t compiler = {0};
	EXPECT_PTR(gfx_shader_compiler_init(&compiler, ALLOC_STD), &compiler);

	gfx_shader_code_t shader = {0};
	int ret			 = gfx_shader_compiler_transpile(
		 &compiler, strv_cstr(t_gfx_shader_compiler_vec3_mat4_source), GFX_SHADER_STAGE_VERTEX, GFX_SHADER_LANGUAGE_GLSL, &shader);

	EXPECT_EQ(ret, 0);
	if (ret == 0) {
		EXPECT_NOT_NULL(strstr(shader.text, "layout(location = 0) in vec3 position;"));
		EXPECT_NOT_NULL(strstr(shader.text, "vec3 local = position;"));
		EXPECT_NOT_NULL(strstr(shader.text, "mat4 transform = mat4();"));
	}
	gfx_shader_code_free(&shader);

	shader = (gfx_shader_code_t){0};
	ret    = gfx_shader_compiler_transpile(
		   &compiler, strv_cstr(t_gfx_shader_compiler_vec3_mat4_source), GFX_SHADER_STAGE_VERTEX, GFX_SHADER_LANGUAGE_HLSL, &shader);

	EXPECT_EQ(ret, 0);
	if (ret == 0) {
		EXPECT_NOT_NULL(strstr(shader.text, "float3 position : POSITION;"));
		EXPECT_NOT_NULL(strstr(shader.text, "float3 local = input.position;"));
		EXPECT_NOT_NULL(strstr(shader.text, "float4x4 transform = float4x4();"));
	}
	gfx_shader_code_free(&shader);

	gfx_shader_compiler_free(&compiler);
	END;
}

TEST(gfx_shader_compiler_transpile_spirv_extra_input_types)
{
	START;

	gfx_shader_compiler_t compiler = {0};
	EXPECT_PTR(gfx_shader_compiler_init(&compiler, ALLOC_STD), &compiler);
	gfx_shader_code_t shader = {0};

	EXPECT_EQ(gfx_shader_compiler_transpile(&compiler,
						strv_cstr(t_gfx_shader_compiler_spirv_extra_type_source),
						GFX_SHADER_STAGE_VERTEX,
						GFX_SHADER_LANGUAGE_SPIRV,
						&shader),
		  0);
	gfx_shader_code_free(&shader);

	gfx_shader_compiler_free(&compiler);
	END;
}

TEST(gfx_shader_compiler_transpile_spirv_expression_failures)
{
	START;

	gfx_shader_compiler_t compiler = {0};
	EXPECT_PTR(gfx_shader_compiler_init(&compiler, ALLOC_STD), &compiler);

	log_set_quiet(0, 1);
	EXPECT_EQ(t_gfx_shader_compiler_transpile_spirv_position_expr(
			  &compiler, "vec2f", "vec4f(input.unknown.x, input.position.y, 0.0f, 1.0f)", 0),
		  0);
	EXPECT_EQ(t_gfx_shader_compiler_transpile_spirv_position_expr(
			  &compiler, "vec2f", "vec4f(input.position.z, input.position.y, 0.0f, 1.0f)", 0),
		  0);
	EXPECT_EQ(t_gfx_shader_compiler_transpile_spirv_position_expr(
			  &compiler, "vec2f", "vec4f(input.position.w, input.position.y, 0.0f, 1.0f)", 0),
		  0);
	EXPECT_EQ(t_gfx_shader_compiler_transpile_spirv_position_expr(&compiler, "vec2f", "vec4f(2.0f, input.position.y, 0.0f, 1.0f)", 0),
		  0);
	EXPECT_EQ(t_gfx_shader_compiler_transpile_spirv_position_expr(&compiler, "vec2f", "input.position + input.position", 0), 0);
	EXPECT_EQ(t_gfx_shader_compiler_transpile_spirv_position_expr(&compiler, "vec3f", "input.position", 0), 1);
	EXPECT_EQ(t_gfx_shader_compiler_transpile_spirv_position_expr(&compiler, "vec2f", "unknown * vec4f(0.0f, 0.0f, 0.0f, 1.0f)", 1), 0);
	log_set_quiet(0, 0);

	gfx_shader_compiler_free(&compiler);
	END;
}

TEST(gfx_shader_compiler_transpile_rejects_overflow_buffer_slot)
{
	START;

	gfx_shader_compiler_t compiler = {0};
	EXPECT_PTR(gfx_shader_compiler_init(&compiler, ALLOC_STD), &compiler);
	gfx_shader_code_t shader = {0};

	log_set_quiet(0, 1);
	EXPECT_EQ(gfx_shader_compiler_transpile(&compiler,
						strv_cstr(t_gfx_shader_compiler_overflow_buffer_slot_source),
						GFX_SHADER_STAGE_VERTEX,
						GFX_SHADER_LANGUAGE_GLSL,
						&shader),
		  1);
	log_set_quiet(0, 0);
	gfx_shader_code_free(&shader);

	gfx_shader_compiler_free(&compiler);
	END;
}

TEST(gfx_shader_compiler_ir_contracts)
{
	START;

	gfx_shader_ir_t ir = {0};
	log_set_quiet(0, 1);
	EXPECT_EQ(gfx_shader_compiler_ir(NULL, strv_cstr(t_gfx_shader_compiler_source), &ir), 1);
	EXPECT_EQ(gfx_shader_compiler_ir(NULL, strv_cstr(t_gfx_shader_compiler_source), NULL), 1);
	log_set_quiet(0, 0);

	gfx_shader_compiler_t compiler = {0};
	EXPECT_PTR(gfx_shader_compiler_init(&compiler, ALLOC_STD), &compiler);

	EXPECT_EQ(gfx_shader_compiler_ir(&compiler, strv_cstr(t_gfx_shader_compiler_source), &ir), 0);
	EXPECT_EQ(ir.vs_in.present, 1);
	EXPECT_EQ(ir.vertex.present, 1);

	log_set_quiet(0, 1);
	EXPECT_EQ(gfx_shader_compiler_ir(&compiler, STRV("not shader"), &ir), 1);
	EXPECT_EQ(gfx_shader_compiler_ir(&compiler, STRV("vs_in 0 VertexIn {\n}\n"), &ir), 1);
	EXPECT_EQ(gfx_shader_compiler_ir(&compiler, strv_cstr(t_gfx_shader_compiler_invalid_buffer_source), &ir), 1);
	log_set_quiet(0, 0);

	gfx_shader_compiler_free(&compiler);
	END;
}

TEST(gfx_shader_compiler_transpile_extra_semantic)
{
	START;

	gfx_shader_compiler_t compiler = {0};
	EXPECT_PTR(gfx_shader_compiler_init(&compiler, ALLOC_STD), &compiler);
	gfx_shader_code_t shader = {0};

	log_set_quiet(0, 1);
	EXPECT_EQ(gfx_shader_compiler_transpile(&compiler,
						strv_cstr(t_gfx_shader_compiler_extra_semantic_source),
						GFX_SHADER_STAGE_VERTEX,
						GFX_SHADER_LANGUAGE_HLSL,
						&shader),
		  1);
	log_set_quiet(0, 0);
	gfx_shader_code_free(&shader);

	shader = (gfx_shader_code_t){0};
	EXPECT_EQ(gfx_shader_compiler_transpile(&compiler,
						strv_cstr(t_gfx_shader_compiler_extra_semantic_source),
						GFX_SHADER_STAGE_VERTEX,
						GFX_SHADER_LANGUAGE_SPIRV,
						&shader),
		  0);
	gfx_shader_code_free(&shader);

	gfx_shader_compiler_free(&compiler);
	END;
}

TEST(gfx_shader_compiler_transpile_extra_type)
{
	START;

	gfx_shader_compiler_t compiler = {0};
	EXPECT_PTR(gfx_shader_compiler_init(&compiler, ALLOC_STD), &compiler);
	gfx_shader_code_t shader = {0};

	log_set_quiet(0, 1);
	EXPECT_EQ(gfx_shader_compiler_transpile(&compiler,
						strv_cstr(t_gfx_shader_compiler_extra_type_source),
						GFX_SHADER_STAGE_VERTEX,
						GFX_SHADER_LANGUAGE_GLSL,
						&shader),
		  1);
	gfx_shader_code_free(&shader);

	shader = (gfx_shader_code_t){0};
	EXPECT_EQ(gfx_shader_compiler_transpile(&compiler,
						strv_cstr(t_gfx_shader_compiler_extra_type_source),
						GFX_SHADER_STAGE_VERTEX,
						GFX_SHADER_LANGUAGE_SPIRV,
						&shader),
		  1);
	log_set_quiet(0, 0);
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
	RUN(gfx_shader_compiler_transpile_expression_operator_spacing);
	RUN(gfx_shader_compiler_transpile_expression_parse_failures);
	RUN(gfx_shader_compiler_transpile_expression_multi_char_operators);
	RUN(gfx_shader_compiler_transpile_alloc_failure);
	RUN(gfx_shader_compiler_transpile_outputs);
	RUN(gfx_shader_compiler_transpile_uniform_block_outputs);
	RUN(gfx_shader_compiler_transpile_rejects_invalid_buffer_member);
	RUN(gfx_shader_compiler_transpile_vec3_mat4_outputs);
	RUN(gfx_shader_compiler_transpile_spirv_extra_input_types);
	RUN(gfx_shader_compiler_transpile_spirv_expression_failures);
	RUN(gfx_shader_compiler_transpile_rejects_overflow_buffer_slot);
	RUN(gfx_shader_compiler_ir_contracts);
	RUN(gfx_shader_compiler_transpile_extra_semantic);
	RUN(gfx_shader_compiler_transpile_extra_type);
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
