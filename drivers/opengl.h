#ifndef OPENGL_H
#define OPENGL_H

#include <stddef.h>

/**
 * @defgroup opengl OpenGL
 * @ingroup graphics
 */

/**
 * @defgroup opengl_common Common
 * @ingroup opengl
 * @{
 */

typedef unsigned int GLenum;
typedef unsigned char GLboolean;
typedef unsigned int GLbitfield;
typedef int GLint;
typedef unsigned char GLubyte;
typedef unsigned int GLuint;
typedef int GLsizei;
typedef float GLfloat;
typedef char GLchar;
typedef ptrdiff_t GLsizeiptr;

typedef enum glBool_e {
	GL_FALSE = 0,
	GL_TRUE	 = 1,
} glBool_t;

typedef enum glDrawMode_e {
	GL_TRIANGLES = 0x0004,
} glDrawMode_t;

typedef enum glTextureTarget_e {
	GL_TEXTURE_2D = 0x0DE1,
} glTextureTarget_t;

typedef enum glDataType_e {
	GL_UNSIGNED_BYTE = 0x1401,
	GL_UNSIGNED_INT	 = 0x1405,
	GL_FLOAT	 = 0x1406,
} glDataType_t;

typedef enum glFormat_e {
	GL_DEPTH_COMPONENT = 0x1902,
	GL_RGBA		   = 0x1908,
} glFormat_t;

typedef enum glStringName_e {
	GL_VENDOR		    = 0x1F00,
	GL_RENDERER		    = 0x1F01,
	GL_VERSION		    = 0x1F02,
	GL_SHADING_LANGUAGE_VERSION = 0x8B8C,
} glStringName_t;

typedef enum glTexParameteriParam_e {
	GL_NEAREST	 = 0x2600,
	GL_CLAMP_TO_EDGE = 0x812F,
} glTexParameteriParam_t;

typedef enum glTexParameteriName_e {
	GL_TEXTURE_MAG_FILTER = 0x2800,
	GL_TEXTURE_MIN_FILTER = 0x2801,
	GL_TEXTURE_WRAP_S     = 0x2802,
	GL_TEXTURE_WRAP_T     = 0x2803,
} glTexParameteriName_t;

typedef enum glInternalFormat_e {
	GL_DEPTH_COMPONENT32F = 0x8CAC,
	GL_RGBA8	      = 0x8058,
} glInternalFormat_t;

typedef enum glBufferTarget_e {
	GL_ARRAY_BUFFER		= 0x8892,
	GL_ELEMENT_ARRAY_BUFFER = 0x8893,
	GL_UNIFORM_BUFFER	= 0x8A11,
} glBufferTarget_t;

typedef enum glBufferUsage_e {
	GL_STATIC_DRAW	= 0x88E4,
	GL_DYNAMIC_DRAW = 0x88E8,
} glBufferUsage_t;

typedef enum glShaderType_e {
	GL_FRAGMENT_SHADER = 0x8B30,
	GL_VERTEX_SHADER   = 0x8B31,
} glShaderType_t;

#define GL_INVALID_INDEX 0xFFFFFFFFu

typedef enum glParameterName_e {
	GL_COMPILE_STATUS = 0x8B81,
	GL_LINK_STATUS	  = 0x8B82,
} glParameterName_t;

typedef enum glFramebufferAttachment_e {
	GL_COLOR_ATTACHMENT0 = 0x8CE0,
	GL_DEPTH_ATTACHMENT  = 0x8D00,
} glFramebufferAttachment_t;

typedef enum glClearMask_e {
	GL_DEPTH_BUFFER_BIT = 0x00000100,
	GL_COLOR_BUFFER_BIT = 0x00004000,
} glClearMask_t;

typedef enum glCapability_e {
	GL_DEPTH_TEST = 0x0B71,
} glCapability_t;

typedef enum glCompareFunc_e {
	GL_LESS = 0x0201,
} glCompareFunc_t;

typedef enum glFramebufferStatus_e {
	GL_FRAMEBUFFER_COMPLETE = 0x8CD5,
} glFramebufferStatus_t;

typedef enum glFramebufferTarget_e {
	GL_FRAMEBUFFER = 0x8D40,
} glFramebufferTarget_t;

typedef enum glError_e {
	GL_NO_ERROR = 0,
} glError_t;

/**
 * @}
 * @defgroup opengl_framebuffer Framebuffer
 * @ingroup opengl
 * @{
 */

/**
 * @brief Generate framebuffer object names
 * @param[out] ids must be freed using PFN_glDeleteFramebuffers()
 * @see https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGenFramebuffers.xhtml
 */
typedef void (*PFN_glGenFramebuffers)(GLsizei n, GLuint *ids);

/**
 * @brief Delete framebuffer objects
 * @param[in] framebuffers created by PFN_glGenFramebuffers()
 * @see https://registry.khronos.org/OpenGL-Refpages/gl4/html/glDeleteFramebuffers.xhtml
 */
typedef void (*PFN_glDeleteFramebuffers)(GLsizei n, const GLuint *framebuffers);

/**
 * @brief Bind a framebuffer to a framebuffer target
 * @param[in] framebuffer created by PFN_glGenFramebuffers()
 * @see https://registry.khronos.org/OpenGL-Refpages/gl4/html/glBindFramebuffer.xhtml
 */
typedef void (*PFN_glBindFramebuffer)(GLenum target, GLuint framebuffer);

/**
 * @brief Check the completeness status of a framebuffer
 * @see https://registry.khronos.org/OpenGL-Refpages/gl4/html/glCheckFramebufferStatus.xhtml
 */
typedef GLenum (*PFN_glCheckFramebufferStatus)(GLenum target);

/**
 * @brief Attach a level of a texture object as a logical buffer to the currently bound framebuffer object
 * @see https://registry.khronos.org/OpenGL-Refpages/es3/html/glFramebufferTexture2D.xhtml
 */
typedef void (*PFN_glFramebufferTexture2D)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);

/**
 * @brief Read a block of pixels from the frame buffer
 * @see https://registry.khronos.org/OpenGL-Refpages/gl4/html/glReadPixels.xhtml
 */
typedef void (*PFN_glReadPixels)(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void *data);

/**
 * @}
 * @defgroup opengl_texture Texture
 * @ingroup opengl
 * @{
 */

/**
 * @brief Generate texture names
 * @param[out] textures must be freed using PFN_glDeleteTextures()
 * @see https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGenTextures.xhtml
 */
typedef void (*PFN_glGenTextures)(GLsizei n, GLuint *textures);

/**
 * @brief Delete named textures
 * @param[in] textures created by PFN_glGenTextures()
 * @see https://registry.khronos.org/OpenGL-Refpages/gl4/html/glDeleteTextures.xhtml
 */
typedef void (*PFN_glDeleteTextures)(GLsizei n, const GLuint *textures);

/**
 * @brief Bind a named texture to a texturing target
 * @param[in] texture created by PFN_glGenTextures()
 * @see https://registry.khronos.org/OpenGL-Refpages/gl4/html/glBindTexture.xhtml
 */
typedef void (*PFN_glBindTexture)(GLenum target, GLuint texture);

/**
 * @brief Set texture parameters
 * @see https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexParameter.xhtml
 */
typedef void (*PFN_glTexParameteri)(GLenum target, GLenum pname, GLint param);

/**
 * @brief Specify a two-dimensional texture image
 * @see https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml
 */
typedef void (*PFN_glTexImage2D)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border,
				 GLenum format, GLenum type, const void *data);

/**
 * @}
 * @defgroup opengl_shader Shader
 * @ingroup opengl
 * @{
 */

/**
 * @brief Creates a shader object
 * @return shader must be freed using PFN_glDeleteShader()
 * @see https://registry.khronos.org/OpenGL-Refpages/gl4/html/glCreateShader.xhtml
 */
typedef GLuint (*PFN_glCreateShader)(GLenum shaderType);

/**
 * @brief Deletes a shader object
 * @param[in] shader created by PFN_glCreateShader()
 * @see https://registry.khronos.org/OpenGL-Refpages/gl4/html/glDeleteShader.xhtml
 */
typedef void (*PFN_glDeleteShader)(GLuint shader);

/**
 * @brief Replaces the source code in a shader object
 * @param[in] shader created by PFN_glCreateShader()
 * @see https://registry.khronos.org/OpenGL-Refpages/gl4/html/glShaderSource.xhtml
 */
typedef void (*PFN_glShaderSource)(GLuint shader, GLsizei count, const GLchar **string, const GLint *length);

/**
 * @brief Compiles a shader object
 * @param[in] shader created by PFN_glCreateShader()
 * @see https://registry.khronos.org/OpenGL-Refpages/gl4/html/glCompileShader.xhtml
 */
typedef void (*PFN_glCompileShader)(GLuint shader);

/**
 * @brief Returns a parameter from a shader object
 * @param[in] shader created by PFN_glCreateShader()
 * @see https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGetShader.xhtml
 */
typedef void (*PFN_glGetShaderiv)(GLuint shader, GLenum pname, GLint *params);

/**
 * @brief Returns the information log for a shader object
 * @param[in] shader created by PFN_glCreateShader()
 * @see https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGetShaderInfoLog.xhtml
 */
typedef void (*PFN_glGetShaderInfoLog)(GLuint shader, GLsizei maxLength, GLsizei *length, GLchar *infoLog);

/**
 * @}
 * @defgroup opengl_program Program
 * @ingroup opengl
 * @{
 */

/**
 * @brief Creates a program object
 * @return shader must be freed using PFN_glDeleteProgram()
 * @see https://registry.khronos.org/OpenGL-Refpages/gl4/html/glCreateProgram.xhtml
 */
typedef GLuint (*PFN_glCreateProgram)(void);

/**
 * @brief Deletes a program object
 * @param[in] program created by PFN_glCreateProgram()
 * @see https://registry.khronos.org/OpenGL-Refpages/gl4/html/glDeleteProgram.xhtml
 */
typedef void (*PFN_glDeleteProgram)(GLuint program);

/**
 * @brief Attaches a shader object to a program object
 * @param[in] program created by PFN_glCreateProgram()
 * @param[in] shader created by PFN_glCreateShader()
 * @see https://registry.khronos.org/OpenGL-Refpages/gl4/html/glAttachShader.xhtml
 */
typedef void (*PFN_glAttachShader)(GLuint program, GLuint shader);

/**
 * @brief Links a program object
 * @param[in] program created by PFN_glCreateProgram()
 * @see https://registry.khronos.org/OpenGL-Refpages/gl4/html/glLinkProgram.xhtml
 */
typedef void (*PFN_glLinkProgram)(GLuint program);

/**
 * @brief Returns a parameter from a program object
 * @param[in] program created by PFN_glCreateProgram()
 * @see https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGetProgram.xhtml
 */
typedef void (*PFN_glGetProgramiv)(GLuint program, GLenum pname, GLint *params);

/**
 * @brief Returns the information log for a program object
 * @param[in] program created by PFN_glCreateProgram()
 * @see https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGetProgramInfoLog.xhtml
 */
typedef void (*PFN_glGetProgramInfoLog)(GLuint program, GLsizei maxLength, GLsizei *length, GLchar *infoLog);

/**
 * @brief Installs a program object as part of current rendering state
 * @param[in] program created by PFN_glCreateProgram()
 * @see https://registry.khronos.org/OpenGL-Refpages/gl4/html/glUseProgram.xhtml
 */
typedef void (*PFN_glUseProgram)(GLuint program);

/**
 * @brief Retrieve the index of a named uniform block
 * @param[in] program created by PFN_glCreateProgram()
 * @see https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGetUniformBlockIndex.xhtml
 */
typedef GLuint (*PFN_glGetUniformBlockIndex)(GLuint program, const GLchar *uniformBlockName);

/**
 * @brief Assign a binding point to an active uniform block
 * @param[in] program created by PFN_glCreateProgram()
 * @see https://registry.khronos.org/OpenGL-Refpages/gl4/html/glUniformBlockBinding.xhtml
 */
typedef void (*PFN_glUniformBlockBinding)(GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding);

/**
 * @}
 * @defgroup opengl_buffer Buffer
 * @ingroup opengl
 * @{
 */

/**
 * @brief Generate buffer object names
 * @param[out] buffers must be freed using PFN_glDeleteBuffers()
 * @see https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGenBuffers.xhtml
 */
typedef void (*PFN_glGenBuffers)(GLsizei n, GLuint *buffers);

/**
 * @brief Delete named buffer objects
 * @param[in] buffers created by PFN_glGenBuffers()
 * @see https://registry.khronos.org/OpenGL-Refpages/gl4/html/glDeleteBuffers.xhtml
 */
typedef void (*PFN_glDeleteBuffers)(GLsizei n, const GLuint *buffers);

/**
 * @brief Bind a named buffer object
 * @param[in] buffer created by PFN_glGenBuffers()
 * @see https://registry.khronos.org/OpenGL-Refpages/gl4/html/glBindBuffer.xhtml
 */
typedef void (*PFN_glBindBuffer)(GLenum target, GLuint buffer);

/**
 * @brief Bind a buffer object to an indexed buffer target
 * @param[in] buffer created by PFN_glGenBuffers()
 * @see https://registry.khronos.org/OpenGL-Refpages/gl4/html/glBindBufferBase.xhtml
 */
typedef void (*PFN_glBindBufferBase)(GLenum target, GLuint index, GLuint buffer);

/**
 * @brief Creates and initializes a buffer object's data store
 * @see https://registry.khronos.org/OpenGL-Refpages/gl4/html/glBufferData.xhtml
 */
typedef void (*PFN_glBufferData)(GLenum target, GLsizeiptr size, const void *data, GLenum usage);

/**
 * @brief Enable a generic vertex attribute array
 * @see https://registry.khronos.org/OpenGL-Refpages/gl4/html/glEnableVertexAttribArray.xhtml
 */
typedef void (*PFN_glEnableVertexAttribArray)(GLuint index);

/**
 * @brief Disable a generic vertex attribute array
 * @see https://registry.khronos.org/OpenGL-Refpages/gl4/html/glEnableVertexAttribArray.xhtml
 */
typedef void (*PFN_glDisableVertexAttribArray)(GLuint index);

/**
 * @brief Define an array of generic vertex attribute data
 * @see https://registry.khronos.org/OpenGL-Refpages/gl4/html/glVertexAttribPointer.xhtml
 */
typedef void (*PFN_glVertexAttribPointer)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);

/**
 * @}
 * @defgroup opengl_cmd Cmd
 * @ingroup opengl
 * @{
 */

/**
 * @brief Specify clear values for the color buffers
 * @see https://registry.khronos.org/OpenGL-Refpages/gl4/html/glClearColor.xhtml
 */
typedef void (*PFN_glClearColor)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);

/**
 * @brief Clear buffers to preset values
 * @see https://registry.khronos.org/OpenGL-Refpages/gl4/html/glClear.xhtml
 */
typedef void (*PFN_glClear)(GLbitfield mask);

/**
 * @brief Specify the clear value for the depth buffer
 * @see https://registry.khronos.org/OpenGL-Refpages/gl4/html/glClearDepth.xhtml
 */
typedef void (*PFN_glClearDepth)(double depth);

/**
 * @brief Enable server-side GL capability
 * @see https://registry.khronos.org/OpenGL-Refpages/gl4/html/glEnable.xhtml
 */
typedef void (*PFN_glEnable)(GLenum cap);

/**
 * @brief Disable server-side GL capability
 * @see https://registry.khronos.org/OpenGL-Refpages/gl4/html/glDisable.xhtml
 */
typedef void (*PFN_glDisable)(GLenum cap);

/**
 * @brief Specify depth comparison function
 * @see https://registry.khronos.org/OpenGL-Refpages/gl4/html/glDepthFunc.xhtml
 */
typedef void (*PFN_glDepthFunc)(GLenum func);

/**
 * @brief Set the viewport
 * @see https://registry.khronos.org/OpenGL-Refpages/gl4/html/glViewport.xhtml
 */
typedef void (*PFN_glViewport)(GLint x, GLint y, GLsizei width, GLsizei height);

/**
 * @brief Return error information
 * @see https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGetError.xhtml
 */
typedef GLenum (*PFN_glGetError)(void);

/**
 * @brief Return a string describing the current GL connection
 * @see https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGetString.xhtml
 */
typedef const GLubyte *(*PFN_glGetString)(GLenum name);

/**
 * @brief Render primitives from array data
 * @see https://registry.khronos.org/OpenGL-Refpages/gl4/html/glDrawArrays.xhtml
 */
typedef void (*PFN_glDrawArrays)(GLenum mode, GLint first, GLsizei count);

/**
 * @brief Render primitives from array data
 * @see https://registry.khronos.org/OpenGL-Refpages/gl4/html/glDrawElements.xhtml
 */
typedef void (*PFN_glDrawElements)(GLenum mode, GLsizei count, GLenum type, const void *indices);

/**
 * @}
 */

#endif
