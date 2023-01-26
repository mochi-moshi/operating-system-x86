#pragma once
#include "stddef.h"
#include "scancodes.h"

#define PS2_CONTROLLER_COMMAND_PORT 0x64
#define PS2_CONTROLLER_STATUS_PORT  0x64
#define PS2_CONTROLLER_DATA_PORT    0x60    

enum ps2_controller_Command
{
    ps2_controller_read_config  = 0x20,
    ps2_controller_read_byteN  = 0x20, // command & byte_number up to 0x1F
    ps2_controller_write_config  = 0x60,
    ps2_controller_write_byteN = 0x60, // command & byte_number up to 0x1F
    ps2_controller_disable_2nd_port = 0xA7,
    ps2_controller_enable_2nd_port = 0xA8,
    ps2_controller_test_2nd_port = 0xA9,
    ps2_controller_test_controller = 0xAA,
    ps2_controller_1st_port = 0xAB,
    ps2_controller_diagnostic_dump = 0xAC,
    ps2_controller_disable_1st_port = 0xAD,
    ps2_controller_enable_1st_port = 0xAE,
    ps2_controller_read_controller_input = 0xC0,
    ps2_controller_copy_status0_3to4_7 = 0xC1,
    ps2_controller_copy_status4_7to_0_3 = 0xC2,
    ps2_controller_read_controller_output = 0xD0,
    ps2_controller_write_next_output = 0xD1,
    ps2_controller_write_next_1st_output = 0xD2,
    ps2_controller_write_next_2nd_output = 0xD3,
    ps2_controller_write_next_2nd_input = 0xD4,
    ps2_controller_reset = 0xFE // Bits 0-3 mask lines (0=pulse,1=don't), bit0 = reset
};
enum ps2_device_type
{
    ps2_mouse = 0x00,
    ps2_mouse_with_scroll = 0x03,
    ps2_5btn_mouse = 0x04,
    ps2_MF2_keyboard = 0xAB,
    ps2_MF2_keyboard_trans2 = 0x41,
    ps2_MF2_keyboard_trans3 = 0xC1,
    ps2_MF2_keyboard2 = 0x83
};

struct __attribute__((packed)) ps2_status_t
{
    char output_full    : 1;
    char input_full     : 1;
    char system_flag    : 1;
    char for_controller : 1;
    char res1           : 1;
    char res2           : 1;
    char timeout        : 1;
    char parity         : 1;
};

struct __attribute__((packed)) ps2_config_t
{
    union
    {
        struct {
            char first_port_int     : 1;
            char second_port_int    : 1;
            char system_flag        : 1;
            char res0               : 1;
            char first_port_clock   : 1;
            char second_port_clock  : 1;
            char first_port_trans   : 1;
            char res1               : 1;
        };
        uint8_t _data;
    };
    
} ;

struct __attribute__((packed)) ps2_output_t
{
    char system_reset             : 1;
    char a20_gate                 : 1;
    char second_port_clock        : 1;
    char second_port_data         : 1;
    char output_full_from_first   : 1;
    char output_full_from_second  : 1;
    char first_port_clock         : 1;
    char first_port_data          : 1;
};

typedef struct {
    char ascii;
	Scancode_t scancode;
	bool_t keypressed;
    bool_t shifted;
    bool_t control;
} KeyEvent_t;

typedef void(*KeyEventHandler_t)(KeyEvent_t);

#define ps2_get_status() \
    ((ps2_status_t)inb(PS2_CONTROLLER_STATUS_PORT))
#define ps2_input_available() \
	(!(inb(PS2_CONTROLLER_STATUS_PORT) & 2))
#define ps2_output_available() \
	(inb(PS2_CONTROLLER_STATUS_PORT) & 1)
#define ps2_input_wait() \
	while(!ps2_input_available())
#define ps2_output_wait() \
	while(!ps2_output_available())
void ps2_controller_command(const uint8_t code);
void ps2_controller_out_data(const uint8_t data);
uint8_t ps2_controller_in_data(void);
void initialize_ps2(void);

bool_t key_pressed(Scancode_t key);
void keyboard_handler(ptr_t stack) __attribute__((interrupt));
void register_keyevent_handler(KeyEventHandler_t handler);
void unregister_keyevent_handler();