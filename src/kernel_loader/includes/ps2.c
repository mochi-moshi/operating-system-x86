#include "ps2.h"
#include "pic.h"
#include "idt.h"
#include "scancodes.h"
#include "stddef.h"
#include "vbe_driver.h"

void ps2_controller_command(const uint8_t code) {
	ps2_input_wait()
	;
    outb(PS2_CONTROLLER_COMMAND_PORT, code);
}
void ps2_controller_out_data(const uint8_t data) {
	ps2_input_wait()
	;
    outb(PS2_CONTROLLER_DATA_PORT, data);
}
uint8_t ps2_controller_in_data(void) {
	ps2_output_wait()
	;
    return inb(PS2_CONTROLLER_DATA_PORT);
}
void initialize_ps2(void) {
	ps2_controller_command(ps2_controller_disable_1st_port);
	ps2_controller_command(ps2_controller_disable_2nd_port);

	while(ps2_output_available())
		ps2_controller_in_data(); // flush buffer

	ps2_controller_command(ps2_controller_read_config); // read configuration byte
	// struct ps2_config_t config;
	// config._data = ps2_controller_in_data();
	// config.first_port_int = false;
	// config.second_port_int = false;
	uint8_t config = ps2_controller_in_data();
	config = config | 0b11111100;	// disable irqs for now
	ps2_controller_command(ps2_controller_write_config); // write configuration byte
	ps2_controller_out_data(config);

	ps2_controller_command(ps2_controller_test_controller); // test controller
	if(ps2_controller_in_data() != 0x55) {
		sc_set_foreground(0xffffffff);
		sc_set_background(0xffff0000);
		sc_print(0, 0, "Error testing PS2 Controler");
		cli();
		for(;;);
	}
	
	ps2_controller_command(ps2_controller_enable_1st_port);
	while(ps2_output_available()) ps2_controller_in_data(); // flush'

	ps2_controller_out_data(0xFF); // reset keyboard
	while(ps2_output_available()) ps2_controller_in_data(); // flush'

	pic_set_irq(1, keyboard_handler);
	pic_enable_irq(1);
	ps2_controller_command(ps2_controller_read_config); // read configuration byte
	config = ps2_controller_in_data();
	config = config | 0b1;	// enable keyboard interrupt
	ps2_controller_command(ps2_controller_write_config); // write configuration byte
	ps2_controller_out_data(config);


	// ps2_input_wait()
	// ;
	// ps2_controller_out_data(0xF0);
	// while(ps2_output_available()) ps2_controller_in_data(); // flush'
	// ps2_input_wait()
	// ;
	// ps2_controller_out_data(1);
	// while(ps2_output_available()) ps2_controller_in_data(); // flush'

	// // ps2_output_wait();
	// // __asm__ __volatile__("movb %0,%%al"::"g"(ps2_controller_in_data()));
	// // __asm__ __volatile__("movb %0,%%ah"::"g"(ps2_controller_in_data()));
	// // BOCH_break();
	// // if(ps2_controller_in_data() != 0xAA) __asm__ __volatile__ ("cli\n\thlt");
	// do {
	// 	ps2_input_wait();
	// 	ps2_controller_out_data(0xF0); // set scancode set
	// 	ps2_output_wait();
	// } while (ps2_controller_in_data() == 0xFE);
	// ps2_input_wait();
	// ps2_controller_out_data(1);
	// BOCH_break();
}

static bool_t keymap[256];
static bool_t shift_active = 0;
static bool_t control_active = 0;

bool_t key_pressed(Scancode_t key) {
    return keymap[key];
}

static KeyEventHandler_t kehandler = 0;


static const char scancode_to_ascii[] = "\000\0001234567890-=\b\tqwertyuiop[]\n\0asdfghjkl;'`\0\\zxcvbnm,./\0\0\0 ";
static const char scancode_to_ascii_shifted[] = "\000\000!@#$%^&*()_+\b\tQWERTYUIOP{}\r\0ASDFGHJKL:\"~\0|ZXCVBNM<>?\0\0\0 ";
void kbh(void) {
	// sc_print(0, 0, "INTERRUPT");
    register uint8_t al= inb(PS2_CONTROLLER_DATA_PORT);
	if(al == KEY_ALTERNATE) {
		inb(PS2_CONTROLLER_DATA_PORT);
		return;
	}
	keymap[al & (~KEY_RELEASED)] = !(al & KEY_RELEASED);
	if((al & KEY_LEFTSHIFT) == KEY_LEFTSHIFT || (al & KEY_RIGHTSHIFT) == KEY_RIGHTSHIFT) {
		shift_active = !(al & KEY_RELEASED);
	}
	if((al & KEY_LEFTCONTROL) == KEY_LEFTCONTROL) {
		control_active = !(al & KEY_RELEASED);
	}
	KeyEvent_t ke = {
		.ascii = (shift_active) ? scancode_to_ascii_shifted[al & ~KEY_RELEASED] : scancode_to_ascii[al & ~KEY_RELEASED],
		.scancode = al,
		.keypressed = ((al & KEY_RELEASED) != KEY_RELEASED),
		.shifted = shift_active,
		.control = control_active
	};
	if(kehandler) {
		kehandler(ke);
	}
}

__attribute__((interrupt))
void keyboard_handler(ptr_t stack) {
    __asm__ __volatile__ ("push %eax; push %ecx; push %edx; push %ebx; push %esp; push %ebp; push %esi; push %edi;cld;");
	kbh();
    pic_eoi(1);
    __asm__ __volatile__ ("pop %edi; pop %esi; pop %ebp; pop %esp; pop %ebx; pop %edx; pop %ecx; pop %eax;");
}

void register_keyevent_handler(KeyEventHandler_t handler) {
	kehandler = handler;
}
void unregister_keyevent_handler() {
	kehandler = 0;
}