#ifndef _BASE64_H
#define _BASE64_H

/**
 * base64.h
 * Base64 encode and decode.
 * @author Dylan McCall <dmccall@sfu.ca>
 */

#include <stdbool.h>
#include <stdlib.h>

typedef struct _Base64 Base64;

Base64 *base64_new();
void base64_free(Base64 *base64);

unsigned char *base64_encode(Base64 *base64, const unsigned char *data, size_t input_length, size_t *output_length);
unsigned char *base64_decode(Base64 *base64, const unsigned char *data, size_t input_length, size_t *output_length);

#endif
