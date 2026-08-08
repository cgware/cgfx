#include "ebnf.h"
#include "eprs.h"
#include "gfx_shader_driver.h"
#include "log.h"

static const char *gfx_shader_stage_name(gfx_shader_stage_t stage)
{
	if (stage == GFX_SHADER_STAGE_VERTEX) {
		return "vertex";
	}
	if (stage == GFX_SHADER_STAGE_FRAGMENT) {
		return "fragment";
	}
	return "unknown";
}

static const char *gfx_shader_language_name(gfx_shader_language_t language)
{
	if (language == GFX_SHADER_LANGUAGE_GLSL) {
		return "GLSL 330";
	}
	if (language == GFX_SHADER_LANGUAGE_HLSL) {
		return "HLSL";
	}
	if (language == GFX_SHADER_LANGUAGE_SPIRV) {
		return "SPIR-V";
	}
	return "unknown";
}

static const char *gfx_shader_input_ebnf =
	"program = item+ EOF\n"
	"item = vs_in_struct | vs_out_struct | fs_in_struct | fs_out_struct | buffer_struct | function_definition | function_prototype | "
	"textures_declaration\n"
	"textures_declaration = 'textures ' identifier '[' int '];' NL\n"
	"vs_in_struct = 'vs_in ' int ' ' identifier ' {' NL struct_mem+ '}' NL\n"
	"vs_out_struct = 'vs_out ' identifier ' {' NL struct_mem+ '}' NL\n"
	"fs_in_struct = 'fs_in ' identifier ' {' NL struct_mem+ '}' NL\n"
	"fs_out_struct = 'fs_out ' identifier ' {' NL struct_mem+ '}' NL\n"
	"buffer_struct = 'buffer ' int ' ' identifier ' {' NL buffer_mem* '}' NL\n"
	"buffer_mem = TAB type_name ' ' identifier ';' NL\n"
	"struct_mem = TAB type_name ' ' identifier semantic? ';' NL\n"
	"semantic = ' : ' identifier\n"
	"function_prototype = function_header ';' NL\n"
	"function_definition = function_header ' {' NL statement* '}' NL\n"
	"function_header = type_name ' ' identifier '(' parameters? ')'\n"
	"parameters = parameter (', ' parameter)*\n"
	"parameter = type_name ' ' identifier\n"
	"statement = TAB (return_statement | declaration_expression | assignment_expression | function_call_statement)\n"
	"return_statement = 'return ' expression ';' NL\n"
	"declaration_expression = type_name ' ' identifier initialization? ';' NL\n"
	"initialization = ' = ' expression\n"
	"assignment_expression = lvalue ' ' assignment_operator ' ' expression ';' NL\n"
	"function_call_statement = function_call ';' NL\n"
	"expression = value (spaces expression_operator spaces value)*\n"
	"value = float | int | function_call | lvalue\n"
	"function_call = identifier '(' arguments? ')'\n"
	"arguments = expression (', ' expression)*\n"
	"lvalue = identifier ('.' identifier)*\n"
	"type_name = identifier\n"
	"identifier = ALPHA (ALPHA | DIGIT | '_')*\n"
	"int = DIGIT+\n"
	"float = DIGIT+ '.' DIGIT+ 'f'\n"
	"spaces = ' '*\n"
	"expression_operator = '+' | '-' | '*' | '/' | '<' | '>' | '==' | '>=' | '<='\n"
	"assignment_operator = '=' | '+=' | '-=' | '*=' | '/='\n";

static int gfx_shader_compile_grammar_cleanup(int ret, estx_t *estx, ebnf_t *ebnf, lex_t *lex, prs_t *prs, int estx_initialized,
					      int ebnf_initialized, int lex_initialized, int prs_initialized)
{
	if (ret && estx_initialized) {
		estx_free(estx); // LCOV_EXCL_LINE
	}
	if (prs_initialized) {
		prs_free(prs);
	}
	if (lex_initialized) {
		lex_free(lex);
	}
	if (ebnf_initialized) {
		ebnf_free(ebnf);
	}
	return ret;
}

static int gfx_shader_compile_grammar(estx_t *estx, gfx_shader_rules_t *rules, alloc_t alloc)
{
	ebnf_t ebnf	     = {0};
	lex_t lex	     = {0};
	prs_t prs	     = {0};
	int ebnf_initialized = 0;
	int lex_initialized  = 0;
	int prs_initialized  = 0;
	int estx_initialized = 0;

	if (estx == NULL || rules == NULL) {
		return 1; // LCOV_EXCL_LINE
	}
	if (ebnf_init(&ebnf, alloc) == NULL) {
		return gfx_shader_compile_grammar_cleanup(
			1, estx, &ebnf, &lex, &prs, estx_initialized, ebnf_initialized, lex_initialized, prs_initialized);
	}
	ebnf_initialized = 1;
	if (lex_init(&lex, 0, 4096, alloc) == NULL) {
		return gfx_shader_compile_grammar_cleanup( // LCOV_EXCL_LINE
			1,
			estx,
			&ebnf,
			&lex,
			&prs,
			estx_initialized,
			ebnf_initialized,
			lex_initialized,
			prs_initialized);
	}
	lex_initialized = 1;
	if (prs_init(&prs, 4096, alloc) == NULL) {
		return gfx_shader_compile_grammar_cleanup( // LCOV_EXCL_LINE
			1,
			estx,
			&ebnf,
			&lex,
			&prs,
			estx_initialized,
			ebnf_initialized,
			lex_initialized,
			prs_initialized);
	}
	prs_initialized = 1;
	if (estx_init(estx, 4096, alloc) == NULL) {
		return gfx_shader_compile_grammar_cleanup( // LCOV_EXCL_LINE
			1,
			estx,
			&ebnf,
			&lex,
			&prs,
			estx_initialized,
			ebnf_initialized,
			lex_initialized,
			prs_initialized);
	}
	estx_initialized = 1;

	log_info("cgfx", "gfx_shader", NULL, "shader grammar: loading EBNF syntax");
	if (ebnf_get_stx(&ebnf, alloc, DST_NONE()) == NULL) {
		return gfx_shader_compile_grammar_cleanup( // LCOV_EXCL_LINE
			1,
			estx,
			&ebnf,
			&lex,
			&prs,
			estx_initialized,
			ebnf_initialized,
			lex_initialized,
			prs_initialized);
	}
	log_info("cgfx", "gfx_shader", NULL, "shader grammar: loaded EBNF syntax");

	if (lex_tokenize(&lex, strv_cstr(gfx_shader_input_ebnf), STRV("gfx_shader_ebnf"), 0)) {
		return gfx_shader_compile_grammar_cleanup( // LCOV_EXCL_LINE
			1,
			estx,
			&ebnf,
			&lex,
			&prs,
			estx_initialized,
			ebnf_initialized,
			lex_initialized,
			prs_initialized);
	}

	prs_node_t prs_root = 0;
	estx_node_t root    = 0;
	log_info("cgfx", "gfx_shader", NULL, "shader grammar: parsing EBNF grammar");
	if (prs_parse(&prs, &lex, &ebnf.stx, ebnf.file, &prs_root, DST_NONE())) {
		return gfx_shader_compile_grammar_cleanup( // LCOV_EXCL_LINE
			1,
			estx,
			&ebnf,
			&lex,
			&prs,
			estx_initialized,
			ebnf_initialized,
			lex_initialized,
			prs_initialized);
	}
	log_info("cgfx", "gfx_shader", NULL, "shader grammar: parsed EBNF grammar");

	if (estx_from_ebnf(&ebnf, &prs, prs_root, estx, &root)) {
		return gfx_shader_compile_grammar_cleanup( // LCOV_EXCL_LINE
			1,
			estx,
			&ebnf,
			&lex,
			&prs,
			estx_initialized,
			ebnf_initialized,
			lex_initialized,
			prs_initialized);
	}

	if (estx_find_rule(estx, STRV("program"), &rules->program) || estx_find_rule(estx, STRV("item"), &rules->item) ||
	    estx_find_rule(estx, STRV("vs_in_struct"), &rules->vs_in_struct) ||
	    estx_find_rule(estx, STRV("vs_out_struct"), &rules->vs_out_struct) ||
	    estx_find_rule(estx, STRV("fs_in_struct"), &rules->fs_in_struct) ||
	    estx_find_rule(estx, STRV("fs_out_struct"), &rules->fs_out_struct) ||
	    estx_find_rule(estx, STRV("struct_mem"), &rules->struct_mem) ||
	    estx_find_rule(estx, STRV("function_definition"), &rules->function_definition) ||
	    estx_find_rule(estx, STRV("function_header"), &rules->function_header) ||
	    estx_find_rule(estx, STRV("statement"), &rules->statement) ||
	    estx_find_rule(estx, STRV("return_statement"), &rules->return_statement) ||
	    estx_find_rule(estx, STRV("declaration_expression"), &rules->declaration_expression) ||
	    estx_find_rule(estx, STRV("initialization"), &rules->initialization) ||
	    estx_find_rule(estx, STRV("assignment_expression"), &rules->assignment_expression) ||
	    estx_find_rule(estx, STRV("assignment_operator"), &rules->assignment_operator) ||
	    estx_find_rule(estx, STRV("expression"), &rules->expression) || estx_find_rule(estx, STRV("lvalue"), &rules->lvalue) ||
	    estx_find_rule(estx, STRV("type_name"), &rules->type_name) || estx_find_rule(estx, STRV("identifier"), &rules->identifier) ||
	    estx_find_rule(estx, STRV("semantic"), &rules->semantic)) {
		return gfx_shader_compile_grammar_cleanup( // LCOV_EXCL_LINE
			1,
			estx,
			&ebnf,
			&lex,
			&prs,
			estx_initialized,
			ebnf_initialized,
			lex_initialized,
			prs_initialized);
	}
	return gfx_shader_compile_grammar_cleanup(
		0, estx, &ebnf, &lex, &prs, estx_initialized, ebnf_initialized, lex_initialized, prs_initialized);
}

gfx_shader_compiler_t *gfx_shader_compiler_init(gfx_shader_compiler_t *compiler, alloc_t alloc)
{
	if (compiler == NULL) {
		return NULL;
	}

	compiler->alloc = alloc;

	log_info("cgfx", "gfx_shader_compiler", NULL, "compiling shader grammar");
	if (gfx_shader_compile_grammar(&compiler->estx, &compiler->rules, compiler->alloc)) {
		log_error("cgfx", "gfx_shader", NULL, "failed to create shader compiler: grammar compilation failed");
		return NULL;
	}
	log_info("cgfx", "gfx_shader_compiler", NULL, "compiled shader grammar");

	return compiler;
}

void gfx_shader_compiler_free(gfx_shader_compiler_t *compiler)
{
	if (compiler == NULL) {
		return;
	}

	estx_free(&compiler->estx);
}

static int gfx_shader_parse_source(strv_t source, const estx_t *estx, const gfx_shader_rules_t *rules, alloc_t alloc, lex_t *lex,
				   eprs_t *eprs, eprs_node_t *root)
{
	if (source.data == NULL || source.len == 0 || estx == NULL || rules == NULL || lex == NULL || eprs == NULL || root == NULL) {
		return 1; // LCOV_EXCL_LINE
	}
	if (lex_init(lex, 0, (uint)(source.len + 1), alloc) == NULL) {
		return 1;
	}
	if (eprs_init(eprs, 4096, alloc) == NULL) {
		lex_free(lex);
		return 1;
	}
	if (lex_tokenize(lex, source, STRV("shader"), 0)) {
		eprs_free(eprs); // LCOV_EXCL_LINE
		lex_free(lex);	 // LCOV_EXCL_LINE
		return 1;	 // LCOV_EXCL_LINE
	}
	if (eprs_parse(eprs, lex, estx, rules->program, root, DST_NONE())) {
		eprs_free(eprs);
		lex_free(lex);
		return 1;
	}
	return 0;
}

static int gfx_shader_node_text(const eprs_t *eprs, const lex_t *lex, eprs_node_t node, strv_t *out)
{
	tok_t tok = {0};
	if (eprs == NULL || lex == NULL || out == NULL || eprs_get_str(eprs, node, &tok)) {
		return 1; // LCOV_EXCL_LINE
	}
	*out = lex_get_tok_val(lex, tok);
	return out->data == NULL || out->len == 0;
}

static int gfx_shader_child_rule(const eprs_t *eprs, eprs_node_t parent, estx_node_t rule, u32 index, eprs_node_t *out)
{
	if (eprs == NULL || out == NULL) {
		return 1; // LCOV_EXCL_LINE
	}

	eprs_node_t child;
	const void *data;
	tree_foreach_child(&eprs->nodes, parent, child, data)
	{
		(void)data;
		if (eprs_get_rule(eprs, child, rule, NULL) == 0) {
			if (index == 0) {
				*out = child;
				return 0;
			}
			index--;
		}
	}
	return 1;
}

static int gfx_shader_child_text(const eprs_t *eprs, const lex_t *lex, eprs_node_t parent, estx_node_t rule, u32 index, strv_t *out)
{
	eprs_node_t node = 0;
	if (gfx_shader_child_rule(eprs, parent, rule, index, &node)) {
		return 1; // LCOV_EXCL_LINE
	}
	return gfx_shader_node_text(eprs, lex, node, out);
}

static int gfx_shader_parse_struct(const eprs_t *eprs, const lex_t *lex, const gfx_shader_rules_t *rules, eprs_node_t node,
				   gfx_shader_struct_ir_t *ir)
{
	if (ir == NULL || gfx_shader_child_text(eprs, lex, node, rules->identifier, 0, &ir->name)) {
		return 1; // LCOV_EXCL_LINE
	}

	ir->present = 1;

	eprs_node_t child;
	const void *data;
	tree_foreach_child(&eprs->nodes, node, child, data)
	{
		(void)data;
		eprs_node_t member_node = 0;
		if (eprs_get_rule(eprs, child, rules->struct_mem, &member_node) || ir->member_count >= 16) {
			continue;
		}

		gfx_shader_member_t *member = &ir->members[ir->member_count];
		if (gfx_shader_child_text(eprs, lex, member_node, rules->type_name, 0, &member->type) ||
		    gfx_shader_child_text(eprs, lex, member_node, rules->identifier, 1, &member->name)) {
			return 1; // LCOV_EXCL_LINE
		}

		eprs_node_t semantic = 0;
		if (gfx_shader_child_rule(eprs, member_node, rules->semantic, 0, &semantic) == 0 &&
		    gfx_shader_child_text(eprs, lex, semantic, rules->identifier, 0, &member->semantic)) {
			return 1; // LCOV_EXCL_LINE
		}
		ir->member_count++;
	}

	return ir->member_count == 0;
}

static int gfx_shader_parse_statement(const eprs_t *eprs, const lex_t *lex, const gfx_shader_rules_t *rules, eprs_node_t node,
				      gfx_shader_statement_ir_t *stmt)
{
	eprs_node_t child = 0;
	if (eprs_get_rule(eprs, node, rules->declaration_expression, &child) == 0) {
		*stmt = (gfx_shader_statement_ir_t){.kind = GFX_SHADER_STMT_DECL};
		if (gfx_shader_child_text(eprs, lex, child, rules->type_name, 0, &stmt->type) ||
		    gfx_shader_child_text(eprs, lex, child, rules->identifier, 1, &stmt->name)) {
			return 1; // LCOV_EXCL_LINE
		}
		eprs_node_t init = 0;
		if (gfx_shader_child_rule(eprs, child, rules->initialization, 0, &init) == 0) {
			stmt->has_init = 1;
			return gfx_shader_child_text(eprs, lex, init, rules->expression, 0, &stmt->expr);
		}
		return 0;
	}
	if (eprs_get_rule(eprs, node, rules->assignment_expression, &child) == 0) {
		*stmt = (gfx_shader_statement_ir_t){.kind = GFX_SHADER_STMT_ASSIGN};
		return gfx_shader_child_text(eprs, lex, child, rules->lvalue, 0, &stmt->lhs) ||
		       gfx_shader_child_text(eprs, lex, child, rules->assignment_operator, 0, &stmt->op) ||
		       gfx_shader_child_text(eprs, lex, child, rules->expression, 0, &stmt->expr);
	}
	if (eprs_get_rule(eprs, node, rules->return_statement, &child) == 0) {
		*stmt = (gfx_shader_statement_ir_t){.kind = GFX_SHADER_STMT_RETURN};
		return gfx_shader_child_text(eprs, lex, child, rules->expression, 0, &stmt->expr);
	}
	return 1; // LCOV_EXCL_LINE
}

static int gfx_shader_parse_function(const eprs_t *eprs, const lex_t *lex, const gfx_shader_rules_t *rules, eprs_node_t node,
				     gfx_shader_ir_t *ir)
{
	eprs_node_t header = 0;
	strv_t ret	   = STRV_NULL;
	strv_t name	   = STRV_NULL;
	if (gfx_shader_child_rule(eprs, node, rules->function_header, 0, &header) ||
	    gfx_shader_child_text(eprs, lex, header, rules->type_name, 0, &ret) ||
	    gfx_shader_child_text(eprs, lex, header, rules->identifier, 1, &name)) {
		return 1; // LCOV_EXCL_LINE
	}

	gfx_shader_function_ir_t fn = {.present = 1, .ret = ret, .name = name};
	eprs_node_t child;
	const void *data;
	tree_foreach_child(&eprs->nodes, node, child, data)
	{
		(void)data;
		eprs_node_t statement = 0;
		if (eprs_get_rule(eprs, child, rules->statement, &statement) || fn.statement_count >= 32) {
			continue;
		}
		if (gfx_shader_parse_statement(eprs, lex, rules, statement, &fn.statements[fn.statement_count])) {
			return 1; // LCOV_EXCL_LINE
		}
		fn.statement_count++;
	}
	if (fn.statement_count == 0) {
		return 1; // LCOV_EXCL_LINE
	}

	if (strv_eq(name, STRV("vertex"))) {
		ir->vertex = fn;
	} else if (strv_eq(name, STRV("fragment"))) {
		ir->fragment = fn;
	}
	return 0;
}

static int gfx_shader_build_ir(const eprs_t *eprs, const lex_t *lex, const gfx_shader_rules_t *rules, eprs_node_t root, gfx_shader_ir_t *ir)
{
	if (eprs == NULL || lex == NULL || rules == NULL || ir == NULL) {
		return 1; // LCOV_EXCL_LINE
	}

	*ir = (gfx_shader_ir_t){0};

	eprs_node_t item;
	const void *data;
	tree_foreach_child(&eprs->nodes, root, item, data)
	{
		(void)data;
		eprs_node_t node = 0;
		if (eprs_get_rule(eprs, item, rules->vs_in_struct, &node) == 0) {
			if (gfx_shader_parse_struct(eprs, lex, rules, node, &ir->vs_in)) {
				return 1; // LCOV_EXCL_LINE
			}
		} else if (eprs_get_rule(eprs, item, rules->vs_out_struct, &node) == 0) {
			if (gfx_shader_parse_struct(eprs, lex, rules, node, &ir->vs_out)) {
				return 1; // LCOV_EXCL_LINE
			}
		} else if (eprs_get_rule(eprs, item, rules->fs_in_struct, &node) == 0) {
			if (gfx_shader_parse_struct(eprs, lex, rules, node, &ir->fs_in)) {
				return 1; // LCOV_EXCL_LINE
			}
		} else if (eprs_get_rule(eprs, item, rules->fs_out_struct, &node) == 0) {
			if (gfx_shader_parse_struct(eprs, lex, rules, node, &ir->fs_out)) {
				return 1; // LCOV_EXCL_LINE
			}
		} else if (eprs_get_rule(eprs, item, rules->function_definition, &node) == 0) {
			if (gfx_shader_parse_function(eprs, lex, rules, node, ir)) {
				return 1; // LCOV_EXCL_LINE
			}
		}
	}

	return 0;
}

static int gfx_shader_struct_has_semantic(const gfx_shader_struct_ir_t *ir, strv_t semantic, strv_t type)
{
	if (ir == NULL || !ir->present) {
		return 0; // LCOV_EXCL_LINE
	}

	for (u32 i = 0; i < ir->member_count; i++) {
		if (strv_eq(ir->members[i].semantic, semantic) && strv_eq(ir->members[i].type, type)) {
			return 1;
		}
	}
	return 0;
}

static int gfx_shader_ir_supported(const gfx_shader_ir_t *ir)
{
	return ir != NULL && ir->vs_in.present && ir->vs_out.present && ir->fs_in.present && ir->fs_out.present && ir->vertex.present &&
	       ir->fragment.present && gfx_shader_struct_has_semantic(&ir->vs_in, STRV("POSITION"), STRV("vec2f")) &&
	       gfx_shader_struct_has_semantic(&ir->vs_in, STRV("COLOR0"), STRV("vec4f")) &&
	       gfx_shader_struct_has_semantic(&ir->vs_out, STRV("POSITION"), STRV("vec4f")) &&
	       gfx_shader_struct_has_semantic(&ir->vs_out, STRV("COLOR0"), STRV("vec4f")) &&
	       gfx_shader_struct_has_semantic(&ir->fs_in, STRV("COLOR0"), STRV("vec4f")) &&
	       gfx_shader_struct_has_semantic(&ir->fs_out, STRV("COLOR0"), STRV("vec4f"));
}

static int gfx_shader_compiler_emit(gfx_shader_compiler_t *compiler, const eprs_t *eprs, const lex_t *lex, eprs_node_t root,
				    gfx_shader_stage_t stage, gfx_shader_language_t language, gfx_shader_code_t *shader)
{
	gfx_shader_ir_t ir = {0};
	if (gfx_shader_build_ir(eprs, lex, &compiler->rules, root, &ir)) {
		log_error("cgfx", // LCOV_EXCL_LINE
			  "gfx_shader",
			  NULL,
			  "failed to transpile %s shader to %s: IR build failed",
			  gfx_shader_stage_name(stage),
			  gfx_shader_language_name(language));
		return 1; // LCOV_EXCL_LINE
	}

	if (!gfx_shader_ir_supported(&ir)) {
		log_error("cgfx",
			  "gfx_shader",
			  NULL,
			  "failed to transpile %s shader to %s: IR is not supported by current backends",
			  gfx_shader_stage_name(stage),
			  gfx_shader_language_name(language));
		return 1;
	}

	*shader = (gfx_shader_code_t){
		.stage	  = stage,
		.language = language,
	};

	gfx_shader_driver_t *shader_driver = NULL;

	for (driver_t *i = DRIVER_START; i < DRIVER_END; i++) {
		if (i->type != GFX_SHADER_DRIVER_TYPE) {
			continue;
		}

		gfx_shader_driver_t *drv = i->data;
		if (drv != NULL && drv->language == language) {
			shader_driver = drv;
			break;
		}
	}

	int ret = 1;
	if (shader_driver == NULL) {
		log_error("cgfx",
			  "gfx_shader",
			  NULL,
			  "failed to transpile %s shader: unknown shader language %d",
			  gfx_shader_stage_name(stage),
			  (int)language);
	} else {
		ret = shader_driver->emit(&ir, stage, shader);
	}

	if (ret) {
		log_error("cgfx",
			  "gfx_shader",
			  NULL,
			  "failed to emit %s shader as %s",
			  gfx_shader_stage_name(stage),
			  gfx_shader_language_name(language));
	} else {
		log_info("cgfx",
			 "gfx_shader",
			 NULL,
			 "transpiled %s shader to %s",
			 gfx_shader_stage_name(stage),
			 gfx_shader_language_name(language));
	}
	return ret;
}

int gfx_shader_compiler_transpile(gfx_shader_compiler_t *compiler, strv_t source, gfx_shader_stage_t stage, gfx_shader_language_t language,
				  gfx_shader_code_t *shader)
{
	if (shader == NULL) {
		log_error("cgfx", "gfx_shader", NULL, "failed to transpile shader: output shader is null");
		return 1;
	}

	*shader = (gfx_shader_code_t){0};

	lex_t lex	 = {0};
	eprs_t eprs	 = {0};
	eprs_node_t root = 0;

	if (compiler == NULL) {
		log_error("cgfx",
			  "gfx_shader",
			  NULL,
			  "failed to transpile %s shader to %s: shader compiler is null",
			  gfx_shader_stage_name(stage),
			  gfx_shader_language_name(language));
		return 1;
	}

	if (gfx_shader_parse_source(source, &compiler->estx, &compiler->rules, compiler->alloc, &lex, &eprs, &root)) {
		log_error("cgfx",
			  "gfx_shader",
			  NULL,
			  "failed to transpile %s shader to %s: source parsing failed",
			  gfx_shader_stage_name(stage),
			  gfx_shader_language_name(language));
		return 1;
	}

	int ret = gfx_shader_compiler_emit(compiler, &eprs, &lex, root, stage, language, shader);
	eprs_free(&eprs);
	lex_free(&lex);
	if (ret) {
		gfx_shader_code_free(shader);
	}
	return ret;
}

void gfx_shader_code_free(gfx_shader_code_t *shader)
{
	if (shader == NULL) {
		return;
	}

	buf_free(&shader->code);
}
