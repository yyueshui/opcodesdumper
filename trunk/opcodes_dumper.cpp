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
  | Author: Steven Van Ingelgem  <steven@salvania.be>                    |
  +----------------------------------------------------------------------+
   $Id$
 */


#include <cstdio>
#include <string>
#include <sstream>
#include "./opcodes.h"
#include <Zend/zend_compile.h>
#include <sapi/embed/php_embed.h>


#define BUFFER_LEN 40


using namespace std;


string str_replace(const string &search, const string &replace, const string &subject)
{
  string buffer;

  int sealeng = search.length();
  int strleng = subject.length();

  if (sealeng==0)
    return subject;//no change

  for(int i=0, j=0; i<strleng; j=0 )
  {
    while (i+j<strleng && j<sealeng && subject[i+j]==search[j])
      j++;
    if (j==sealeng)//found 'search'
    {
      buffer.append(replace);
      i+=sealeng;
    }
    else
    {
      buffer.append( &subject[i++], 1);
    }
  }
  return buffer;
}

string format_zval(zval *z) {
	static char buffer[BUFFER_LEN];

	switch(z->type) {
    case IS_BOOL:
      if ( z->value.lval == 1 ) return "true";
      else return "false";

		case IS_LONG:
      snprintf(buffer, BUFFER_LEN, "%d", z->value.lval);
      return buffer;

		case IS_DOUBLE:
			snprintf(buffer, BUFFER_LEN, "%f", z->value.dval);
			return buffer;

    case IS_STRING:
      {
        string new_el(z->value.str.val);

        new_el = new_el.substr(0, BUFFER_LEN-2); // Optimize replacements for large strings
        new_el = str_replace("\\", "\\\\", new_el);
        new_el = str_replace("\"", "\\\"", new_el);
        new_el = str_replace("$", "\\$", new_el);
        new_el = str_replace("\n", "\\n", new_el);
        new_el = str_replace("\r", "\\r", new_el);
        new_el = str_replace("\t", "\\t", new_el);
        new_el = str_replace("\b", "\\b", new_el);
        new_el = str_replace("\v", "\\v", new_el);

        return ("\"" + new_el.substr(0, BUFFER_LEN-2) + "\"").c_str();
      }

    case IS_NULL:           return "NULL";
		case IS_ARRAY:          return "Array()";
		case IS_OBJECT:         return "Object";
		case IS_RESOURCE:       return "resource";
    case IS_CONSTANT:       return "constant";
		case IS_CONSTANT_ARRAY: return "const Array()";
		default:                return "unknown";
	}
}

string format_znode(znode *n){
	static char buffer[BUFFER_LEN];

	switch (n->op_type) {
		case IS_CONST:
			return format_zval(&n->u.constant);

    case IS_VAR:
			snprintf(buffer, BUFFER_LEN, "$%d",  n->u.var);
			return buffer;

    case IS_TMP_VAR:
      snprintf(buffer, BUFFER_LEN, "~%d",  n->u.var);
      return buffer;

    case IS_UNUSED:
      snprintf(buffer, BUFFER_LEN, ":%d",  n->u.var);
      return buffer;

    case IS_CV:
      snprintf(buffer, BUFFER_LEN, "%%%d",  n->u.var);
      return buffer;

    default:
			return "";
			break;
	}
}

void dump_op(zend_op *op, int num){
	printf("%5d  %5d %30s % 40s % 40s % 40s\n", num, op->lineno,
			get_opname(op->opcode),
			format_znode(&op->op1).c_str(),
			format_znode(&op->op2).c_str(),
			format_znode(&op->result).c_str());
}

void dump_var_array(zend_op_array *op_array) {
  if ( !op_array ) {
    return;
  }

  printf("Variables:\n");
  printf("----------\n");
  for ( int i = 0; i < op_array->last_var; ++i ) {
    printf("%%%d: '$%s'\n", i, op_array->vars[i].name);
  }
  printf("\n");
}

void dump_op_array(zend_op_array *op_array) {
  if ( !op_array ) {
    return;
  }

  printf("%5s  %5s %30s % 40s % 40s % 40s\n", "opnum", "line", "opcode", "op1", "op2", "result");
  printf("%5s  %5s %30s % 40s % 40s % 40s\n", "-----", "-----", "------------------------------", "----------------------------------------", "----------------------------------------", "----------------------------------------");
	for(zend_uint i = 0; i < op_array->last; ++i) {
		dump_op(&op_array->opcodes[i], i);
	}
}

int main(int argc, char **argv){
	if(argc != 2) {
		printf("usage:  op_dumper <script>\n");
		return 1;
	}
	printf("Script:\n-------\n%s\n\n", argv[1]);

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
  dump_var_array(op_array);
	dump_op_array(op_array);

	PHP_EMBED_END_BLOCK();

	return 0;
}
