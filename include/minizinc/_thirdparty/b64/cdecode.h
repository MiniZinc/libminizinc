/*
cdecode.h - c header for a base64 decoding algorithm

This is part of the libb64 project, and has been placed in the public domain.
For details, see http://sourceforge.net/projects/libb64
*/

#ifndef BASE64_CDECODE_H
#define BASE64_CDECODE_H

#include "ccommon.h"

#define BASE64_CDEC_VER_MAJOR   2
#define BASE64_CDEC_VER_MINOR   0

typedef enum
{
	step_a, step_b, step_c, step_d
} base64_decodestep;

typedef struct
{
	base64_decodestep step;
	char plainchar;
} base64_decodestate;

extern void base64_init_decodestate(base64_decodestate* state_in);

extern size_t base64_decode_maxlength(size_t encode_len);

extern int base64_decode_value(signed char value_in);
extern size_t base64_decode_block(const char* code_in, const size_t length_in, void* plaintext_out, base64_decodestate* state_in);

#endif /* BASE64_CDECODE_H */
