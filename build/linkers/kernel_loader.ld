SECTIONS
{
	.text 0x10000 :
	{
		*(kernel_entry);
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
}
