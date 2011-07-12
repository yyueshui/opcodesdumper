/*
  +----------------------------------------------------------------------+
  | PHP Opcodes Dumper                                                   |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Laruence  <laruence@gmail.com>                               |
  +----------------------------------------------------------------------+
   $Id$
 */

#include <cstdio>
#include "./opcodes.h"
#include <Zend/zend_compile.h>
#include <sapi/embed/php_embed.h>
#define BUFFER_LEN 40



const char * format_zval(zval *z) {
	static char buffer[BUFFER_LEN];

	switch(z->type) {
		case IS_NULL:
			return "NULL";
		case IS_LONG:
		case IS_BOOL:
			snprintf(buffer, BUFFER_LEN, "%d", z->value.lval);
			return buffer;
		case IS_DOUBLE:
			snprintf(buffer, BUFFER_LEN, "%f", z->value.dval);
			return buffer;
		case IS_STRING:
			snprintf(buffer, BUFFER_LEN, "\"%s\"", z->value.str.val);
			return buffer;
		case IS_ARRAY:
		case IS_OBJECT:
		case IS_RESOURCE:
		case IS_CONSTANT:
		case IS_CONSTANT_ARRAY:
			return "";
		default:
			return "unknown";
	}
}

const char * format_znode(znode *n){
	static char buffer[BUFFER_LEN];

	switch (n->op_type) {
		case IS_CONST:
			return format_zval(&n->u.constant);
			break;
		case IS_VAR:
			snprintf(buffer, BUFFER_LEN, "$%d",  n->u.var);
			return buffer;
			break;
		case IS_TMP_VAR:
			snprintf(buffer, BUFFER_LEN, "~%d",  n->u.var);
			return buffer;
			break;
		default:
			return "";
			break;
	}
}

void dump_op(zend_op *op, int num){
	printf("%5d  %5d %30s %040s %040s %040s\n", num, op->lineno,
			get_opname(op->opcode),
			format_znode(&op->op1),
			format_znode(&op->op2),
			format_znode(&op->result)) ;
}

void dump_op_array(zend_op_array *op_array){
	if(op_array) {
		printf("%5s  %5s %30s %040s %040s %040s\n", "opnum", "line", "opcode", "op1", "op2", "result");
		for(zend_uint i = 0; i < op_array->last; i++) {
			dump_op(&op_array->opcodes[i], i);
		}
	}
}

int main(int argc, char **argv){
	if(argc != 2) {
		printf("usage:  op_dumper <script>\n");
		return 1;
	}
	printf("Script: %s\n", argv[1]);

	PHP_EMBED_START_BLOCK(argc,argv);

	zend_file_handle file_handle;
  memset(&file_handle, 0, sizeof(file_handle));

	file_handle.filename = argv[1];
	file_handle.free_filename = 0;
	file_handle.type = ZEND_HANDLE_FILENAME;
	file_handle.opened_path = NULL;

	zend_op_array *op_array = zend_compile_file(&file_handle, ZEND_INCLUDE TSRMLS_CC);
	if(!op_array) {
		printf("Error parsing script: %s\n", file_handle.filename);
		return 1;
	}
	dump_op_array(op_array);

	PHP_EMBED_END_BLOCK();

	return 0;
}
