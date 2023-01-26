#pragma once
typedef char                int8_t;
#define INT8_MAX             0x7F
typedef unsigned char       uint8_t;
#define UINT8_MAX            0xFF
typedef short               int16_t;
#define INT16_MAX            0x7FFF
typedef unsigned short      uint16_t;
#define UINT16_MAX           0xFFFF
typedef int                 int32_t;
#define INT32_MAX            0x7FFFFFFF
typedef unsigned int        uint32_t;
#define UINT32_MAX           0xFFFFFFFF
typedef long long           int64_t;
#define INT64_MAX            0x7FFFFFFFFFFFFFFF
typedef unsigned long long  uint64_t;
#define UINT64_MAX           0xFFFFFFFFFFFFFFFF

typedef void*               ptr_t;
typedef unsigned int        addr_t;
typedef unsigned int        size_t;
typedef unsigned char       bool_t;

typedef unsigned char       byte;
#define false 0
#define true  1

#define halt() __asm__ __volatile__ ("cli;hlt")

#define outb(port, val) \
    __asm__ __volatile__ ("outb %0, %1": :"a"((uint8_t)(val)), "Nd"((uint16_t)(port)))
#define outw(port, val) \
    __asm__ __volatile__ ("outw %0, %1": :"a"((uint16_t)(val)), "Nd"((uint16_t)(port)))
__attribute__((no_caller_saved_registers))
inline void outsw(uint16_t port, const void *buf, unsigned long n) {
	__asm__ __volatile__("cld; rep; outsw"
			: "+S"(buf), "+c"(n)
			: "d"(port));
}
__attribute__((no_caller_saved_registers))
inline void outsl(uint16_t port, const void *buf, unsigned long n) {
	__asm__ __volatile__("cld; rep; outsl"
			: "+S"(buf), "+c"(n)
			: "d"(port));
}
__attribute__((no_caller_saved_registers))
inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ __volatile__ ("inb %1, %0": "=a"(ret): "Nd"(port));
    return ret;
}
__attribute__((no_caller_saved_registers))
inline uint16_t inw(uint16_t port) {
    uint16_t ret;
    __asm__ __volatile__ ("inw %1, %0": "=a"(ret): "Nd"(port));
    return ret;
}
__attribute__((no_caller_saved_registers))
inline void insw(uint16_t port, void *buf, unsigned long n) {
	__asm__ __volatile__("cld; rep; insw"
			: "+D"(buf), "+c"(n)
			: "d"(port));
}
#define io_wait() \
    outb(0x80, 0)

#define do_interrupt(code)      \
    __asm__ __volatile__ (      \
        "movb %0, (0f+1);"      \
        "0:;"                   \
        "int $1"                \
        : :"r"((uint8_t)(code)) \
        : "cc","memory"         \
    )
#define int(code) \
    __asm__ __volatile__ ("int $" ## #code)

#define sti() __asm__ __volatile__ ("sti")
#define cli() __asm__ __volatile__ ("cli")

// void print_hex(ptr_t bytes, size_t amt);
// void print_byte(byte bytes);
// void print(const char* string);
// void print_c(const char* string, size_t amt);
// void putch(char c);
// void clear();

void memset(ptr_t location, const byte value, size_t length);
void memcpy(ptr_t to, const ptr_t from, size_t length);
void memmove(ptr_t to, const ptr_t from, size_t length);
int8_t memcmp(const ptr_t a, const ptr_t b, size_t length);