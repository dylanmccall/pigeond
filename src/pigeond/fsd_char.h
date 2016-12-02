#ifndef _FSD_CHAR_H
#define _FSD_CHAR_H

/**
 * fsd_char.h
 * Convert character strings to bit fields for a multi-segment display
 * @author Dylan McCall <dmccall@sfu.ca>
 */

#include <stdbool.h>
#include <stdlib.h>

typedef struct {
    const char char_code;
    unsigned char pixel_values_top;
    unsigned char pixel_values_bottom;
} FSDChar;

const FSDChar *get_fsd_char(const char char_code);
int str_to_fsd_chars(const char *char_string, const FSDChar **out_fsd_chars, size_t length);
int strn_to_fsd_chars(const char *char_string, size_t char_string_length, const FSDChar **out_fsd_chars, size_t length);

#endif
