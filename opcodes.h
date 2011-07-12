#ifndef _OPCODES__H
#define _OPCODES__H 1


#include <Zend/zend.h>


#ifdef __cplusplus
extern "C" {
#endif


const char* get_opname(zend_uchar opcode);


#ifdef __cplusplus
}
#endif


#endif /* _OPCODES__H */
