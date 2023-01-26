SECTIONS
{
    . = 0x7C00;
	.text ALIGN(512):
	{
		*(.entry*);
		. = ALIGN(512);
		*(.text*);
		. = ALIGN(512);
	}
	END_OF_BOOTCODE = .;
	SIZE_OF_BOOTCODE = (END_OF_BOOTCODE - 0x7C00) / 512;
}