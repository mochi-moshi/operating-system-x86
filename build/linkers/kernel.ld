SECTIONS
{
	. = 0xC0000000;
	.header :
	{
		LONG(__kernel_start);
		LONG(__bss_end);
	}
	.text  :
	{
		__kernel_start = .;
		*(entry);
		*(.text*);
	}
	.rodata	:
	{
		*(.rodata*);
	}
	__bss_start = .;
	.bss :
	{
		*(.bss*);
	}
	__bss_end = .;
	MALLOC_START_ADDRESS = .;
}
