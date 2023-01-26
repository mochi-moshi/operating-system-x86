#include "vbe_driver.h"
#include "FAT16.h"
#include "ctype.h"

#define SCREEN_PIXEL_WIDTH  (ModeInfoBlock.XResolution)//1920
#define SCREEN_PIXEL_HEIGHT (ModeInfoBlock.YResolution)//1080
#define SCREEN_BUFFER_SIZE  (ModeInfoBlock.XResolution*ModeInfoBlock.BitsPerPixel/4 + ModeInfoBlock.YResolution*ModeInfoBlock.BytesPerScanline/4)

static VBEINFOBLOCK_t VBEInfoBlock;
static MODEINFOBLOCK_t ModeInfoBlock;

static Character_t Font[128];

static uint32_t foreground_color, background_color;

void initialize_vbe(const uint32_t foreground, const uint32_t background)
{
    BOOTCODE_t *bootcode = (BOOTCODE_t*)0x7C00;
    memcpy(&VBEInfoBlock, (ptr_t)bootcode->VBEInfoBlock, sizeof(VBEINFOBLOCK_t));
    memcpy(&ModeInfoBlock, (ptr_t)bootcode->ModeInfoBlock, sizeof(MODEINFOBLOCK_t));
    DirectoryEntry_t *font_file = find_entry("font    bin");
    if(font_file != 0)
        read_all_to(&Font[0], font_file);
    foreground_color = foreground;
    background_color = background;
}

void sc_set_foreground(const uint32_t foreground) {
    foreground_color = foreground;
}

void sc_set_background(const uint32_t background) {
    background_color = background;
}

VBEINFOBLOCK_t sc_get_vbeinfoblock() {
    return VBEInfoBlock;
}

MODEINFOBLOCK_t sc_get_modeinfoblock() {
    return ModeInfoBlock;
}

Character_t* sc_get_font() {
    return Font;
}

uint32_t sc_get_width() {
    return ModeInfoBlock.XResolution;
}

uint32_t sc_get_height() {
    return ModeInfoBlock.YResolution;
}

void sc_setpixel(uint32_t * offset, uint32_t value)
{
    if((value & 0xFF000000) == 0xFF000000) {
        *offset = value;
    } else {
        Pixel_t *add = (Pixel_t*)&value;
        Pixel_t *prev = (Pixel_t*)offset;
        prev->red   = (prev->red * (0xFF - add->alpha) + add->red * add->alpha) >> 8;
        prev->green = (prev->green * (0xFF - add->alpha) + add->green * add->alpha) >> 8;
        prev->blue  = (prev->blue * (0xFF - add->alpha) + add->blue * add->alpha) >> 8;
    }
}

void sc_print(uint32_t x, uint32_t y, const char *string)
{
    for(const char *c = string; *c != 0; c++)
    {
        if(*c == 0xA) {
            y+=16;
            if(y >= SCREEN_PIXEL_HEIGHT)
            {
                return;
            }
        } else if (*c == 0xD) {
           x = 0;
        } else {
            sc_putch(x, y, *c);
            if(isprint(*c))
            {
                x+=8;
                if(x >= SCREEN_PIXEL_WIDTH)
                {
                    x = 0;
                    y+=16;
                }
                if(y >= SCREEN_PIXEL_HEIGHT)
                {
                    return;
                }
            }
        }
    }
}
void sc_putch(uint32_t x, uint32_t y, const char chr)
{
    //if(!isprint(chr)) return;
    Character_t font_data = Font[(int)chr-1];
    uint32_t *cbuff = ((uint32_t*)ModeInfoBlock.PhysicalBasePointer) + x + ModeInfoBlock.BytesPerScanline*y/4;
    for(uint8_t l=0; l<16; l++) {
        for(uint8_t r=0x80; r != 0; r >>= 1) {
            sc_setpixel(cbuff, (font_data.lines[l]&r)?foreground_color:background_color); // ARGB
            cbuff++;
            if(cbuff >= (uint32_t*)ModeInfoBlock.PhysicalBasePointer + SCREEN_BUFFER_SIZE) return;
        }
        cbuff += ModeInfoBlock.BytesPerScanline/4 - 8;
        if(cbuff >= (uint32_t*)ModeInfoBlock.PhysicalBasePointer + SCREEN_BUFFER_SIZE) return;
    }
}
char hextoasc[16] = "0123456789ABCDEF";
void sc_print_byte(uint32_t x, uint32_t y, const byte b)
{
    sc_putch(x, y, hextoasc[(b >> 4 ) & 0xF]);
    x+=8;
    if(x >= SCREEN_PIXEL_WIDTH)
    {
        x = 0;
        y+=16;
    }
    if(y >= SCREEN_PIXEL_HEIGHT)
    {
        return;
    }
    sc_putch(x, y, hextoasc[(b      ) & 0xF]);
}
void sc_print_hex(uint32_t x, uint32_t y, ptr_t bytes, size_t amt)
{
    for(size_t i = 0; i < amt; i++) {
        sc_print_byte(x, y, *(byte*)(bytes+i));
        x+=16;
        if(x >= SCREEN_PIXEL_WIDTH)
        {
            x = 0;
            y+=16;
        }
        if(y >= SCREEN_PIXEL_HEIGHT)
        {
            return;
        }
        sc_putch(x, y, ' ');
        x+=8;
        if(x >= SCREEN_PIXEL_WIDTH)
        {
            x = 0;
            y+=16;
        }
        if(y >= SCREEN_PIXEL_HEIGHT)
        {
            return;
        }
    }
}
void sc_print_rhex(uint32_t x, uint32_t y, ptr_t bytes, size_t amt)
{
    for(int i = amt-1; i >= 0; i--) {
        sc_print_byte(x, y, *(byte*)(bytes+i));
        x+=16;
        if(x >= SCREEN_PIXEL_WIDTH)
        {
            x = 0;
            y+=16;
        }
        if(y >= SCREEN_PIXEL_HEIGHT)
        {
            return;
        }
    }
}

void sc_line(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint32_t color) {
    uint32_t *framebuffer = (uint32_t*)ModeInfoBlock.PhysicalBasePointer;
    int dx = x1-x0;
    int dy = y1-y0;
    uint32_t x, y;
    int D;
    if(dx > dy && dx > -dy) {
    	D = 2*dy - dx;
	    y = y0;
	    for(x = x0; x < x1; x++) {
            sc_setpixel((framebuffer + x*ModeInfoBlock.BitsPerPixel/32 + y*ModeInfoBlock.BytesPerScanline/4), color);
	    	if(D > 0) {
	    		y++;
	    		D -= 2*dx;
	    	}
	    	D += 2*dy;
	    }
    } else {
    	D = 2*dx - dy;
	    x = x0;
	    for(y = y0; y < y1; y++) {
            sc_setpixel((framebuffer + x*ModeInfoBlock.BitsPerPixel/32 + y*ModeInfoBlock.BytesPerScanline/4), color);
	    	if(D > 0) {
	    		x++;
	    		D -= 2*dy;
	    	}
	    	D += 2*dx;
	    }
    }
}

void sc_clear()
{
    sc_fill(0, 0, SCREEN_PIXEL_WIDTH-1, SCREEN_PIXEL_HEIGHT-1, background_color);
}

void sc_fill(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t color) {
    uint32_t *framebuffer = (uint32_t*)ModeInfoBlock.PhysicalBasePointer;
    for(uint32_t y=y1; y<=y2; y++) {
        for(uint32_t x=x1; x<=x2; x++) {
            sc_setpixel((framebuffer + x*ModeInfoBlock.BitsPerPixel/32 + y*ModeInfoBlock.BytesPerScanline/4), color);
        }
    }
}

void sc_box(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t thickness, uint32_t color) {
    if(thickness == 0) return;
    thickness -= 1;
    sc_fill(x1, y1, x2, y1+thickness, color);
    sc_fill(x1, y1, x1+thickness, y2, color);
    sc_fill(x2-thickness, y1, x2, y2, color);
    sc_fill(x1, y2-thickness, x2, y2, color);
}

void sc_putimg(uint32_t x0, uint32_t y0, uint32_t width, uint32_t height, uint32_t* data) {
    uint32_t *framebuffer = (uint32_t*)ModeInfoBlock.PhysicalBasePointer;
	for(uint32_t y = y0; y < y0+height; y++) {
	    for(uint32_t x = x0; x < x0+width; x++) {
            sc_setpixel((framebuffer + x + y*ModeInfoBlock.BytesPerScanline/4), *data++);
    	}
    }
}
void sc_putimg_masked(uint32_t x0, uint32_t y0, uint32_t width, uint32_t height, uint32_t* data, uint32_t color1, uint32_t color2) {
    uint32_t *framebuffer = (uint32_t*)ModeInfoBlock.PhysicalBasePointer;
    uint32_t r = 0x1;
	for(uint32_t y = y0; y < y0+height; y++) {
	    for(uint32_t x = x0; x < x0+width; x++) {
            if(r & *data) {
                sc_setpixel((framebuffer + x + y*ModeInfoBlock.BytesPerScanline/4), color1);
            } else {
                sc_setpixel((framebuffer + x + y*ModeInfoBlock.BytesPerScanline/4), color2);
            }
            r <<= 1;
            if(r == 0) {
                r = 0x1;
                data++;
            }
    	}
    }
}