#include "fsd_char.h"

#define TOP_BAR_LEFT 0x08
#define TOP_X_LEFT 0x40
#define TOP_MIDDLE 0x20
#define TOP_X_RIGHT 0x10
#define TOP_LEFT 0x80
#define TOP_TOP 0x04
#define TOP_RIGHT 0x02

#define BOTTOM_BAR_RIGHT 0x10
#define BOTTOM_X_RIGHT 0x08
#define BOTTOM_MIDDLE 0x04
#define BOTTOM_X_LEFT 0x02
#define BOTTOM_RIGHT 0x80
#define BOTTOM_BOTTOM 0x20
#define BOTTOM_LEFT 0x01

#define BOTTOM_DOT 0x40

const FSDChar FSD_CHARS[] = {
	{
		' ',
		0,
		0
	},
	{
		'0',
		TOP_LEFT | TOP_TOP | TOP_RIGHT,
		BOTTOM_RIGHT | BOTTOM_BOTTOM | BOTTOM_LEFT
	},
	{
		'1',
		TOP_X_RIGHT | TOP_RIGHT,
		BOTTOM_RIGHT
	},
	{
		'2',
		TOP_TOP | TOP_RIGHT | TOP_BAR_LEFT,
		BOTTOM_BAR_RIGHT | BOTTOM_LEFT | BOTTOM_BOTTOM
	},
	{
		'3',
		TOP_TOP | TOP_RIGHT,
		BOTTOM_BAR_RIGHT | BOTTOM_RIGHT | BOTTOM_BOTTOM
	},
	{
		'4',
		TOP_LEFT | TOP_BAR_LEFT | TOP_RIGHT,
		BOTTOM_BAR_RIGHT | BOTTOM_RIGHT
	},
	{
		'5',
		TOP_TOP | TOP_LEFT | TOP_BAR_LEFT,
		BOTTOM_BAR_RIGHT | BOTTOM_RIGHT | BOTTOM_BOTTOM
	},
	{
		'6',
		TOP_TOP | TOP_LEFT | TOP_BAR_LEFT,
		BOTTOM_BAR_RIGHT | BOTTOM_RIGHT | BOTTOM_BOTTOM | BOTTOM_LEFT
	},
	{
		'7',
		TOP_TOP | TOP_X_RIGHT,
		BOTTOM_X_LEFT
	},
	{
		'8',
		TOP_BAR_LEFT | TOP_LEFT | TOP_TOP | TOP_RIGHT,
		BOTTOM_BAR_RIGHT | BOTTOM_RIGHT | BOTTOM_BOTTOM | BOTTOM_LEFT
	},
	{
		'9',
		TOP_BAR_LEFT | TOP_LEFT | TOP_TOP | TOP_RIGHT,
		BOTTOM_BAR_RIGHT | BOTTOM_RIGHT
	},
	{
		'A',
		TOP_TOP | TOP_LEFT | TOP_RIGHT | TOP_BAR_LEFT,
		BOTTOM_BAR_RIGHT | BOTTOM_RIGHT | BOTTOM_LEFT
	},
	{
		'B',
		TOP_TOP | TOP_MIDDLE | TOP_RIGHT,
		BOTTOM_BAR_RIGHT | BOTTOM_MIDDLE | BOTTOM_RIGHT | BOTTOM_BOTTOM
	},
	{
		'C',
		TOP_TOP | TOP_LEFT,
		BOTTOM_LEFT | BOTTOM_BOTTOM
	},
	{
		'D',
		TOP_TOP | TOP_MIDDLE | TOP_RIGHT,
		BOTTOM_MIDDLE | BOTTOM_RIGHT | BOTTOM_BOTTOM
	},
	{
		'E',
		TOP_TOP | TOP_LEFT | TOP_BAR_LEFT,
		BOTTOM_BAR_RIGHT | BOTTOM_LEFT | BOTTOM_BOTTOM
	},
	{
		'F',
		TOP_TOP | TOP_LEFT | TOP_BAR_LEFT,
		BOTTOM_BAR_RIGHT | BOTTOM_LEFT
	},
	{
		'G',
		TOP_TOP | TOP_LEFT,
		BOTTOM_LEFT | BOTTOM_BOTTOM | BOTTOM_RIGHT | BOTTOM_BAR_RIGHT
	},
	{
		'H',
		TOP_LEFT | TOP_BAR_LEFT | TOP_RIGHT,
		BOTTOM_BAR_RIGHT | BOTTOM_LEFT | BOTTOM_RIGHT
	},
	{
		'I',
		TOP_TOP | TOP_MIDDLE,
		BOTTOM_MIDDLE | BOTTOM_BOTTOM
	},
	{
		'J',
		TOP_RIGHT,
		BOTTOM_RIGHT | BOTTOM_BOTTOM | BOTTOM_LEFT
	},
	{
		'K',
		TOP_LEFT | TOP_BAR_LEFT | TOP_X_RIGHT,
		BOTTOM_LEFT | BOTTOM_X_RIGHT
	},
	{
		'L',
		TOP_LEFT,
		BOTTOM_LEFT | BOTTOM_BOTTOM
	},
	{
		'M',
		TOP_LEFT | TOP_X_LEFT | TOP_X_RIGHT | TOP_RIGHT,
		BOTTOM_LEFT | BOTTOM_RIGHT
	},
	{
		'N',
		TOP_LEFT | TOP_X_LEFT | TOP_RIGHT,
		BOTTOM_LEFT | BOTTOM_X_RIGHT | BOTTOM_RIGHT
	},
	{
		'O',
		TOP_LEFT | TOP_TOP | TOP_RIGHT,
		BOTTOM_RIGHT | BOTTOM_BOTTOM | BOTTOM_LEFT
	},
	{
		'P',
		TOP_BAR_LEFT | TOP_LEFT | TOP_TOP | TOP_RIGHT,
		BOTTOM_BAR_RIGHT | BOTTOM_LEFT
	},
	{
		'Q',
		TOP_LEFT | TOP_TOP | TOP_RIGHT,
		BOTTOM_X_RIGHT | BOTTOM_RIGHT | BOTTOM_BOTTOM | BOTTOM_LEFT
	},
	{
		'R',
		TOP_BAR_LEFT | TOP_LEFT | TOP_TOP | TOP_RIGHT,
		BOTTOM_BAR_RIGHT | BOTTOM_X_RIGHT | BOTTOM_LEFT
	},
	{
		'S',
		TOP_TOP | TOP_X_LEFT,
		BOTTOM_BAR_RIGHT | BOTTOM_RIGHT | BOTTOM_BOTTOM
	},
	{
		'T',
		TOP_TOP | TOP_MIDDLE,
		BOTTOM_MIDDLE
	},
	{
		'U',
		TOP_LEFT | TOP_RIGHT,
		BOTTOM_RIGHT | BOTTOM_BOTTOM | BOTTOM_LEFT
	},
	{
		'V',
		TOP_LEFT | TOP_X_RIGHT,
		BOTTOM_LEFT | BOTTOM_X_LEFT
	},
	{
		'W',
		TOP_LEFT | TOP_RIGHT,
		BOTTOM_RIGHT | BOTTOM_X_RIGHT | BOTTOM_X_LEFT | BOTTOM_LEFT
	},
	{
		'X',
		TOP_X_LEFT | TOP_X_RIGHT,
		BOTTOM_X_RIGHT | BOTTOM_X_LEFT
	},
	{
		'Y',
		TOP_X_LEFT | TOP_X_RIGHT,
		BOTTOM_MIDDLE
	},
	{
		'Z',
		TOP_TOP | TOP_X_RIGHT,
		BOTTOM_X_LEFT | BOTTOM_BOTTOM
	},
	{
		'-',
		TOP_BAR_LEFT,
		BOTTOM_BAR_RIGHT
	},
	{
		'_',
		0,
		BOTTOM_BOTTOM
	},
	{
		'=',
		TOP_BAR_LEFT,
		BOTTOM_BAR_RIGHT | BOTTOM_BOTTOM
	},
	{
		'+',
		TOP_BAR_LEFT | TOP_MIDDLE,
		BOTTOM_BAR_RIGHT | BOTTOM_MIDDLE
	},
	{
		'*',
		TOP_X_LEFT | TOP_MIDDLE | TOP_X_RIGHT,
		BOTTOM_X_RIGHT | BOTTOM_MIDDLE | BOTTOM_X_LEFT
	},
	{
		'[',
		TOP_TOP | TOP_LEFT,
		BOTTOM_LEFT | BOTTOM_BOTTOM
	},
	{
		']',
		TOP_TOP | TOP_RIGHT,
		BOTTOM_RIGHT | BOTTOM_BOTTOM
	},
	{
		'<',
		TOP_X_RIGHT,
		BOTTOM_X_RIGHT
	},
	{
		'>',
		TOP_X_LEFT,
		BOTTOM_X_LEFT
	},
	{
		'{', // special left-pointing arrow (<-)
		TOP_X_RIGHT,
		BOTTOM_X_RIGHT | BOTTOM_BAR_RIGHT
	},
	{
		'}', // special right-pointing arrow (->)
		TOP_X_LEFT | TOP_BAR_LEFT,
		BOTTOM_X_LEFT
	},
	{
		'/',
		TOP_X_RIGHT,
		BOTTOM_X_LEFT
	},
	{
		'\\',
		TOP_X_LEFT,
		BOTTOM_X_RIGHT
	},
	{
		'.',
		0,
		BOTTOM_DOT
	},
};

const size_t FSD_CHARS_COUNT = sizeof(FSD_CHARS) / sizeof(FSDChar);

const FSDChar *get_fsd_char(const char char_code) {
	for (size_t i = 0; i < FSD_CHARS_COUNT; i++) {
		const FSDChar *fsd_char = &FSD_CHARS[i];
		if (fsd_char->char_code == char_code) {
			return fsd_char;
		}
	}
	return NULL;
}

int str_to_fsd_chars(const char *char_string, const FSDChar **out_fsd_chars, size_t out_length) {
	size_t i;
	bool in_str = true;
	char char_code;
	for (i = 0; i < out_length; i++) {
		char_code = in_str ? char_string[i] : 0;
		if (char_code == 0) in_str = false;
		out_fsd_chars[i] = get_fsd_char(char_code);
	}
	out_fsd_chars[out_length-1] = NULL;
	return i;
}

int strn_to_fsd_chars(const char *char_string, size_t char_string_length, const FSDChar **out_fsd_chars, size_t out_length) {
	size_t i;
	char char_code;
	for (i = 0; i < out_length; i++) {
		char_code = (i < char_string_length) ? char_string[i] : 0;
		out_fsd_chars[i] = get_fsd_char(char_code);
	}
	out_fsd_chars[out_length-1] = NULL;
	return i;
}
