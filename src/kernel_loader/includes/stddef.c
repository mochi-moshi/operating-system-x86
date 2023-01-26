#include "stddef.h"

// #define WIDTH 80
// #define HEIGHT 25
// #define VIDEO_MEMORY ((char*) 0xB8000)

// size_t video_offset = 0;

// void print_c(const char* string, size_t amt)
// {
//     for(size_t i = 0; i < amt; i++) putch(*(string+i));
// }
// void print(const char* string)
// {
//     for(; *string != 0; string++) putch(*string);
// }
// void putch(const char c)
// {
//     if(c == 0xA) {
//         video_offset += WIDTH;
//         if(video_offset > WIDTH*HEIGHT) {
//             memcpy(VIDEO_MEMORY, VIDEO_MEMORY + WIDTH, WIDTH*(HEIGHT - 1));
//             memset( VIDEO_MEMORY + WIDTH*(HEIGHT - 1), ' ', WIDTH);
//         }
//     } else if (c == 0xD) {
//         video_offset -= video_offset % WIDTH;
//     } else {
//         *(VIDEO_MEMORY + (video_offset++)*2) = c;
//     }
// }
// void clear()
// {
//     for(video_offset = 0; video_offset < WIDTH*HEIGHT; video_offset++) *(VIDEO_MEMORY + video_offset*2) = ' ';
//     video_offset = 0;
// }


void memset(ptr_t location, const byte value, size_t length)
{
    for(; length != 0; length--)
    {
        *((byte*)location++) = value;
    }
}
void memcpy(ptr_t to, const ptr_t from, size_t length)
{
    for(; length != 0; length--)
    {
        *((byte*)to + length - 1) = *((byte*)from + length - 1);
    }
}
void memmove(ptr_t to, const ptr_t from, size_t length)
{
    if(to > from) {
        memcpy(to, from, length);
    } else {
        for(size_t i = 0; i < length; i++)
        {
            *((byte*)to + i) = *((byte*)from + i);
        }
    }
}
int8_t memcmp(const ptr_t a, const ptr_t b, size_t length)
{
    int8_t diff = 0;
    for(size_t i = 0; i < length; i++)
    {
        diff = *((byte*)a + i) - *((byte*)b + i);
        if (diff != 0) return diff;
    }
    return diff;
}