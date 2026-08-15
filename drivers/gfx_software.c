#include "gfx_driver.h"
#include "gfx_shader_driver.h"

#include "cmath.h"
#include "log.h"
#include "mem.h"

enum {
	GFX_SOFTWARE_MAX_ATTRIBUTES    = 16,
	GFX_SOFTWARE_MAX_CLIP_VERTICES = 16,
};

typedef struct gfx_software_buffer_s {
	buf_t buf;
} gfx_software_buffer_t;

typedef struct gfx_software_attribute_s {
	size_t offset;
	u32 count;
} gfx_software_attribute_t;

typedef struct gfx_software_shader_s {
	gfx_shader_stage_t stage;
	gfx_shader_ir_t ir;
} gfx_software_shader_t;

typedef struct gfx_software_pipeline_s {
	gfx_shader_ir_t ir;
	gfx_software_attribute_t position;
	gfx_software_attribute_t color;
	size_t stride;
	int shader;
} gfx_software_pipeline_t;

typedef struct gfx_software_s {
	gfx_image_t image;
	gfx_swapchain_t *swapchain;
	float *depth;
	size_t depth_size;
	u16 viewport_x;
	u16 viewport_y;
	u16 viewport_width;
	u16 viewport_height;
} gfx_software_t;

typedef struct gfx_software_surface_target_s {
	gfx_surface_memory_t memory;
} gfx_software_surface_target_t;

typedef struct gfx_software_color_layout_s {
	u8 r;
	u8 g;
	u8 b;
	u8 a;
} gfx_software_color_layout_t;

typedef struct gfx_software_vertex_s {
	float x;
	float y;
	float z;
	float w;
	float r;
	float g;
	float b;
	float a;
} gfx_software_vertex_t;

static int gfx_software_image_init(gfx_image_t *image);

typedef enum gfx_software_value_kind_e {
	GFX_SOFTWARE_VALUE_NONE,
	GFX_SOFTWARE_VALUE_FLOAT,
	GFX_SOFTWARE_VALUE_VEC4,
	GFX_SOFTWARE_VALUE_MAT4,
} gfx_software_value_kind_t;

typedef struct gfx_software_value_s {
	gfx_software_value_kind_t kind;
	float f;
	vec4f_t v4;
	mat4f_t m4;
} gfx_software_value_t;

static u8 color_u8(float value)
{
	if (value <= 0.0f) {
		return 0;
	}
	if (value >= 1.0f) {
		return 255;
	}
	return (u8)(value * 255.0f + 0.5f);
}

static int gfx_software_format_valid(gfx_format_t format)
{
	return format == GFX_FORMAT_RGBA8_UNORM || format == GFX_FORMAT_BGRA8_UNORM;
}

static gfx_software_color_layout_t gfx_software_color_layout(gfx_format_t format)
{
	if (format == GFX_FORMAT_BGRA8_UNORM) {
		return (gfx_software_color_layout_t){.r = 2, .g = 1, .b = 0, .a = 3};
	}
	return (gfx_software_color_layout_t){.r = 0, .g = 1, .b = 2, .a = 3};
}

static int memory_image_valid(const gfx_image_t *image)
{
	if (image == NULL || !gfx_software_format_valid(image->format) || image->data == NULL || image->width == 0 || image->height == 0) {
		return 0;
	}

	return image->stride >= (size_t)image->width * 4;
}

static int surface_image_valid(const gfx_image_t *image)
{
	return image != NULL && image->origin == GFX_IMAGE_ORIGIN_SURFACE && gfx_software_format_valid(image->format) &&
	       image->swapchain != NULL && image->swapchain->surface != NULL && image->swapchain->surface->api == GFX_API_SOFTWARE &&
	       image->swapchain->surface->ops != NULL && image->swapchain->surface->ops->memory != NULL && image->width != 0 &&
	       image->height != 0;
}

static int image_valid(const gfx_image_t *image)
{
	return image != NULL && image->origin == GFX_IMAGE_ORIGIN_MEMORY && memory_image_valid(image);
}

static int gfx_software_strv_suffix(strv_t str, strv_t suffix)
{
	return str.len >= suffix.len && mem_cmp(str.data + str.len - suffix.len, suffix.data, suffix.len) == 0;
}

static int gfx_software_layout_semantic(const gfx_layout_t *layout, strv_t semantic)
{
	if (layout == NULL || layout->semantic == NULL) {
		return 0;
	}
	strv_t name = strv_cstr(layout->semantic);
	if (strv_eq(name, semantic)) {
		return 1;
	}
	return layout->semantic_index == 0 && semantic.len > 1 && semantic.data[semantic.len - 1] == '0' &&
	       strv_eq(name, STRVN(semantic.data, semantic.len - 1));
}

static size_t gfx_software_type_size(strv_t type)
{
	if (strv_eq(type, STRV("mat4f"))) {
		return sizeof(mat4f_t);
	}
	if (strv_eq(type, STRV("vec4f"))) {
		return sizeof(vec4f_t);
	}
	if (strv_eq(type, STRV("vec3f"))) { // LCOV_EXCL_LINE
		return sizeof(vec3f_t);	    // LCOV_EXCL_LINE
	}
	if (strv_eq(type, STRV("vec2f"))) {
		return sizeof(vec2f_t);
	}
	return 0; // LCOV_EXCL_LINE
}

static int gfx_software_parse_float(strv_t text, float *out)
{
	if (out == NULL || text.data == NULL || text.len == 0) { // LCOV_EXCL_LINE
		return 1;					 // LCOV_EXCL_LINE
	}
	float value = 0.0f;
	size_t pos  = 0;
	while (pos < text.len && text.data[pos] >= '0' && text.data[pos] <= '9') {
		value = value * 10.0f + (float)(text.data[pos] - '0');
		pos++;
	}
	if (pos < text.len && text.data[pos] == '.') {
		float place = 0.1f;
		pos++;
		while (pos < text.len && text.data[pos] >= '0' && text.data[pos] <= '9') {
			value += place * (float)(text.data[pos] - '0');
			place *= 0.1f;
			pos++;
		}
	}
	if (pos < text.len && text.data[pos] == 'f') {
		pos++;
	}
	if (pos != text.len) { // LCOV_EXCL_LINE
		return 1;      // LCOV_EXCL_LINE
	}
	*out = value;
	return 0;
}

static const gfx_buffer_t *gfx_software_uniform_binding(const gfx_frame_t *frame, u32 slot)
{
	if (frame == NULL) {
		return NULL; // LCOV_EXCL_LINE
	}
	for (u32 i = 0; i < frame->resource_binding_count; i++) {
		const gfx_resource_binding_t *binding = &frame->resource_bindings[i];
		if (binding->type == GFX_RESOURCE_UNIFORM_BUFFER && binding->binding == slot) {
			return binding->buffer;
		}
	}
	return NULL;
}

static int gfx_software_uniform_member(const gfx_frame_t *frame, const gfx_software_pipeline_t *pipeline, strv_t name,
				       gfx_software_value_t *out)
{
	if (frame == NULL || pipeline == NULL || out == NULL) {
		return 1; // LCOV_EXCL_LINE
	}
	for (u32 i = 0; i < pipeline->ir.buffer_count; i++) {
		const gfx_shader_struct_ir_t *buffer = &pipeline->ir.buffers[i];
		size_t offset			     = 0;
		for (u32 j = 0; j < buffer->member_count; j++) {
			const gfx_shader_member_t *member = &buffer->members[j];
			size_t size			  = gfx_software_type_size(member->type);
			if (size == 0) {  // LCOV_EXCL_LINE
				return 1; // LCOV_EXCL_LINE
			}
			if (strv_eq(member->name, name)) {
				const gfx_buffer_t *uniform_buffer = gfx_software_uniform_binding(frame, buffer->slot);
				if (uniform_buffer == NULL || uniform_buffer->data == NULL) {
					return 1;
				}
				const gfx_software_buffer_t *uniform = uniform_buffer->data;
				if (uniform->buf.used < offset + size) {
					return 1;
				}
				if (strv_eq(member->type, STRV("mat4f"))) {
					*out = (gfx_software_value_t){.kind = GFX_SOFTWARE_VALUE_MAT4};
					mem_copy(&out->m4, sizeof(out->m4), (const u8 *)uniform->buf.data + offset, sizeof(out->m4));
					return 0;
				}
				if (strv_eq(member->type, STRV("vec4f"))) {
					*out = (gfx_software_value_t){.kind = GFX_SOFTWARE_VALUE_VEC4};
					mem_copy(&out->v4, sizeof(out->v4), (const u8 *)uniform->buf.data + offset, sizeof(out->v4));
					return 0;
				}
			}
			offset += size;
		}
	}
	return 1;
}

static int gfx_software_value_component(gfx_software_value_t value, char component, gfx_software_value_t *out)
{
	if (out == NULL || value.kind != GFX_SOFTWARE_VALUE_VEC4) { // LCOV_EXCL_LINE
		return 1;					    // LCOV_EXCL_LINE
	}
	*out = (gfx_software_value_t){.kind = GFX_SOFTWARE_VALUE_FLOAT};
	switch (component) {
	case 'x': {
		out->f = value.v4.x;
		return 0;
	}
	case 'y': {
		out->f = value.v4.y;
		return 0;
	}
	case 'z': {
		out->f = value.v4.z;
		return 0;
	}
	case 'w': {
		out->f = value.v4.w;
		return 0;
	}
	default: {	  // LCOV_EXCL_LINE
		return 1; // LCOV_EXCL_LINE
	}
	}
}

static int gfx_software_eval_expr(const gfx_frame_t *frame, const gfx_software_pipeline_t *pipeline, const gfx_shader_statement_ir_t *stmt,
				  u32 expr, vec4f_t input_position, vec4f_t input_color, gfx_software_value_t *out)
{
	if (frame == NULL || pipeline == NULL || stmt == NULL || out == NULL || expr >= stmt->expr_count) { // LCOV_EXCL_LINE
		return 1;										    // LCOV_EXCL_LINE
	}

	const gfx_shader_expr_ir_t *node = &stmt->expr_nodes[expr];
	switch (node->kind) {
	case GFX_SHADER_EXPR_INT:
	case GFX_SHADER_EXPR_FLOAT: {
		float value = 0.0f;
		if (gfx_software_parse_float(node->text, &value)) { // LCOV_EXCL_LINE
			return 1;				    // LCOV_EXCL_LINE
		}
		*out = (gfx_software_value_t){.kind = GFX_SOFTWARE_VALUE_FLOAT, .f = value};
		return 0;
	}
	case GFX_SHADER_EXPR_LVALUE: {
		if (strv_eq(node->text, STRV("input.position"))) {
			*out = (gfx_software_value_t){.kind = GFX_SOFTWARE_VALUE_VEC4, .v4 = input_position};
			return 0;
		}
		if (strv_eq(node->text, STRV("input.color"))) {
			*out = (gfx_software_value_t){.kind = GFX_SOFTWARE_VALUE_VEC4, .v4 = input_color};
			return 0;
		}
		if (gfx_software_strv_suffix(node->text, STRV(".x")) || gfx_software_strv_suffix(node->text, STRV(".y")) ||
		    gfx_software_strv_suffix(node->text, STRV(".z")) || gfx_software_strv_suffix(node->text, STRV(".w"))) {
			strv_t base			    = STRVN(node->text.data, node->text.len - 2);
			gfx_software_value_t value	    = {0};
			gfx_shader_expr_ir_t base_node	    = {.kind = GFX_SHADER_EXPR_LVALUE, .text = base};
			gfx_shader_statement_ir_t base_stmt = *stmt;
			if (base_stmt.expr_count >= 64) { // LCOV_EXCL_LINE
				return 1;		  // LCOV_EXCL_LINE
			}
			u32 base_expr			= base_stmt.expr_count++;
			base_stmt.expr_nodes[base_expr] = base_node;
			if (gfx_software_eval_expr(frame, pipeline, &base_stmt, base_expr, input_position, input_color, &value)) {
				return 1;
			}
			return gfx_software_value_component(value, node->text.data[node->text.len - 1], out);
		}
		return gfx_software_uniform_member(frame, pipeline, node->text, out);
	}
	case GFX_SHADER_EXPR_CALL: {
		if (!strv_eq(node->text, STRV("vec4f")) || node->arg_count != 4) {
			return 1;
		}
		float args[4] = {0};
		for (u32 i = 0; i < 4; i++) {
			gfx_software_value_t value = {0};
			if (gfx_software_eval_expr(frame, pipeline, stmt, node->args[i], input_position, input_color, &value) ||
			    value.kind != GFX_SOFTWARE_VALUE_FLOAT) {
				return 1;
			}
			args[i] = value.f;
		}
		*out = (gfx_software_value_t){.kind = GFX_SOFTWARE_VALUE_VEC4, .v4 = vec4f(args[0], args[1], args[2], args[3])};
		return 0;
	}
	case GFX_SHADER_EXPR_BINARY: {
		gfx_software_value_t left  = {0};
		gfx_software_value_t right = {0};
		if (!strv_eq(node->op, STRV("*")) ||
		    gfx_software_eval_expr(frame, pipeline, stmt, node->left, input_position, input_color, &left) ||
		    gfx_software_eval_expr(frame, pipeline, stmt, node->right, input_position, input_color, &right)) {
			return 1;
		}
		if (left.kind == GFX_SOFTWARE_VALUE_MAT4 && right.kind == GFX_SOFTWARE_VALUE_MAT4) {
			*out = (gfx_software_value_t){.kind = GFX_SOFTWARE_VALUE_MAT4, .m4 = mat4f_mul(left.m4, right.m4)};
			return 0;
		}
		if (left.kind == GFX_SOFTWARE_VALUE_MAT4 && right.kind == GFX_SOFTWARE_VALUE_VEC4) {
			*out = (gfx_software_value_t){.kind = GFX_SOFTWARE_VALUE_VEC4, .v4 = mat4f_mul_vec4(left.m4, right.v4)};
			return 0;
		}
		return 1;
	}
	default: {	  // LCOV_EXCL_LINE
		return 1; // LCOV_EXCL_LINE
	}
	}
}

static int gfx_software_run_vertex_shader(const gfx_frame_t *frame, const gfx_software_pipeline_t *pipeline, vec4f_t input_position,
					  vec4f_t input_color, gfx_software_vertex_t *out)
{
	if (frame == NULL || pipeline == NULL || out == NULL) { // LCOV_EXCL_LINE
		return 1;					// LCOV_EXCL_LINE
	}

	vec4f_t position = input_position;
	vec4f_t color	 = input_color;
	if (pipeline->shader) {
		for (u32 i = 0; i < pipeline->ir.vertex.statement_count; i++) {
			const gfx_shader_statement_ir_t *stmt = &pipeline->ir.vertex.statements[i];
			if (stmt->kind != GFX_SHADER_STMT_ASSIGN) {
				continue;
			}
			gfx_software_value_t value = {0};
			if (gfx_software_eval_expr(frame, pipeline, stmt, stmt->expr_root, input_position, input_color, &value) ||
			    value.kind != GFX_SOFTWARE_VALUE_VEC4) {
				return 1;
			}
			if (strv_eq(stmt->lhs, STRV("output.position"))) {
				position = value.v4;
			} else if (strv_eq(stmt->lhs, STRV("output.color"))) {
				color = value.v4;
			}
		}
	}

	*out = (gfx_software_vertex_t){
		.x = position.x,
		.y = position.y,
		.z = position.z,
		.w = position.w,
		.r = color.x,
		.g = color.y,
		.b = color.z,
		.a = color.w,
	};
	return 0;
}

static int gfx_software_fetch_vertex(const gfx_frame_t *frame, const gfx_software_pipeline_t *pipeline, const gfx_software_buffer_t *buffer,
				     u32 index, gfx_software_vertex_t *out)
{
	if (frame == NULL || pipeline == NULL || buffer == NULL || out == NULL || pipeline->stride == 0 ||
	    buffer->buf.used < pipeline->stride * ((size_t)index + 1)) {
		return 1;
	}
	const u8 *vertex = (const u8 *)buffer->buf.data + pipeline->stride * (size_t)index;
	const float *pos = (const float *)(const void *)(vertex + pipeline->position.offset);
	const float *col = (const float *)(const void *)(vertex + pipeline->color.offset);

	vec4f_t input_position =
		vec4f(pos[0], pipeline->position.count > 1 ? pos[1] : 0.0f, pipeline->position.count > 2 ? pos[2] : 0.0f, 1.0f);
	vec4f_t input_color = vec4f(col[0],
				    pipeline->color.count > 1 ? col[1] : 0.0f,
				    pipeline->color.count > 2 ? col[2] : 0.0f,
				    pipeline->color.count > 3 ? col[3] : 1.0f);
	return gfx_software_run_vertex_shader(frame, pipeline, input_position, input_color, out);
}

static int gfx_software_init(gfx_t *gfx, const gfx_config_t *config)
{
	if (gfx == NULL || config == NULL || gfx->alloc.alloc == NULL) {
		return 1;
	}

	gfx_software_t *render = alloc_alloc(&gfx->alloc, sizeof(gfx_software_t));
	if (render == NULL) {
		return 1;
	}
	*render	  = (gfx_software_t){0};
	gfx->data = render;
	return 0;
}

static int gfx_software_free(gfx_t *gfx)
{
	if (gfx == NULL || gfx->data == NULL) {
		return 1;
	}

	gfx_software_t *render = gfx->data;
	if (render->depth != NULL) {
		alloc_free(&gfx->alloc, render->depth, render->depth_size * sizeof(float));
	}
	alloc_free(&gfx->alloc, render, sizeof(gfx_software_t));
	gfx->data = NULL;
	return 0;
}

static int gfx_software_image_set(gfx_t *gfx, gfx_image_t *image)
{
	gfx_software_t *render = gfx->data;
	switch (image->origin) {
	case GFX_IMAGE_ORIGIN_NONE: {
		render->image	  = (gfx_image_t){0};
		render->swapchain = NULL;
		return 0;
	}
	case GFX_IMAGE_ORIGIN_SURFACE: {
		if (!surface_image_valid(image)) {
			return 1;
		}
		if (image->driver_data == NULL && gfx_software_image_init(image)) {
			return 1;
		}

		gfx_software_surface_target_t *surface_target = image->driver_data;

		render->image = (gfx_image_t){
			.gfx	= image->gfx,
			.origin = GFX_IMAGE_ORIGIN_MEMORY,
			.format = surface_target->memory.format,
			.data	= surface_target->memory.data,
			.width	= surface_target->memory.width,
			.height = surface_target->memory.height,
			.stride = surface_target->memory.stride,
		};
		if (!image_valid(&render->image)) {
			return 1;
		}
		render->swapchain = image->swapchain;
		break;
	}
	case GFX_IMAGE_ORIGIN_MEMORY: {
		if (!image_valid(image)) {
			return 1;
		}
		render->image	  = *image;
		render->swapchain = NULL;
		break;
	}
	default: {
		return 1;
	}
	}

	render->viewport_x	= 0;
	render->viewport_y	= 0;
	render->viewport_width	= render->image.width;
	render->viewport_height = render->image.height;
	return 0;
}

static int gfx_software_image_init(gfx_image_t *image)
{
	if (image == NULL || image->gfx == NULL) {
		return 1;
	}
	if (image->origin == GFX_IMAGE_ORIGIN_MEMORY) {
		return !image_valid(image);
	}
	if (!surface_image_valid(image)) {
		return 1;
	}

	gfx_software_surface_target_t *surface_target = alloc_alloc(&image->gfx->alloc, sizeof(gfx_software_surface_target_t));
	if (surface_target == NULL) {
		return 1;
	}
	gfx_surface_memory_t memory = {
		.format = image->format,
		.data	= image->data,
		.width	= image->width,
		.height = image->height,
		.stride = image->stride,
	};
	if (image->swapchain->surface->ops->memory(image->swapchain->surface, &memory)) {
		alloc_free(&image->gfx->alloc, surface_target, sizeof(gfx_software_surface_target_t));
		return 1;
	}
	surface_target->memory = memory;
	if (!image_valid(&(gfx_image_t){
		    .origin = GFX_IMAGE_ORIGIN_MEMORY,
		    .format = memory.format,
		    .data   = memory.data,
		    .width  = memory.width,
		    .height = memory.height,
		    .stride = memory.stride,
	    })) {
		alloc_free(&image->gfx->alloc, surface_target, sizeof(gfx_software_surface_target_t));
		return 1;
	}
	image->driver_data = surface_target;
	return 0;
}

static void gfx_software_image_free(gfx_image_t *image)
{
	if (image == NULL || image->gfx == NULL || image->gfx->data == NULL) {
		return;
	}

	gfx_software_t *render = image->gfx->data;
	if (image->origin == GFX_IMAGE_ORIGIN_SURFACE && image->driver_data != NULL) {
		alloc_free(&image->gfx->alloc, image->driver_data, sizeof(gfx_software_surface_target_t));
		image->driver_data = NULL;
	}
	if (render->image.swapchain == image->swapchain || render->image.data == image->data) {
		render->image	  = (gfx_image_t){0};
		render->swapchain = NULL;
	}
}

static int gfx_software_swapchain_init(gfx_swapchain_t *swapchain, const gfx_swapchain_config_t *config)
{
	(void)config;

	if (swapchain == NULL || swapchain->surface == NULL || swapchain->surface->api != GFX_API_SOFTWARE ||
	    swapchain->surface->ops == NULL || swapchain->surface->ops->memory == NULL || swapchain->width == 0 || swapchain->height == 0) {
		return 1;
	}
	swapchain->actual_present_mode = GFX_PRESENT_MODE_IMMEDIATE;
	return 0;
}

static void gfx_software_swapchain_free(gfx_swapchain_t *swapchain)
{
	if (swapchain == NULL || swapchain->images == NULL || swapchain->gfx == NULL) {
		return;
	}
	for (u32 i = 0; i < swapchain->image_capacity; i++) {
		gfx_software_image_free(&swapchain->images[i]);
	}
}

static int gfx_software_swapchain_resize(gfx_swapchain_t *swapchain, u16 width, u16 height)
{
	if (swapchain == NULL || swapchain->surface == NULL || swapchain->surface->ops == NULL || swapchain->surface->ops->memory == NULL ||
	    width == 0 || height == 0) {
		return 1;
	}

	swapchain->width  = width;
	swapchain->height = height;
	return 0;
}

static int gfx_software_swapchain_present(gfx_swapchain_t *swapchain)
{
	if (swapchain == NULL || swapchain->surface == NULL || swapchain->surface->ops == NULL ||
	    swapchain->surface->ops->present == NULL) {
		return 1;
	}

	return swapchain->surface->ops->present(swapchain->surface, swapchain->actual_present_mode);
}

static void gfx_software_clear(gfx_software_t *render, gfx_color_t color)
{
	gfx_software_color_layout_t layout = gfx_software_color_layout(render->image.format);
	u8 clear[4]			   = {
		       color_u8(color.r),
		       color_u8(color.g),
		       color_u8(color.b),
		       color_u8(color.a),
	       };
	for (u16 y = 0; y < render->image.height; y++) {
		u8 *row = (u8 *)render->image.data + (size_t)y * render->image.stride;
		for (u16 x = 0; x < render->image.width; x++) {
			u8 *pixel	= row + (size_t)x * 4;
			pixel[layout.r] = clear[0];
			pixel[layout.g] = clear[1];
			pixel[layout.b] = clear[2];
			pixel[layout.a] = clear[3];
		}
	}
}

static int gfx_software_image_read(gfx_image_t *image, const gfx_memory_readback_config_t *config)
{
	if (image == NULL || image->gfx == NULL || image->gfx->data == NULL || config == NULL) {
		return 1;
	}

	gfx_software_t *render = image->gfx->data;
	if (!image_valid(&render->image)) {
		return 1;
	}

	for (u16 y = 0; y < render->image.height; y++) {
		u8 *dst	      = (u8 *)config->data + (size_t)y * config->stride;
		const u8 *src = (const u8 *)render->image.data + (size_t)y * render->image.stride;
		mem_copy(dst, config->stride, src, (size_t)render->image.width * 4);
	}
	return 0;
}

static int gfx_software_framebuffer_pass_begin(gfx_framebuffer_t *framebuffer, gfx_frame_t *frame)
{
	if (frame == NULL || frame->gfx == NULL || frame->gfx->data == NULL || framebuffer == NULL || framebuffer->image == NULL ||
	    framebuffer->render_pass == NULL) {
		return 1;
	}

	if (gfx_software_image_set(frame->gfx, framebuffer->image)) {
		return 1;
	}

	gfx_software_t *render	= frame->gfx->data;
	render->viewport_x	= frame->pass.viewport.x;
	render->viewport_y	= frame->pass.viewport.y;
	render->viewport_width	= frame->pass.viewport.width;
	render->viewport_height = frame->pass.viewport.height;
	if (framebuffer->render_pass->load == GFX_LOAD_CLEAR) {
		gfx_software_clear(render, frame->pass.clear);
	}
	if (framebuffer->render_pass->depth_format == GFX_FORMAT_NONE) {
		if (render->depth != NULL) {
			alloc_free(&frame->gfx->alloc, render->depth, render->depth_size * sizeof(float));
			render->depth	   = NULL;
			render->depth_size = 0;
		}
		return 0;
	}

	size_t depth_size = (size_t)render->image.width * render->image.height;
	if (render->depth_size != depth_size) {
		float *depth = alloc_alloc(&frame->gfx->alloc, depth_size * sizeof(float));
		if (depth == NULL) {
			return 1;
		}
		if (render->depth != NULL) {
			alloc_free(&frame->gfx->alloc, render->depth, render->depth_size * sizeof(float));
		}
		render->depth	   = depth;
		render->depth_size = depth_size;
	}
	if (framebuffer->render_pass->depth_load == GFX_LOAD_CLEAR) {
		for (size_t i = 0; i < render->depth_size; i++) {
			render->depth[i] = frame->pass.clear_depth;
		}
	}
	return 0;
}

static float edge(const gfx_software_vertex_t *a, const gfx_software_vertex_t *b, float x, float y)
{
	return (x - a->x) * (b->y - a->y) - (y - a->y) * (b->x - a->x);
}

static int point_inside(float w0, float w1, float w2, float area)
{
	if (area > 0.0f) {
		return w0 >= 0.0f && w1 >= 0.0f && w2 >= 0.0f;
	}
	return w0 <= 0.0f && w1 <= 0.0f && w2 <= 0.0f;
}

static int triangle_culled(const gfx_pipeline_t *pipeline, float area)
{
	if (pipeline->raster.cull == GFX_CULL_NONE) {
		return 0;
	}

	int front = pipeline->raster.front_face == GFX_WINDING_CLOCKWISE ? area < 0.0f : area > 0.0f;
	return (pipeline->raster.cull == GFX_CULL_FRONT && front) || (pipeline->raster.cull == GFX_CULL_BACK && !front);
}

static void draw_pixel(gfx_software_t *render, u16 x, u16 y, const u8 color[4])
{
	gfx_software_color_layout_t layout = gfx_software_color_layout(render->image.format);
	u8 *row				   = (u8 *)render->image.data + (size_t)y * render->image.stride;
	u8 *pixel			   = row + (size_t)x * 4;
	pixel[layout.r]			   = color[0];
	pixel[layout.g]			   = color[1];
	pixel[layout.b]			   = color[2];
	pixel[layout.a]			   = color[3];
}

static float float_abs_local(float value)
{
	return value < 0.0f ? -value : value;
}

static int line_steps(const gfx_software_vertex_t *a, const gfx_software_vertex_t *b)
{
	float dx = float_abs_local(b->x - a->x);
	float dy = float_abs_local(b->y - a->y);
	float d	 = dx > dy ? dx : dy;
	return d < 1.0f ? 1 : (int)d;
}

static int pixel_in_viewport(const gfx_software_t *render, int x, int y)
{
	u32 x1 = (u32)render->viewport_x + render->viewport_width;
	u32 y1 = (u32)render->viewport_y + render->viewport_height;
	if (x1 > render->image.width) {
		x1 = render->image.width;
	}
	if (y1 > render->image.height) {
		y1 = render->image.height;
	}
	return x >= render->viewport_x && y >= render->viewport_y && x < (int)x1 && y < (int)y1;
}

static void raster_line(gfx_software_t *render, const gfx_frame_t *frame, const gfx_software_vertex_t *a, const gfx_software_vertex_t *b)
{
	int depth_test	= frame->pipeline->depth.test && frame->render_pass->depth_format != GFX_FORMAT_NONE && render->depth != NULL;
	int depth_write = frame->pipeline->depth.write && frame->render_pass->depth_format != GFX_FORMAT_NONE && render->depth != NULL;
	int steps	= line_steps(a, b);
	for (int i = 0; i <= steps; i++) {
		float t = (float)i / (float)steps;
		int x	= (int)(a->x + (b->x - a->x) * t);
		int y	= (int)(a->y + (b->y - a->y) * t);
		if (!pixel_in_viewport(render, x, y)) {
			continue;
		}

		float z = a->z + (b->z - a->z) * t;
		if (depth_test || depth_write) {
			size_t index = (size_t)y * render->image.width + (u16)x;
			if (depth_test && z >= render->depth[index]) {
				continue;
			}
			if (depth_write) {
				render->depth[index] = z;
			}
		}
		u8 color[4] = {
			color_u8(a->r + (b->r - a->r) * t),
			color_u8(a->g + (b->g - a->g) * t),
			color_u8(a->b + (b->b - a->b) * t),
			color_u8(a->a + (b->a - a->a) * t),
		};
		draw_pixel(render, (u16)x, (u16)y, color);
	}
}

static float clip_distance(const gfx_software_vertex_t *vertex, u32 plane)
{
	switch (plane) {
	case 0:
		return vertex->x + vertex->w;
	case 1:
		return vertex->w - vertex->x;
	case 2:
		return vertex->y + vertex->w;
	case 3:
		return vertex->w - vertex->y;
	case 4:
		return vertex->z + vertex->w;
	default:
		return vertex->w - vertex->z;
	}
}

static gfx_software_vertex_t vertex_lerp(const gfx_software_vertex_t *a, const gfx_software_vertex_t *b, float t)
{
	return (gfx_software_vertex_t){
		.x = a->x + (b->x - a->x) * t,
		.y = a->y + (b->y - a->y) * t,
		.z = a->z + (b->z - a->z) * t,
		.w = a->w + (b->w - a->w) * t,
		.r = a->r + (b->r - a->r) * t,
		.g = a->g + (b->g - a->g) * t,
		.b = a->b + (b->b - a->b) * t,
		.a = a->a + (b->a - a->a) * t,
	};
}

static u32 clip_polygon_plane(gfx_software_vertex_t *out, const gfx_software_vertex_t *in, u32 count, u32 plane)
{
	u32 out_count			  = 0;
	const gfx_software_vertex_t *prev = &in[count - 1];
	float prev_distance		  = clip_distance(prev, plane);
	int prev_inside			  = prev_distance >= 0.0f;
	for (u32 i = 0; i < count; i++) {
		const gfx_software_vertex_t *cur = &in[i];
		float cur_distance		 = clip_distance(cur, plane);
		int cur_inside			 = cur_distance >= 0.0f;
		if (cur_inside != prev_inside && out_count < GFX_SOFTWARE_MAX_CLIP_VERTICES) {
			float t		 = prev_distance / (prev_distance - cur_distance);
			out[out_count++] = vertex_lerp(prev, cur, t);
		}
		if (cur_inside && out_count < GFX_SOFTWARE_MAX_CLIP_VERTICES) {
			out[out_count++] = *cur;
		}
		prev	      = cur;
		prev_distance = cur_distance;
		prev_inside   = cur_inside;
	}
	return out_count;
}

static u32 clip_triangle(gfx_software_vertex_t *out, const gfx_software_vertex_t *vertices)
{
	gfx_software_vertex_t a[GFX_SOFTWARE_MAX_CLIP_VERTICES] = {vertices[0], vertices[1], vertices[2]};
	gfx_software_vertex_t b[GFX_SOFTWARE_MAX_CLIP_VERTICES] = {0};
	gfx_software_vertex_t *src				= a;
	gfx_software_vertex_t *dst				= b;
	u32 count						= 3;
	for (u32 plane = 0; plane < 6 && count != 0; plane++) {
		count			   = clip_polygon_plane(dst, src, count, plane);
		gfx_software_vertex_t *tmp = src;
		src			   = dst;
		dst			   = tmp;
	}
	for (u32 i = 0; i < count; i++) {
		out[i] = src[i];
	}
	return count;
}

static void vertex_to_screen(gfx_software_vertex_t *out, const gfx_software_vertex_t *vertex, const gfx_software_t *render)
{
	float inv_w = vertex->w != 0.0f ? 1.0f / vertex->w : 0.0f;
	float x	    = vertex->x * inv_w;
	float y	    = vertex->y * inv_w;
	float z	    = vertex->z * inv_w;
	*out	    = (gfx_software_vertex_t){
		       .x = (float)render->viewport_x + (x + 1.0f) * 0.5f * (float)render->viewport_width,
		       .y = (float)render->viewport_y + (1.0f - y) * 0.5f * (float)render->viewport_height,
		       .z = (z + 1.0f) * 0.5f,
		       .w = 1.0f,
		       .r = vertex->r,
		       .g = vertex->g,
		       .b = vertex->b,
		       .a = vertex->a,
	       };
}

static void raster_triangle(gfx_software_t *render, const gfx_frame_t *frame, const gfx_software_vertex_t *src_vertices)
{
	gfx_software_vertex_t vertices[3];
	for (u32 i = 0; i < 3; i++) {
		vertex_to_screen(&vertices[i], &src_vertices[i], render);
	}

	float area = edge(&vertices[0], &vertices[1], vertices[2].x, vertices[2].y);
	if (area == 0.0f) {
		return;
	}
	if (triangle_culled(frame->pipeline, area)) {
		return;
	}
	if (frame->pipeline->raster.fill == GFX_FILL_WIREFRAME) {
		raster_line(render, frame, &vertices[0], &vertices[1]);
		raster_line(render, frame, &vertices[1], &vertices[2]);
		raster_line(render, frame, &vertices[2], &vertices[0]);
		return;
	}

	u16 x0 = render->viewport_x;
	u16 y0 = render->viewport_y;
	u32 x1 = (u32)x0 + render->viewport_width;
	u32 y1 = (u32)y0 + render->viewport_height;
	if (x1 > render->image.width) {
		x1 = render->image.width;
	}
	if (y1 > render->image.height) {
		y1 = render->image.height;
	}

	int depth_test	= frame->pipeline->depth.test && frame->render_pass->depth_format != GFX_FORMAT_NONE && render->depth != NULL;
	int depth_write = frame->pipeline->depth.write && frame->render_pass->depth_format != GFX_FORMAT_NONE && render->depth != NULL;
	for (u16 y = y0; y < y1; y++) {
		for (u16 x = x0; x < x1; x++) {
			float px = (float)x + 0.5f;
			float py = (float)y + 0.5f;
			float w0 = edge(&vertices[1], &vertices[2], px, py);
			float w1 = edge(&vertices[2], &vertices[0], px, py);
			float w2 = edge(&vertices[0], &vertices[1], px, py);
			if (!point_inside(w0, w1, w2, area)) {
				continue;
			}

			float inv_area = 1.0f / area;
			w0 *= inv_area;
			w1 *= inv_area;
			w2 *= inv_area;
			float z = vertices[0].z * w0 + vertices[1].z * w1 + vertices[2].z * w2;
			if (depth_test || depth_write) {
				size_t index = (size_t)y * render->image.width + x;
				if (depth_test && z >= render->depth[index]) {
					continue;
				}
				if (depth_write) {
					render->depth[index] = z;
				}
			}
			u8 color[4] = {
				color_u8(vertices[0].r * w0 + vertices[1].r * w1 + vertices[2].r * w2),
				color_u8(vertices[0].g * w0 + vertices[1].g * w1 + vertices[2].g * w2),
				color_u8(vertices[0].b * w0 + vertices[1].b * w1 + vertices[2].b * w2),
				color_u8(vertices[0].a * w0 + vertices[1].a * w1 + vertices[2].a * w2),
			};
			draw_pixel(render, x, y, color);
		}
	}
}

static void draw_triangle(gfx_software_t *render, const gfx_frame_t *frame, const gfx_software_vertex_t *src_vertices)
{
	gfx_software_vertex_t clipped[GFX_SOFTWARE_MAX_CLIP_VERTICES] = {0};
	u32 count						      = clip_triangle(clipped, src_vertices);
	for (u32 i = 1; i + 1 < count; i++) {
		gfx_software_vertex_t triangle[3] = {clipped[0], clipped[i], clipped[i + 1]};
		raster_triangle(render, frame, triangle);
	}
}

static void gfx_software_buffer_free(gfx_buffer_t *buffer)
{
	if (buffer == NULL || buffer->gfx == NULL || buffer->data == NULL) {
		return;
	}

	gfx_software_buffer_t *sw_buffer = buffer->data;

	if (sw_buffer->buf.data != NULL) {
		buf_free(&sw_buffer->buf);
		sw_buffer->buf.data = NULL;
	}
	alloc_free(&buffer->gfx->alloc, sw_buffer, sizeof(gfx_software_buffer_t));
	buffer->data = NULL;
}

static int gfx_software_buffer_init(gfx_buffer_t *buffer, const gfx_buffer_config_t *config)
{
	if (buffer == NULL || buffer->gfx == NULL || config == NULL ||
	    (config->type != GFX_BUFFER_VERTEX && config->type != GFX_BUFFER_INDEX && config->type != GFX_BUFFER_UNIFORM)) {
		return 1;
	}

	gfx_software_buffer_t *sw_buffer = alloc_alloc(&buffer->gfx->alloc, sizeof(gfx_software_buffer_t));
	if (sw_buffer == NULL) {
		return 1;
	}
	*sw_buffer   = (gfx_software_buffer_t){0};
	buffer->data = sw_buffer;

	if (config->size != 0) {
		if (buf_init(&sw_buffer->buf, config->size, buffer->gfx->alloc) == NULL) {
			gfx_software_buffer_free(buffer);
			return 1;
		}
		buffer->size = config->size;
	}
	if (config->data != NULL) {
		if (buf_set(&sw_buffer->buf, 0, config->size, config->data)) {
			gfx_software_buffer_free(buffer); // LCOV_EXCL_LINE
			return 1;			  // LCOV_EXCL_LINE
		}
		sw_buffer->buf.used = config->size;
	}

	return 0;
}

static int gfx_software_buffer_set_data(gfx_buffer_t *buffer, const void *data, size_t size)
{
	if (buffer == NULL || buffer->gfx == NULL || buffer->data == NULL || data == NULL || size == 0 ||
	    buffer->usage == GFX_BUFFER_USAGE_STATIC) {
		return 1;
	}

	gfx_software_buffer_t *sw_buffer = buffer->data;
	if (sw_buffer->buf.data == NULL) {
		if (buf_init(&sw_buffer->buf, size, buffer->gfx->alloc) == NULL) {
			return 1;
		}
		buffer->size = size;
	} else if (size > buffer->size) {
		if (buf_resize(&sw_buffer->buf, size)) {
			return 1;
		}
		buffer->size = size;
	}

	if (buf_set(&sw_buffer->buf, 0, size, data)) {
		return 1;
	}
	sw_buffer->buf.used = size;

	return 0;
}

static int gfx_software_buffer_bind(gfx_frame_t *frame, const gfx_buffer_t *buffer)
{
	if (frame == NULL || frame->gfx == NULL || frame->gfx->data == NULL || buffer == NULL || buffer->data == NULL) {
		return 1;
	}

	return 0;
}

static int gfx_software_bind_resources(gfx_frame_t *frame, const gfx_resource_binding_t *bindings, u32 binding_count)
{
	if (frame == NULL || frame->gfx == NULL || frame->gfx->data == NULL || (bindings == NULL && binding_count != 0)) {
		return 1;
	}
	for (u32 i = 0; i < binding_count; i++) {
		const gfx_resource_binding_t *binding = &bindings[i];
		if (binding->type != GFX_RESOURCE_UNIFORM_BUFFER || binding->buffer == NULL || binding->buffer->gfx != frame->gfx ||
		    binding->buffer->type != GFX_BUFFER_UNIFORM || binding->buffer->data == NULL) {
			return 1;
		}
	}
	return 0;
}

static int gfx_software_shader_init(gfx_shader_t *shader, const gfx_shader_config_t *config)
{
	if (shader == NULL || shader->gfx == NULL || config == NULL) {
		return 1;
	}
	if (config->compiler == NULL) {
		return 0;
	}
	gfx_software_shader_t *sw_shader = alloc_alloc(&shader->gfx->alloc, sizeof(gfx_software_shader_t));
	if (sw_shader == NULL) {
		return 1;
	}
	*sw_shader   = (gfx_software_shader_t){.stage = config->stage};
	shader->data = sw_shader;
	if (gfx_shader_compiler_ir(config->compiler, config->source, &sw_shader->ir)) {
		alloc_free(&shader->gfx->alloc, sw_shader, sizeof(gfx_software_shader_t));
		shader->data = NULL;
		return 1;
	}
	return 0;
}

static void gfx_software_shader_free(gfx_shader_t *shader)
{
	if (shader == NULL || shader->gfx == NULL || shader->data == NULL) {
		return;
	}
	alloc_free(&shader->gfx->alloc, shader->data, sizeof(gfx_software_shader_t));
	shader->data = NULL;
}

static int gfx_software_pipeline_init(gfx_pipeline_t *pipeline, const gfx_pipeline_config_t *config)
{
	if (pipeline == NULL || pipeline->gfx == NULL || config == NULL) {
		return 1;
	}
	gfx_software_pipeline_t *sw_pipeline = alloc_alloc(&pipeline->gfx->alloc, sizeof(gfx_software_pipeline_t));
	if (sw_pipeline == NULL) {
		return 1;
	}
	*sw_pipeline = (gfx_software_pipeline_t){
		.position = {.offset = 0, .count = 2},
		.color	  = {.offset = sizeof(float) * 2, .count = 4},
		.stride	  = sizeof(gfx_vertex_2d_t),
	};
	if (config->input_layout != NULL || config->input_layout_size != 0) {
		if (config->input_layout == NULL || config->input_layout_size == 0 ||
		    config->input_layout_size % sizeof(gfx_layout_t) != 0 ||
		    config->input_layout_size / sizeof(gfx_layout_t) > GFX_SOFTWARE_MAX_ATTRIBUTES) {
			alloc_free(&pipeline->gfx->alloc, sw_pipeline, sizeof(gfx_software_pipeline_t));
			return 1;
		}
		u32 count	      = (u32)(config->input_layout_size / sizeof(gfx_layout_t));
		sw_pipeline->position = (gfx_software_attribute_t){0};
		sw_pipeline->color    = (gfx_software_attribute_t){0};
		sw_pipeline->stride   = 0;
		for (u32 i = 0; i < count; i++) {
			const gfx_layout_t *layout = &config->input_layout[i];
			if (layout->type != GFX_VALUE_FLOAT32 || layout->count == 0 || layout->count > 4 ||
			    sw_pipeline->stride > SIZE_MAX - sizeof(float) * layout->count) {
				alloc_free(&pipeline->gfx->alloc, sw_pipeline, sizeof(gfx_software_pipeline_t));
				return 1;
			}
			if (gfx_software_layout_semantic(layout, STRV("POSITION"))) {
				sw_pipeline->position = (gfx_software_attribute_t){.offset = sw_pipeline->stride, .count = layout->count};
			} else if (gfx_software_layout_semantic(layout, STRV("COLOR0"))) {
				sw_pipeline->color = (gfx_software_attribute_t){.offset = sw_pipeline->stride, .count = layout->count};
			}
			sw_pipeline->stride += sizeof(float) * layout->count;
		}
		if (sw_pipeline->position.count < 2 || sw_pipeline->position.count > 3 || sw_pipeline->color.count != 4) {
			alloc_free(&pipeline->gfx->alloc, sw_pipeline, sizeof(gfx_software_pipeline_t));
			return 1;
		}
	}
	if (config->vs.data != NULL) {
		const gfx_software_shader_t *shader = config->vs.data;
		sw_pipeline->ir			    = shader->ir;
		sw_pipeline->shader		    = 1;
	}
	pipeline->data = sw_pipeline;
	return 0;
}

static void gfx_software_pipeline_free(gfx_pipeline_t *pipeline)
{
	if (pipeline == NULL || pipeline->gfx == NULL || pipeline->data == NULL) {
		return;
	}
	alloc_free(&pipeline->gfx->alloc, pipeline->data, sizeof(gfx_software_pipeline_t));
	pipeline->data = NULL;
}

static int gfx_software_pipeline_bind(gfx_frame_t *frame, const gfx_pipeline_t *pipeline)
{
	if (frame == NULL || pipeline == NULL || pipeline->data == NULL) {
		return 1;
	}
	return 0;
}

static int gfx_software_draw(gfx_frame_t *frame, u32 vertex_count, u32 first_vertex)
{
	if (frame == NULL || frame->gfx == NULL || frame->gfx->data == NULL || frame->vertex_buffer == NULL ||
	    frame->vertex_buffer->data == NULL || frame->pipeline == NULL || frame->pipeline->data == NULL || vertex_count < 3) {
		return 1;
	}

	gfx_software_t *render = frame->gfx->data;
	if (!image_valid(&render->image) || render->viewport_width == 0 || render->viewport_height == 0) {
		return 1;
	}

	const gfx_software_pipeline_t *pipeline = frame->pipeline->data;
	const gfx_software_buffer_t *sw_buffer	= frame->vertex_buffer->data;
	for (u32 i = 0; i + 2 < vertex_count; i += 3) {
		gfx_software_vertex_t triangle[3] = {0};
		if (gfx_software_fetch_vertex(frame, pipeline, sw_buffer, first_vertex + i, &triangle[0]) ||
		    gfx_software_fetch_vertex(frame, pipeline, sw_buffer, first_vertex + i + 1, &triangle[1]) ||
		    gfx_software_fetch_vertex(frame, pipeline, sw_buffer, first_vertex + i + 2, &triangle[2])) {
			return 1;
		}
		draw_triangle(render, frame, triangle);
	}

	return 0;
}

static int gfx_software_draw_indexed(gfx_frame_t *frame, u32 index_count)
{
	if (frame == NULL || frame->gfx == NULL || frame->gfx->data == NULL || frame->vertex_buffer == NULL ||
	    frame->vertex_buffer->data == NULL || frame->index_buffer == NULL || frame->index_buffer->data == NULL ||
	    frame->pipeline == NULL || frame->pipeline->data == NULL || index_count < 3) {
		return 1;
	}

	gfx_software_t *render = frame->gfx->data;
	if (!image_valid(&render->image) || render->viewport_width == 0 || render->viewport_height == 0) {
		return 1;
	}

	const gfx_software_pipeline_t *pipeline	   = frame->pipeline->data;
	const gfx_software_buffer_t *vertex_buffer = frame->vertex_buffer->data;
	const gfx_software_buffer_t *index_buffer  = frame->index_buffer->data;
	const u32 *indices			   = index_buffer->buf.data;
	size_t vertex_count			   = vertex_buffer->buf.used / pipeline->stride;
	if (index_buffer->buf.used < sizeof(u32) * (size_t)index_count) {
		return 1;
	}

	for (u32 i = 0; i + 2 < index_count; i += 3) {
		if (indices[i] >= vertex_count || indices[i + 1] >= vertex_count || indices[i + 2] >= vertex_count) {
			return 1;
		}

		gfx_software_vertex_t triangle[3] = {0};
		if (gfx_software_fetch_vertex(frame, pipeline, vertex_buffer, indices[i], &triangle[0]) ||
		    gfx_software_fetch_vertex(frame, pipeline, vertex_buffer, indices[i + 1], &triangle[1]) ||
		    gfx_software_fetch_vertex(frame, pipeline, vertex_buffer, indices[i + 2], &triangle[2])) {
			return 1;
		}
		draw_triangle(render, frame, triangle);
	}
	return 0;
}

static int gfx_software_end(gfx_frame_t *frame)
{
	if (frame == NULL || frame->gfx == NULL || frame->gfx->data == NULL) {
		return 1;
	}

	return 0;
}

static gfx_driver_t gfx_software = {
	.name			= "software",
	.api			= GFX_API_SOFTWARE,
	.init			= gfx_software_init,
	.free			= gfx_software_free,
	.swapchain_init		= gfx_software_swapchain_init,
	.swapchain_free		= gfx_software_swapchain_free,
	.swapchain_resize	= gfx_software_swapchain_resize,
	.swapchain_present	= gfx_software_swapchain_present,
	.image_init		= gfx_software_image_init,
	.image_free		= gfx_software_image_free,
	.image_read		= gfx_software_image_read,
	.framebuffer_pass_begin = gfx_software_framebuffer_pass_begin,
	.buffer_init		= gfx_software_buffer_init,
	.buffer_free		= gfx_software_buffer_free,
	.buffer_set_data	= gfx_software_buffer_set_data,
	.buffer_bind		= gfx_software_buffer_bind,
	.bind_resources		= gfx_software_bind_resources,
	.shader_init		= gfx_software_shader_init,
	.shader_free		= gfx_software_shader_free,
	.pipeline_init		= gfx_software_pipeline_init,
	.pipeline_free		= gfx_software_pipeline_free,
	.pipeline_bind		= gfx_software_pipeline_bind,
	.draw			= gfx_software_draw,
	.draw_indexed		= gfx_software_draw_indexed,
	.end			= gfx_software_end,
};

GFX_DRIVER(gfx_software, &gfx_software);
