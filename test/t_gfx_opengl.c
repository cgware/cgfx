#include "gfx_driver.h"

#include "log.h"
#include "opengl.h"
#include "test.h"

typedef void (*t_gfx_opengl_symbol_t)(void);

static int t_gl_clear_color_calls;
static int t_gl_clear_calls;
static int t_gl_gen_framebuffers_calls;
static int t_gl_delete_framebuffers_calls;
static int t_gl_bind_framebuffer_calls;
static int t_gl_check_framebuffer_status_calls;
static int t_gl_framebuffer_texture_2d_calls;
static int t_gl_gen_textures_calls;
static int t_gl_delete_textures_calls;
static int t_gl_bind_texture_calls;
static int t_gl_tex_parameteri_calls;
static int t_gl_tex_image_2d_calls;
static int t_gl_viewport_calls;
static int t_gl_read_pixels_calls;
static int t_gl_create_shader_calls;
static int t_gl_shader_source_calls;
static int t_gl_compile_shader_calls;
static int t_gl_get_shader_iv_calls;
static int t_gl_get_shader_info_log_calls;
static int t_gl_delete_shader_calls;
static int t_gl_create_program_calls;
static int t_gl_get_program_info_log_calls;
static int t_gl_attach_shader_calls;
static int t_gl_link_program_calls;
static int t_gl_get_program_iv_calls;
static int t_gl_delete_program_calls;
static int t_gl_gen_buffers_calls;
static int t_gl_delete_buffers_calls;
static int t_gl_bind_buffer_calls;
static int t_gl_buffer_data_calls;
static int t_gl_use_program_calls;
static int t_gl_enable_vertex_attrib_array_calls;
static int t_gl_disable_vertex_attrib_array_calls;
static int t_gl_vertex_attrib_pointer_calls;
static int t_gl_draw_arrays_calls;
static int t_gl_draw_elements_calls;
static int t_surface_make_current_calls;
static int t_surface_clear_current_calls;
static int t_surface_present_calls;
static int t_gl_read_pixels_first_y;
static float t_gl_r;
static float t_gl_g;
static float t_gl_b;
static float t_gl_a;
static unsigned int t_gl_mask;
static unsigned int t_gl_framebuffer;
static unsigned int t_gl_texture;
static unsigned int t_gl_framebuffer_status;
static int t_gl_viewport_x;
static int t_gl_viewport_y;
static int t_gl_viewport_width;
static int t_gl_viewport_height;
static int t_gl_read_pixels_y;
static unsigned int t_gl_shader_type;
static unsigned int t_gl_program;
static unsigned int t_gl_buffer;
static unsigned int t_gl_buffer_target;
static unsigned int t_gl_draw_mode;
static unsigned int t_gl_draw_type;
static unsigned int t_gl_attrib_index;
static unsigned int t_gl_buffer_usage;
static int t_gl_buffer_data_size;
static int t_gl_draw_count;
static const void *t_gl_draw_indices;
static int t_gl_vertex_attrib_size;
static int t_gl_vertex_attrib_stride;
static float t_gl_buffer_first_x;
static float t_gl_buffer_last_y;
static void *t_gl_read_pixels_data;
static const void *t_gl_vertex_attrib_pointer;
static int t_gl_shader_status;
static unsigned int t_gl_error_ret;
static const unsigned char *t_gl_string_ret;
static int t_gl_shader_info_log_available;
static int t_gl_program_info_log_available;
static int t_gl_get_error_available;
static int t_gl_get_string_available;
static int t_gl_program_status;
static int t_gl_shader_fail_call;
static unsigned int t_gl_create_shader_ret;
static unsigned int t_gl_create_program_ret;
static unsigned int t_gl_gen_buffer_ret;
static int t_surface_make_current_ret;
static gfx_shader_compiler_t t_gfx_opengl_compiler;
static int t_gfx_opengl_compiler_initialized;
static gfx_surface_t t_gfx_opengl_surface;
static gfx_render_pass_t *t_gfx_opengl_active_render_pass;

typedef struct t_gfx_opengl_data_s {
	void *gl_lib;
	const gfx_target_t *target;
	const gfx_framebuffer_t *framebuffer;
	gfx_surface_t *surface;
} t_gfx_opengl_data_t;

typedef struct t_gfx_opengl_target_data_s {
	unsigned int texture;
} t_gfx_opengl_target_data_t;

typedef struct t_gfx_opengl_framebuffer_data_s {
	unsigned int framebuffer;
} t_gfx_opengl_framebuffer_data_t;

typedef struct t_gfx_opengl_buffer_data_s {
	unsigned int buffer;
	unsigned int target;
} t_gfx_opengl_buffer_data_t;

typedef struct t_gfx_opengl_shader_data_s {
	unsigned int shader;
} t_gfx_opengl_shader_data_t;

typedef struct t_gfx_opengl_pipeline_data_s {
	unsigned int program;
	const gfx_layout_t *input_layout;
	size_t input_layout_size;
	int stride;
} t_gfx_opengl_pipeline_data_t;

static const gfx_layout_t t_gfx_opengl_input_layout[] = {
	{.index = 0, .semantic = "POSITION", .count = 2, .type = GFX_VALUE_FLOAT32},
	{.index = 1, .semantic = "COLOR", .count = 4, .type = GFX_VALUE_FLOAT32},
};

static gfx_pipeline_config_t t_gfx_opengl_pipeline_config(gfx_shader_t vs, gfx_shader_t fs)
{
	return (gfx_pipeline_config_t){
		.render_pass	   = t_gfx_opengl_active_render_pass,
		.vs		   = vs,
		.fs		   = fs,
		.input_layout	   = t_gfx_opengl_input_layout,
		.input_layout_size = sizeof(t_gfx_opengl_input_layout),
	};
}

static void *t_gfx_opengl_alloc_fail(alloc_t *alloc, size_t size)
{
	(void)alloc;
	(void)size;
	return NULL;
}

static void *t_gfx_opengl_symbol(t_gfx_opengl_symbol_t fn)
{
	union {
		t_gfx_opengl_symbol_t fn;
		void *ptr;
	} symbol = {.fn = fn};

	return symbol.ptr;
}

static void t_glClearColor(float r, float g, float b, float a)
{
	t_gl_clear_color_calls++;
	t_gl_r = r;
	t_gl_g = g;
	t_gl_b = b;
	t_gl_a = a;
}

static void t_glClear(unsigned int mask)
{
	t_gl_clear_calls++;
	t_gl_mask = mask;
}

static void t_glGenFramebuffers(int count, unsigned int *framebuffers)
{
	t_gl_gen_framebuffers_calls++;
	if (count > 0) {
		*framebuffers = 22;
	}
}

static void t_glDeleteFramebuffers(int count, const unsigned int *framebuffers)
{
	(void)count;
	t_gl_delete_framebuffers_calls++;
	t_gl_framebuffer = *framebuffers;
}

static void t_glBindFramebuffer(unsigned int target, unsigned int framebuffer)
{
	(void)target;
	t_gl_bind_framebuffer_calls++;
	t_gl_framebuffer = framebuffer;
}

static unsigned int t_glCheckFramebufferStatus(unsigned int target)
{
	(void)target;
	t_gl_check_framebuffer_status_calls++;
	return t_gl_framebuffer_status;
}

static void t_glFramebufferTexture2D(unsigned int target, unsigned int attachment, unsigned int textarget, unsigned int texture, int level)
{
	(void)target;
	(void)attachment;
	(void)textarget;
	(void)level;
	t_gl_framebuffer_texture_2d_calls++;
	t_gl_texture = texture;
}

static void t_glGenTextures(int count, unsigned int *textures)
{
	t_gl_gen_textures_calls++;
	if (count > 0) {
		*textures = 11;
	}
}

static void t_glDeleteTextures(int count, const unsigned int *textures)
{
	(void)count;
	t_gl_delete_textures_calls++;
	t_gl_texture = *textures;
}

static void t_glBindTexture(unsigned int target, unsigned int texture)
{
	(void)target;
	t_gl_bind_texture_calls++;
	t_gl_texture = texture;
}

static void t_glTexParameteri(unsigned int target, unsigned int name, int value)
{
	(void)target;
	(void)name;
	(void)value;
	t_gl_tex_parameteri_calls++;
}

static void t_glTexImage2D(unsigned int target, int level, int internal_format, int width, int height, int border, unsigned int format,
			   unsigned int type, const void *data)
{
	(void)target;
	(void)level;
	(void)internal_format;
	(void)width;
	(void)height;
	(void)border;
	(void)format;
	(void)type;
	(void)data;
	t_gl_tex_image_2d_calls++;
}

static void t_glViewport(int x, int y, int width, int height)
{
	t_gl_viewport_calls++;
	t_gl_viewport_x	     = x;
	t_gl_viewport_y	     = y;
	t_gl_viewport_width  = width;
	t_gl_viewport_height = height;
}

static void t_glReadPixels(int x, int y, int width, int height, unsigned int format, unsigned int type, void *data)
{
	(void)x;
	(void)width;
	(void)height;
	(void)format;
	(void)type;
	if (t_gl_read_pixels_calls == 0) {
		t_gl_read_pixels_first_y = y;
	}
	t_gl_read_pixels_calls++;
	t_gl_read_pixels_y    = y;
	t_gl_read_pixels_data = data;
}

static unsigned int t_glCreateShader(unsigned int type)
{
	t_gl_create_shader_calls++;
	t_gl_shader_type = type;
	if (t_gl_create_shader_ret == 0) {
		return 0;
	}
	return t_gl_create_shader_ret + (unsigned int)t_gl_create_shader_calls;
}

static void t_glShaderSource(unsigned int shader, int count, const char **string, const int *length)
{
	(void)shader;
	(void)count;
	(void)string;
	(void)length;
	t_gl_shader_source_calls++;
}

static void t_glCompileShader(unsigned int shader)
{
	(void)shader;
	t_gl_compile_shader_calls++;
}

static void t_glGetShaderiv(unsigned int shader, unsigned int name, int *params)
{
	(void)shader;
	(void)name;
	t_gl_get_shader_iv_calls++;
	*params = t_gl_get_shader_iv_calls == t_gl_shader_fail_call ? 0 : t_gl_shader_status;
}

static void t_glGetShaderInfoLog(unsigned int shader, int max_length, int *length, char *info)
{
	(void)shader;
	t_gl_get_shader_info_log_calls++;
	if (max_length > 0 && info != NULL) {
		info[0] = 'x';
		info[1] = '\0';
	}
	if (length != NULL) {
		*length = 1;
	}
}

static void t_glDeleteShader(unsigned int shader)
{
	(void)shader;
	t_gl_delete_shader_calls++;
}

static unsigned int t_glCreateProgram(void)
{
	t_gl_create_program_calls++;
	return t_gl_create_program_ret;
}

static void t_glAttachShader(unsigned int program, unsigned int shader)
{
	(void)shader;
	t_gl_attach_shader_calls++;
	t_gl_program = program;
}

static void t_glLinkProgram(unsigned int program)
{
	t_gl_link_program_calls++;
	t_gl_program = program;
}

static void t_glGetProgramiv(unsigned int program, unsigned int name, int *params)
{
	(void)name;
	t_gl_get_program_iv_calls++;
	t_gl_program = program;
	*params	     = t_gl_program_status;
}

static void t_glGetProgramInfoLog(unsigned int program, int max_length, int *length, char *info)
{
	(void)program;
	t_gl_get_program_info_log_calls++;
	if (max_length > 0 && info != NULL) {
		info[0] = 'x';
		info[1] = '\0';
	}
	if (length != NULL) {
		*length = 1;
	}
}

static void t_glDeleteProgram(unsigned int program)
{
	t_gl_delete_program_calls++;
	t_gl_program = program;
}

static void t_glGenBuffers(int count, unsigned int *buffers)
{
	t_gl_gen_buffers_calls++;
	if (count > 0) {
		*buffers = t_gl_gen_buffer_ret;
	}
}

static void t_glDeleteBuffers(int count, const unsigned int *buffers)
{
	(void)count;
	t_gl_delete_buffers_calls++;
	t_gl_buffer = *buffers;
}

static void t_glBindBuffer(unsigned int target, unsigned int buffer)
{
	t_gl_bind_buffer_calls++;
	t_gl_buffer_target = target;
	t_gl_buffer	   = buffer;
}

static void t_glBufferData(unsigned int target, size_t size, const void *data, unsigned int usage)
{
	(void)target;
	const float *values = data;
	t_gl_buffer_data_calls++;
	t_gl_buffer_data_size = (int)size;
	t_gl_buffer_usage     = usage;
	t_gl_buffer_first_x   = values[0];
	t_gl_buffer_last_y    = values[13];
}

static void t_glUseProgram(unsigned int program)
{
	t_gl_use_program_calls++;
	t_gl_program = program;
}

static void t_glEnableVertexAttribArray(unsigned int index)
{
	t_gl_enable_vertex_attrib_array_calls++;
	t_gl_attrib_index = index;
}

static void t_glDisableVertexAttribArray(unsigned int index)
{
	t_gl_disable_vertex_attrib_array_calls++;
	t_gl_attrib_index = index;
}

static void t_glVertexAttribPointer(unsigned int index, int size, unsigned int type, unsigned char normalized, int stride,
				    const void *pointer)
{
	(void)type;
	(void)normalized;
	t_gl_vertex_attrib_pointer_calls++;
	t_gl_attrib_index	   = index;
	t_gl_vertex_attrib_size	   = size;
	t_gl_vertex_attrib_stride  = stride;
	t_gl_vertex_attrib_pointer = pointer;
}

static void t_glDrawArrays(unsigned int mode, int first, int count)
{
	(void)first;
	t_gl_draw_arrays_calls++;
	t_gl_draw_mode	= mode;
	t_gl_draw_count = count;
}

static void t_glDrawElements(unsigned int mode, int count, unsigned int type, const void *indices)
{
	t_gl_draw_elements_calls++;
	t_gl_draw_mode	  = mode;
	t_gl_draw_count	  = count;
	t_gl_draw_type	  = type;
	t_gl_draw_indices = indices;
}

static unsigned int t_glGetError(void)
{
	return t_gl_error_ret;
}

static const unsigned char *t_glGetString(unsigned int name)
{
	(void)name;
	return t_gl_string_ret;
}

static void *t_gfx_opengl_surface_symbol(strv_t name)
{
	if (strv_eq(name, STRV("glGenFramebuffers"))) {
		return t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glGenFramebuffers);
	}
	if (strv_eq(name, STRV("glDeleteFramebuffers"))) {
		return t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glDeleteFramebuffers);
	}
	if (strv_eq(name, STRV("glBindFramebuffer"))) {
		return t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glBindFramebuffer);
	}
	if (strv_eq(name, STRV("glCheckFramebufferStatus"))) {
		return t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glCheckFramebufferStatus);
	}
	if (strv_eq(name, STRV("glFramebufferTexture2D"))) {
		return t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glFramebufferTexture2D);
	}
	if (strv_eq(name, STRV("glCreateShader"))) {
		return t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glCreateShader);
	}
	if (strv_eq(name, STRV("glShaderSource"))) {
		return t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glShaderSource);
	}
	if (strv_eq(name, STRV("glCompileShader"))) {
		return t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glCompileShader);
	}
	if (strv_eq(name, STRV("glGetShaderiv"))) {
		return t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glGetShaderiv);
	}
	if (strv_eq(name, STRV("glDeleteShader"))) {
		return t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glDeleteShader);
	}
	if (strv_eq(name, STRV("glCreateProgram"))) {
		return t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glCreateProgram);
	}
	if (strv_eq(name, STRV("glAttachShader"))) {
		return t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glAttachShader);
	}
	if (strv_eq(name, STRV("glLinkProgram"))) {
		return t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glLinkProgram);
	}
	if (strv_eq(name, STRV("glGetProgramiv"))) {
		return t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glGetProgramiv);
	}
	if (strv_eq(name, STRV("glDeleteProgram"))) {
		return t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glDeleteProgram);
	}
	if (strv_eq(name, STRV("glGenBuffers"))) {
		return t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glGenBuffers);
	}
	if (strv_eq(name, STRV("glDeleteBuffers"))) {
		return t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glDeleteBuffers);
	}
	if (strv_eq(name, STRV("glBindBuffer"))) {
		return t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glBindBuffer);
	}
	if (strv_eq(name, STRV("glBufferData"))) {
		return t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glBufferData);
	}
	if (strv_eq(name, STRV("glUseProgram"))) {
		return t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glUseProgram);
	}
	if (strv_eq(name, STRV("glEnableVertexAttribArray"))) {
		return t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glEnableVertexAttribArray);
	}
	if (strv_eq(name, STRV("glDisableVertexAttribArray"))) {
		return t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glDisableVertexAttribArray);
	}
	if (strv_eq(name, STRV("glVertexAttribPointer"))) {
		return t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glVertexAttribPointer);
	}
	if (strv_eq(name, STRV("glDrawArrays"))) {
		return t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glDrawArrays);
	}
	if (strv_eq(name, STRV("glDrawElements"))) {
		return t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glDrawElements);
	}
	if (strv_eq(name, STRV("glGetError"))) {
		return t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glGetError);
	}
	if (strv_eq(name, STRV("glGetString"))) {
		return t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glGetString);
	}
	return NULL;
}

static int t_gfx_opengl_surface_proc(gfx_surface_t *surface, strv_t name, void **proc)
{
	(void)surface;
	*proc = t_gfx_opengl_surface_symbol(name);
	return *proc == NULL;
}

static int t_gfx_opengl_surface_make_current(gfx_surface_t *surface)
{
	(void)surface;
	t_surface_make_current_calls++;
	return !t_surface_make_current_ret;
}

static int t_gfx_opengl_surface_clear_current(gfx_surface_t *surface)
{
	(void)surface;
	t_surface_clear_current_calls++;
	return 0;
}

static int t_gfx_opengl_surface_present(gfx_surface_t *surface, gfx_present_mode_t present_mode)
{
	(void)surface;
	(void)present_mode;
	t_surface_present_calls++;
	return 0;
}

static int t_gfx_opengl_surface_present_mode_failure(gfx_surface_t *surface, gfx_present_mode_t requested, gfx_present_mode_t *actual)
{
	(void)surface;
	(void)requested;
	(void)actual;
	return 1;
}

static const gfx_surface_ops_t t_gfx_opengl_surface_ops = {
	.proc	       = t_gfx_opengl_surface_proc,
	.make_current  = t_gfx_opengl_surface_make_current,
	.clear_current = t_gfx_opengl_surface_clear_current,
	.present       = t_gfx_opengl_surface_present,
};

static void t_gfx_opengl_reset(void)
{
	t_gl_clear_color_calls		       = 0;
	t_gl_clear_calls		       = 0;
	t_gl_gen_framebuffers_calls	       = 0;
	t_gl_delete_framebuffers_calls	       = 0;
	t_gl_bind_framebuffer_calls	       = 0;
	t_gl_check_framebuffer_status_calls    = 0;
	t_gl_framebuffer_texture_2d_calls      = 0;
	t_gl_gen_textures_calls		       = 0;
	t_gl_delete_textures_calls	       = 0;
	t_gl_bind_texture_calls		       = 0;
	t_gl_tex_parameteri_calls	       = 0;
	t_gl_tex_image_2d_calls		       = 0;
	t_gl_viewport_calls		       = 0;
	t_gl_read_pixels_calls		       = 0;
	t_gl_create_shader_calls	       = 0;
	t_gl_shader_source_calls	       = 0;
	t_gl_compile_shader_calls	       = 0;
	t_gl_get_shader_iv_calls	       = 0;
	t_gl_get_shader_info_log_calls	       = 0;
	t_gl_delete_shader_calls	       = 0;
	t_gl_create_program_calls	       = 0;
	t_gl_get_program_info_log_calls	       = 0;
	t_gl_attach_shader_calls	       = 0;
	t_gl_link_program_calls		       = 0;
	t_gl_get_program_iv_calls	       = 0;
	t_gl_delete_program_calls	       = 0;
	t_gl_gen_buffers_calls		       = 0;
	t_gl_delete_buffers_calls	       = 0;
	t_gl_bind_buffer_calls		       = 0;
	t_gl_buffer_data_calls		       = 0;
	t_gl_use_program_calls		       = 0;
	t_gl_enable_vertex_attrib_array_calls  = 0;
	t_gl_disable_vertex_attrib_array_calls = 0;
	t_gl_vertex_attrib_pointer_calls       = 0;
	t_gl_draw_arrays_calls		       = 0;
	t_gl_draw_elements_calls	       = 0;
	t_surface_make_current_calls	       = 0;
	t_surface_clear_current_calls	       = 0;
	t_surface_present_calls		       = 0;
	t_gl_r				       = 0.0f;
	t_gl_g				       = 0.0f;
	t_gl_b				       = 0.0f;
	t_gl_a				       = 0.0f;
	t_gl_mask			       = 0;
	t_gl_framebuffer		       = 0;
	t_gl_texture			       = 0;
	t_gl_framebuffer_status		       = 0x8CD5;
	t_gl_viewport_x			       = 0;
	t_gl_viewport_y			       = 0;
	t_gl_viewport_width		       = 0;
	t_gl_viewport_height		       = 0;
	t_gl_read_pixels_first_y	       = 0;
	t_gl_read_pixels_y		       = 0;
	t_gl_shader_type		       = 0;
	t_gl_program			       = 0;
	t_gl_buffer			       = 0;
	t_gl_buffer_target		       = 0;
	t_gl_draw_mode			       = 0;
	t_gl_draw_type			       = 0;
	t_gl_attrib_index		       = 0;
	t_gl_buffer_usage		       = 0;
	t_gl_buffer_data_size		       = 0;
	t_gl_draw_count			       = 0;
	t_gl_vertex_attrib_size		       = 0;
	t_gl_vertex_attrib_stride	       = 0;
	t_gl_buffer_first_x		       = 0.0f;
	t_gl_buffer_last_y		       = 0.0f;
	t_gl_read_pixels_data		       = NULL;
	t_gl_vertex_attrib_pointer	       = NULL;
	t_gl_draw_indices		       = NULL;
	t_gl_shader_status		       = 1;
	t_gl_error_ret			       = 0;
	t_gl_string_ret			       = (const unsigned char *)"test";
	t_gl_shader_info_log_available	       = 1;
	t_gl_program_info_log_available	       = 0;
	t_gl_get_error_available	       = 1;
	t_gl_get_string_available	       = 1;
	t_gl_program_status		       = 1;
	t_gl_shader_fail_call		       = 0;
	t_gl_create_shader_ret		       = 30;
	t_gl_create_program_ret		       = 40;
	t_gl_gen_buffer_ret		       = 55;
	t_surface_make_current_ret	       = 1;

	t_gfx_opengl_surface = (gfx_surface_t){
		.api	= GFX_API_OPENGL,
		.handle = 0x4321,
		.ops	= &t_gfx_opengl_surface_ops,
	};
}

static gfx_driver_t *t_gfx_opengl_driver(void)
{
	return gfx_driver_find(STRV("opengl"));
}

static void t_gfx_opengl_gl_symbols(proc_t *proc, strv_t lib)
{
	proc_setdlsym(proc, lib, STRV("glClearColor"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glClearColor));
	proc_setdlsym(proc, lib, STRV("glClear"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glClear));
	proc_setdlsym(proc, lib, STRV("glGenFramebuffers"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glGenFramebuffers));
	proc_setdlsym(proc, lib, STRV("glDeleteFramebuffers"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glDeleteFramebuffers));
	proc_setdlsym(proc, lib, STRV("glBindFramebuffer"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glBindFramebuffer));
	proc_setdlsym(proc, lib, STRV("glCheckFramebufferStatus"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glCheckFramebufferStatus));
	proc_setdlsym(proc, lib, STRV("glFramebufferTexture2D"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glFramebufferTexture2D));
	proc_setdlsym(proc, lib, STRV("glGenTextures"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glGenTextures));
	proc_setdlsym(proc, lib, STRV("glDeleteTextures"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glDeleteTextures));
	proc_setdlsym(proc, lib, STRV("glBindTexture"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glBindTexture));
	proc_setdlsym(proc, lib, STRV("glTexParameteri"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glTexParameteri));
	proc_setdlsym(proc, lib, STRV("glTexImage2D"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glTexImage2D));
	proc_setdlsym(proc, lib, STRV("glViewport"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glViewport));
	proc_setdlsym(proc, lib, STRV("glReadPixels"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glReadPixels));
	proc_setdlsym(proc, lib, STRV("glCreateShader"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glCreateShader));
	proc_setdlsym(proc, lib, STRV("glShaderSource"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glShaderSource));
	proc_setdlsym(proc, lib, STRV("glCompileShader"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glCompileShader));
	proc_setdlsym(proc, lib, STRV("glGetShaderiv"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glGetShaderiv));
	if (t_gl_shader_info_log_available) {
		proc_setdlsym(proc, lib, STRV("glGetShaderInfoLog"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glGetShaderInfoLog));
	}
	proc_setdlsym(proc, lib, STRV("glDeleteShader"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glDeleteShader));
	proc_setdlsym(proc, lib, STRV("glCreateProgram"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glCreateProgram));
	proc_setdlsym(proc, lib, STRV("glAttachShader"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glAttachShader));
	proc_setdlsym(proc, lib, STRV("glLinkProgram"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glLinkProgram));
	proc_setdlsym(proc, lib, STRV("glGetProgramiv"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glGetProgramiv));
	if (t_gl_program_info_log_available) {
		proc_setdlsym(proc, lib, STRV("glGetProgramInfoLog"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glGetProgramInfoLog));
	}
	proc_setdlsym(proc, lib, STRV("glDeleteProgram"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glDeleteProgram));
	proc_setdlsym(proc, lib, STRV("glGenBuffers"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glGenBuffers));
	proc_setdlsym(proc, lib, STRV("glDeleteBuffers"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glDeleteBuffers));
	proc_setdlsym(proc, lib, STRV("glBindBuffer"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glBindBuffer));
	proc_setdlsym(proc, lib, STRV("glBufferData"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glBufferData));
	proc_setdlsym(proc, lib, STRV("glUseProgram"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glUseProgram));
	proc_setdlsym(
		proc, lib, STRV("glEnableVertexAttribArray"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glEnableVertexAttribArray));
	proc_setdlsym(
		proc, lib, STRV("glDisableVertexAttribArray"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glDisableVertexAttribArray));
	proc_setdlsym(proc, lib, STRV("glVertexAttribPointer"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glVertexAttribPointer));
	proc_setdlsym(proc, lib, STRV("glDrawArrays"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glDrawArrays));
	proc_setdlsym(proc, lib, STRV("glDrawElements"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glDrawElements));
	if (t_gl_get_error_available) {
		proc_setdlsym(proc, lib, STRV("glGetError"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glGetError));
	}
	if (t_gl_get_string_available) {
		proc_setdlsym(proc, lib, STRV("glGetString"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glGetString));
	}
}

static void t_gfx_opengl_core_symbols(proc_t *proc, strv_t lib)
{
	proc_setdlsym(proc, lib, STRV("glClearColor"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glClearColor));
	proc_setdlsym(proc, lib, STRV("glClear"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glClear));
	proc_setdlsym(proc, lib, STRV("glGenTextures"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glGenTextures));
	proc_setdlsym(proc, lib, STRV("glDeleteTextures"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glDeleteTextures));
	proc_setdlsym(proc, lib, STRV("glBindTexture"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glBindTexture));
	proc_setdlsym(proc, lib, STRV("glTexParameteri"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glTexParameteri));
	proc_setdlsym(proc, lib, STRV("glTexImage2D"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glTexImage2D));
	proc_setdlsym(proc, lib, STRV("glViewport"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glViewport));
	proc_setdlsym(proc, lib, STRV("glReadPixels"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glReadPixels));
}

static void t_gfx_opengl_symbols(proc_t *proc)
{
	t_gfx_opengl_gl_symbols(proc, STRV("libGL.so.1"));
}

static void t_gfx_opengl_fallback_symbols(proc_t *proc)
{
	t_gfx_opengl_gl_symbols(proc, STRV("libGL.so"));
}

static void t_gfx_opengl_glvnd_symbols(proc_t *proc)
{
	t_gfx_opengl_gl_symbols(proc, STRV("libOpenGL.so.0"));
}

static int t_gfx_opengl_init_gfx_configured(gfx_t *gfx, proc_t *proc)
{
	proc_init(proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_symbols(proc);
	gfx_driver_t *drv = t_gfx_opengl_driver();
	return gfx_init(gfx, drv, &(gfx_config_t){0}, proc, ALLOC_STD) != gfx;
}

static int t_gfx_opengl_init_gfx(gfx_t *gfx, proc_t *proc)
{
	t_gfx_opengl_reset();
	return t_gfx_opengl_init_gfx_configured(gfx, proc);
}

static gfx_target_t *t_gfx_opengl_init_swapchain_target(gfx_t *gfx, gfx_swapchain_t *swapchain, gfx_target_t *target, u16 width, u16 height)
{
	gfx_swapchain_config_t swapchain_config = {
		.format	 = GFX_FORMAT_RGBA8,
		.surface = &t_gfx_opengl_surface,
		.width	 = width,
		.height	 = height,
	};
	if (gfx_swapchain_init(swapchain, gfx, &swapchain_config) != swapchain) {
		return NULL;
	}
	if (gfx_target_init_swapchain(target, swapchain) != target) {
		gfx_swapchain_free(swapchain);
		return NULL;
	}
	return target;
}

static int t_gfx_opengl_compiler_init(void)
{
	if (t_gfx_opengl_compiler_initialized) {
		return 0;
	}
	if (gfx_shader_compiler_init(&t_gfx_opengl_compiler, ALLOC_STD) == NULL) {
		return 1;
	}
	t_gfx_opengl_compiler_initialized = 1;
	return 0;
}

static void t_gfx_opengl_compiler_free(void)
{
	if (!t_gfx_opengl_compiler_initialized) {
		return;
	}
	gfx_shader_compiler_free(&t_gfx_opengl_compiler);
	t_gfx_opengl_compiler		  = (gfx_shader_compiler_t){0};
	t_gfx_opengl_compiler_initialized = 0;
}

static int t_gfx_opengl_shader_stage(gfx_t *gfx, gfx_shader_t *shader, gfx_shader_stage_t stage)
{
	if (!t_gfx_opengl_compiler_initialized) {
		return 1;
	}
	const char *triangle_src = "vs_in 0 VertexIn {\n"
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
				   "\toutput.position = vec4f(input.position.x, input.position.y, 0.0f, 1.0f);\n"
				   "\toutput.color = input.color;\n"
				   "\treturn output;\n"
				   "}\n"
				   "FragmentOut fragment(FragmentIn input) {\n"
				   "\tFragmentOut output;\n"
				   "\toutput.color = input.color;\n"
				   "\treturn output;\n"
				   "}\n";

	gfx_shader_config_t config = {
		.compiler = &t_gfx_opengl_compiler,
		.source	  = strv_cstr(triangle_src),
		.stage	  = stage,
	};
	return gfx_shader_init(shader, gfx, &config) != shader;
}

static int t_gfx_opengl_shader(gfx_t *gfx, gfx_shader_t *shader)
{
	return t_gfx_opengl_shader_stage(gfx, shader, GFX_SHADER_STAGE_VERTEX);
}

static gfx_target_t t_gfx_opengl_memory_target(u8 *pixels, u16 width, u16 height, size_t stride)
{
	return (gfx_target_t){
		.type	= GFX_TARGET_MEMORY,
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= width,
		.height = height,
		.stride = stride,
	};
}

TEST(gfx_opengl_driver_is_registered)
{
	START;

	EXPECT_NOT_NULL(t_gfx_opengl_driver());

	END;
}

TEST(gfx_opengl_init_null_gfx)
{
	START;

	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_EQ(drv->init(NULL, &(gfx_config_t){0}), 1);

	END;
}

TEST(gfx_opengl_init_null_config)
{
	START;

	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_EQ(drv->init(&gfx, NULL), 1);

	END;
}

TEST(gfx_opengl_init_null_proc)
{
	START;

	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_EQ(drv->init(&gfx, &(gfx_config_t){0}), 1);

	END;
}

TEST(gfx_opengl_init_alloc_failure)
{
	START;

	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_NULL(gfx_init(&gfx, drv, &(gfx_config_t){0}, &proc, (alloc_t){.alloc = t_gfx_opengl_alloc_fail}));

	proc_free(&proc);
	END;
}

TEST(gfx_opengl_init_missing_library)
{
	START;

	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);

	log_set_quiet(0, 1);
	EXPECT_NULL(gfx_init(&gfx, drv, &(gfx_config_t){0}, &proc, ALLOC_STD));
	log_set_quiet(0, 0);

	proc_free(&proc);
	END;
}

TEST(gfx_opengl_init_fallback_library)
{
	START;

	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_fallback_symbols(&proc);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_PTR(gfx_init(&gfx, drv, &(gfx_config_t){0}, &proc, ALLOC_STD), &gfx);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_init_glvnd_libraries)
{
	START;

	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_glvnd_symbols(&proc);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_PTR(gfx_init(&gfx, drv, &(gfx_config_t){0}, &proc, ALLOC_STD), &gfx);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_init_windows_library_name)
{
	START;

	t_gfx_opengl_reset();
	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_core_symbols(&proc, STRV("opengl32.dll"));
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_PTR(gfx_init(&gfx, drv, &(gfx_config_t){.surface = &t_gfx_opengl_surface}, &proc, ALLOC_STD), &gfx);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_init_invalid_surface)
{
	START;

	t_gfx_opengl_reset();
	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_symbols(&proc);
	gfx_surface_t surface = {
		.api	= (gfx_api_t)99,
		.handle = 0x1234,
		.ops	= &t_gfx_opengl_surface_ops,
	};
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_NULL(gfx_init(&gfx, drv, &(gfx_config_t){.surface = &surface}, &proc, ALLOC_STD));

	proc_free(&proc);
	END;
}

TEST(gfx_opengl_init_surface_make_current_failure)
{
	START;

	t_gfx_opengl_reset();
	t_surface_make_current_ret = 0;
	proc_t proc		   = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_symbols(&proc);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);

	log_set_quiet(0, 1);
	EXPECT_NULL(gfx_init(&gfx, drv, &(gfx_config_t){.surface = &t_gfx_opengl_surface}, &proc, ALLOC_STD));
	log_set_quiet(0, 0);

	proc_free(&proc);
	END;
}

TEST(gfx_opengl_init_surface_missing_clear_current)
{
	START;

	t_gfx_opengl_reset();
	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_symbols(&proc);
	gfx_surface_ops_t ops = t_gfx_opengl_surface_ops;
	ops.clear_current     = NULL;
	gfx_surface_t surface = {
		.api	= GFX_API_OPENGL,
		.handle = 0x1234,
		.ops	= &ops,
	};
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);

	log_set_quiet(0, 1);
	EXPECT_NULL(gfx_init(&gfx, drv, &(gfx_config_t){.surface = &surface}, &proc, ALLOC_STD));
	log_set_quiet(0, 0);

	proc_free(&proc);
	END;
}

TEST(gfx_opengl_init_missing_clear_symbol)
{
	START;

	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	proc_setdlsym(&proc, STRV("libGL.so.1"), STRV("glClearColor"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glClearColor));
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);

	log_set_quiet(0, 1);
	EXPECT_NULL(gfx_init(&gfx, drv, &(gfx_config_t){0}, &proc, ALLOC_STD));
	log_set_quiet(0, 0);

	proc_free(&proc);
	END;
}

TEST(gfx_opengl_init_success)
{
	START;

	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_symbols(&proc);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_PTR(gfx_init(&gfx, drv, &(gfx_config_t){0}, &proc, ALLOC_STD), &gfx);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_free_does_not_use_init_surface)
{
	START;

	t_gfx_opengl_reset();
	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_core_symbols(&proc, STRV("opengl32.dll"));
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);
	EXPECT_PTR(gfx_init(&gfx, drv, &(gfx_config_t){.surface = &t_gfx_opengl_surface}, &proc, ALLOC_STD), &gfx);

	t_gfx_opengl_surface.ops = (const gfx_surface_ops_t *)(uintptr_t)0xFFFFFFFFFFFFFFEF;
	gfx_free(&gfx);

	proc_free(&proc);
	END;
}

TEST(gfx_opengl_free_null_data)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_opengl_driver(),
	};
	EXPECT_NOT_NULL(gfx.drv);

	EXPECT_EQ(gfx.drv->free(&gfx), 1);

	END;
}
TEST(gfx_opengl_clear_color_null_data)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_opengl_driver(),
	};
	EXPECT_NOT_NULL(gfx.drv);

	EXPECT_EQ(gfx.drv->framebuffer_pass_begin(NULL, NULL), 1);

	END;
}

TEST(gfx_opengl_viewport_null_data)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_opengl_driver(),
	};
	EXPECT_NOT_NULL(gfx.drv);

	EXPECT_EQ(gfx.drv->framebuffer_pass_begin(NULL, NULL), 1);

	END;
}

TEST(gfx_opengl_draw_null_data)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_opengl_driver(),
	};
	EXPECT_NOT_NULL(gfx.drv);

	EXPECT_EQ(gfx.drv->draw(NULL, 3, 0), 1);

	END;
}

TEST(gfx_opengl_begin_null_frame)
{
	START;

	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_EQ(drv->framebuffer_pass_begin(NULL, NULL), 1);

	END;
}

TEST(gfx_opengl_buffer_bind_null_frame)
{
	START;

	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_EQ(drv->buffer_bind(NULL, NULL), 1);

	END;
}

TEST(gfx_opengl_pipeline_bind_null_frame)
{
	START;

	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_EQ(drv->pipeline_bind(NULL, NULL), 1);

	END;
}

TEST(gfx_opengl_init_missing_draw_symbol)
{
	START;

	t_gfx_opengl_reset();
	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_core_symbols(&proc, STRV("libGL.so.1"));
	proc_setdlsym(
		&proc, STRV("libGL.so.1"), STRV("glGenFramebuffers"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glGenFramebuffers));
	proc_setdlsym(&proc,
		      STRV("libGL.so.1"),
		      STRV("glDeleteFramebuffers"),
		      t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glDeleteFramebuffers));
	proc_setdlsym(
		&proc, STRV("libGL.so.1"), STRV("glBindFramebuffer"), t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glBindFramebuffer));
	proc_setdlsym(&proc,
		      STRV("libGL.so.1"),
		      STRV("glCheckFramebufferStatus"),
		      t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glCheckFramebufferStatus));
	proc_setdlsym(&proc,
		      STRV("libGL.so.1"),
		      STRV("glFramebufferTexture2D"),
		      t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glFramebufferTexture2D));
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);

	log_set_quiet(0, 1);
	EXPECT_NULL(gfx_init(&gfx, drv, &(gfx_config_t){0}, &proc, ALLOC_STD));
	log_set_quiet(0, 0);

	proc_free(&proc);
	END;
}

TEST(gfx_opengl_buffer_free_null_data)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	gfx_buffer_t buffer = {.gfx = &gfx};

	gfx_buffer_free(&buffer);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_buffer_init_null_config)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	gfx_buffer_t buffer = {0};

	EXPECT_NULL(gfx_buffer_init(&buffer, &gfx, NULL));

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_buffer_init_alloc_failure)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	gfx_buffer_t buffer = {0};
	gfx.alloc	    = (alloc_t){.alloc = t_gfx_opengl_alloc_fail, .realloc = alloc_realloc_std, .free = alloc_free_std};

	log_set_quiet(0, 1);
	EXPECT_NULL(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX, .usage = GFX_BUFFER_USAGE_DYNAMIC}));
	log_set_quiet(0, 0);

	gfx.alloc = ALLOC_STD;
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_buffer_init_create_buffer_failure)
{
	START;

	t_gfx_opengl_reset();
	t_gl_gen_buffer_ret = 0;
	proc_t proc	    = {0};
	gfx_t gfx	    = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx_configured(&gfx, &proc), 0);
	gfx_buffer_t buffer = {0};

	EXPECT_NULL(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX, .usage = GFX_BUFFER_USAGE_DYNAMIC}));
	EXPECT_NULL(buffer.data);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_buffer_init_index_buffer)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	gfx_buffer_t buffer = {0};

	EXPECT_PTR(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_INDEX, .usage = GFX_BUFFER_USAGE_DYNAMIC}),
		   &buffer);
	EXPECT_EQ(t_gl_gen_buffers_calls, 1);

	gfx_buffer_free(&buffer);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_buffer_init_unsupported_type)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	gfx_buffer_t buffer = {0};

	log_set_quiet(0, 1);
	EXPECT_NULL(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_UNKNOWN, .usage = GFX_BUFFER_USAGE_DYNAMIC}));
	log_set_quiet(0, 0);
	EXPECT_NULL(buffer.data);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_buffer_init_rejects_invalid_direct)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	gfx_buffer_t buffer = {.gfx = &gfx};

	EXPECT_EQ(gfx.drv->buffer_init(NULL, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX, .usage = GFX_BUFFER_USAGE_DYNAMIC}), 1);
	log_set_quiet(0, 1);
	EXPECT_EQ(gfx.drv->buffer_init(&buffer, &(gfx_buffer_config_t){.type = GFX_BUFFER_UNKNOWN, .usage = GFX_BUFFER_USAGE_DYNAMIC}), 1);
	log_set_quiet(0, 0);
	EXPECT_NULL(buffer.data);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_buffer_init_static_uploads_data)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	gfx_vertex_2d_t vertices[3] = {
		{.x = 1.0f},
		{.x = 0.0f, .y = 0.0f, .r = 0.0f, .g = 0.0f, .b = 0.0f, .a = 0.0f},
		{.y = 2.0f},
	};
	gfx_buffer_t buffer = {0};

	gfx_buffer_config_t buffer_config = {
		.type  = GFX_BUFFER_VERTEX,
		.usage = GFX_BUFFER_USAGE_STATIC,
		.size  = sizeof(vertices),
		.data  = vertices,
	};

	EXPECT_PTR(gfx_buffer_init(&buffer, &gfx, &buffer_config), &buffer);
	EXPECT_EQ(t_gl_bind_buffer_calls, 2);
	EXPECT_EQ(t_gl_buffer_data_calls, 1);
	EXPECT_EQ(t_gl_buffer_data_size, sizeof(vertices));
	EXPECT_EQ(t_gl_buffer_usage, GL_STATIC_DRAW);
	EXPECT_EQ(t_gl_buffer_first_x, 1.0f);
	EXPECT_EQ(t_gl_buffer_last_y, 2.0f);

	gfx_buffer_free(&buffer);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_buffer_free_make_current_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	gfx_buffer_t buffer = {0};
	EXPECT_PTR(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX, .usage = GFX_BUFFER_USAGE_DYNAMIC}),
		   &buffer);
	((t_gfx_opengl_data_t *)gfx.data)->surface = &t_gfx_opengl_surface;
	t_surface_make_current_ret		   = 0;

	log_set_quiet(0, 1);
	gfx.drv->buffer_free(&buffer);
	log_set_quiet(0, 0);
	EXPECT_NOT_NULL(buffer.data);
	EXPECT_EQ(t_gl_delete_buffers_calls, 0);

	t_surface_make_current_ret		   = 1;
	((t_gfx_opengl_data_t *)gfx.data)->surface = NULL;
	gfx_buffer_free(&buffer);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_buffer_init_make_current_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	((t_gfx_opengl_data_t *)gfx.data)->surface = &t_gfx_opengl_surface;
	t_surface_make_current_ret		   = 0;
	gfx_buffer_t buffer			   = {0};

	log_set_quiet(0, 1);
	EXPECT_NULL(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX, .usage = GFX_BUFFER_USAGE_DYNAMIC}));
	log_set_quiet(0, 0);

	t_surface_make_current_ret = 1;
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_buffer_init_missing_make_current_callback)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	gfx_surface_t surface = {
		.api	= GFX_API_OPENGL,
		.handle = 0x4321,
		.ops	= NULL,
	};
	((t_gfx_opengl_data_t *)gfx.data)->surface = &surface;
	gfx_buffer_t buffer			   = {0};

	log_set_quiet(0, 1);
	EXPECT_NULL(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX, .usage = GFX_BUFFER_USAGE_DYNAMIC}));
	log_set_quiet(0, 0);
	EXPECT_EQ(t_surface_make_current_calls, 0);

	((t_gfx_opengl_data_t *)gfx.data)->surface = NULL;
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_buffer_set_data_null_data)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_opengl_driver(),
	};
	EXPECT_NOT_NULL(gfx.drv);
	gfx_vertex_2d_t vertices[3] = {0};
	gfx_buffer_t buffer	    = {.gfx = &gfx};

	EXPECT_EQ(gfx_buffer_set_data(&buffer, vertices, sizeof(vertices)), 1);

	END;
}

TEST(gfx_opengl_buffer_set_data_uploads_vertices)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	gfx_buffer_t buffer = {0};
	EXPECT_PTR(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX, .usage = GFX_BUFFER_USAGE_DYNAMIC}),
		   &buffer);
	gfx_vertex_2d_t vertices[3] = {
		{.x = 1.0f},
		{.x = 0.0f, .y = 0.0f, .r = 0.0f, .g = 0.0f, .b = 0.0f, .a = 0.0f},
		{.y = 2.0f},
	};

	EXPECT_EQ(gfx_buffer_set_data(&buffer, vertices, sizeof(vertices)), 0);
	EXPECT_EQ(t_gl_bind_buffer_calls, 2);
	EXPECT_EQ(t_gl_buffer_data_calls, 1);
	EXPECT_EQ(t_gl_buffer_data_size, sizeof(vertices));
	EXPECT_EQ(t_gl_buffer_first_x, 1.0f);
	EXPECT_EQ(t_gl_buffer_last_y, 2.0f);

	gfx_buffer_free(&buffer);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_buffer_set_data_make_current_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	gfx_buffer_t buffer = {0};
	EXPECT_PTR(gfx_buffer_init(&buffer, &gfx, &(gfx_buffer_config_t){.type = GFX_BUFFER_VERTEX, .usage = GFX_BUFFER_USAGE_DYNAMIC}),
		   &buffer);
	((t_gfx_opengl_data_t *)gfx.data)->surface = &t_gfx_opengl_surface;
	t_surface_make_current_ret		   = 0;
	gfx_vertex_2d_t vertices[3]		   = {0};

	log_set_quiet(0, 1);
	EXPECT_EQ(gfx_buffer_set_data(&buffer, vertices, sizeof(vertices)), 1);
	log_set_quiet(0, 0);

	t_surface_make_current_ret = 1;
	gfx_buffer_free(&buffer);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_shader_init_create_shader_failure)
{
	START;

	t_gfx_opengl_reset();
	t_gl_create_shader_ret = 0;
	proc_t proc	       = {0};
	gfx_t gfx	       = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx_configured(&gfx, &proc), 0);
	gfx_shader_t shader = {0};

	log_set_quiet(0, 1);
	EXPECT_EQ(t_gfx_opengl_shader(&gfx, &shader), 1);
	log_set_quiet(0, 0);

	t_surface_make_current_ret = 1;
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_shader_init_without_diagnostic_symbols)
{
	START;

	t_gfx_opengl_reset();
	t_gl_get_error_available  = 0;
	t_gl_get_string_available = 0;
	t_gl_create_shader_ret	  = 0;
	proc_t proc		  = {0};
	gfx_t gfx		  = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx_configured(&gfx, &proc), 0);
	gfx_shader_t shader = {0};

	log_set_quiet(0, 1);
	EXPECT_EQ(t_gfx_opengl_shader(&gfx, &shader), 1);
	log_set_quiet(0, 0);

	t_surface_make_current_ret = 1;
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_shader_free_null_data)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_opengl_driver(),
	};
	EXPECT_NOT_NULL(gfx.drv);
	gfx_shader_t shader = {.gfx = &gfx};

	gfx_shader_free(&shader);

	END;
}

TEST(gfx_opengl_shader_free_make_current_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	t_gfx_opengl_shader_data_t driver_shader = {.shader = 88};

	gfx_shader_t shader = {
		.gfx  = &gfx,
		.data = &driver_shader,
	};
	((t_gfx_opengl_data_t *)gfx.data)->surface = &t_gfx_opengl_surface;
	t_surface_make_current_ret		   = 0;

	log_set_quiet(0, 1);
	gfx.drv->shader_free(&shader);
	log_set_quiet(0, 0);
	EXPECT_PTR(shader.data, &driver_shader);
	EXPECT_EQ(t_gl_delete_shader_calls, 0);

	t_surface_make_current_ret		   = 1;
	((t_gfx_opengl_data_t *)gfx.data)->surface = NULL;
	shader.data				   = NULL;
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_shader_init_null_config)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	gfx_shader_t shader = {0};

	log_set_quiet(0, 1);
	EXPECT_NULL(gfx_shader_init(&shader, &gfx, NULL));
	log_set_quiet(0, 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_shader_init_null_data)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_opengl_driver(),
	};
	EXPECT_NOT_NULL(gfx.drv);
	gfx_shader_t shader = {0};

	log_set_quiet(0, 1);
	EXPECT_NULL(gfx_shader_init(&shader, &gfx, &(gfx_shader_config_t){0}));
	log_set_quiet(0, 0);

	END;
}

TEST(gfx_opengl_shader_init_make_current_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	((t_gfx_opengl_data_t *)gfx.data)->surface = &t_gfx_opengl_surface;
	t_surface_make_current_ret		   = 0;
	gfx_shader_t shader			   = {0};

	log_set_quiet(0, 1);
	EXPECT_EQ(t_gfx_opengl_shader_stage(&gfx, &shader, GFX_SHADER_STAGE_VERTEX), 1);
	log_set_quiet(0, 0);

	t_surface_make_current_ret = 1;
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_shader_init_null_gl_version)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	t_gl_string_ret	    = NULL;
	gfx_shader_t shader = {0};

	log_set_quiet(0, 1);
	EXPECT_EQ(t_gfx_opengl_shader(&gfx, &shader), 1);
	log_set_quiet(0, 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_shader_init_create_shader_reports_gl_error)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	t_gl_error_ret	    = 0x0500;
	gfx_shader_t shader = {0};

	log_set_quiet(0, 1);
	EXPECT_EQ(t_gfx_opengl_shader(&gfx, &shader), 0);
	log_set_quiet(0, 0);

	gfx_shader_free(&shader);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_shader_init_compile_failure_with_info_log)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	t_gl_shader_status  = 0;
	gfx_shader_t shader = {0};

	log_set_quiet(0, 1);
	EXPECT_EQ(t_gfx_opengl_shader_stage(&gfx, &shader, GFX_SHADER_STAGE_VERTEX), 1);
	log_set_quiet(0, 0);
	EXPECT_EQ(t_gl_get_shader_info_log_calls, 1);
	EXPECT_EQ(t_gl_delete_shader_calls, 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_shader_init_compile_failure_without_info_log)
{
	START;

	t_gfx_opengl_reset();
	t_gl_shader_info_log_available = 0;
	proc_t proc		       = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_symbols(&proc);
	gfx_t gfx = {0};
	EXPECT_PTR(gfx_init(&gfx, t_gfx_opengl_driver(), &(gfx_config_t){0}, &proc, ALLOC_STD), &gfx);
	t_gl_shader_status  = 0;
	gfx_shader_t shader = {0};

	log_set_quiet(0, 1);
	EXPECT_EQ(t_gfx_opengl_shader_stage(&gfx, &shader, GFX_SHADER_STAGE_VERTEX), 1);
	log_set_quiet(0, 0);
	EXPECT_EQ(t_gl_get_shader_info_log_calls, 0);
	EXPECT_EQ(t_gl_delete_shader_calls, 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_shader_init_alloc_failure)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	gfx.alloc	    = (alloc_t){.alloc = t_gfx_opengl_alloc_fail, .realloc = alloc_realloc_std, .free = alloc_free_std};
	gfx_shader_t shader = {0};

	log_set_quiet(0, 1);
	EXPECT_EQ(t_gfx_opengl_shader(&gfx, &shader), 1);
	log_set_quiet(0, 0);

	gfx.alloc = ALLOC_STD;
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_shader_init_transpile_failure)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	gfx_shader_t shader = {0};

	log_set_quiet(0, 1);
	gfx_shader_config_t shader_config = {
		.compiler = &t_gfx_opengl_compiler,
		.source	  = STRV("not shader source\n"),
		.stage	  = GFX_SHADER_STAGE_VERTEX,
	};
	EXPECT_NULL(gfx_shader_init(&shader, &gfx, &shader_config));
	log_set_quiet(0, 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_shader_init_unsupported_stage)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	gfx_shader_t shader = {0};

	log_set_quiet(0, 1);
	EXPECT_EQ(t_gfx_opengl_shader_stage(&gfx, &shader, (gfx_shader_stage_t)99), 1);
	log_set_quiet(0, 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_pipeline_init_alloc_failure)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	gfx_shader_t vs = {0};
	gfx_shader_t fs = {0};
	EXPECT_EQ(t_gfx_opengl_shader_stage(&gfx, &vs, GFX_SHADER_STAGE_VERTEX), 0);
	EXPECT_EQ(t_gfx_opengl_shader_stage(&gfx, &fs, GFX_SHADER_STAGE_FRAGMENT), 0);
	gfx.alloc		     = (alloc_t){.alloc = t_gfx_opengl_alloc_fail, .realloc = alloc_realloc_std, .free = alloc_free_std};
	gfx_pipeline_t pipe	     = {0};
	gfx_pipeline_config_t config = t_gfx_opengl_pipeline_config(vs, fs);

	log_set_quiet(0, 1);
	EXPECT_NULL(gfx_pipeline_init(&pipe, &gfx, &config));
	log_set_quiet(0, 0);

	gfx.alloc = ALLOC_STD;
	gfx_shader_free(&fs);
	gfx_shader_free(&vs);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_pipeline_init_make_current_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	gfx_shader_t vs		      = {0};
	gfx_shader_t fs		      = {0};
	gfx_render_pass_t render_pass = {0};
	gfx_pipeline_t pipeline	      = {0};
	EXPECT_EQ(t_gfx_opengl_shader_stage(&gfx, &vs, GFX_SHADER_STAGE_VERTEX), 0);
	EXPECT_EQ(t_gfx_opengl_shader_stage(&gfx, &fs, GFX_SHADER_STAGE_FRAGMENT), 0);
	gfx_render_pass_config_t render_pass_config = {
		.color_format = GFX_FORMAT_RGBA8,
		.load	      = GFX_LOAD_LOAD,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);
	t_gfx_opengl_active_render_pass		   = &render_pass;
	gfx_pipeline_config_t config		   = t_gfx_opengl_pipeline_config(vs, fs);
	((t_gfx_opengl_data_t *)gfx.data)->surface = &t_gfx_opengl_surface;
	t_surface_make_current_ret		   = 0;

	log_set_quiet(0, 1);
	EXPECT_NULL(gfx_pipeline_init(&pipeline, &gfx, &config));
	log_set_quiet(0, 0);

	t_surface_make_current_ret	= 1;
	t_gfx_opengl_active_render_pass = NULL;
	gfx_render_pass_free(&render_pass);
	gfx_shader_free(&fs);
	gfx_shader_free(&vs);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_pipeline_init_null_config)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	gfx_pipeline_t pipeline = {0};

	EXPECT_NULL(gfx_pipeline_init(&pipeline, &gfx, NULL));

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_pipeline_init_null_data)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_opengl_driver(),
	};
	EXPECT_NOT_NULL(gfx.drv);
	gfx_pipeline_t pipeline = {0};

	EXPECT_NULL(gfx_pipeline_init(&pipeline, &gfx, &(gfx_pipeline_config_t){0}));

	END;
}

TEST(gfx_opengl_pipeline_init_success)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	gfx_shader_t vs		      = {0};
	gfx_shader_t fs		      = {0};
	gfx_render_pass_t render_pass = {0};
	gfx_pipeline_t pipeline	      = {0};
	EXPECT_EQ(t_gfx_opengl_shader_stage(&gfx, &vs, GFX_SHADER_STAGE_VERTEX), 0);
	EXPECT_EQ(t_gfx_opengl_shader_stage(&gfx, &fs, GFX_SHADER_STAGE_FRAGMENT), 0);
	gfx_render_pass_config_t render_pass_config = {
		.color_format = GFX_FORMAT_RGBA8,
		.load	      = GFX_LOAD_LOAD,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);
	t_gfx_opengl_active_render_pass = &render_pass;
	gfx_pipeline_config_t config	= t_gfx_opengl_pipeline_config(vs, fs);

	EXPECT_PTR(gfx_pipeline_init(&pipeline, &gfx, &config), &pipeline);
	EXPECT_EQ(t_gl_create_program_calls, 1);
	EXPECT_EQ(t_gl_attach_shader_calls, 2);
	EXPECT_EQ(t_gl_link_program_calls, 1);
	EXPECT_EQ(t_gl_get_program_iv_calls, 1);

	gfx_pipeline_free(&pipeline);
	t_gfx_opengl_active_render_pass = NULL;
	gfx_render_pass_free(&render_pass);
	gfx_shader_free(&fs);
	gfx_shader_free(&vs);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_pipeline_init_create_program_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	gfx_shader_t vs		      = {0};
	gfx_shader_t fs		      = {0};
	gfx_render_pass_t render_pass = {0};
	gfx_pipeline_t pipeline	      = {0};
	EXPECT_EQ(t_gfx_opengl_shader_stage(&gfx, &vs, GFX_SHADER_STAGE_VERTEX), 0);
	EXPECT_EQ(t_gfx_opengl_shader_stage(&gfx, &fs, GFX_SHADER_STAGE_FRAGMENT), 0);
	gfx_render_pass_config_t render_pass_config = {
		.color_format = GFX_FORMAT_RGBA8,
		.load	      = GFX_LOAD_LOAD,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);
	t_gfx_opengl_active_render_pass = &render_pass;
	gfx_pipeline_config_t config	= t_gfx_opengl_pipeline_config(vs, fs);
	t_gl_create_program_ret		= 0;

	log_set_quiet(0, 1);
	EXPECT_NULL(gfx_pipeline_init(&pipeline, &gfx, &config));
	log_set_quiet(0, 0);

	t_gfx_opengl_active_render_pass = NULL;
	gfx_render_pass_free(&render_pass);
	gfx_shader_free(&fs);
	gfx_shader_free(&vs);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_pipeline_init_link_failure_with_info_log)
{
	START;

	t_gfx_opengl_reset();
	t_gl_program_info_log_available = 1;
	proc_t proc			= {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_symbols(&proc);
	gfx_t gfx = {0};
	EXPECT_PTR(gfx_init(&gfx, t_gfx_opengl_driver(), &(gfx_config_t){0}, &proc, ALLOC_STD), &gfx);
	gfx_shader_t vs		      = {0};
	gfx_shader_t fs		      = {0};
	gfx_render_pass_t render_pass = {0};
	gfx_pipeline_t pipeline	      = {0};
	EXPECT_EQ(t_gfx_opengl_shader_stage(&gfx, &vs, GFX_SHADER_STAGE_VERTEX), 0);
	EXPECT_EQ(t_gfx_opengl_shader_stage(&gfx, &fs, GFX_SHADER_STAGE_FRAGMENT), 0);
	gfx_render_pass_config_t render_pass_config = {
		.color_format = GFX_FORMAT_RGBA8,
		.load	      = GFX_LOAD_LOAD,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);
	t_gfx_opengl_active_render_pass = &render_pass;
	gfx_pipeline_config_t config	= t_gfx_opengl_pipeline_config(vs, fs);
	t_gl_program_status		= 0;

	log_set_quiet(0, 1);
	EXPECT_NULL(gfx_pipeline_init(&pipeline, &gfx, &config));
	log_set_quiet(0, 0);
	EXPECT_EQ(t_gl_get_program_info_log_calls, 1);

	t_gfx_opengl_active_render_pass = NULL;
	gfx_render_pass_free(&render_pass);
	gfx_shader_free(&fs);
	gfx_shader_free(&vs);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_pipeline_free_null_data_direct)
{
	START;

	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);

	drv->pipeline_free(NULL);

	END;
}

TEST(gfx_opengl_pipeline_free_make_current_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	t_gfx_opengl_pipeline_data_t driver_pipeline = {.program = 99};

	gfx_pipeline_t pipeline = {
		.gfx  = &gfx,
		.data = &driver_pipeline,
	};
	((t_gfx_opengl_data_t *)gfx.data)->surface = &t_gfx_opengl_surface;
	t_surface_make_current_ret		   = 0;

	log_set_quiet(0, 1);
	gfx.drv->pipeline_free(&pipeline);
	log_set_quiet(0, 0);
	EXPECT_PTR(pipeline.data, &driver_pipeline);
	EXPECT_EQ(t_gl_delete_program_calls, 0);

	t_surface_make_current_ret		   = 1;
	((t_gfx_opengl_data_t *)gfx.data)->surface = NULL;
	pipeline.data				   = NULL;
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_pipeline_init_invalid_config_direct)
{
	START;

	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);
	gfx_t gfx		= {0};
	gfx_pipeline_t pipeline = {
		.gfx = &gfx,
	};

	EXPECT_EQ(drv->pipeline_init(&pipeline, &(gfx_pipeline_config_t){0}), 1);

	END;
}

TEST(gfx_opengl_pipeline_init_alloc_failure_direct)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	unsigned int vs_shader = 1;
	unsigned int fs_shader = 2;

	gfx_shader_t vs = {
		.data = &vs_shader,
	};
	gfx_shader_t fs = {
		.data = &fs_shader,
	};
	gfx_pipeline_t pipeline = {
		.gfx = &gfx,
	};
	gfx_pipeline_config_t config = t_gfx_opengl_pipeline_config(vs, fs);
	gfx.alloc		     = (alloc_t){.alloc = t_gfx_opengl_alloc_fail, .realloc = alloc_realloc_std, .free = alloc_free_std};

	log_set_quiet(0, 1);
	EXPECT_EQ(gfx.drv->pipeline_init(&pipeline, &config), 1);
	log_set_quiet(0, 0);

	gfx.alloc = ALLOC_STD;
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_pipeline_init_link_failure_without_info_log)
{
	START;

	t_gfx_opengl_reset();
	t_gl_program_info_log_available = 0;
	proc_t proc			= {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_symbols(&proc);
	gfx_t gfx = {0};
	EXPECT_PTR(gfx_init(&gfx, t_gfx_opengl_driver(), &(gfx_config_t){0}, &proc, ALLOC_STD), &gfx);
	gfx_shader_t vs		      = {0};
	gfx_shader_t fs		      = {0};
	gfx_render_pass_t render_pass = {0};
	gfx_pipeline_t pipeline	      = {0};
	EXPECT_EQ(t_gfx_opengl_shader_stage(&gfx, &vs, GFX_SHADER_STAGE_VERTEX), 0);
	EXPECT_EQ(t_gfx_opengl_shader_stage(&gfx, &fs, GFX_SHADER_STAGE_FRAGMENT), 0);
	gfx_render_pass_config_t render_pass_config = {
		.color_format = GFX_FORMAT_RGBA8,
		.load	      = GFX_LOAD_LOAD,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);
	t_gfx_opengl_active_render_pass = &render_pass;
	gfx_pipeline_config_t config	= t_gfx_opengl_pipeline_config(vs, fs);
	t_gl_program_status		= 0;

	log_set_quiet(0, 1);
	EXPECT_NULL(gfx_pipeline_init(&pipeline, &gfx, &config));
	log_set_quiet(0, 0);
	EXPECT_EQ(t_gl_get_program_info_log_calls, 0);

	t_gfx_opengl_active_render_pass = NULL;
	gfx_render_pass_free(&render_pass);
	gfx_shader_free(&fs);
	gfx_shader_free(&vs);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_pipeline_init_unsupported_input_layout_direct)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	unsigned int vs_shader = 1;
	unsigned int fs_shader = 2;

	gfx_shader_t vs = {
		.data = &vs_shader,
	};
	gfx_shader_t fs = {
		.data = &fs_shader,
	};
	const gfx_layout_t layout[] = {
		{.index = 0, .semantic = "POSITION", .count = 2, .type = GFX_VALUE_UNKNOWN},
	};
	gfx_pipeline_t pipeline = {
		.gfx = &gfx,
	};
	gfx_pipeline_config_t config = t_gfx_opengl_pipeline_config(vs, fs);
	config.input_layout	     = layout;
	config.input_layout_size     = sizeof(layout);

	log_set_quiet(0, 1);
	EXPECT_EQ(gfx.drv->pipeline_init(&pipeline, &config), 1);
	log_set_quiet(0, 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_pipeline_init_rejects_large_layout_stride_direct)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	unsigned int vs_shader = 1;
	unsigned int fs_shader = 2;

	gfx_shader_t vs = {
		.data = &vs_shader,
	};
	gfx_shader_t fs = {
		.data = &fs_shader,
	};
	const gfx_layout_t layout[] = {
		{.index = 0, .semantic = "POSITION", .count = U32_MAX, .type = GFX_VALUE_FLOAT32},
	};
	gfx_pipeline_t pipeline = {
		.gfx = &gfx,
	};
	gfx_pipeline_config_t config = t_gfx_opengl_pipeline_config(vs, fs);
	config.input_layout	     = layout;
	config.input_layout_size     = sizeof(layout);

	log_set_quiet(0, 1);
	EXPECT_EQ(gfx.drv->pipeline_init(&pipeline, &config), 1);
	log_set_quiet(0, 0);
	EXPECT_NULL(pipeline.data);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_pipeline_init_unsupported_input_layout)
{
	START;

	proc_t proc = {0};
	gfx_t gfx   = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	gfx_shader_t vs = {0};
	gfx_shader_t fs = {0};
	EXPECT_EQ(t_gfx_opengl_shader_stage(&gfx, &vs, GFX_SHADER_STAGE_VERTEX), 0);
	EXPECT_EQ(t_gfx_opengl_shader_stage(&gfx, &fs, GFX_SHADER_STAGE_FRAGMENT), 0);
	const gfx_layout_t layout[] = {
		{.index = 0, .semantic = "POSITION", .count = 2, .type = GFX_VALUE_UNKNOWN},
	};
	gfx_pipeline_t pipeline	     = {0};
	gfx_pipeline_config_t config = t_gfx_opengl_pipeline_config(vs, fs);
	config.input_layout	     = layout;
	config.input_layout_size     = sizeof(layout);

	log_set_quiet(0, 1);
	EXPECT_NULL(gfx_pipeline_init(&pipeline, &gfx, &config));
	log_set_quiet(0, 0);

	gfx_shader_free(&fs);
	gfx_shader_free(&vs);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_end_null_frame)
{
	START;

	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_EQ(drv->end(NULL), 1);

	END;
}

TEST(gfx_opengl_pipeline_bind_uses_program)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	gfx_render_pass_t render_pass		     = {.gfx = &gfx, .data = &render_pass};
	t_gfx_opengl_pipeline_data_t driver_pipeline = {
		.program	   = 99,
		.input_layout	   = t_gfx_opengl_input_layout,
		.input_layout_size = sizeof(t_gfx_opengl_input_layout),
		.stride		   = sizeof(gfx_vertex_2d_t),
	};
	gfx_pipeline_t pipeline = {
		.gfx	     = &gfx,
		.render_pass = &render_pass,
		.data	     = &driver_pipeline,
	};
	gfx_frame_t frame = {
		.gfx	     = &gfx,
		.render_pass = &render_pass,
		.active	     = 1,
	};
	gfx.frame = &frame;

	EXPECT_EQ(gfx_pipeline_bind(&frame, &pipeline), 0);
	EXPECT_EQ(t_gl_use_program_calls, 1);
	EXPECT_EQ(t_gl_program, 99);

	gfx.frame = NULL;
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_pipeline_bind_make_current_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	t_gfx_opengl_pipeline_data_t driver_pipeline = {.program = 99};

	gfx_pipeline_t pipeline = {
		.gfx  = &gfx,
		.data = &driver_pipeline,
	};
	gfx_frame_t frame = {
		.gfx = &gfx,
	};
	((t_gfx_opengl_data_t *)gfx.data)->surface = &t_gfx_opengl_surface;
	t_surface_make_current_ret		   = 0;

	log_set_quiet(0, 1);
	EXPECT_EQ(gfx.drv->pipeline_bind(&frame, &pipeline), 1);
	log_set_quiet(0, 0);
	EXPECT_EQ(t_gl_use_program_calls, 0);

	t_surface_make_current_ret = 1;
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_buffer_bind_sets_attributes)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	gfx_render_pass_t render_pass		     = {.gfx = &gfx, .data = &render_pass};
	t_gfx_opengl_pipeline_data_t driver_pipeline = {
		.program	   = 99,
		.input_layout	   = t_gfx_opengl_input_layout,
		.input_layout_size = sizeof(t_gfx_opengl_input_layout),
		.stride		   = sizeof(gfx_vertex_2d_t),
	};
	t_gfx_opengl_buffer_data_t driver_buffer = {.buffer = 77, .target = GL_ARRAY_BUFFER};

	gfx_pipeline_t pipeline = {
		.gfx	     = &gfx,
		.render_pass = &render_pass,
		.data	     = &driver_pipeline,
	};
	gfx_buffer_t buffer = {
		.gfx  = &gfx,
		.data = &driver_buffer,
	};
	gfx_frame_t frame = {
		.gfx	     = &gfx,
		.render_pass = &render_pass,
		.pipeline    = &pipeline,
		.active	     = 1,
	};

	EXPECT_EQ(gfx.drv->buffer_bind(&frame, &buffer), 0);
	EXPECT_EQ(t_gl_enable_vertex_attrib_array_calls, 2);
	EXPECT_EQ(t_gl_vertex_attrib_pointer_calls, 2);
	EXPECT_EQ(t_gl_attrib_index, 1);
	EXPECT_EQ(t_gl_vertex_attrib_size, 4);
	EXPECT_EQ(t_gl_vertex_attrib_stride, sizeof(gfx_vertex_2d_t));

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_buffer_bind_make_current_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	t_gfx_opengl_pipeline_data_t driver_pipeline = {
		.program	   = 99,
		.input_layout	   = t_gfx_opengl_input_layout,
		.input_layout_size = sizeof(t_gfx_opengl_input_layout),
		.stride		   = sizeof(gfx_vertex_2d_t),
	};
	t_gfx_opengl_buffer_data_t driver_buffer = {.buffer = 77, .target = GL_ARRAY_BUFFER};

	gfx_pipeline_t pipeline = {
		.gfx  = &gfx,
		.data = &driver_pipeline,
	};
	gfx_buffer_t buffer = {
		.gfx  = &gfx,
		.data = &driver_buffer,
	};
	gfx_frame_t frame = {
		.gfx	  = &gfx,
		.pipeline = &pipeline,
	};
	((t_gfx_opengl_data_t *)gfx.data)->surface = &t_gfx_opengl_surface;
	t_surface_make_current_ret		   = 0;

	log_set_quiet(0, 1);
	EXPECT_EQ(gfx.drv->buffer_bind(&frame, &buffer), 1);
	log_set_quiet(0, 0);
	EXPECT_EQ(t_gl_bind_buffer_calls, 0);

	t_surface_make_current_ret = 1;
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_draw_calls_gl)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	gfx_frame_t frame = {.gfx = &gfx};

	EXPECT_EQ(gfx.drv->draw(&frame, 3, 2), 0);
	EXPECT_EQ(t_gl_draw_arrays_calls, 1);
	EXPECT_EQ(t_gl_draw_mode, 0x0004);
	EXPECT_EQ(t_gl_draw_count, 3);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_draw_indexed_calls_gl)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	gfx_frame_t frame = {.gfx = &gfx};

	EXPECT_EQ(gfx.drv->draw_indexed(&frame, 3), 0);
	EXPECT_EQ(t_gl_draw_elements_calls, 1);
	EXPECT_EQ(t_gl_draw_mode, GL_TRIANGLES);
	EXPECT_EQ(t_gl_draw_count, 3);
	EXPECT_EQ(t_gl_draw_type, GL_UNSIGNED_INT);
	EXPECT_NULL(t_gl_draw_indices);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_draw_indexed_null_data)
{
	START;

	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_EQ(drv->draw_indexed(NULL, 3), 1);
	EXPECT_EQ(drv->draw_indexed(&(gfx_frame_t){.gfx = &(gfx_t){0}}, 3), 1);

	END;
}

TEST(gfx_opengl_end_disables_attributes)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	t_gfx_opengl_pipeline_data_t driver_pipeline = {
		.program	   = 99,
		.input_layout	   = t_gfx_opengl_input_layout,
		.input_layout_size = sizeof(t_gfx_opengl_input_layout),
		.stride		   = sizeof(gfx_vertex_2d_t),
	};
	gfx_pipeline_t pipeline = {
		.gfx  = &gfx,
		.data = &driver_pipeline,
	};
	gfx_frame_t frame = {
		.gfx	  = &gfx,
		.pipeline = &pipeline,
	};

	EXPECT_EQ(gfx.drv->end(&frame), 0);
	EXPECT_EQ(t_gl_disable_vertex_attrib_array_calls, 2);
	EXPECT_EQ(t_gl_bind_buffer_calls, 2);
	EXPECT_EQ(t_gl_use_program_calls, 1);
	EXPECT_EQ(t_gl_program, 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_end_make_current_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	t_gfx_opengl_pipeline_data_t driver_pipeline = {
		.program	   = 99,
		.input_layout	   = t_gfx_opengl_input_layout,
		.input_layout_size = sizeof(t_gfx_opengl_input_layout),
		.stride		   = sizeof(gfx_vertex_2d_t),
	};
	gfx_pipeline_t pipeline = {
		.gfx  = &gfx,
		.data = &driver_pipeline,
	};
	gfx_frame_t frame = {
		.gfx	  = &gfx,
		.pipeline = &pipeline,
	};
	((t_gfx_opengl_data_t *)gfx.data)->surface = &t_gfx_opengl_surface;
	t_surface_make_current_ret		   = 0;

	log_set_quiet(0, 1);
	EXPECT_EQ(gfx.drv->end(&frame), 1);
	log_set_quiet(0, 0);
	EXPECT_EQ(t_gl_disable_vertex_attrib_array_calls, 0);

	t_surface_make_current_ret = 1;
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_proc_loads_symbol)
{
	START;

	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_symbols(&proc);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);
	gfx_init(&gfx, drv, &(gfx_config_t){0}, &proc, ALLOC_STD);
	void *sym = NULL;

	EXPECT_EQ(gfx_proc(&gfx, STRV("glClearColor"), &sym), 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_proc_sets_symbol)
{
	START;

	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_symbols(&proc);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);
	gfx_init(&gfx, drv, &(gfx_config_t){0}, &proc, ALLOC_STD);
	void *sym = NULL;

	gfx_proc(&gfx, STRV("glClearColor"), &sym);
	EXPECT_PTR(sym, t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glClearColor));

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_proc_null_data)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_opengl_driver(),
	};
	EXPECT_NOT_NULL(gfx.drv);
	void *sym = NULL;

	EXPECT_EQ(gfx.drv->proc(&gfx, STRV("glClearColor"), &sym), 1);

	END;
}

TEST(gfx_opengl_proc_loads_framebuffer_symbol)
{
	START;

	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_symbols(&proc);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);
	EXPECT_PTR(gfx_init(&gfx, drv, &(gfx_config_t){0}, &proc, ALLOC_STD), &gfx);
	void *sym = NULL;

	EXPECT_EQ(gfx_proc(&gfx, STRV("glGenFramebuffers"), &sym), 0);
	EXPECT_PTR(sym, t_gfx_opengl_symbol((t_gfx_opengl_symbol_t)t_glGenFramebuffers));

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_proc_missing_symbol)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	void *sym = NULL;

	log_set_quiet(0, 1);
	EXPECT_EQ(gfx_proc(&gfx, STRV("glMissing"), &sym), 1);
	log_set_quiet(0, 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_render_pass_free_null_data)
{
	START;

	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);

	drv->render_pass_free(NULL);

	END;
}

TEST(gfx_opengl_render_pass_init_invalid_config)
{
	START;

	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);
	gfx_t gfx		      = {0};
	gfx_render_pass_t render_pass = {
		.gfx = &gfx,
	};

	EXPECT_EQ(drv->render_pass_init(&render_pass, &(gfx_render_pass_config_t){.color_format = GFX_FORMAT_NONE}), 1);

	END;
}

TEST(gfx_opengl_render_pass_init_alloc_failure)
{
	START;

	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);
	gfx_t gfx = {
		.alloc = {.alloc = t_gfx_opengl_alloc_fail, .realloc = alloc_realloc_std, .free = alloc_free_std},
	};
	gfx_render_pass_t render_pass = {
		.gfx = &gfx,
	};

	EXPECT_EQ(drv->render_pass_init(&render_pass, &(gfx_render_pass_config_t){.color_format = GFX_FORMAT_RGBA8}), 1);

	END;
}

TEST(gfx_opengl_set_target_null_data)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_opengl_driver(),
	};
	EXPECT_NOT_NULL(gfx.drv);
	u8 pixels[4]	    = {0};
	gfx_target_t target = t_gfx_opengl_memory_target(pixels, 1, 1, 4);

	EXPECT_EQ(gfx.drv->target_init(&target), 1);

	END;
}

TEST(gfx_opengl_target_free_null_data)
{
	START;

	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);

	drv->target_free(NULL);

	END;
}

TEST(gfx_opengl_target_free_surface_make_current_failure)
{
	START;

	t_gfx_opengl_reset();
	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_symbols(&proc);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);
	EXPECT_PTR(gfx_init(&gfx, drv, &(gfx_config_t){.surface = &t_gfx_opengl_surface}, &proc, ALLOC_STD), &gfx);
	u8 pixels[4]	    = {0};
	gfx_target_t target = t_gfx_opengl_memory_target(pixels, 1, 1, 4);
	target.gfx	    = &gfx;
	EXPECT_EQ(drv->target_init(&target), 0);
	t_surface_make_current_ret = 0;

	log_set_quiet(0, 1);
	drv->target_free(&target);
	log_set_quiet(0, 0);
	EXPECT_NOT_NULL(target.driver_data);

	t_surface_make_current_ret = 1;
	drv->target_free(&target);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_render_pass_init_success)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	gfx_render_pass_t render_pass = {0};

	gfx_render_pass_config_t render_pass_config = {
		.color_format = GFX_FORMAT_RGBA8,
		.load	      = GFX_LOAD_CLEAR,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);

	gfx_render_pass_free(&render_pass);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_memory_target_init_invalid_config)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	gfx_target_t target = {
		.gfx	= &gfx,
		.type	= GFX_TARGET_MEMORY,
		.format = GFX_FORMAT_NONE,
	};

	EXPECT_EQ(gfx.drv->target_init(&target), 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_memory_target_init_alloc_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	gfx.alloc	    = (alloc_t){.alloc = t_gfx_opengl_alloc_fail, .realloc = alloc_realloc_std, .free = alloc_free_std};
	u8 pixels[4]	    = {0};
	gfx_target_t target = t_gfx_opengl_memory_target(pixels, 1, 1, 4);
	target.gfx	    = &gfx;

	EXPECT_EQ(gfx.drv->target_init(&target), 1);

	gfx.alloc = ALLOC_STD;
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_memory_target_init_surface_make_current_failure)
{
	START;

	t_gfx_opengl_reset();
	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_symbols(&proc);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);
	EXPECT_PTR(gfx_init(&gfx, drv, &(gfx_config_t){.surface = &t_gfx_opengl_surface}, &proc, ALLOC_STD), &gfx);
	t_surface_make_current_ret = 0;
	u8 pixels[4]		   = {0};
	gfx_target_t target	   = t_gfx_opengl_memory_target(pixels, 1, 1, 4);
	target.gfx		   = &gfx;

	log_set_quiet(0, 1);
	EXPECT_EQ(drv->target_init(&target), 1);
	log_set_quiet(0, 0);

	t_surface_make_current_ret = 1;
	drv->target_free(&target);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_surface_target_init_invalid_config)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	gfx_target_t target = {
		.gfx	= &gfx,
		.type	= GFX_TARGET_SWAPCHAIN,
		.format = GFX_FORMAT_RGBA8,
	};

	EXPECT_EQ(gfx.drv->target_init(&target), 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_swapchain_init_rejects_invalid_direct)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);

	EXPECT_EQ(gfx.drv->swapchain_init(NULL, NULL), 1);
	EXPECT_EQ(gfx.drv->swapchain_init(&(gfx_swapchain_t){.gfx = &gfx}, NULL), 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_swapchain_init_present_mode_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	gfx_surface_ops_t ops	      = t_gfx_opengl_surface_ops;
	ops.present_mode	      = t_gfx_opengl_surface_present_mode_failure;
	t_gfx_opengl_surface.ops      = &ops;
	gfx_swapchain_config_t config = {
		.format	 = GFX_FORMAT_RGBA8,
		.surface = &t_gfx_opengl_surface,
		.width	 = 2,
		.height	 = 2,
	};
	gfx_swapchain_t swapchain = {0};

	EXPECT_NULL(gfx_swapchain_init(&swapchain, &gfx, &config));

	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_surface_target_init_success)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	gfx_swapchain_t swapchain = {0};
	gfx_target_t target	  = {0};

	EXPECT_PTR(t_gfx_opengl_init_swapchain_target(&gfx, &swapchain, &target, 2, 2), &target);
	EXPECT_EQ(t_surface_make_current_calls, 1);

	gfx_target_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_surface_target_init_make_current_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	t_surface_make_current_ret = 0;
	gfx_swapchain_t swapchain  = {0};
	gfx_target_t target	   = {0};

	log_set_quiet(0, 1);
	EXPECT_NULL(t_gfx_opengl_init_swapchain_target(&gfx, &swapchain, &target, 2, 2));
	log_set_quiet(0, 0);

	t_surface_make_current_ret = 1;
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_swapchain_free_rejects_invalid_direct)
{
	START;

	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);

	drv->swapchain_free(NULL);
	drv->swapchain_free(&(gfx_swapchain_t){0});

	END;
}

TEST(gfx_opengl_swapchain_resize_rejects_invalid_direct)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);

	EXPECT_EQ(gfx.drv->swapchain_resize(NULL, 1, 1), 1);
	EXPECT_EQ(gfx.drv->swapchain_resize(&(gfx_swapchain_t){.gfx = &gfx}, 1, 1), 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_swapchain_present_calls_surface)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	gfx_swapchain_t swapchain = {0};
	gfx_target_t target	  = {0};
	EXPECT_PTR(t_gfx_opengl_init_swapchain_target(&gfx, &swapchain, &target, 2, 2), &target);

	EXPECT_EQ(gfx_swapchain_present(&swapchain), 0);
	EXPECT_EQ(t_surface_present_calls, 1);

	gfx_target_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_target_init_none)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	gfx_target_t target = {
		.gfx  = &gfx,
		.type = GFX_TARGET_NONE,
	};

	EXPECT_EQ(gfx.drv->target_init(&target), 0);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_target_init_unknown_type)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	gfx_target_t target = {
		.gfx  = &gfx,
		.type = (gfx_target_type_t)99,
	};

	EXPECT_EQ(gfx.drv->target_init(&target), 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_target_read_null_config)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);

	EXPECT_EQ(gfx.drv->target_read(&(gfx_target_t){.gfx = &gfx}, NULL), 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_target_read_without_bound_target)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	u8 pixels[4]	    = {0};
	gfx_target_t target = t_gfx_opengl_memory_target(pixels, 1, 1, 4);
	target.gfx	    = &gfx;
	EXPECT_EQ(gfx.drv->target_init(&target), 0);

	EXPECT_EQ(gfx.drv->target_read(&target, &(gfx_memory_readback_config_t){.data = pixels, .stride = 4}), 1);

	gfx.drv->target_free(&target);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_target_read_without_bound_framebuffer)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	u8 pixels[4]	    = {0};
	gfx_target_t target = t_gfx_opengl_memory_target(pixels, 1, 1, 4);
	target.gfx	    = &gfx;
	EXPECT_EQ(gfx.drv->target_init(&target), 0);
	((t_gfx_opengl_data_t *)gfx.data)->target      = &target;
	((t_gfx_opengl_data_t *)gfx.data)->framebuffer = NULL;

	EXPECT_EQ(gfx.drv->target_read(&target, &(gfx_memory_readback_config_t){.data = pixels, .stride = 4}), 1);
	EXPECT_EQ(t_gl_read_pixels_calls, 0);

	gfx.drv->target_free(&target);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_target_read_reads_rows)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	u8 pixels[8]					= {0};
	gfx_swapchain_t swapchain			= {0};
	gfx_target_t target				= {0};
	gfx_render_pass_t render_pass			= {0};
	gfx_framebuffer_t framebuffer			= {0};
	gfx_frame_t frame				= {0};
	gfx_memory_target_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 1,
		.height = 2,
		.stride = 4,
	};
	EXPECT_PTR(gfx_target_init_memory(&target, &gfx, &memory_target_config), &target);
	gfx_render_pass_config_t render_pass_config = {
		.color_format = GFX_FORMAT_RGBA8,
		.load	      = GFX_LOAD_LOAD,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);
	EXPECT_PTR(gfx_framebuffer_init(&framebuffer, &target, &render_pass), &framebuffer);
	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &(gfx_pass_config_t){0}), 0);

	EXPECT_EQ(gfx.drv->target_read(&target, &(gfx_memory_readback_config_t){.data = pixels, .stride = 4}), 0);
	EXPECT_EQ(t_gl_read_pixels_calls, 2);
	EXPECT_EQ(t_gl_read_pixels_first_y, 1);
	EXPECT_EQ(t_gl_read_pixels_y, 0);

	gfx_end(&frame);
	gfx_framebuffer_free(&framebuffer);
	gfx_render_pass_free(&render_pass);
	gfx_target_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}
TEST(gfx_opengl_init_missing_framebuffer_symbol)
{
	START;

	t_gfx_opengl_reset();
	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_core_symbols(&proc, STRV("libGL.so.1"));
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);

	log_set_quiet(0, 1);
	EXPECT_NULL(gfx_init(&gfx, drv, &(gfx_config_t){0}, &proc, ALLOC_STD));
	log_set_quiet(0, 0);

	proc_free(&proc);
	END;
}
TEST(gfx_opengl_framebuffer_free_null_data)
{
	START;

	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);

	drv->framebuffer_free(NULL);

	END;
}

TEST(gfx_opengl_framebuffer_free_surface_make_current_failure)
{
	START;

	t_gfx_opengl_reset();
	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_symbols(&proc);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);
	EXPECT_PTR(gfx_init(&gfx, drv, &(gfx_config_t){.surface = &t_gfx_opengl_surface}, &proc, ALLOC_STD), &gfx);
	u8 pixels[4]	    = {0};
	gfx_target_t target = t_gfx_opengl_memory_target(pixels, 1, 1, 4);
	target.gfx	    = &gfx;
	EXPECT_EQ(drv->target_init(&target), 0);
	gfx_render_pass_t render_pass = {
		.gfx  = &gfx,
		.data = &render_pass,
	};
	gfx_framebuffer_t framebuffer = {
		.gfx	     = &gfx,
		.target	     = &target,
		.render_pass = &render_pass,
	};
	EXPECT_EQ(drv->framebuffer_init(&framebuffer), 0);
	t_surface_make_current_ret = 0;

	log_set_quiet(0, 1);
	drv->framebuffer_free(&framebuffer);
	log_set_quiet(0, 0);
	EXPECT_NOT_NULL(framebuffer.data);

	t_surface_make_current_ret = 1;
	drv->framebuffer_free(&framebuffer);
	drv->target_free(&target);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_framebuffer_init_invalid_config)
{
	START;

	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);

	EXPECT_EQ(drv->framebuffer_init(NULL), 1);

	END;
}

TEST(gfx_opengl_framebuffer_init_alloc_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	gfx.alloc		      = (alloc_t){.alloc = t_gfx_opengl_alloc_fail, .realloc = alloc_realloc_std, .free = alloc_free_std};
	gfx_render_pass_t render_pass = {
		.gfx  = &gfx,
		.data = &render_pass,
	};
	u8 pixels[4]	    = {0};
	gfx_target_t target = {
		.gfx	     = &gfx,
		.type	     = GFX_TARGET_MEMORY,
		.format	     = GFX_FORMAT_RGBA8,
		.data	     = pixels,
		.driver_data = &target,
		.width	     = 1,
		.height	     = 1,
		.stride	     = 4,
	};
	gfx_framebuffer_t framebuffer = {
		.gfx	     = &gfx,
		.target	     = &target,
		.render_pass = &render_pass,
	};

	EXPECT_EQ(gfx.drv->framebuffer_init(&framebuffer), 1);

	gfx.alloc = ALLOC_STD;
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_framebuffer_init_invalid_target_type)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	gfx_render_pass_t render_pass = {
		.gfx  = &gfx,
		.data = &render_pass,
	};
	gfx_target_t target = {
		.gfx	= &gfx,
		.type	= (gfx_target_type_t)99,
		.format = GFX_FORMAT_RGBA8,
	};
	gfx_framebuffer_t framebuffer = {
		.gfx	     = &gfx,
		.target	     = &target,
		.render_pass = &render_pass,
	};

	EXPECT_EQ(gfx.drv->framebuffer_init(&framebuffer), 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_framebuffer_init_surface_make_current_failure)
{
	START;

	t_gfx_opengl_reset();
	proc_t proc = {0};
	proc_init(&proc, 0, 1, ALLOC_STD);
	t_gfx_opengl_symbols(&proc);
	gfx_t gfx	  = {0};
	gfx_driver_t *drv = t_gfx_opengl_driver();
	EXPECT_NOT_NULL(drv);
	EXPECT_PTR(gfx_init(&gfx, drv, &(gfx_config_t){.surface = &t_gfx_opengl_surface}, &proc, ALLOC_STD), &gfx);
	u8 pixels[4]	    = {0};
	gfx_target_t target = t_gfx_opengl_memory_target(pixels, 1, 1, 4);
	target.gfx	    = &gfx;
	EXPECT_EQ(drv->target_init(&target), 0);
	gfx_render_pass_t render_pass = {
		.gfx  = &gfx,
		.data = &render_pass,
	};
	gfx_framebuffer_t framebuffer = {
		.gfx	     = &gfx,
		.target	     = &target,
		.render_pass = &render_pass,
	};
	t_surface_make_current_ret = 0;

	log_set_quiet(0, 1);
	EXPECT_EQ(drv->framebuffer_init(&framebuffer), 1);
	log_set_quiet(0, 0);

	t_surface_make_current_ret = 1;
	drv->framebuffer_free(&framebuffer);
	drv->target_free(&target);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_framebuffer_init_surface_success)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	gfx_swapchain_t swapchain     = {0};
	gfx_target_t target	      = {0};
	gfx_render_pass_t render_pass = {0};
	gfx_framebuffer_t framebuffer = {0};
	EXPECT_PTR(t_gfx_opengl_init_swapchain_target(&gfx, &swapchain, &target, 2, 2), &target);
	gfx_render_pass_config_t render_pass_config = {
		.color_format = GFX_FORMAT_RGBA8,
		.load	      = GFX_LOAD_LOAD,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);

	EXPECT_PTR(gfx_framebuffer_init(&framebuffer, &target, &render_pass), &framebuffer);

	gfx_framebuffer_free(&framebuffer);
	gfx_render_pass_free(&render_pass);
	gfx_target_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_framebuffer_init_incomplete_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	t_gl_framebuffer_status				= 0;
	u8 pixels[4]					= {0};
	gfx_swapchain_t swapchain			= {0};
	gfx_target_t target				= {0};
	gfx_render_pass_t render_pass			= {0};
	gfx_framebuffer_t framebuffer			= {0};
	gfx_memory_target_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 1,
		.height = 1,
		.stride = 4,
	};
	EXPECT_PTR(gfx_target_init_memory(&target, &gfx, &memory_target_config), &target);
	gfx_render_pass_config_t render_pass_config = {
		.color_format = GFX_FORMAT_RGBA8,
		.load	      = GFX_LOAD_LOAD,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);

	log_set_quiet(0, 1);
	EXPECT_NULL(gfx_framebuffer_init(&framebuffer, &target, &render_pass));
	log_set_quiet(0, 0);

	gfx_render_pass_free(&render_pass);
	gfx_target_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_framebuffer_pass_begin_bind_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	gfx_render_pass_t render_pass = {
		.gfx  = &gfx,
		.load = GFX_LOAD_CLEAR,
		.data = &render_pass,
	};
	gfx_target_t target = {
		.gfx	= &gfx,
		.type	= (gfx_target_type_t)99,
		.format = GFX_FORMAT_RGBA8,
	};
	unsigned int gl_framebuffer   = 1;
	gfx_framebuffer_t framebuffer = {
		.gfx	     = &gfx,
		.target	     = &target,
		.render_pass = &render_pass,
		.data	     = &gl_framebuffer,
	};
	gfx_frame_t frame = {
		.gfx = &gfx,
	};

	EXPECT_EQ(gfx.drv->framebuffer_pass_begin(&framebuffer, &frame), 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}
TEST(gfx_opengl_framebuffer_pass_begin_zero_memory_framebuffer)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	gfx_render_pass_t render_pass = {
		.gfx  = &gfx,
		.load = GFX_LOAD_CLEAR,
		.data = &render_pass,
	};
	u8 pixels[4]	    = {0};
	gfx_target_t target = {
		.gfx	     = &gfx,
		.type	     = GFX_TARGET_MEMORY,
		.format	     = GFX_FORMAT_RGBA8,
		.data	     = pixels,
		.driver_data = &target,
		.width	     = 1,
		.height	     = 1,
		.stride	     = 4,
	};
	unsigned int gl_framebuffer   = 0;
	gfx_framebuffer_t framebuffer = {
		.gfx	     = &gfx,
		.target	     = &target,
		.render_pass = &render_pass,
		.data	     = &gl_framebuffer,
	};
	gfx_frame_t frame = {
		.gfx = &gfx,
	};

	EXPECT_EQ(gfx.drv->framebuffer_pass_begin(&framebuffer, &frame), 1);

	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_framebuffer_pass_begin_clears_memory_target)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	u8 pixels[4]					= {0};
	gfx_swapchain_t swapchain			= {0};
	gfx_target_t target				= {0};
	gfx_render_pass_t render_pass			= {0};
	gfx_framebuffer_t framebuffer			= {0};
	gfx_frame_t frame				= {0};
	gfx_memory_target_config_t memory_target_config = {
		.format = GFX_FORMAT_RGBA8,
		.data	= pixels,
		.width	= 1,
		.height = 1,
		.stride = 4,
	};
	EXPECT_PTR(gfx_target_init_memory(&target, &gfx, &memory_target_config), &target);
	gfx_render_pass_config_t render_pass_config = {
		.color_format = GFX_FORMAT_RGBA8,
		.load	      = GFX_LOAD_CLEAR,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);
	EXPECT_PTR(gfx_framebuffer_init(&framebuffer, &target, &render_pass), &framebuffer);

	gfx_pass_config_t pass_config = {
		.clear	  = {.r = 0.1f, .g = 0.2f, .b = 0.3f, .a = 0.4f},
		.viewport = {.x = 1, .y = 2, .width = 3, .height = 4},
	};
	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &pass_config), 0);
	EXPECT_EQ(t_gl_bind_framebuffer_calls, 2);
	EXPECT_EQ(t_gl_viewport_calls, 1);
	EXPECT_EQ(t_gl_viewport_x, 1);
	EXPECT_EQ(t_gl_viewport_y, 2);
	EXPECT_EQ(t_gl_viewport_width, 3);
	EXPECT_EQ(t_gl_viewport_height, 4);
	EXPECT_EQ(t_gl_clear_color_calls, 1);
	EXPECT_EQ(t_gl_clear_calls, 1);
	EXPECT_EQ(t_gl_r, 0.1f);
	EXPECT_EQ(t_gl_g, 0.2f);
	EXPECT_EQ(t_gl_b, 0.3f);
	EXPECT_EQ(t_gl_a, 0.4f);

	gfx_end(&frame);
	gfx_framebuffer_free(&framebuffer);
	gfx_render_pass_free(&render_pass);
	gfx_target_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_framebuffer_pass_begin_binds_surface_framebuffer)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	gfx_swapchain_t swapchain     = {0};
	gfx_target_t target	      = {0};
	gfx_render_pass_t render_pass = {0};
	gfx_framebuffer_t framebuffer = {0};
	gfx_frame_t frame	      = {0};
	EXPECT_PTR(t_gfx_opengl_init_swapchain_target(&gfx, &swapchain, &target, 2, 2), &target);
	gfx_render_pass_config_t render_pass_config = {
		.color_format = GFX_FORMAT_RGBA8,
		.load	      = GFX_LOAD_LOAD,
		.store	      = GFX_STORE_STORE,
	};
	EXPECT_PTR(gfx_render_pass_init(&render_pass, &gfx, &render_pass_config), &render_pass);
	EXPECT_PTR(gfx_framebuffer_init(&framebuffer, &target, &render_pass), &framebuffer);
	t_gl_bind_framebuffer_calls = 0;

	EXPECT_EQ(gfx_framebuffer_pass_begin(&framebuffer, &frame, &(gfx_pass_config_t){0}), 0);
	EXPECT_EQ(t_gl_bind_framebuffer_calls, 1);
	EXPECT_EQ(t_gl_framebuffer, 0);

	gfx_end(&frame);
	gfx_framebuffer_free(&framebuffer);
	gfx_render_pass_free(&render_pass);
	gfx_target_free(&target);
	gfx_swapchain_free(&swapchain);
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_framebuffer_pass_begin_make_current_failure)
{
	START;

	gfx_t gfx   = {0};
	proc_t proc = {0};
	EXPECT_EQ(t_gfx_opengl_init_gfx(&gfx, &proc), 0);
	gfx_render_pass_t render_pass = {
		.gfx  = &gfx,
		.data = &render_pass,
	};
	gfx_target_t target = {
		.gfx	   = &gfx,
		.type	   = GFX_TARGET_SWAPCHAIN,
		.format	   = GFX_FORMAT_RGBA8,
		.swapchain = &(gfx_swapchain_t){.surface = &t_gfx_opengl_surface, .format = GFX_FORMAT_RGBA8, .width = 2, .height = 2},
		.width	   = 2,
		.height	   = 2,
	};
	gfx_framebuffer_t framebuffer = {
		.gfx	     = &gfx,
		.target	     = &target,
		.render_pass = &render_pass,
		.data	     = &framebuffer,
	};
	gfx_frame_t frame	   = {.gfx = &gfx};
	t_surface_make_current_ret = 0;

	log_set_quiet(0, 1);
	EXPECT_EQ(gfx.drv->framebuffer_pass_begin(&framebuffer, &frame), 1);
	log_set_quiet(0, 0);

	t_surface_make_current_ret = 1;
	gfx_free(&gfx);
	proc_free(&proc);
	END;
}

TEST(gfx_opengl_clear_null_data)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_opengl_driver(),
	};
	EXPECT_NOT_NULL(gfx.drv);

	EXPECT_EQ(gfx.drv->framebuffer_pass_begin(NULL, NULL), 1);

	END;
}

TEST(gfx_opengl_present_null_data)
{
	START;

	gfx_t gfx = {
		.drv = t_gfx_opengl_driver(),
	};
	EXPECT_NOT_NULL(gfx.drv);

	EXPECT_EQ(gfx.drv->swapchain_present(&(gfx_swapchain_t){0}), 1);

	END;
}

STEST(gfx_opengl)
{
	SSTART;

	(void)t_gfx_opengl_compiler_init();

	RUN(gfx_opengl_driver_is_registered);
	RUN(gfx_opengl_init_null_gfx);
	RUN(gfx_opengl_init_null_config);
	RUN(gfx_opengl_init_null_proc);
	RUN(gfx_opengl_init_alloc_failure);
	RUN(gfx_opengl_init_missing_library);
	RUN(gfx_opengl_init_fallback_library);
	RUN(gfx_opengl_init_glvnd_libraries);
	RUN(gfx_opengl_init_windows_library_name);
	RUN(gfx_opengl_init_invalid_surface);
	RUN(gfx_opengl_init_surface_make_current_failure);
	RUN(gfx_opengl_init_surface_missing_clear_current);
	RUN(gfx_opengl_init_missing_clear_symbol);
	RUN(gfx_opengl_init_success);
	RUN(gfx_opengl_free_does_not_use_init_surface);
	RUN(gfx_opengl_free_null_data);
	RUN(gfx_opengl_clear_color_null_data);
	RUN(gfx_opengl_viewport_null_data);
	RUN(gfx_opengl_draw_null_data);
	RUN(gfx_opengl_begin_null_frame);
	RUN(gfx_opengl_buffer_bind_null_frame);
	RUN(gfx_opengl_pipeline_bind_null_frame);
	RUN(gfx_opengl_init_missing_draw_symbol);
	RUN(gfx_opengl_buffer_free_null_data);
	RUN(gfx_opengl_buffer_free_make_current_failure);
	RUN(gfx_opengl_buffer_init_null_config);
	RUN(gfx_opengl_buffer_init_alloc_failure);
	RUN(gfx_opengl_buffer_init_create_buffer_failure);
	RUN(gfx_opengl_buffer_init_index_buffer);
	RUN(gfx_opengl_buffer_init_unsupported_type);
	RUN(gfx_opengl_buffer_init_rejects_invalid_direct);
	RUN(gfx_opengl_buffer_init_static_uploads_data);
	RUN(gfx_opengl_buffer_init_make_current_failure);
	RUN(gfx_opengl_buffer_init_missing_make_current_callback);
	RUN(gfx_opengl_buffer_set_data_null_data);
	RUN(gfx_opengl_buffer_set_data_uploads_vertices);
	RUN(gfx_opengl_buffer_set_data_make_current_failure);
	RUN(gfx_opengl_shader_init_create_shader_failure);
	RUN(gfx_opengl_shader_init_without_diagnostic_symbols);
	RUN(gfx_opengl_shader_free_null_data);
	RUN(gfx_opengl_shader_free_make_current_failure);
	RUN(gfx_opengl_shader_init_null_config);
	RUN(gfx_opengl_shader_init_null_data);
	RUN(gfx_opengl_shader_init_make_current_failure);
	RUN(gfx_opengl_shader_init_null_gl_version);
	RUN(gfx_opengl_shader_init_create_shader_reports_gl_error);
	RUN(gfx_opengl_shader_init_compile_failure_with_info_log);
	RUN(gfx_opengl_shader_init_compile_failure_without_info_log);
	RUN(gfx_opengl_shader_init_alloc_failure);
	RUN(gfx_opengl_shader_init_transpile_failure);
	RUN(gfx_opengl_shader_init_unsupported_stage);
	RUN(gfx_opengl_pipeline_init_alloc_failure);
	RUN(gfx_opengl_pipeline_init_null_config);
	RUN(gfx_opengl_pipeline_init_null_data);
	RUN(gfx_opengl_pipeline_init_make_current_failure);
	RUN(gfx_opengl_pipeline_init_create_program_failure);
	RUN(gfx_opengl_pipeline_init_link_failure_with_info_log);
	RUN(gfx_opengl_pipeline_init_success);
	RUN(gfx_opengl_pipeline_free_null_data_direct);
	RUN(gfx_opengl_pipeline_free_make_current_failure);
	RUN(gfx_opengl_pipeline_init_invalid_config_direct);
	RUN(gfx_opengl_pipeline_init_alloc_failure_direct);
	RUN(gfx_opengl_pipeline_init_unsupported_input_layout_direct);
	RUN(gfx_opengl_pipeline_init_rejects_large_layout_stride_direct);
	RUN(gfx_opengl_pipeline_init_link_failure_without_info_log);
	RUN(gfx_opengl_pipeline_init_unsupported_input_layout);
	RUN(gfx_opengl_end_null_frame);
	RUN(gfx_opengl_pipeline_bind_uses_program);
	RUN(gfx_opengl_pipeline_bind_make_current_failure);
	RUN(gfx_opengl_buffer_bind_sets_attributes);
	RUN(gfx_opengl_buffer_bind_make_current_failure);
	RUN(gfx_opengl_draw_calls_gl);
	RUN(gfx_opengl_draw_indexed_calls_gl);
	RUN(gfx_opengl_draw_indexed_null_data);
	RUN(gfx_opengl_end_disables_attributes);
	RUN(gfx_opengl_end_make_current_failure);
	RUN(gfx_opengl_proc_loads_symbol);
	RUN(gfx_opengl_proc_sets_symbol);
	RUN(gfx_opengl_proc_null_data);
	RUN(gfx_opengl_proc_loads_framebuffer_symbol);
	RUN(gfx_opengl_proc_missing_symbol);
	RUN(gfx_opengl_render_pass_free_null_data);
	RUN(gfx_opengl_render_pass_init_invalid_config);
	RUN(gfx_opengl_render_pass_init_alloc_failure);
	RUN(gfx_opengl_render_pass_init_success);
	RUN(gfx_opengl_set_target_null_data);
	RUN(gfx_opengl_target_free_null_data);
	RUN(gfx_opengl_target_free_surface_make_current_failure);
	RUN(gfx_opengl_memory_target_init_invalid_config);
	RUN(gfx_opengl_memory_target_init_alloc_failure);
	RUN(gfx_opengl_memory_target_init_surface_make_current_failure);
	RUN(gfx_opengl_surface_target_init_invalid_config);
	RUN(gfx_opengl_swapchain_init_rejects_invalid_direct);
	RUN(gfx_opengl_swapchain_init_present_mode_failure);
	RUN(gfx_opengl_surface_target_init_success);
	RUN(gfx_opengl_surface_target_init_make_current_failure);
	RUN(gfx_opengl_swapchain_free_rejects_invalid_direct);
	RUN(gfx_opengl_swapchain_resize_rejects_invalid_direct);
	RUN(gfx_opengl_swapchain_present_calls_surface);
	RUN(gfx_opengl_target_init_none);
	RUN(gfx_opengl_target_init_unknown_type);
	RUN(gfx_opengl_target_read_null_config);
	RUN(gfx_opengl_target_read_without_bound_target);
	RUN(gfx_opengl_target_read_without_bound_framebuffer);
	RUN(gfx_opengl_target_read_reads_rows);
	RUN(gfx_opengl_init_missing_framebuffer_symbol);
	RUN(gfx_opengl_framebuffer_free_null_data);
	RUN(gfx_opengl_framebuffer_free_surface_make_current_failure);
	RUN(gfx_opengl_framebuffer_init_invalid_config);
	RUN(gfx_opengl_framebuffer_init_alloc_failure);
	RUN(gfx_opengl_framebuffer_init_invalid_target_type);
	RUN(gfx_opengl_framebuffer_init_surface_make_current_failure);
	RUN(gfx_opengl_framebuffer_init_surface_success);
	RUN(gfx_opengl_framebuffer_init_incomplete_failure);
	RUN(gfx_opengl_framebuffer_pass_begin_bind_failure);
	RUN(gfx_opengl_framebuffer_pass_begin_zero_memory_framebuffer);
	RUN(gfx_opengl_framebuffer_pass_begin_clears_memory_target);
	RUN(gfx_opengl_framebuffer_pass_begin_binds_surface_framebuffer);
	RUN(gfx_opengl_framebuffer_pass_begin_make_current_failure);
	RUN(gfx_opengl_clear_null_data);
	RUN(gfx_opengl_present_null_data);

	t_gfx_opengl_compiler_free();

	SEND;
}
