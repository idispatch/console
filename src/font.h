#ifndef FONT_H_
#define FONT_H_

typedef struct {
    unsigned char_width;
    unsigned char_height;
    const char * font_name;
    unsigned char * font_bitmap;
} font_t;

extern const font_t console_fonts[];

#endif /* FONT_H_ */
