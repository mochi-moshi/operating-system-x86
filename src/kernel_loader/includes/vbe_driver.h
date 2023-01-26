#include "stddef.h"
#include "bootcode.h"

typedef struct __attribute__((packed))
{
    uint8_t lines[16];
} Character_t;

typedef struct __attribute__((packed))
{
    uint8_t alpha;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} Pixel_t;

#define PIXRAW(pixel) *((uint32_t*)&pixel)

void initialize_vbe(const uint32_t foreground, const uint32_t background);

void sc_set_foreground(const uint32_t foreground);
void sc_set_background(const uint32_t background);
VBEINFOBLOCK_t sc_get_vbeinfoblock();
MODEINFOBLOCK_t sc_get_modeinfoblock();
Character_t* sc_get_font();
uint32_t sc_get_width();
uint32_t sc_get_height();
VBEINFOBLOCK_t sc_get_vbeinfoblock();
MODEINFOBLOCK_t sc_get_modeinfoblock();
Character_t* sc_get_font();

void sc_setpixel(uint32_t* offset, uint32_t value);

void sc_print(uint32_t x, uint32_t y, const char *string);
void sc_putch(uint32_t x, uint32_t y, const char chr);
void sc_print_byte(uint32_t x, uint32_t y, const byte b);
void sc_print_hex(uint32_t x, uint32_t y, ptr_t bytes, size_t amt);
void sc_print_rhex(uint32_t x, uint32_t y, ptr_t bytes, size_t amt);
void sc_clear();
void sc_line(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint32_t color);
void sc_fill(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t color);
void sc_box(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t thickness, uint32_t color);

void sc_putimg(uint32_t x0, uint32_t y0, uint32_t width, uint32_t height, uint32_t* data);
void sc_putimg_masked(uint32_t x0, uint32_t y0, uint32_t width, uint32_t height, uint32_t* data, uint32_t color1, uint32_t color2);