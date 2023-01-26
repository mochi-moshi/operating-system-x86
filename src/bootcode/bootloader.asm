    BITS 16
    section .entry

extern second_stage, SIZE_OF_BOOTCODE, vbe_info_block, mode_info_block, SMAP_entries

global OEM_Name, BS_BytesPerSec, BS_SecPerClst, BS_ReservedSec, BS_FATCopies, BS_NumRootEnt
global BS_TotalSec, BS_MediaDesc, BS_NumSecPerFAT, BS_NumSecPerFAT, BS_SecPerTrk, BS_SecPerTrk, BS_NumHeads
global BS_NumHiddenSec, BS_TotalSec2, BS_DriveNum, BS_Reserved, BS_ExtSig, BS_SerialNum, BS_VolLabel, BS_FSType
jmpBoot:          jmp start
OEM_Name:         db "????????" ; pad with 0x20
BS_BytesPerSec:   dw 512
BS_SecPerClst:    db 1
BS_ReservedSec:   dw SIZE_OF_BOOTCODE
BS_FATCopies:     db 1 ; pref 2
BS_NumRootEnt:    dw 512
BS_TotalSec:      dw 2880
BS_MediaDesc:     db 0xf8 ; hard disk
BS_NumSecPerFAT:  dw 12 ; 1 sec per 256 cluster
BS_SecPerTrk:     dw 0
BS_NumHeads:      dw 0
BS_NumHiddenSec:  dd 0
BS_TotalSec2:     dd 2880
BS_DriveNum:      db 0
BS_Reserved:      db 0
BS_ExtSig:        db 0x29
BS_SerialNum:     dd 0x69696969
BS_VolLabel:      db "NO NAME    "
BS_FSType:        db "FAT16   "

; BS_NumDrives:     db 0
; BS_NumCylind:     db 0
start:
    mov ax, 0
    mov ds, ax

    mov byte [BS_DriveNum], dl

;   probe disk system
    mov ax,  0x0800
    int      0x13
    ; mov byte [BS_NumDrives],  dl
    mov byte [BS_NumHeads],   dh
    ; mov byte [BS_NumCylind],  ch
    mov byte [BS_SecPerTrk],  cl

;   Set registers to known state
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov si, 0
    mov di, 0

    mov ax, 0x07BFF
    mov ss, ax
    mov sp, 0x0000
    mov bp, 0x0000
    

    mov ah, 0x41
    mov bx, 0x55AA
    mov dl, byte [BS_DriveNum]
    int 0x13
    jnc ext
    mov si, NO_EXT
    call print
    jmp $
NO_EXT: db "BIOS does not support extensions", 0
ext:
    ; load FAT and Root directory
    mov ax, word [BS_NumRootEnt]
    mov cx, 32
    mul cx
    div word [BS_BytesPerSec]
    cmp dx, 0
    je .skip
    inc ax
.skip:
    add ax, word [BS_NumSecPerFAT]
    add ax, word [BS_ReservedSec]
    dec ax
    mov word [DK_number_of_sectors], ax
    mov word [DK_lower_lba], 1
    mov word [DK_upper_lba], 0
    mov word [DK_transfer_buffer_es], 0x7E0
    mov word [DK_transfer_buffer_bx], 0
    call load_lba_sector

    ; mov word [DK_transfer_buffer_es], 0xBF0
    ; mov word [DK_transfer_buffer_bx], 0
    ; mov si, SECOND_STAGE
    ; call find_entry

    ; set segment registers to known state
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; Jump to 2nd stage
    jmp 0:second_stage
global print, print_al, print_ax, load_lba_sector
print:
    push ax
    push si
    mov ah, 0x0e
    .loop:
        lodsb
        cmp al, 0
        je .end
        int 0x10
        jmp .loop
    .end:
    pop si
    pop ax
    ret

print_al:
    push ds
    push bx
    push ax
    push ax
    mov bx, 0
    mov ds, bx
    mov ah, 0x0e
    mov bx, hex_to_ascii
    and al, 0xF0
    shr al, 4
    xlatb
    int 0x10
    pop ax
    mov ah, 0x0e
    and al, 0xF
    xlatb
    int 0x10
    pop ax
    pop bx
    pop ds
    ret

hex_to_ascii: db "0123456789ABCDEF"
print_ax:
    push ax
    mov al, ah
    call print_al
    pop ax
    call print_al
    ret

load_lba_sector:
    push ax
    push si
    mov si, DK_packet_size
    mov ax, 0x4200
    mov dl, byte [BS_DriveNum]
    int 0x13
    jc .err
    pop si
    pop ax
    ret
    .err:
        mov si, .errmsg
        call print
        call print_ax
        mov ax, 0x0E0A
        int 0x10
        mov al, 0x0D
        int 0x10
        mov ax, word [DK_lower_lba]
        call print_ax
        mov ax, 0x0E0A
        int 0x10
        mov al, 0x0D
        int 0x10
        mov ax, word [DK_number_of_sectors]
        call print_ax
        jmp $
    .errmsg: db "There was an error reading: ", 0

global DK_packet_size, DK_number_of_sectors, DK_transfer_buffer_bx, DK_transfer_buffer_es, DK_lower_lba, DK_upper_lba
align 4
DK_packet_size:        db 16
                       db 0
DK_number_of_sectors:  dw 0
DK_transfer_buffer_bx: dw 0
DK_transfer_buffer_es: dw 0
DK_lower_lba:          dd 0
DK_upper_lba:          dd 0

times 498-($-$$) db 0
dd vbe_info_block
dd mode_info_block
dd SMAP_entries
dw 0xAA55
END_OF_BOOTLOADER: